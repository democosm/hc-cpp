// UDP socket
//
// Copyright 2018 Democosm
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

#include "udpsocket.hh"
#include "error.hh"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>

using namespace std;

UDPSocket::UDPSocket(uint16_t port, const char *destipaddr, uint16_t destport)
{
  struct sockaddr_in addr;
  int optval;

  //Create the mutex
  _mutex = new Mutex();

  //Create the socket
  if((_socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error creating socket" << endl;

  //Bind socket to specified port
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if(bind(_socketfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error binding socket" << endl;

  //Set socket to reuseable
  optval = 1;
  if(setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting socket reuse" << endl;

  //Set TTL
  optval = 10;
  if(setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting TTL" << endl;

  //Zero destination information
  memset(&_dest, 0, sizeof(_dest));

  //Set address family to IPv4
  _dest.sin_family = AF_INET;

  //Convert destination IP address string to integer and check for error
  if(destipaddr != 0)
    if(inet_pton(AF_INET, destipaddr, &(_dest.sin_addr)) != 1)
      cout << __FILE__ << ":" << __LINE__ << " - Error converting destination IP address" << endl;

  //Set destination port
  _dest.sin_port = htons(destport);

  //Set destination information on read if zero passed in for dest IP address or port
  _setdestonread = ((destipaddr == 0) || (destport == 0));
}

UDPSocket::~UDPSocket()
{
  //Close the socket
  if(_socketfd != 0)
    close(_socketfd);

  //Destroy the mutex
  delete _mutex;
}

uint32_t UDPSocket::Read(void *buf, uint32_t maxlen)
{
  struct sockaddr_in source;
  socklen_t sourcelen;
  ssize_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Read from the socket
  sourcelen = sizeof(source);
  if((retval = recvfrom(_socketfd, buf, maxlen, 0, (struct sockaddr *)&source, &sourcelen)) <= 0)
    return 0;

  //Begin mutual exclusion
  _mutex->Wait();

  //Check for destination information to be set to source of received packet
  if(_setdestonread)
  {
    //Set destination to source of incoming datagram
    memcpy(&_dest, &source, sourcelen);
  }

  //End mutual exclusion
  _mutex->Give();

  return (uint32_t)retval;
}

uint32_t UDPSocket::Write(const void *buf, uint32_t len)
{
  struct sockaddr_in dest;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Begin mutual exclusion
  _mutex->Wait();

  //Copy destination information to local buffer
  memcpy(&dest, &_dest, sizeof(_dest));

  //End mutual exclusion
  _mutex->Give();

  //Write to the socket
  if(sendto(_socketfd, buf, len, 0, (struct sockaddr *)&dest, sizeof(dest)) <= 0)
    return 0;

  return len;
}
