HC — Host Control
==================

## About

We want to help embedded system designers succeed, so we've created some very powerful source code to help. It's called Host Control (HC). It's easy to understand, easy to use, and doesn't require you to pollute your code with ours.

- HC provides a simple way to locally control your application from a console or GUI
- HC provides a simple way to remotely control your application from a console or GUI
- HC allows aggregation of server capabilities and chaining multiple applications together
- HC is application layer code, so it's easy to create multiple servers and clients on the same machine
- HC provides a built-in way to make a web based REST API for your embedded application

---

## Server Aggregation

HC allows multiple server applications to be controlled from a single client application. This client application can aggregate the capabilities of each of the server applications into a single unified server application. The diagram below depicts this (the <HC> symbol denotes HC inter-process communications).

`ClientA <HC> ServerA AGGREGATE(ClientB <HC> ServerB, ClientC <HC> ServerC)`

In this example, there are 4 processes:
1. ClientA
2. ServerA + ClientB + ClientC
3. ServerB
4. ServerC

ClientA would provide proxy control of ServerB and ServerC similar to the way NFS works.

## Chaining

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


## Libraries for HC

Before you can build, you will need to install the openssl libs and headers.

To download the openssl development package, use ```sudo apt-get install libssl-dev``` or ```sudo apt-get install openssl-dev```.

Disclaimer for openssl (an excerpt from the openssl website):

Please remember that export/import and/or use of strong cryptography software, providing cryptography hooks, or even just communicating technical details about cryptography software is illegal in some parts of the world. So when you import this package to your country, re-distribute it from there or even just email technical suggestions or even source patches to the authors or other people you are strongly advised to pay close attention to any laws or regulations which apply to you. The authors of OpenSSL are not liable for any violations you make here. So be careful, it is your responsibility.

## License

Copyright 2018 Democosm

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

