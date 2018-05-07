// TLS client
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

#include "crc.hh"
#include "tlsclient.hh"
#include <cassert>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <openssl/err.h>

using namespace std;

TLSClient::TLSClient(uint16_t port, const char *srvipaddr, uint16_t srvport, const char *authstring)
: Device()
{
  struct sockaddr_in addr;
  const SSL_METHOD *sslmethod;

  //Assert valid arguments
  assert((srvipaddr != 0) && (authstring != 0));

  //Create mutex
  _mutex = new Mutex();

  //Remember authorization code
  _authcode = CRC32(0, authstring, strlen(authstring));

  //Remember port
  _port = port;

  //Convert server IP address string to integer and check for error
  if(srvipaddr != 0)
    if(inet_pton(AF_INET, srvipaddr, &(addr.sin_addr)) != 1)
      cout << __FILE__ << ":" << __LINE__ << " - Error converting server IP address" << endl;

  //Remember server IP address and port
  _srvipaddr = addr.sin_addr.s_addr;
  _srvport = srvport;

  //Initialize connection socket to indicate not connected
  _connfd = -1;

  //Create SSL context and check for error
  sslmethod = TLS_client_method();
  if((_sslctx = SSL_CTX_new(sslmethod)) == NULL)
    cout << __FILE__ << ":" << __LINE__ << " - Error creating SSL context" << endl;

  //Initialize SSL wrapper to indicate not connected
  _ssl = NULL;
}

TLSClient::~TLSClient()
{
  //Free SSL context
  SSL_CTX_free(_sslctx);

  //Free SSL wrapper
  if(_ssl != NULL)
    SSL_free(_ssl);

  //Close connection socket
  if(_connfd >= 0)
    close(_connfd);

  //Cleanup
  delete _mutex;
}

uint32_t TLSClient::Read(void *buf, uint32_t maxlen)
{
  SSL *ssl;
  ssize_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Wait until connected
  _mutex->Wait();
  ssl = WaitForConnection();
  _mutex->Give();

  //Read from the socket and keep trying until success
  while((retval = SSL_read(ssl, buf, maxlen)) <= 0)
  {
    //Close connection and wait for new connection
    _mutex->Wait();
    CloseConnection();
    ssl = WaitForConnection();
    _mutex->Give();
  }

  return (uint32_t)retval;
}

uint32_t TLSClient::Write(void *buf, uint32_t len)
{
  SSL *ssl;
  ssize_t wlen;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Wait until connected
  _mutex->Wait();
  ssl = WaitForConnection();
  _mutex->Give();

  //Write to the socket and keep trying until success
  while((wlen = SSL_write(ssl, buf, len)) <= 0)
  {
    //Close connection and wait for new connection
    _mutex->Wait();
    CloseConnection();
    ssl = WaitForConnection();
    _mutex->Give();
  }

  return (uint32_t)wlen;
}

void TLSClient::CloseConnection(void)
{
  //Free SSL wrapper
  SSL_free(_ssl);

  //Invalidate SSL wrapper
  _ssl = NULL;

  //Close the connection socket
  close(_connfd);

  //Invalidate connection socket
  _connfd = -1;
}

SSL *TLSClient::WaitForConnection(void)
{
  struct sockaddr_in addr;
  int optval;
  struct sockaddr_in saddr;

  //Check for connection
  if(_ssl != NULL)
    return _ssl;

  //Try forever
  while(true)
  {
    //Create connection socket
    if((_connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      cout << __FILE__ << ":" << __LINE__ << " - Error creating connection socket" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Bind connection socket to specified port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);
    if(bind(_connfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
      close(_connfd);
      cout << __FILE__ << ":" << __LINE__ << " - Error binding socket" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Set connection socket to reuseable
    optval = 1;
    if(setsockopt(_connfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    {
      close(_connfd);
      cout << __FILE__ << ":" << __LINE__ << " - Error setting socket reuse" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Connect to the server
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = _srvipaddr;
    saddr.sin_port = htons(_srvport);
    if(connect(_connfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
      close(_connfd);
      cout << __FILE__ << ":" << __LINE__ << " - Error connecting" << endl;
      ThreadSleep(1000000);
      continue;
    }

    //Create new SSL wrapper around socket
    _ssl = SSL_new(_sslctx);
    SSL_set_fd(_ssl, _connfd);

    //SSL needs to connect and check for error
    if(SSL_connect(_ssl) <= 0)
    {
      ERR_print_errors_fp(stderr);
      CloseConnection();
      ThreadSleep(1000000);
      continue;
    }

    //Authenticate
    Authenticate(_ssl);

    //Connection established
    return _ssl;
  }
}

void TLSClient::Authenticate(SSL *ssl)
{
  uint8_t buf[4];

  //Assert valid arguments
  assert(ssl != 0);

  //Write hash
  buf[0] = (uint8_t)(_authcode >> 24);
  buf[1] = (uint8_t)(_authcode >> 16);
  buf[2] = (uint8_t)(_authcode >> 8);
  buf[3] = (uint8_t)_authcode;
  SSL_write(ssl, buf, 4);
}
