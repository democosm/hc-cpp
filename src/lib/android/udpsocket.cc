// UDP socket
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
#include "udpsocket.hh"
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

UDPSocket::UDPSocket(uint16_t port, const char *bindif)
{
  struct sockaddr_in addr;
  struct ifreq bindreq;
  int optval;

  //Create socket
  if((_socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error creating socket" << "\n";

  //Bind socket to specified port
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if(bind(_socketfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error binding socket" << "\n";

  //Check for desire to bind to an interface
  if((bindif != 0) && (strlen(bindif) != 0))
  {
    //Copy interface name to bind request structure and ensure null termination
    strncpy(bindreq.ifr_name, bindif, IFNAMSIZ);
    bindreq.ifr_name[IFNAMSIZ - 1] = '\0';

    //Bind socket to interface
    if(setsockopt(_socketfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&bindreq, sizeof(bindreq)) < 0)
      cout << __FILE__ << ":" << __LINE__ << " - Error binding socket to interface '" << bindif << "'\n";
  }

  //Set socket to reuseable
  optval = 1;
  if(setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting socket reuse" << "\n";

  //Set TTL
  optval = 10;
  if(setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting TTL" << "\n";
}

UDPSocket::~UDPSocket()
{
  //Close socket
  if(_socketfd != 0)
    close(_socketfd);
}

uint32_t UDPSocket::RecvFrom(void *buf, uint32_t maxlen, uint32_t &srcipaddr, uint16_t &srcport)
{
  struct sockaddr_in src;
  socklen_t srclen;
  ssize_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Read from socket
  srclen = sizeof(src);
  if((retval = recvfrom(_socketfd, buf, maxlen, 0, (struct sockaddr *)&src, &srclen)) <= 0)
  {
    srcipaddr = 0;
    srcport = 0;
    return 0;
  }

  //Return source information and received UDP payload length
  srcipaddr = ntohl(src.sin_addr.s_addr);
  srcport = ntohs(src.sin_port);
  return (uint32_t)retval;
}

uint32_t UDPSocket::SendTo(const void *buf, uint32_t len, uint32_t dstipaddr, uint16_t dstport)
{
  struct sockaddr_in dst;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Populate destination structure
  memset(&dst, 0, sizeof(dst));
  dst.sin_family = AF_INET;
  dst.sin_addr.s_addr = htonl(dstipaddr);
  dst.sin_port = htons(dstport);

  //Write to socket
  if(sendto(_socketfd, buf, len, 0, (struct sockaddr *)&dst, sizeof(dst)) <= 0)
    return 0;

  return len;
}
