// TLS server
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

#pragma once

#include "device.hh"
#include "mutex.hh"
#include "thread.hh"
#include <inttypes.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

class TLSServer : public Device
{
public:
  TLSServer(uint16_t port, const char* certfile, const char* keyfile, uint32_t authcode);
  virtual ~TLSServer();
  virtual uint32_t Read(void* buf, uint32_t maxlen);
  virtual uint32_t Write(const void* buf, uint32_t len);

private:
  void CloseConnection(void);
  SSL* WaitForConnection(void);
  bool Authenticate(SSL* ssl);

private:
  Mutex* _mutex;
  int _listenfd;
  int _connfd;
  SSL_CTX* _sslctx;
  SSL* _ssl;
  uint32_t _authcode;
};
