// HC aggregator
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

#ifndef _HCAGGREGATOR_HH_
#define _HCAGGREGATOR_HH_

#include "hcconnection.hh"
#include "hccontainer.hh"
#include "hcserver.hh"
#include "slipframer.hh"
#include "tlsclient.hh"
#include "tcpclient.hh"
#include "tinyxml2.hh"
#include "udpsocket.hh"
#include <string>

using namespace std;
using namespace tinyxml2;

class HCAggregator
{
public:
  HCAggregator(const string &filename);
  virtual ~HCAggregator();
  HCContainer *GetTopCont(void);
  void AddParamsToServer(HCContainer *startcont);
  HCServer *ParseServer(XMLElement *pelt);
  HCConnection *ParseConn(XMLElement *pelt);
  UDPSocket *ParseUDPSocket(XMLElement *pelt);
  SLIPFramer *ParseSLIPFramer(XMLElement *pelt);
  TCPClient *ParseTCPClient(XMLElement *pelt);
  TLSClient *ParseTLSClient(XMLElement *pelt);
  bool ParseValue(XMLElement *pelt, const char *name, string &val);
  bool ParseValue(XMLElement *pelt, const char *name, uint16_t &val);
  bool ParseValue(XMLElement *pelt, const char *name, uint32_t &val);

private:
  HCContainer *_topcont;
  HCConnection **_conn;
  uint32_t _conncnt;
  UDPSocket *_srvdev;
  HCServer *_srv;
};

#endif //_HCAGGREGATOR_HH_
