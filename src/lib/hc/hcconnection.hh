// HC connection
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

class HCConnection
{
public:
  HCConnection(Device *dev, HCContainer *pcont, const std::string &contname, uint32_t timeout, const std::string &sifname="");
  virtual ~HCConnection();

private:
  void ParseServer(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseCont(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseCall(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseCallT(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseBool(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseBoolT(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  HCBooleanEnum *ParseBoolEnum(tinyxml2::XMLElement *pelt);
  void ParseStr(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseStrT(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseStrL(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  void ParseFile(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseInt(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseIntT(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseIntL(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseIntA(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> HCIntegerEnum<T> *ParseIntEnum(tinyxml2::XMLElement *pelt);
  HCEIDEnum *ParseEIDEnum(tinyxml2::XMLElement *pelt);
  template <typename T> void ParseFloat(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseFloatTable(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseVec2(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseVec2T(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseVec3(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> void ParseVec3T(tinyxml2::XMLElement *pelt, HCContainer *pcont);
  template <typename T> bool ParseValue(tinyxml2::XMLElement *pelt, const char *name, T &val);

private:
  Device *_dev;
  HCClient *_cli;
  HCContainer *_cont;
};

#endif
