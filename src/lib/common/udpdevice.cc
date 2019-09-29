// UDP device
//
// Copyright 2019 Democosm
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "error.hh"
#include "udpdevice.hh"
#include <arpa/inet.h>
#include <cassert>
#include <iostream>
#include <netdb.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

UDPDevice::UDPDevice(uint16_t port, const char *bindif, const char *dstipaddr, uint16_t dstport)
{
  //Create socket
  _sock = new UDPSocket(port, bindif);

  //Create mutex
  _mutex = new Mutex();

  //Check for null pointer passed in for destination IP address string
  if(dstipaddr == 0)
  {
    _dstipaddr = 0;
  }
  else
  {
    //Convert destination IP address string to integer and check for error
    if(inet_pton(AF_INET, dstipaddr, &_dstipaddr) != 1)
      cout << __FILE__ << ":" << __LINE__ << " - Error converting destination IP address" << "\n";

    //Convert destination IP address to host byte order
    _dstipaddr = ntohl(_dstipaddr);
  }

  //Set destination port
  _dstport = dstport;

  //Set destination information on read if zero passed in for IP address or port
  _setdstonread = ((_dstipaddr == 0) || (_dstport == 0));
}

UDPDevice::~UDPDevice()
{
  //Cleanup
  delete _mutex;
  delete _sock;
}

uint32_t UDPDevice::Read(void *buf, uint32_t maxlen)
{
  uint32_t srcipaddr;
  uint16_t srcport;
  uint32_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Read from socket
  if((retval = _sock->RecvFrom(buf, maxlen, srcipaddr, srcport)) == 0)
    return 0;

  //Begin mutual exclusion
  _mutex->Wait();

  //Check for destination information to be set to source of received packet
  if(_setdstonread)
  {
    //Set destination to source of incoming datagram
    _dstipaddr = srcipaddr;
    _dstport = srcport;
  }

  //End mutual exclusion
  _mutex->Give();

  return retval;
}

uint32_t UDPDevice::Write(const void *buf, uint32_t len)
{
  uint32_t dstipaddr;
  uint16_t dstport;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Begin mutual exclusion
  _mutex->Wait();

  //Copy destination information to local buffer
  dstipaddr = _dstipaddr;
  dstport = _dstport;

  //End mutual exclusion
  _mutex->Give();

  //Write to socket
  return _sock->SendTo(buf, len, dstipaddr, dstport);
}
