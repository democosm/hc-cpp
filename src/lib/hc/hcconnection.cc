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

#include "crc.hh"
#include "hcboolean.hh"
#include "hccall.hh"
#include "hcconnection.hh"
#include "hcfile.hh"
#include "hcfloat.hh"
#include "hcinteger.hh"
#include "hcmessage.hh"
#include "hcstring.hh"
#include "hcvector.hh"
#include "str.hh"
#include <cassert>
#include <iostream>

using namespace std;
using namespace tinyxml2;

HCConnection::HCConnection(Device *dev, HCContainer *pcont, const string &contname, uint32_t timeout, const string &sifname)
{
  XMLDocument doc;
  string srvname;
  string srvvers;
  uint32_t srvinfocrc;
  string lsifname;
  uint32_t lsifcrc;
  int ierr;

  //Assert valid arguments
  assert((dev != 0) && (pcont != 0));

  //Remember device
  _dev = dev;

  //Create container and add to parent container
  _cont = new HCContainer(contname);
  pcont->Add(_cont);

  //Create client
  _cli = new HCClient(_dev, _cont, timeout);

  //Get server name and check for error
  if((ierr = _cli->Get(HCServer::PID_NAME, srvname)) != ERR_NONE)
  {
    cout << "Error getting server name (" << ErrToString(ierr) << ')' << "\n";
    return;
  }

  //Print info
  cout << "Server name: " << srvname << "\n";

  //Get server version and check for error
  if((ierr = _cli->Get(HCServer::PID_VERSION, srvvers)) != ERR_NONE)
  {
    cout << "Error getting server version (" << ErrToString(ierr) << ')' << "\n";
    return;
  }

  //Print info
  cout << "Server version: " << srvvers << "\n";

  //Get server information file CRC and check for error
  if((ierr = _cli->Get(HCServer::PID_INFOFILECRC, srvinfocrc)) != ERR_NONE)
  {
    cout << "Error getting server information file CRC (" << ErrToString(ierr) << ')' << "\n";
    return;
  }

  //Print info
  cout << "Server information file CRC: " << srvinfocrc << "\n";

  //Check for no server information file name specified
  if(sifname == "")
  {
    //Create server information file name
    lsifname = ".client-";
    lsifname += srvname;
    lsifname += ".xml";
  }
  else
  {
    //Just copy passed server information file name to local variable
    lsifname = sifname;
  }

  //Calculate CRC of local server information file
  lsifcrc = CRC32File(lsifname.c_str());

  //Print info
  cout << "Local server information file CRC: " << lsifcrc << "\n";

  //Check for difference in CRCs
  if(lsifcrc != srvinfocrc)
  {
    //Print info
    cout << "Downloading server information file" << "\n";

    //Get server information file from server and check for error
    if((ierr = _cli->DownloadSIF(HCServer::PID_INFOFILE, lsifname.c_str())) != ERR_NONE)
    {
      cout << "Error getting server information file (" << ErrToString(ierr) << ')' << "\n";
      return;
    }
  }

  //Parse file and check for error
  if((doc.LoadFile(lsifname.c_str())) != 0)
  {
    cout << "Error parsing file (" << lsifname << ')' << "\n";
    return;
  }

  //Parse server from DOM
  ParseServer(doc.FirstChildElement("server"), _cont);
}

HCConnection::~HCConnection()
{
  //Cleanup
  delete _cli;
  delete _dev;
}

void HCConnection::ParseServer(XMLElement *pelt, HCContainer *pcont)
{
  XMLElement *elt;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Loop through all children
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "cont") == 0)
      ParseCont(elt, pcont);
    else if(strcmp(elt->Name(), "call") == 0)
      ParseCall(elt, pcont);
    else if(strcmp(elt->Name(), "callt") == 0)
      ParseCallT(elt, pcont);
    else if(strcmp(elt->Name(), "bool") == 0)
      ParseBool(elt, pcont);
    else if(strcmp(elt->Name(), "boolt") == 0)
      ParseBoolT(elt, pcont);
    else if(strcmp(elt->Name(), "str") == 0)
      ParseStr(elt, pcont);
    else if(strcmp(elt->Name(), "strt") == 0)
      ParseStrT(elt, pcont);
    else if(strcmp(elt->Name(), "strl") == 0)
      ParseStrL(elt, pcont);
    else if(strcmp(elt->Name(), "file") == 0)
      ParseFile(elt, pcont);
    else if(strcmp(elt->Name(), "i8") == 0)
      ParseInt<int8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i8t") == 0)
      ParseIntT<int8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i8l") == 0)
      ParseIntL<int8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i8a") == 0)
      ParseIntA<int8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i16") == 0)
      ParseInt<int16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i16t") == 0)
      ParseIntT<int16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i16l") == 0)
      ParseIntL<int16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i16a") == 0)
      ParseIntA<int16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i32") == 0)
      ParseInt<int32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i32t") == 0)
      ParseIntT<int32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i32l") == 0)
      ParseIntL<int32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i32a") == 0)
      ParseIntA<int32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i64") == 0)
      ParseInt<int64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i64t") == 0)
      ParseIntT<int64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i64l") == 0)
      ParseIntL<int64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "i64a") == 0)
      ParseIntA<int64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u8") == 0)
      ParseInt<uint8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u8t") == 0)
      ParseIntT<uint8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u8l") == 0)
      ParseIntL<uint8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u8a") == 0)
      ParseIntA<uint8_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u16") == 0)
      ParseInt<uint16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u16t") == 0)
      ParseIntT<uint16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u16l") == 0)
      ParseIntL<uint16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u16a") == 0)
      ParseIntA<uint16_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u32") == 0)
      ParseInt<uint32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u32t") == 0)
      ParseIntT<uint32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u32l") == 0)
      ParseIntL<uint32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u32a") == 0)
      ParseIntA<uint32_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u64") == 0)
      ParseInt<uint64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u64t") == 0)
      ParseIntT<uint64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u64l") == 0)
      ParseIntL<uint64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "u64a") == 0)
      ParseIntA<uint64_t>(elt, pcont);
    else if(strcmp(elt->Name(), "f32") == 0)
      ParseFloat<float>(elt, pcont);
    else if(strcmp(elt->Name(), "f32t") == 0)
      ParseFloatTable<float>(elt, pcont);
    else if(strcmp(elt->Name(), "f64") == 0)
      ParseFloat<double>(elt, pcont);
    else if(strcmp(elt->Name(), "f64t") == 0)
      ParseFloatTable<double>(elt, pcont);
    else if(strcmp(elt->Name(), "v2f32") == 0)
      ParseVec2<float>(elt, pcont);
    else if(strcmp(elt->Name(), "v2f32t") == 0)
      ParseVec2T<float>(elt, pcont);
    else if(strcmp(elt->Name(), "v2f64") == 0)
      ParseVec2<double>(elt, pcont);
    else if(strcmp(elt->Name(), "v2f64t") == 0)
      ParseVec2T<double>(elt, pcont);
    else if(strcmp(elt->Name(), "v3f32") == 0)
      ParseVec3<float>(elt, pcont);
    else if(strcmp(elt->Name(), "v3f32t") == 0)
      ParseVec3T<float>(elt, pcont);
    else if(strcmp(elt->Name(), "v3f64") == 0)
      ParseVec3<double>(elt, pcont);
    else if(strcmp(elt->Name(), "v3f64t") == 0)
      ParseVec3T<double>(elt, pcont);
  }
}

void HCConnection::ParseCont(XMLElement *pelt, HCContainer *pcont)
{
  string name;
  XMLElement *elt;
  HCContainer *cont;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Create container and add to parent
  cont = new HCContainer(name);
  pcont->Add(cont);

  //Loop through all children
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "cont") == 0)
      ParseCont(elt, cont);
    else if(strcmp(elt->Name(), "call") == 0)
      ParseCall(elt, cont);
    else if(strcmp(elt->Name(), "callt") == 0)
      ParseCallT(elt, cont);
    else if(strcmp(elt->Name(), "bool") == 0)
      ParseBool(elt, cont);
    else if(strcmp(elt->Name(), "boolt") == 0)
      ParseBoolT(elt, cont);
    else if(strcmp(elt->Name(), "str") == 0)
      ParseStr(elt, cont);
    else if(strcmp(elt->Name(), "strt") == 0)
      ParseStrT(elt, cont);
    else if(strcmp(elt->Name(), "strl") == 0)
      ParseStrL(elt, cont);
    else if(strcmp(elt->Name(), "file") == 0)
      ParseFile(elt, cont);
    else if(strcmp(elt->Name(), "i8") == 0)
      ParseInt<int8_t>(elt, cont);
    else if(strcmp(elt->Name(), "i8t") == 0)
      ParseIntT<int8_t>(elt, cont);
    else if(strcmp(elt->Name(), "i8l") == 0)
      ParseIntL<int8_t>(elt, cont);
    else if(strcmp(elt->Name(), "i8a") == 0)
      ParseIntA<int8_t>(elt, cont);
    else if(strcmp(elt->Name(), "i16") == 0)
      ParseInt<int16_t>(elt, cont);
    else if(strcmp(elt->Name(), "i16t") == 0)
      ParseIntT<int16_t>(elt, cont);
    else if(strcmp(elt->Name(), "i16l") == 0)
      ParseIntL<int16_t>(elt, cont);
    else if(strcmp(elt->Name(), "i16a") == 0)
      ParseIntA<int16_t>(elt, cont);
    else if(strcmp(elt->Name(), "i32") == 0)
      ParseInt<int32_t>(elt, cont);
    else if(strcmp(elt->Name(), "i32t") == 0)
      ParseIntT<int32_t>(elt, cont);
    else if(strcmp(elt->Name(), "i32l") == 0)
      ParseIntL<int32_t>(elt, cont);
    else if(strcmp(elt->Name(), "i32a") == 0)
      ParseIntA<int32_t>(elt, cont);
    else if(strcmp(elt->Name(), "i64") == 0)
      ParseInt<int64_t>(elt, cont);
    else if(strcmp(elt->Name(), "i64t") == 0)
      ParseIntT<int64_t>(elt, cont);
    else if(strcmp(elt->Name(), "i64l") == 0)
      ParseIntL<int64_t>(elt, cont);
    else if(strcmp(elt->Name(), "i64a") == 0)
      ParseIntA<int64_t>(elt, cont);
    else if(strcmp(elt->Name(), "u8") == 0)
      ParseInt<uint8_t>(elt, cont);
    else if(strcmp(elt->Name(), "u8t") == 0)
      ParseIntT<uint8_t>(elt, cont);
    else if(strcmp(elt->Name(), "u8l") == 0)
      ParseIntL<uint8_t>(elt, cont);
    else if(strcmp(elt->Name(), "u8a") == 0)
      ParseIntA<uint8_t>(elt, cont);
    else if(strcmp(elt->Name(), "u16") == 0)
      ParseInt<uint16_t>(elt, cont);
    else if(strcmp(elt->Name(), "u16t") == 0)
      ParseIntT<uint16_t>(elt, cont);
    else if(strcmp(elt->Name(), "u16l") == 0)
      ParseIntL<uint16_t>(elt, cont);
    else if(strcmp(elt->Name(), "u16a") == 0)
      ParseIntA<uint16_t>(elt, cont);
    else if(strcmp(elt->Name(), "u32") == 0)
      ParseInt<uint32_t>(elt, cont);
    else if(strcmp(elt->Name(), "u32t") == 0)
      ParseIntT<uint32_t>(elt, cont);
    else if(strcmp(elt->Name(), "u32l") == 0)
      ParseIntL<uint32_t>(elt, cont);
    else if(strcmp(elt->Name(), "u32a") == 0)
      ParseIntA<uint32_t>(elt, cont);
    else if(strcmp(elt->Name(), "u64") == 0)
      ParseInt<uint64_t>(elt, cont);
    else if(strcmp(elt->Name(), "u64t") == 0)
      ParseIntT<uint64_t>(elt, cont);
    else if(strcmp(elt->Name(), "u64l") == 0)
      ParseIntL<uint64_t>(elt, cont);
    else if(strcmp(elt->Name(), "u64a") == 0)
      ParseIntA<uint64_t>(elt, cont);
    else if(strcmp(elt->Name(), "f32") == 0)
      ParseFloat<float>(elt, cont);
    else if(strcmp(elt->Name(), "f32t") == 0)
      ParseFloatTable<float>(elt, cont);
    else if(strcmp(elt->Name(), "f64") == 0)
      ParseFloat<double>(elt, cont);
    else if(strcmp(elt->Name(), "f64t") == 0)
      ParseFloatTable<double>(elt, cont);
    else if(strcmp(elt->Name(), "v2f32") == 0)
      ParseVec2<float>(elt, cont);
    else if(strcmp(elt->Name(), "v2f32t") == 0)
      ParseVec2T<float>(elt, cont);
    else if(strcmp(elt->Name(), "v2f64") == 0)
      ParseVec2<double>(elt, cont);
    else if(strcmp(elt->Name(), "v2f64t") == 0)
      ParseVec2T<double>(elt, cont);
    else if(strcmp(elt->Name(), "v3f32") == 0)
      ParseVec3<float>(elt, cont);
    else if(strcmp(elt->Name(), "v3f32t") == 0)
      ParseVec3T<float>(elt, cont);
    else if(strcmp(elt->Name(), "v3f64") == 0)
      ParseVec3<double>(elt, cont);
    else if(strcmp(elt->Name(), "v3f64t") == 0)
      ParseVec3T<double>(elt, cont);
  }
}

void HCConnection::ParseCall(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  HCCallCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Create client stub
  stub = new HCCallCli(_cli, pid);

  //Create parameter
  param = new HCCall<HCCallCli>(name, stub, &HCCallCli::Call);

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseCallT(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  uint32_t size;
  XMLElement *elt;
  HCEIDEnum *eidenums;
  HCCallCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCCallCli(_cli, pid);

  //Create parameter
  param = new HCCallTable<HCCallCli>(name, stub, &HCCallCli::ICall, size, eidenums);

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseBool(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  XMLElement *elt;
  HCBooleanEnum *valenums;
  HCBooleanCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Loop through all children looking for enums
  valenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "valenums") == 0)
      valenums = ParseBoolEnum(elt);
  }

  //Create client stub
  stub = new HCBooleanCli(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCBooleanS<HCBooleanCli>(name, stub, &HCBooleanCli::Get, &HCBooleanCli::Set, valenums);
    else if(acc == "R")
      param = new HCBooleanS<HCBooleanCli>(name, stub, &HCBooleanCli::Get, 0, valenums);
    else if(acc == "W")
      param = new HCBooleanS<HCBooleanCli>(name, stub, 0, &HCBooleanCli::Set, valenums);
    else
      param = new HCBooleanS<HCBooleanCli>(name, stub, 0, 0, valenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCBoolean<HCBooleanCli>(name, stub, &HCBooleanCli::Get, &HCBooleanCli::Set, valenums);
    else if(acc == "R")
      param = new HCBoolean<HCBooleanCli>(name, stub, &HCBooleanCli::Get, 0, valenums);
    else if(acc == "W")
      param = new HCBoolean<HCBooleanCli>(name, stub, 0, &HCBooleanCli::Set, valenums);
    else
      param = new HCBoolean<HCBooleanCli>(name, stub, 0, 0, valenums);
  }

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseBoolT(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  uint32_t size;
  XMLElement *elt;
  HCBooleanEnum *valenums;
  HCEIDEnum *eidenums;
  HCBooleanCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  valenums = 0;
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "valenums") == 0)
      valenums = ParseBoolEnum(elt);
    else if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCBooleanCli(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCBooleanTableS<HCBooleanCli>(name, stub, &HCBooleanCli::IGet, &HCBooleanCli::ISet, size, eidenums, valenums);
    else if(acc == "R")
      param = new HCBooleanTableS<HCBooleanCli>(name, stub, &HCBooleanCli::IGet, 0, size, eidenums, valenums);
    else if(acc == "W")
      param = new HCBooleanTableS<HCBooleanCli>(name, stub, 0, &HCBooleanCli::ISet, size, eidenums, valenums);
    else
      param = new HCBooleanTableS<HCBooleanCli>(name, stub, 0, 0, size, eidenums, valenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCBooleanTable<HCBooleanCli>(name, stub, &HCBooleanCli::IGet, &HCBooleanCli::ISet, size, eidenums, valenums);
    else if(acc == "R")
      param = new HCBooleanTable<HCBooleanCli>(name, stub, &HCBooleanCli::IGet, 0, size, eidenums, valenums);
    else if(acc == "W")
      param = new HCBooleanTable<HCBooleanCli>(name, stub, 0, &HCBooleanCli::ISet, size, eidenums, valenums);
    else
      param = new HCBooleanTable<HCBooleanCli>(name, stub, 0, 0, size, eidenums, valenums);
  }

  //Add to parent
  pcont->Add(param);
}

HCBooleanEnum *HCConnection::ParseBoolEnum(XMLElement *pelt)
{
  XMLElement *elt;
  uint32_t enumcnt;
  HCBooleanEnum *enums;
  string enumnumstr;
  string enumstr;
  size_t seppos;

  //Check for null parent objects
  if(pelt == 0)
    return 0;

  //Count number of enumerations
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
    if(strcmp(elt->Name(), "eq") == 0)
      enumcnt++;

  //Check for no enumerations
  if(enumcnt == 0)
    return 0;

  //Create storage for enumerations
  enums = new HCBooleanEnum[enumcnt + 1];

  //Loop through all children and gather information
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check for equality element node
    if(strcmp(elt->Name(), "eq") == 0)
    {
      //Get enumeration number string and check for error
      enumstr = elt->GetText();
      if((seppos = enumstr.find(',')) == string::npos)
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Delimiter not found in equality (" << enumstr << ")\n";
        return enums;
      }
      enumnumstr = enumstr.substr(0, seppos);
      enumstr = enumstr.substr(seppos+1, enumstr.length()-seppos);

      //Convert number and check for error
      if(!StringConvert(enumnumstr.c_str(), enums[enumcnt]._num))
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Error converting enumeration number (" << enumnumstr << ")\n";
        return enums;
      }

      //Get enumeration string
      enums[enumcnt]._str = enumstr;

      //Advance to next index into enumeration array
      enumcnt++;
    }
  }

  return enums;
}

void HCConnection::ParseStr(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  HCStringCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Create client stub
  stub = new HCStringCli(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCStringS<HCStringCli>(name, stub, &HCStringCli::Get, &HCStringCli::Set);
    else if(acc == "R")
      param = new HCStringS<HCStringCli>(name, stub, &HCStringCli::Get, 0);
    else if(acc == "W")
      param = new HCStringS<HCStringCli>(name, stub, 0, &HCStringCli::Set);
    else
      param = new HCStringS<HCStringCli>(name, stub, 0, 0);
  }
  else
  {
    if(acc == "RW")
      param = new HCString<HCStringCli>(name, stub, &HCStringCli::Get, &HCStringCli::Set);
    else if(acc == "R")
      param = new HCString<HCStringCli>(name, stub, &HCStringCli::Get, 0);
    else if(acc == "W")
      param = new HCString<HCStringCli>(name, stub, 0, &HCStringCli::Set);
    else
      param = new HCString<HCStringCli>(name, stub, 0, 0);
  }

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseStrT(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  uint32_t size;
  XMLElement *elt;
  HCEIDEnum *eidenums;
  HCStringCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCStringCli(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCStringTableS<HCStringCli>(name, stub, &HCStringCli::IGet, &HCStringCli::ISet, size, eidenums);
    else if(acc == "R")
      param = new HCStringTableS<HCStringCli>(name, stub, &HCStringCli::IGet, 0, size, eidenums);
    else if(acc == "W")
      param = new HCStringTableS<HCStringCli>(name, stub, 0, &HCStringCli::ISet, size, eidenums);
    else
      param = new HCStringTableS<HCStringCli>(name, stub, 0, 0, size, eidenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCStringTable<HCStringCli>(name, stub, &HCStringCli::IGet, &HCStringCli::ISet, size, eidenums);
    else if(acc == "R")
      param = new HCStringTable<HCStringCli>(name, stub, &HCStringCli::IGet, 0, size, eidenums);
    else if(acc == "W")
      param = new HCStringTable<HCStringCli>(name, stub, 0, &HCStringCli::ISet, size, eidenums);
    else
      param = new HCStringTable<HCStringCli>(name, stub, 0, 0, size, eidenums);
  }

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseStrL(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  uint32_t maxsize;
  HCStringCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse maxsize element and check for error
  if(!ParseValue(pelt, "maxsize", maxsize))
    return;

  //Create client stub
  stub = new HCStringCli(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCStringListS<HCStringCli>(name, stub, &HCStringCli::IGet, &HCStringCli::Add, &HCStringCli::Sub, maxsize);
    else if(acc == "R")
      param = new HCStringListS<HCStringCli>(name, stub, &HCStringCli::IGet, 0, 0, maxsize);
    else if(acc == "W")
      param = new HCStringListS<HCStringCli>(name, stub, 0, &HCStringCli::Add, &HCStringCli::Sub, maxsize);
    else
      param = new HCStringListS<HCStringCli>(name, stub, 0, 0, 0, maxsize);
  }
  else
  {
    if(acc == "RW")
      param = new HCStringList<HCStringCli>(name, stub, &HCStringCli::IGet, &HCStringCli::Add, &HCStringCli::Sub, maxsize);
    else if(acc == "R")
      param = new HCStringList<HCStringCli>(name, stub, &HCStringCli::IGet, 0, 0, maxsize);
    else if(acc == "W")
      param = new HCStringList<HCStringCli>(name, stub, 0, &HCStringCli::Add, &HCStringCli::Sub, maxsize);
    else
      param = new HCStringList<HCStringCli>(name, stub, 0, 0, 0, maxsize);
  }

  //Add to parent
  pcont->Add(param);
}

void HCConnection::ParseFile(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  HCFileCli *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Create client stub
  stub = new HCFileCli(_cli, pid);

  //Create parameter
  if(acc == "RW")
    param = new HCFile<HCFileCli>(name, stub, &HCFileCli::Read, &HCFileCli::Write);
  else if(acc == "R")
    param = new HCFile<HCFileCli>(name, stub, &HCFileCli::Read, 0);
  else if(acc == "W")
    param = new HCFile<HCFileCli>(name, stub, 0, &HCFileCli::Write);
  else
    param = new HCFile<HCFileCli>(name, stub, 0, 0);

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseInt(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  XMLElement *elt;
  HCIntegerEnum<T> *valenums;
  HCIntegerCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Loop through all children looking for enums
  valenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "valenums") == 0)
      valenums = ParseIntEnum<T>(elt);
  }

  //Create client stub
  stub = new HCIntegerCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCIntegerS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, &HCIntegerCli<T>::Set, valenums);
    else if(acc == "R")
      param = new HCIntegerS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, 0, valenums);
    else if(acc == "W")
      param = new HCIntegerS<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Set, valenums);
    else
      param = new HCIntegerS<HCIntegerCli<T>, T>(name, stub, 0, 0, valenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCInteger<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, &HCIntegerCli<T>::Set, valenums);
    else if(acc == "R")
      param = new HCInteger<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, 0, valenums);
    else if(acc == "W")
      param = new HCInteger<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Set, valenums);
    else
      param = new HCInteger<HCIntegerCli<T>, T>(name, stub, 0, 0, valenums);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseIntT(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  uint32_t size;
  XMLElement *elt;
  HCIntegerEnum<T> *valenums;
  HCEIDEnum *eidenums;
  HCIntegerCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  valenums = 0;
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "valenums") == 0)
      valenums = ParseIntEnum<T>(elt);
    else if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCIntegerCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCIntegerTableS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, &HCIntegerCli<T>::ISet, size, eidenums, valenums);
    else if(acc == "R")
      param = new HCIntegerTableS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, 0, size, eidenums, valenums);
    else if(acc == "W")
      param = new HCIntegerTableS<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::ISet, size, eidenums, valenums);
    else
      param = new HCIntegerTableS<HCIntegerCli<T>, T>(name, stub, 0, 0, size, eidenums, valenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCIntegerTable<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, &HCIntegerCli<T>::ISet, size, eidenums, valenums);
    else if(acc == "R")
      param = new HCIntegerTable<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, 0, size, eidenums, valenums);
    else if(acc == "W")
      param = new HCIntegerTable<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::ISet, size, eidenums, valenums);
    else
      param = new HCIntegerTable<HCIntegerCli<T>, T>(name, stub, 0, 0, size, eidenums, valenums);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseIntL(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  uint32_t maxsize;
  XMLElement *elt;
  HCIntegerEnum<T> *valenums;
  HCIntegerCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse maxsize element and check for error
  if(!ParseValue(pelt, "maxsize", maxsize))
    return;

  //Loop through all children looking for enums
  valenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "valenums") == 0)
      valenums = ParseIntEnum<T>(elt);
  }

  //Create client stub
  stub = new HCIntegerCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCIntegerListS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, &HCIntegerCli<T>::Add, &HCIntegerCli<T>::Sub, maxsize, valenums);
    else if(acc == "R")
      param = new HCIntegerListS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, 0, 0, maxsize, valenums);
    else if(acc == "W")
      param = new HCIntegerListS<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Add, &HCIntegerCli<T>::Sub, maxsize, valenums);
    else
      param = new HCIntegerListS<HCIntegerCli<T>, T>(name, stub, 0, 0, 0, maxsize, valenums);
  }
  else
  {
    if(acc == "RW")
      param = new HCIntegerList<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, &HCIntegerCli<T>::Add, &HCIntegerCli<T>::Sub, maxsize, valenums);
    else if(acc == "R")
      param = new HCIntegerList<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::IGet, 0, 0, maxsize, valenums);
    else if(acc == "W")
      param = new HCIntegerList<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Add, &HCIntegerCli<T>::Sub, maxsize, valenums);
    else
      param = new HCIntegerList<HCIntegerCli<T>, T>(name, stub, 0, 0, 0, maxsize, valenums);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseIntA(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  HCIntegerCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Create client stub
  stub = new HCIntegerCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCIntegerArrayS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, &HCIntegerCli<T>::Set);
    else if(acc == "R")
      param = new HCIntegerArrayS<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, 0);
    else if(acc == "W")
      param = new HCIntegerArrayS<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Set);
    else
      param = new HCIntegerArrayS<HCIntegerCli<T>, T>(name, stub, 0, 0);
  }
  else
  {
    if(acc == "RW")
      param = new HCIntegerArray<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, &HCIntegerCli<T>::Set);
    else if(acc == "R")
      param = new HCIntegerArray<HCIntegerCli<T>, T>(name, stub, &HCIntegerCli<T>::Get, 0);
    else if(acc == "W")
      param = new HCIntegerArray<HCIntegerCli<T>, T>(name, stub, 0, &HCIntegerCli<T>::Set);
    else
      param = new HCIntegerArray<HCIntegerCli<T>, T>(name, stub, 0, 0);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> HCIntegerEnum<T> *HCConnection::ParseIntEnum(XMLElement *pelt)
{
  XMLElement *elt;
  uint32_t enumcnt;
  HCIntegerEnum<T> *enums;
  string enumnumstr;
  string enumstr;
  size_t seppos;

  //Check for null parent objects
  if(pelt == 0)
    return 0;

  //Count number of enumerations
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
    if(strcmp(elt->Name(), "eq") == 0)
      enumcnt++;

  //Check for no enumerations
  if(enumcnt == 0)
    return 0;

  //Create storage for enumerations
  enums = new HCIntegerEnum<T>[enumcnt + 1];

  //Loop through all children and gather information
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check for equality element node
    if(strcmp(elt->Name(), "eq") == 0)
    {
      //Get enumeration number string and check for error
      enumstr = elt->GetText();
      if((seppos = enumstr.find(',')) == string::npos)
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Delimiter not found in equality (" << enumstr << ")\n";
        return enums;
      }
      enumnumstr = enumstr.substr(0, seppos);
      enumstr = enumstr.substr(seppos+1, enumstr.length()-seppos);

      //Convert number and check for error
      if(!StringConvert(enumnumstr.c_str(), enums[enumcnt]._num))
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Error converting enumeration number (" << enumnumstr << ")\n";
        return enums;
      }

      //Get enumeration string
      enums[enumcnt]._str = enumstr;

      //Advance to next index into enumeration array
      enumcnt++;
    }
  }

  return enums;
}

HCEIDEnum *HCConnection::ParseEIDEnum(XMLElement *pelt)
{
  XMLElement *elt;
  uint32_t enumcnt;
  HCEIDEnum *enums;
  string enumnumstr;
  string enumstr;
  size_t seppos;

  //Check for null parent objects
  if(pelt == 0)
    return 0;

  //Count number of enumerations
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
    if(strcmp(elt->Name(), "eq") == 0)
      enumcnt++;

  //Check for no enumerations
  if(enumcnt == 0)
    return 0;

  //Create storage for enumerations
  enums = new HCEIDEnum[enumcnt + 1];

  //Loop through all children and gather information
  enumcnt = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check for equality element node
    if(strcmp(elt->Name(), "eq") == 0)
    {
      //Get enumeration number string and check for error
      enumstr = elt->GetText();
      if((seppos = enumstr.find(',')) == string::npos)
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Delimiter not found in equality (" << enumstr << ")\n";
        return enums;
      }
      enumnumstr = enumstr.substr(0, seppos);
      enumstr = enumstr.substr(seppos+1, enumstr.length()-seppos);

      //Convert number and check for error
      if(!StringConvert(enumnumstr.c_str(), enums[enumcnt]._num))
      {
        cout << __FILE__ << ' ' << __LINE__ << " - Error converting enumeration number (" << enumnumstr << ")\n";
        return enums;
      }

      //Get enumeration string
      enums[enumcnt]._str = enumstr;

      //Advance to next index into enumeration array
      enumcnt++;
    }
  }

  return enums;
}

template <typename T> void HCConnection::ParseFloat(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl;
  HCFloatCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl element and check for error
  if(!ParseValue(pelt, "scl", scl))
    return;

  //Create client stub
  stub = new HCFloatCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCFloatS<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::Get, &HCFloatCli<T>::Set, scl);
    else if(acc == "R")
      param = new HCFloatS<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::Get, 0, scl);
    else if(acc == "W")
      param = new HCFloatS<HCFloatCli<T>, T>(name, stub, 0, &HCFloatCli<T>::Set, scl);
    else
      param = new HCFloatS<HCFloatCli<T>, T>(name, stub, 0, 0, scl);
  }
  else
  {
    if(acc == "RW")
      param = new HCFloat<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::Get, &HCFloatCli<T>::Set, scl);
    else if(acc == "R")
      param = new HCFloat<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::Get, 0, scl);
    else if(acc == "W")
      param = new HCFloat<HCFloatCli<T>, T>(name, stub, 0, &HCFloatCli<T>::Set, scl);
    else
      param = new HCFloat<HCFloatCli<T>, T>(name, stub, 0, 0, scl);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseFloatTable(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl;
  uint32_t size;
  XMLElement *elt;
  HCEIDEnum *eidenums;
  HCFloatCli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl element and check for error
  if(!ParseValue(pelt, "scl", scl))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCFloatCli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCFloatTableS<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::IGet, &HCFloatCli<T>::ISet, size, eidenums, scl);
    else if(acc == "R")
      param = new HCFloatTableS<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::IGet, 0, size, eidenums, scl);
    else if(acc == "W")
      param = new HCFloatTableS<HCFloatCli<T>, T>(name, stub, 0, &HCFloatCli<T>::ISet, size, eidenums, scl);
    else
      param = new HCFloatTableS<HCFloatCli<T>, T>(name, stub, 0, 0, size, eidenums, scl);
  }
  else
  {
    if(acc == "RW")
      param = new HCFloatTable<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::IGet, &HCFloatCli<T>::ISet, size, eidenums, scl);
    else if(acc == "R")
      param = new HCFloatTable<HCFloatCli<T>, T>(name, stub, &HCFloatCli<T>::IGet, 0, size, eidenums, scl);
    else if(acc == "W")
      param = new HCFloatTable<HCFloatCli<T>, T>(name, stub, 0, &HCFloatCli<T>::ISet, size, eidenums, scl);
    else
      param = new HCFloatTable<HCFloatCli<T>, T>(name, stub, 0, 0, size, eidenums, scl);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseVec2(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl0;
  T scl1;
  HCVec2Cli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl0 element and check for error
  if(!ParseValue(pelt, "scl0", scl0))
    return;

  //Parse scl1 element and check for error
  if(!ParseValue(pelt, "scl1", scl1))
    return;

  //Create client stub
  stub = new HCVec2Cli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCVec2S<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::Get, &HCVec2Cli<T>::Set, scl0, scl1);
    else if(acc == "R")
      param = new HCVec2S<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::Get, 0, scl0, scl1);
    else if(acc == "W")
      param = new HCVec2S<HCVec2Cli<T>, T>(name, stub, 0, &HCVec2Cli<T>::Set, scl0, scl1);
    else
      param = new HCVec2S<HCVec2Cli<T>, T>(name, stub, 0, 0, scl0, scl1);
  }
  else
  {
    if(acc == "RW")
      param = new HCVec2<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::Get, &HCVec2Cli<T>::Set, scl0, scl1);
    else if(acc == "R")
      param = new HCVec2<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::Get, 0, scl0, scl1);
    else if(acc == "W")
      param = new HCVec2<HCVec2Cli<T>, T>(name, stub, 0, &HCVec2Cli<T>::Set, scl0, scl1);
    else
      param = new HCVec2<HCVec2Cli<T>, T>(name, stub, 0, 0, scl0, scl1);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseVec2T(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl0;
  T scl1;
  uint32_t size;
  XMLElement *elt;
  HCEIDEnum *eidenums;
  HCVec2Cli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl0 element and check for error
  if(!ParseValue(pelt, "scl0", scl0))
    return;

  //Parse scl1 element and check for error
  if(!ParseValue(pelt, "scl1", scl1))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCVec2Cli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCVec2TableS<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::IGet, &HCVec2Cli<T>::ISet, size, eidenums, scl0, scl1);
    else if(acc == "R")
      param = new HCVec2TableS<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::IGet, 0, size, eidenums, scl0, scl1);
    else if(acc == "W")
      param = new HCVec2TableS<HCVec2Cli<T>, T>(name, stub, 0, &HCVec2Cli<T>::ISet, size, eidenums, scl0, scl1);
    else
      param = new HCVec2TableS<HCVec2Cli<T>, T>(name, stub, 0, 0, size, eidenums, scl0, scl1);
  }
  else
  {
    if(acc == "RW")
      param = new HCVec2Table<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::IGet, &HCVec2Cli<T>::ISet, size, eidenums, scl0, scl1);
    else if(acc == "R")
      param = new HCVec2Table<HCVec2Cli<T>, T>(name, stub, &HCVec2Cli<T>::IGet, 0, size, eidenums, scl0, scl1);
    else if(acc == "W")
      param = new HCVec2Table<HCVec2Cli<T>, T>(name, stub, 0, &HCVec2Cli<T>::ISet, size, eidenums, scl0, scl1);
    else
      param = new HCVec2Table<HCVec2Cli<T>, T>(name, stub, 0, 0, size, eidenums, scl0, scl1);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseVec3(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl0;
  T scl1;
  T scl2;
  HCVec3Cli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl0 element and check for error
  if(!ParseValue(pelt, "scl0", scl0))
    return;

  //Parse scl1 element and check for error
  if(!ParseValue(pelt, "scl1", scl1))
    return;

  //Parse scl2 element and check for error
  if(!ParseValue(pelt, "scl2", scl2))
    return;

  //Create client stub
  stub = new HCVec3Cli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCVec3S<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::Get, &HCVec3Cli<T>::Set, scl0, scl1, scl2);
    else if(acc == "R")
      param = new HCVec3S<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::Get, 0, scl0, scl1, scl2);
    else if(acc == "W")
      param = new HCVec3S<HCVec3Cli<T>, T>(name, stub, 0, &HCVec3Cli<T>::Set, scl0, scl1, scl2);
    else
      param = new HCVec3S<HCVec3Cli<T>, T>(name, stub, 0, 0, scl0, scl1, scl2);
  }
  else
  {
    if(acc == "RW")
      param = new HCVec3<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::Get, &HCVec3Cli<T>::Set, scl0, scl1, scl2);
    else if(acc == "R")
      param = new HCVec3<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::Get, 0, scl0, scl1, scl2);
    else if(acc == "W")
      param = new HCVec3<HCVec3Cli<T>, T>(name, stub, 0, &HCVec3Cli<T>::Set, scl0, scl1, scl2);
    else
      param = new HCVec3<HCVec3Cli<T>, T>(name, stub, 0, 0, scl0, scl1, scl2);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> void HCConnection::ParseVec3T(XMLElement *pelt, HCContainer *pcont)
{
  uint16_t pid;
  string name;
  string acc;
  string sav;
  T scl0;
  T scl1;
  T scl2;
  uint32_t size;
  XMLElement *elt;
  HCEIDEnum *eidenums;
  HCVec3Cli<T> *stub;
  HCParameter *param;

  //Check for null parent objects
  if((pelt == 0) || (pcont == 0))
    return;

  //Parse pid element and check for error
  if(!ParseValue(pelt, "pid", pid))
    return;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return;

  //Parse acc element and check for error
  if(!ParseValue(pelt, "acc", acc))
    return;

  //Parse sav element and check for error
  if(!ParseValue(pelt, "sav", sav))
    return;

  //Parse scl0 element and check for error
  if(!ParseValue(pelt, "scl0", scl0))
    return;

  //Parse scl1 element and check for error
  if(!ParseValue(pelt, "scl1", scl1))
    return;

  //Parse scl2 element and check for error
  if(!ParseValue(pelt, "scl2", scl2))
    return;

  //Parse size element and check for error
  if(!ParseValue(pelt, "size", size))
    return;

  //Loop through all children looking for enums
  eidenums = 0;
  for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name of element and process it appropriately
    if(strcmp(elt->Name(), "eidenums") == 0)
      eidenums = ParseEIDEnum(elt);
  }

  //Create client stub
  stub = new HCVec3Cli<T>(_cli, pid);

  //Create parameter
  if(sav == "Yes")
  {
    if(acc == "RW")
      param = new HCVec3TableS<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::IGet, &HCVec3Cli<T>::ISet, size, eidenums, scl0, scl1, scl2);
    else if(acc == "R")
      param = new HCVec3TableS<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::IGet, 0, size, eidenums, scl0, scl1, scl2);
    else if(acc == "W")
      param = new HCVec3TableS<HCVec3Cli<T>, T>(name, stub, 0, &HCVec3Cli<T>::ISet, size, eidenums, scl0, scl1, scl2);
    else
      param = new HCVec3TableS<HCVec3Cli<T>, T>(name, stub, 0, 0, size, eidenums, scl0, scl1, scl2);
  }
  else
  {
    if(acc == "RW")
      param = new HCVec3Table<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::IGet, &HCVec3Cli<T>::ISet, size, eidenums, scl0, scl1, scl2);
    else if(acc == "R")
      param = new HCVec3Table<HCVec3Cli<T>, T>(name, stub, &HCVec3Cli<T>::IGet, 0, size, eidenums, scl0, scl1, scl2);
    else if(acc == "W")
      param = new HCVec3Table<HCVec3Cli<T>, T>(name, stub, 0, &HCVec3Cli<T>::ISet, size, eidenums, scl0, scl1, scl2);
    else
      param = new HCVec3Table<HCVec3Cli<T>, T>(name, stub, 0, 0, size, eidenums, scl0, scl1, scl2);
  }

  //Add to parent
  pcont->Add(param);
}

template <typename T> bool HCConnection::ParseValue(XMLElement *pelt, const char *name, T &val)
{
  XMLElement *childelt;
  const char *text;

  //Check for null parent element
  if(pelt == 0)
    return false;

  //Find first child element with matching name
  childelt = pelt->FirstChildElement(name);

  //Check for child not found
  if(childelt == 0)
    return false;

  //Find element with matching name and check for error
  if((text = childelt->GetText()) == 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Could not find element \"" << name << "\"\n";
    return false;
  }

  //Convert value to integer
  return StringConvert(text, val);
}
