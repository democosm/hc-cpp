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

#ifndef _TLSCLIENT_HH_
#define _TLSCLIENT_HH_

#include "device.hh"
#include "mutex.hh"
#include "thread.hh"
#include <inttypes.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

class TLSClient : public Device
{
public:
  TLSClient(uint16_t port, const char *srvipaddr, uint16_t srvport, const char *authstring);
  virtual ~TLSClient();
  virtual uint32_t Read(void *buf, uint32_t maxlen);
  virtual uint32_t Write(void *buf, uint32_t len);

private:
  void CloseConnection(void);
  SSL *WaitForConnection(void);
  void Authenticate(SSL *ssl);

private:
  Mutex *_mutex;
  uint16_t _port;
  uint32_t _srvipaddr;
  uint16_t _srvport;
  int _connfd;
  SSL_CTX *_sslctx;
  SSL *_ssl;
  uint32_t _authcode;
};

#endif //_TLSCLIENT_HH_