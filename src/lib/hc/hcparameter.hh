// HC parameter
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

#include "hcnode.hh"
#include "hccell.hh"
#include <fstream>
#include <string>
#include <inttypes.h>
#include <iostream>

class HCParameter : public HCNode
{
public:
  //Type codes
  static const uint8_t T_CALL = 0x00;
  static const uint8_t T_BOOL = 0x01;
  static const uint8_t T_STR = 0x02;
  static const uint8_t T_FILE = 0x03;
  static const uint8_t T_I8 = 0x08;
  static const uint8_t T_I16 = 0x09;
  static const uint8_t T_I32 = 0x0A;
  static const uint8_t T_I64 = 0x0B;
  static const uint8_t T_U8 = 0x10;
  static const uint8_t T_U16 = 0x11;
  static const uint8_t T_U32 = 0x12;
  static const uint8_t T_U64 = 0x13;
  static const uint8_t T_F32 = 0x1A;
  static const uint8_t T_F64 = 0x1B;
  static const uint8_t T_I8A = 0x20;
  static const uint8_t T_I16A = 0x21;
  static const uint8_t T_I32A = 0x22;
  static const uint8_t T_I64A = 0x23;
  static const uint8_t T_U8A = 0x28;
  static const uint8_t T_U16A = 0x29;
  static const uint8_t T_U32A = 0x2A;
  static const uint8_t T_U64A = 0x2B;
  static const uint8_t T_V2F32 = 0x32;
  static const uint8_t T_V2F64 = 0x33;
  static const uint8_t T_V3F32 = 0x3A;
  static const uint8_t T_V3F64 = 0x3B;

public:
  static bool SkipValue(HCCell* cell, uint8_t type);
  static uint8_t TypeCode(void);
  static const std::string TypeString(void);
  static uint8_t TypeCode(const bool& type);
  static const std::string TypeString(const bool& type);
  static void DefaultVal(bool& val);
  static uint8_t TypeCode(const std::string& type);
  static const std::string TypeString(const std::string& type);
  static void DefaultVal(std::string& val);
  static uint8_t TypeCode(const int8_t& type);
  static const std::string TypeString(const int8_t& type);
  static void DefaultVal(int8_t& val);
  static int16_t PrintCast(const int8_t val);
  static uint8_t TypeCode(const int16_t& type);
  static const std::string TypeString(const int16_t& type);
  static void DefaultVal(int16_t& val);
  static int16_t PrintCast(const int16_t val);
  static uint8_t TypeCode(const int32_t& type);
  static const std::string TypeString(const int32_t& type);
  static void DefaultVal(int32_t& val);
  static int32_t PrintCast(const int32_t val);
  static uint8_t TypeCode(const int64_t& type);
  static const std::string TypeString(const int64_t& type);
  static void DefaultVal(int64_t& val);
  static int64_t PrintCast(const int64_t val);
  static uint8_t TypeCode(const uint8_t& type);
  static const std::string TypeString(const uint8_t& type);
  static void DefaultVal(uint8_t& val);
  static uint16_t PrintCast(const uint8_t val);
  static uint8_t TypeCode(const uint16_t& type);
  static const std::string TypeString(const uint16_t& type);
  static void DefaultVal(uint16_t& val);
  static uint16_t PrintCast(const uint16_t val);
  static uint8_t TypeCode(const uint32_t& type);
  static const std::string TypeString(const uint32_t& type);
  static void DefaultVal(uint32_t& val);
  static uint32_t PrintCast(const uint32_t val);
  static uint8_t TypeCode(const uint64_t& type);
  static const std::string TypeString(const uint64_t& type);
  static void DefaultVal(uint64_t& val);
  static uint64_t PrintCast(const uint64_t val);
  static uint8_t TypeCode(const int8_t* type);
  static const std::string TypeString(const int8_t* type);
  static void DefaultVal(int8_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const int16_t* type);
  static const std::string TypeString(const int16_t* type);
  static void DefaultVal(int16_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const int32_t* type);
  static const std::string TypeString(const int32_t* type);
  static void DefaultVal(int32_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const int64_t* type);
  static const std::string TypeString(const int64_t* type);
  static void DefaultVal(int64_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const uint8_t* type);
  static const std::string TypeString(const uint8_t* type);
  static void DefaultVal(uint8_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const uint16_t* type);
  static const std::string TypeString(const uint16_t* type);
  static void DefaultVal(uint16_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const uint32_t* type);
  static const std::string TypeString(const uint32_t* type);
  static void DefaultVal(uint32_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const uint64_t* type);
  static const std::string TypeString(const uint64_t* type);
  static void DefaultVal(uint64_t* val, uint16_t maxlen, uint16_t& len);
  static uint8_t TypeCode(const float& type);
  static const std::string TypeString(const float& type);
  static void DefaultVal(float& val);
  static uint8_t TypeCode(const double& type);
  static const std::string TypeString(const double& type);
  static void DefaultVal(double& val);
  static uint8_t TypeCode(const float& type0, const float& type1);
  static const std::string TypeString(const float& type0, const float& type1);
  static void DefaultVal(float& val0, float& val1);
  static uint8_t TypeCode(const double& type0, const double& type1);
  static const std::string TypeString(const double& type0, const double& type1);
  static void DefaultVal(double& val0, double& val1);
  static uint8_t TypeCode(const float& type0, const float& type1, const float& type2);
  static const std::string TypeString(const float& type0, const float& type1, const float& type2);
  static void DefaultVal(float& val0, float& val1, float& val2);
  static uint8_t TypeCode(const double& type0, const double& type1, const double& type2);
  static const std::string TypeString(const double& type0, const double& type1, const double& type2);
  static void DefaultVal(double& val0, double& val1, double& val2);
  static int HandleGetPIDError(HCCell* icell, HCCell* ocell);
  static int HandleSetPIDError(HCCell* icell, HCCell* ocell);

public:
  HCParameter(const std::string& name);
  virtual ~HCParameter();
  bool GetNextCharInName(const std::string& name, char& nextchar);
  HCParameter* GetNext(void);
  void SetNext(HCParameter* node);
  void PrintNotReadable(void);
  virtual uint8_t GetType(void);
  virtual bool IsReadable(void);
  virtual bool IsWritable(void);
  virtual bool IsSavable(void);
  virtual bool IsATable(void);
  virtual bool IsAList(void);
  virtual bool HasValEnums(void);
  virtual void PrintVal(void);
  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout);
  virtual void PrintInfo(std::ostream& st=std::cout);
  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid);
  virtual int Call(void);
  virtual int CallTbl(uint32_t eid);
  virtual int GetBool(bool& val);
  virtual int SetBool(const bool val);
  virtual int GetBoolTbl(uint32_t eid, bool& val);
  virtual int SetBoolTbl(uint32_t eid, const bool val);
  virtual int GetStr(std::string& val);
  virtual int SetStr(const std::string& val);
  virtual int SetStrLit(const std::string& val);
  virtual int GetStrTbl(uint32_t eid, std::string& val);
  virtual int SetStrTbl(uint32_t eid, const std::string& val);
  virtual int SetStrLitTbl(uint32_t eid, const std::string& val);
  virtual int AddStr(const std::string& val);
  virtual int AddStrLit(const std::string& val);
  virtual int SubStr(const std::string& val);
  virtual int SubStrLit(const std::string& val);
  virtual int GetInt(int8_t& val);
  virtual int GetInt(int16_t& val);
  virtual int GetInt(int32_t& val);
  virtual int GetInt(int64_t& val);
  virtual int GetInt(uint8_t& val);
  virtual int GetInt(uint16_t& val);
  virtual int GetInt(uint32_t& val);
  virtual int GetInt(uint64_t& val);
  virtual int SetInt(const int8_t val);
  virtual int SetInt(const int16_t val);
  virtual int SetInt(const int32_t val);
  virtual int SetInt(const int64_t val);
  virtual int SetInt(const uint8_t val);
  virtual int SetInt(const uint16_t val);
  virtual int SetInt(const uint32_t val);
  virtual int SetInt(const uint64_t val);
  virtual int GetIntTbl(uint32_t eid, int8_t& val);
  virtual int GetIntTbl(uint32_t eid, int16_t& val);
  virtual int GetIntTbl(uint32_t eid, int32_t& val);
  virtual int GetIntTbl(uint32_t eid, int64_t& val);
  virtual int GetIntTbl(uint32_t eid, uint8_t& val);
  virtual int GetIntTbl(uint32_t eid, uint16_t& val);
  virtual int GetIntTbl(uint32_t eid, uint32_t& val);
  virtual int GetIntTbl(uint32_t eid, uint64_t& val);
  virtual int SetIntTbl(uint32_t eid, const int8_t val);
  virtual int SetIntTbl(uint32_t eid, const int16_t val);
  virtual int SetIntTbl(uint32_t eid, const int32_t val);
  virtual int SetIntTbl(uint32_t eid, const int64_t val);
  virtual int SetIntTbl(uint32_t eid, const uint8_t val);
  virtual int SetIntTbl(uint32_t eid, const uint16_t val);
  virtual int SetIntTbl(uint32_t eid, const uint32_t val);
  virtual int SetIntTbl(uint32_t eid, const uint64_t val);
  virtual int AddInt(const int8_t val);
  virtual int AddInt(const int16_t val);
  virtual int AddInt(const int32_t val);
  virtual int AddInt(const int64_t val);
  virtual int AddInt(const uint8_t val);
  virtual int AddInt(const uint16_t val);
  virtual int AddInt(const uint32_t val);
  virtual int AddInt(const uint64_t val);
  virtual int SubInt(const int8_t val);
  virtual int SubInt(const int16_t val);
  virtual int SubInt(const int32_t val);
  virtual int SubInt(const int64_t val);
  virtual int SubInt(const uint8_t val);
  virtual int SubInt(const uint16_t val);
  virtual int SubInt(const uint32_t val);
  virtual int SubInt(const uint64_t val);
  virtual int GetFlt(float& val);
  virtual int GetFlt(double& val);
  virtual int SetFlt(const float val);
  virtual int SetFlt(const double val);
  virtual int GetFltTbl(uint32_t eid, float& val);
  virtual int GetFltTbl(uint32_t eid, double& val);
  virtual int SetFltTbl(uint32_t eid, const float val);
  virtual int SetFltTbl(uint32_t eid, const double val);
  virtual int Upload(const std::string& val);
  virtual int Download(const std::string& val);
  virtual bool CallCell(HCCell* icell, HCCell* ocell);
  virtual bool CallCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell);
  virtual bool GetCell(HCCell* icell, HCCell* ocell);
  virtual bool SetCell(HCCell* icell, HCCell* ocell);
  virtual bool GetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell);
  virtual bool SetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell);
  virtual bool GetCellLst(HCCell* icell, HCCell* ocell);
  virtual bool AddCell(HCCell* icell, HCCell* ocell);
  virtual bool SubCell(HCCell* icell, HCCell* ocell);
  virtual bool ReadCell(uint32_t offset, uint16_t maxlen, HCCell* icell, HCCell* ocell);
  virtual bool WriteCell(uint32_t offset, HCCell* icell, HCCell* ocell);
  virtual uint32_t GetNumEIDs(void);
  virtual bool EIDStrToNum(const std::string& str, uint32_t& num);
  virtual bool EIDNumToStr(uint32_t num, std::string& str);
  virtual bool GetValEnumStr(uint32_t ind, std::string& str);

private:
  HCParameter* _next;
};

struct HCEIDEnum
{
  HCEIDEnum();
  HCEIDEnum(uint32_t num, const std::string& str);

  uint32_t _num;
  std::string _str;
};
