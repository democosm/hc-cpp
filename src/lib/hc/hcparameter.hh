// HC parameter
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

#ifndef _HCPARAM_HH_
#define _HCPARAM_HH_

#include "hcnode.hh"
#include "hccell.hh"
#include <fstream>
#include <string>
#include <iostream>

class HCParameter : public HCNode
{
public:
  //Type codes
  static const uint8_t TYPE_CALL = 0x00;
  static const uint8_t TYPE_INT8 = 0x10;
  static const uint8_t TYPE_INT16 = 0x11;
  static const uint8_t TYPE_INT32 = 0x12;
  static const uint8_t TYPE_INT64 = 0x13;
  static const uint8_t TYPE_UINT8 = 0x20;
  static const uint8_t TYPE_UINT16 = 0x21;
  static const uint8_t TYPE_UINT32 = 0x22;
  static const uint8_t TYPE_UINT64 = 0x23;
  static const uint8_t TYPE_FLOAT = 0x32;
  static const uint8_t TYPE_DOUBLE = 0x33;
  static const uint8_t TYPE_BOOL = 0x40;
  static const uint8_t TYPE_STRING = 0x50;
  static const uint8_t TYPE_FILE = 0x60;

public:
  static bool SkipValue(HCCell *cell, uint8_t type);
  static uint8_t TypeCode(const int8_t &type);
  static const std::string TypeString(const int8_t &type);
  static void DefaultVal(int8_t &val);
  static int16_t PrintCast(const int8_t val);
  static uint8_t TypeCode(const int16_t &type);
  static const std::string TypeString(const int16_t &type);
  static void DefaultVal(int16_t &val);
  static int16_t PrintCast(const int16_t val);
  static uint8_t TypeCode(const int32_t &type);
  static const std::string TypeString(const int32_t &type);
  static void DefaultVal(int32_t &val);
  static int32_t PrintCast(const int32_t val);
  static uint8_t TypeCode(const int64_t &type);
  static const std::string TypeString(const int64_t &type);
  static void DefaultVal(int64_t &val);
  static int64_t PrintCast(const int64_t val);
  static uint8_t TypeCode(const uint8_t &type);
  static const std::string TypeString(const uint8_t &type);
  static void DefaultVal(uint8_t &val);
  static uint16_t PrintCast(const uint8_t val);
  static uint8_t TypeCode(const uint16_t &type);
  static const std::string TypeString(const uint16_t &type);
  static void DefaultVal(uint16_t &val);
  static uint16_t PrintCast(const uint16_t val);
  static uint8_t TypeCode(const uint32_t &type);
  static const std::string TypeString(const uint32_t &type);
  static void DefaultVal(uint32_t &val);
  static uint32_t PrintCast(const uint32_t val);
  static uint8_t TypeCode(const uint64_t &type);
  static const std::string TypeString(const uint64_t &type);
  static void DefaultVal(uint64_t &val);
  static uint64_t PrintCast(const uint64_t val);
  static uint8_t TypeCode(const float &type);
  static const std::string TypeString(const float &type);
  static void DefaultVal(float &val);
  static uint8_t TypeCode(const double &type);
  static const std::string TypeString(const double &type);
  static void DefaultVal(double &val);
  static uint8_t TypeCode(const bool &type);
  static const std::string TypeString(const bool &type);
  static void DefaultVal(bool &val);
  static uint8_t TypeCode(const std::string &type);
  static const std::string TypeString(const std::string &type);
  static void DefaultVal(std::string &val);
  static int HandleGetPIDError(HCCell *icell, HCCell *ocell);
  static int HandleSetPIDError(HCCell *icell, HCCell *ocell);

public:
  HCParameter(const std::string &name);
  virtual ~HCParameter();
  bool GetNextCharInName(const std::string &name, char &nextchar);
  HCParameter *GetNext(void);
  void SetNext(HCParameter *node);
  uint32_t  SaveNLD(std::ofstream &file, uint16_t pid);
  virtual uint8_t GetType(void);
  virtual bool IsReadable(void);
  virtual bool IsWritable(void);
  virtual bool IsATable(void);
  virtual bool IsAList(void);
  virtual bool HasValEnums(void);
  virtual void PrintVal(void);
  virtual void PrintInfo(std::ostream &st=std::cout);
  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid);
  virtual int Call(void);
  virtual int CallTbl(uint32_t eid);
  virtual int GetBool(bool &val);
  virtual int SetBool(const bool val);
  virtual int GetBoolTbl(uint32_t eid, bool &val);
  virtual int SetBoolTbl(uint32_t eid, const bool val);
  virtual int GetStr(std::string &val);
  virtual int SetStr(const std::string &val);
  virtual int SetStrLit(const std::string &val);
  virtual int GetStrTbl(uint32_t eid, std::string &val);
  virtual int SetStrTbl(uint32_t eid, const std::string &val);
  virtual int SetStrLitTbl(uint32_t eid, const std::string &val);
  virtual int AddStr(const std::string &val);
  virtual int AddStrLit(const std::string &val);
  virtual int SubStr(const std::string &val);
  virtual int SubStrLit(const std::string &val);
  virtual int Upload(const std::string &val);
  virtual int Download(const std::string &val);
  virtual bool CallCell(HCCell *icell, HCCell *ocell);
  virtual bool CallCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell);
  virtual bool GetCell(HCCell *icell, HCCell *ocell);
  virtual bool SetCell(HCCell *icell, HCCell *ocell);
  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell);
  virtual bool SetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell);
  virtual bool GetCellLst(HCCell *icell, HCCell *ocell);
  virtual bool AddCell(HCCell *icell, HCCell *ocell);
  virtual bool SubCell(HCCell *icell, HCCell *ocell);
  virtual bool ReadCell(uint32_t offset, uint16_t maxlen, HCCell *icell, HCCell *ocell);
  virtual bool WriteCell(uint32_t offset, HCCell *icell, HCCell *ocell);
  virtual uint32_t GetNumEIDs(void);
  virtual bool EIDStrToNum(const std::string &str, uint32_t &num);
  virtual bool EIDNumToStr(uint32_t num, std::string &str);
  virtual bool GetValEnumStr(uint32_t ind, std::string &str);

private:
  HCParameter *_next;
};

struct HCEIDEnum
{
  HCEIDEnum();
  HCEIDEnum(uint32_t num, const std::string &str);

  uint32_t _num;
  std::string _str;
};

#endif //_HCPARAM_HH_
