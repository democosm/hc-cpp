// TLS server
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

#include "tlsserver.hh"
#include <cassert>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <openssl/err.h>

using namespace std;

TLSServer::TLSServer(uint16_t port, const char *certfile, const char *keyfile, uint32_t authcode)
: Device()
{
  struct sockaddr_in addr;
  int optval;
  const SSL_METHOD *sslmethod;

  //Assert valid arguments
  assert((certfile != 0) && (keyfile != 0));

  //Create mutex
  _mutex = new Mutex();

  //Remember authorization code
  _authcode = authcode;

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

  //Create SSL context and check for error
  sslmethod = TLSv1_2_server_method();
  if((_sslctx = SSL_CTX_new(sslmethod)) == NULL)
    cout << __FILE__ << ":" << __LINE__ << " - Error creating SSL context" << endl;

  //Set certificate and check for error
  if(SSL_CTX_use_certificate_file(_sslctx, certfile, SSL_FILETYPE_PEM) <= 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error setting SSL certificate" << endl;

  //Set key and check for error
  if(SSL_CTX_use_PrivateKey_file(_sslctx, keyfile, SSL_FILETYPE_PEM) <= 0 )
    cout << __FILE__ << ":" << __LINE__ << " - Error setting SSL private key" << endl;

  //Initialize SSL wrapper to indicate not connected
  _ssl = NULL;
}

TLSServer::~TLSServer()
{
  //Free SSL wrapper
  if(_ssl != NULL)
    SSL_free(_ssl);

  //Free SSL context
  SSL_CTX_free(_sslctx);

  //Close connection socket
  if(_connfd >= 0)
    close(_connfd);

  //Close listening socket
  if(_listenfd >= 0)
    close(_listenfd);

  //Cleanup
  delete _mutex;
}

uint32_t TLSServer::Read(void *buf, uint32_t maxlen)
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

uint32_t TLSServer::Write(void *buf, uint32_t len)
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

void TLSServer::CloseConnection(void)
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

SSL *TLSServer::WaitForConnection(void)
{
  struct sockaddr_in caddr;
  socklen_t caddrsiz;

  //Check for connection
  if(_ssl != NULL)
    return _ssl;

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

    //Create new SSL wrapper around socket
    _ssl = SSL_new(_sslctx);
    SSL_set_fd(_ssl, _connfd);

    //SSL needs to accept and check for error
    if(SSL_accept(_ssl) <= 0)
    {
      ERR_print_errors_fp(stderr);
      CloseConnection();
      ThreadSleep(1000000);
      continue;
    }

    //Authenticate and check for error
    if(!Authenticate(_ssl))
    {
      cout << __FILE__ << ":" << __LINE__ << " - Error authenticating" << endl;
      CloseConnection();
      ThreadSleep(1000000);
      continue;
    }

    //Connection established
    return _ssl;
  }
}

bool TLSServer::Authenticate(SSL *ssl)
{
  uint8_t byte;
  uint32_t hash;
  uint32_t i;

  //Assert valid arguments
  assert(ssl != 0);

  //Wait for hash
  for(i=0, hash=0; i<4; i++)
  {
    //Read a byte and check for error
    if(SSL_read(ssl, &byte, 1) != 1)
      return false;

    //Update hash
    hash |= (uint32_t)byte << (24 - i*8);
  }

  //Compare hash against authorized list
  if(hash == _authcode)
    return true;

  //Not authorized
  return false;
}
