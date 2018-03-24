// TCP server
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

#include "tcpserver.hh"
#include <cassert>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

using namespace std;

TCPServer::TCPServer(uint16_t port)
: Device()
{
  struct sockaddr_in addr;
  int optval;

  //Create mutex
  _mutex = new Mutex();

  //Initialize connection socket to indicate not connected
  _connfd = -1;

  //Create the listening socket
  if((_listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error creating listening socket" << endl;

  //Bind listening socket to specified port
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if(bind(_listenfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error binding listening socket" << endl;

  //Set listening socket to reuseable
  optval = 1;
  if(setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting listening socket reuse" << endl;
}

TCPServer::~TCPServer()
{
  //Close connection socket
  if(_connfd >= 0)
    close(_connfd);

  //Close listening socket
  if(_listenfd >= 0)
    close(_listenfd);

  //Cleanup
  delete _mutex;
}

uint32_t TCPServer::Read(void *buf, uint32_t maxlen)
{
  int connfd;
  ssize_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Wait until connected
  _mutex->Wait();
  connfd = WaitForConnection();
  _mutex->Give();

  //Read from the socket and keep trying until success
  while((retval = read(connfd, buf, maxlen)) <= 0)
  {
    //Close connection and wait for new connection
    _mutex->Wait();
    CloseConnection();
    connfd = WaitForConnection();
    _mutex->Give();
  }

  return (uint32_t)retval;
}

uint32_t TCPServer::Write(void *buf, uint32_t len)
{
  int connfd;
  ssize_t wlen;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Wait until connected
  _mutex->Wait();
  connfd = WaitForConnection();
  _mutex->Give();

  //Write to the socket and keep trying until success
  while((wlen = write(connfd, buf, len)) <= 0)
  {
    //Close connection and wait for new connection
    _mutex->Wait();
    CloseConnection();
    connfd = WaitForConnection();
    _mutex->Give();
  }

  return (uint32_t)wlen;
}

void TCPServer::CloseConnection(void)
{
  //Close the connection socket
  close(_connfd);

  //Invalidate connection socket
  _connfd = -1;
}

int TCPServer::WaitForConnection(void)
{
  struct sockaddr_in caddr;
  socklen_t caddrsiz;

  //Check for connection
  if(_connfd >= 0)
    return _connfd;

  //Try forever
  while(true)
  {
    //Listen
    if(listen(_listenfd, 1024) != 0)
    {
      cout << __FILE__ << ":" << __LINE__ << " - Error listening" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Accept
    caddrsiz = sizeof(caddr);
    if((_connfd = accept(_listenfd, (struct sockaddr *)&caddr, &caddrsiz)) < 0)
    {
      cout << __FILE__ << ":" << __LINE__ << " - Error accepting" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Connection established
    return _connfd;
  }
}
