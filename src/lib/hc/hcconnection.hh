// HC connection
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

#ifndef _HCCONNECTION_HH_
#define _HCCONNECTION_HH_

#include "device.hh"
#include "hcboolean.hh"
#include "hcclient.hh"
#include "hccontainer.hh"
#include "hcserver.hh"
#include "hcinteger.hh"
#include "tinyxml2.hh"
#include <string>

using namespace std;
using namespace tinyxml2;

class HCConnection
{
public:
  HCConnection(Device *dev, HCContainer *pcont, const string &contname, uint32_t timeout, const string &sifname="");
  virtual ~HCConnection();
  void ParseServer(XMLElement *pelt, HCContainer *pcont);
  void ParseCont(XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseInteger(XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseIntegerTable(XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseIntegerList(XMLElement *pelt, HCContainer *pcont);
  template <typename T> HCIntegerEnum<T> *ParseIntegerEnum(XMLElement *pelt);
  HCEIDEnum *ParseEIDEnum(XMLElement *pelt);
  template <typename T> void ParseFloatingPoint(XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseFloatingPointTable(XMLElement *pelt, HCContainer *pcont);
  void ParseBool(XMLElement *pelt, HCContainer *pcont);
  void ParseBoolT(XMLElement *pelt, HCContainer *pcont);
  HCBooleanEnum *ParseBoolEnum(XMLElement *pelt);
  void ParseStr(XMLElement *pelt, HCContainer *pcont);
  void ParseStrT(XMLElement *pelt, HCContainer *pcont);
  void ParseStrL(XMLElement *pelt, HCContainer *pcont);
  void ParseCall(XMLElement *pelt, HCContainer *pcont);
  void ParseCallT(XMLElement *pelt, HCContainer *pcont);
  void ParseFile(XMLElement *pelt, HCContainer *pcont);
  template <typename T> bool ParseValue(XMLElement *pelt, const char *name, T &val);

private:
  Device *_dev;
  HCClient *_cli;
  HCContainer *_cont;
};

#endif //_HCCONNECTION_HH_
