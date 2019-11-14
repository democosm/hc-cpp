HC — Host Control
==================

## About

We want to help embedded system designers succeed, so we've created some very powerful source code to help. It's called Host Control (HC). It's easy to understand, easy to use, and doesn't require you to pollute your code with ours.

- HC implements server discovery so a generic client can mimic the capabilities of a server
- HC allows aggregation of server capabilities and chaining multiple applications together
- HC provides a simple way to make a web based REST API for your embedded application
- HC handles the complexity of saving and loading the state of your application
- HC makes it easy to create multiple servers and clients on the same machine
- HC can encrypt the connection between client and server using TLS

---

## What is Server Discovery?

We have found that all too often in embedded system design, the people writing the c++ lower level code on the embedded system often don't communicate effectively to the GUI developers. Also, the control parameter dictionary (think MIB, but don't think SNMP) is often incomplete and inaccurate. This usually makes for a clunky GUI experience for both users and developers. We have seen all too often that when embedded system c++ developers add a capability to the system, they need to spend too much time documenting the new capability, passing that information to the GUI developers, and waiting for the GUI to materialize.

With HC, ebedded system c++ designers can focus more on the implementation of their lower level code and leave more of the burden of documentation and informing the GUI of capability changes to HC. One of the first things an HC client does when in negotiates a connection with an HC server is to download the server information file describing all of the controllable parameters of the server. This file includes, but may not be limited to, heirarchy, names, access permissions, valid ranges, enumerations, scaling, and savability.

For example, you could develop an embedded system and use HC for your middleware between a web GUI and the c++ backend code. Let's say your system has a way to control the color of an LED. You might want this control to be accessible from the HC heirarchial console at a path like "/led/color". You might also have implemented a few color settings which you might enumerate in HC as "Red", "Green" and "Blue". These enumerated colors will be written to the server's information file associating "Red", "Green" and "Blue" as possible states of the "/led/color" parameter. The GUI should be designed to parse this information file to determine the valid states of the "/led/color" parameter and populate its corresponding widget (perhaps a dropdown) with those settings.

Now if the backend c++ designers add two new colors (orange and purple) to the LED color control code, they should update the HC wrapper code to indicate that two new enumerations "Orange" and "Purple" are associated with the "/led/color" control parameter. Then when the HC client renegotiates with the server, the new server information file will contain those new enumerations and the GUI should automatically update the LED color control widget(s). Note that the GUI did NOT need any modification to add the two new colors to the widget.

## What is Server Aggregation?

HC allows multiple server applications to be controlled from a single client application. This client application can aggregate the capabilities of each of the server applications into a single unified server application. The diagram below depicts this (the <HC> symbol denotes HC inter-process communications).

`ClientA <HC> ServerA AGGREGATE(ClientB <HC> ServerB, ClientC <HC> ServerC)`

In this example, there are 4 processes:
1. ClientA
2. ServerA + ClientB + ClientC
3. ServerB
4. ServerC

ClientA would provide proxy control of ServerB and ServerC similar to the way NFS works.

## What is Application Chaining

Since HC allows a client application to "re-serve" its controls, a chain of clients and servers can be created. This is depicted below (the <HC> symbol denotes HC inter-process communications).

`ClientA <HC> ServerA AGGREGATE(ClientB <HC> ServerB AGGREGATE(ClientC <HC> ServerC ...))`

In this example, there are 4 processes:
   1. ClientA
   2. ServerA + ClientB
   3. ServerB + ClientC
   4. ServerC

ClientA would provide proxy control of ServerB and ServerC similar to the way NFS works.

## Implemented Features

* Basic types (int8 to int64, uint8 to uint64, float, double, bool, string)
* Data-less "call" type
* Basic file type
* Console interface to application
* Property tree discovery

## How to Build

Note: paths could change depending on cloned repo directory, architecture and OS.

```bash
# To build and execute the example server with all basic types
Open a terminal
cd hc-cpp
source activate
cd src/app/scratchsrv
mkposd
../../../dst/x86_64posixdbg/bin/scratchsrv

# To build and execute the example peer to peer message server
Open a terminal
cd hc-cpp
source activate
cd src/app/msgsrv
mkposd
../../../dst/x86_64posixdbg/bin/msgsrv 1501

# To build and execute the HC client
Open a terminal
cd hc-cpp
source activate
cd src/app/hccli
mkposd
../../../dst/x86_64posixdbg/bin/hccli scratchudp.xml
../../../dst/x86_64posixdbg/bin/hccli scratchmsg.xml
../../../dst/x86_64posixdbg/bin/hccli proxy.xml
```


## OpenSSL Libraries and HC

Since HC can encrypt the client/server connection using TLS, it is necessary to install openssl developer libraries before you can compile HC.

To download the openssl development package, use ```sudo apt-get install libssl-dev``` or ```sudo apt-get install openssl-dev```.

Disclaimer for openssl (an excerpt from the openssl website):

Please remember that export/import and/or use of strong cryptography software, providing cryptography hooks, or even just communicating technical details about cryptography software is illegal in some parts of the world. So when you import this package to your country, re-distribute it from there or even just email technical suggestions or even source patches to the authors or other people you are strongly advised to pay close attention to any laws or regulations which apply to you. The authors of OpenSSL are not liable for any violations you make here. So be careful, it is your responsibility.

## License

Copyright 2019 Democosm

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

---
