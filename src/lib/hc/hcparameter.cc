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

#include "const.hh"
#include "hcparameter.hh"
#include "error.hh"
#include <cassert>

using namespace std;

HCParameter::HCParameter(const string &name)
: HCNode(name)
{
  //Initialize member variables
  _next = 0;
}

HCParameter::~HCParameter()
{
  //Cleanup
}

bool HCParameter::SkipValue(HCCell *cell, uint8_t type)
{
  bool boolval;
  string strval;
  int8_t i8val;
  int16_t i16val;
  int32_t i32val;
  int64_t i64val;
  uint8_t u8val;
  uint16_t u16val;
  uint32_t u32val;
  uint64_t u64val;
  float f32val;
  double f64val;

  //Read value depending on type
  switch(type)
  {
  case T_BOOL:
    return cell->Read(boolval);
  case T_STR:
    return cell->Read(strval);
  case T_I8:
    return cell->Read(i8val);
  case T_I16:
    return cell->Read(i16val);
  case T_I32:
    return cell->Read(i32val);
  case T_I64:
    return cell->Read(i64val);
  case T_U8:
    return cell->Read(u8val);
  case T_U16:
    return cell->Read(u16val);
  case T_U32:
    return cell->Read(u32val);
  case T_U64:
    return cell->Read(u64val);
  case T_F32:
    return cell->Read(f32val);
  case T_F64:
    return cell->Read(f64val);
  case T_V2F32:
    return cell->Read(f32val, f32val);
  case T_V2F64:
    return cell->Read(f64val, f64val);
  case T_V3F32:
    return cell->Read(f32val, f32val, f32val);
  case T_V3F64:
    return cell->Read(f64val, f64val, f64val);
  }

  //Error
  return false;
}

uint8_t HCParameter::TypeCode(void)
{
  return HCParameter::T_CALL;
}

const string HCParameter::TypeString(void)
{
  return "call";
}

uint8_t HCParameter::TypeCode(const bool &)
{
  return HCParameter::T_BOOL;
}

const string HCParameter::TypeString(const bool &)
{
  return "bool";
}

void HCParameter::DefaultVal(bool &val)
{
  val = false;
}

uint8_t HCParameter::TypeCode(const string &)
{
  return HCParameter::T_STR;
}

const string HCParameter::TypeString(const string &)
{
  return "str";
}

void HCParameter::DefaultVal(string &val)
{
  val = "";
}

uint8_t HCParameter::TypeCode(const int8_t &)
{
  return HCParameter::T_I8;
}

const string HCParameter::TypeString(const int8_t &)
{
  return "i8";
}

void HCParameter::DefaultVal(int8_t &val)
{
  val = 0;
}

int16_t HCParameter::PrintCast(const int8_t val)
{
  return (int16_t)val;
}

uint8_t HCParameter::TypeCode(const int16_t &)
{
  return HCParameter::T_I16;
}

const string HCParameter::TypeString(const int16_t &)
{
  return "i16";
}

void HCParameter::DefaultVal(int16_t &val)
{
  val = 0;
}

int16_t HCParameter::PrintCast(const int16_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const int32_t &)
{
  return HCParameter::T_I32;
}

const string HCParameter::TypeString(const int32_t &)
{
  return "i32";
}

void HCParameter::DefaultVal(int32_t &val)
{
  val = 0;
}

int32_t HCParameter::PrintCast(const int32_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const int64_t &)
{
  return HCParameter::T_I64;
}

const string HCParameter::TypeString(const int64_t &)
{
  return "i64";
}

void HCParameter::DefaultVal(int64_t &val)
{
  val = 0;
}

int64_t HCParameter::PrintCast(const int64_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const uint8_t &)
{
  return HCParameter::T_U8;
}

const string HCParameter::TypeString(const uint8_t &)
{
  return "u8";
}

void HCParameter::DefaultVal(uint8_t &val)
{
  val = 0;
}

uint16_t HCParameter::PrintCast(const uint8_t val)
{
  return (uint16_t)val;
}

uint8_t HCParameter::TypeCode(const uint16_t &)
{
  return HCParameter::T_U16;
}

const string HCParameter::TypeString(const uint16_t &)
{
  return "u16";
}

void HCParameter::DefaultVal(uint16_t &val)
{
  val = 0;
}

uint16_t HCParameter::PrintCast(const uint16_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const uint32_t &)
{
  return HCParameter::T_U32;
}

const string HCParameter::TypeString(const uint32_t &)
{
  return "u32";
}

void HCParameter::DefaultVal(uint32_t &val)
{
  val = 0;
}

uint32_t HCParameter::PrintCast(const uint32_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const uint64_t &)
{
  return HCParameter::T_U64;
}

const string HCParameter::TypeString(const uint64_t &)
{
  return "u64";
}

void HCParameter::DefaultVal(uint64_t &val)
{
  val = 0;
}

uint64_t HCParameter::PrintCast(const uint64_t val)
{
  return val;
}

uint8_t HCParameter::TypeCode(const float &)
{
  return HCParameter::T_F32;
}

const string HCParameter::TypeString(const float &)
{
  return "f32";
}

void HCParameter::DefaultVal(float &val)
{
  val = 0.0;
}

uint8_t HCParameter::TypeCode(const double &)
{
  return HCParameter::T_F64;
}

const string HCParameter::TypeString(const double &)
{
  return "f64";
}

void HCParameter::DefaultVal(double &val)
{
  val = 0.0;
}

uint8_t HCParameter::TypeCode(const float &type0, const float &type1)
{
  return HCParameter::T_V2F32;
}

const std::string HCParameter::TypeString(const float &type0, const float &type1)
{
  return "v2f32";
}

void HCParameter::DefaultVal(float &val0, float &val1)
{
  val0 = 0.0;
  val1 = 0.0;
}

uint8_t HCParameter::TypeCode(const double &type0, const double &type1)
{
  return HCParameter::T_V2F64;
}

const std::string HCParameter::TypeString(const double &type0, const double &type1)
{
  return "v2f64";
}

void HCParameter::DefaultVal(double &val0, double &val1)
{
  val0 = 0.0;
  val1 = 0.0;
}

uint8_t HCParameter::TypeCode(const float &type0, const float &type1, const float &type2)
{
  return HCParameter::T_V3F32;
}

const std::string HCParameter::TypeString(const float &type0, const float &type1, const float &type2)
{
  return "v3f32";
}

void HCParameter::DefaultVal(float &val0, float &val1, float &val2)
{
  val0 = 0.0;
  val1 = 0.0;
  val2 = 0.0;
}

uint8_t HCParameter::TypeCode(const double &type0, const double &type1, const double &type2)
{
  return HCParameter::T_V3F64;
}

const std::string HCParameter::TypeString(const double &type0, const double &type1, const double &type2)
{
  return "v3f64";
}

void HCParameter::DefaultVal(double &val0, double &val1, double &val2)
{
  val0 = 0.0;
  val1 = 0.0;
  val2 = 0.0;
}

int HCParameter::HandleGetPIDError(HCCell *icell, HCCell *ocell)
{
  int8_t i8val;

  //Assert valid arguments
  assert((icell != 0) && (ocell != 0));

  //Write type code to outbound cell and check for error
  if(!ocell->Write(T_I8))
    return false;

  //Write default value to outbound cell and check for error
  DefaultVal(i8val);
  if(!ocell->Write(i8val))
    return false;

  //Write PID error code to outbound cell and check for error
  if(!ocell->Write((int8_t)ERR_PID))
    return false;

  return true;
}

int HCParameter::HandleSetPIDError(HCCell *icell, HCCell *ocell)
{
  uint8_t type;

  //Assert valid arguments
  assert((icell != 0) && (ocell != 0));

  //Get type code from inbound cell and check for error
  if(!icell->Read(type))
    return false;

  //Write type code to outbound cell and check for error
  if(!ocell->Write(type))
    return false;

  //Skip value in inbound cell and check for error
  if(!SkipValue(icell, type))
    return false;

  //Write PID error code to outbound cell and check for error
  if(!ocell->Write((int8_t)ERR_PID))
    return false;

  return true;
}

bool HCParameter::GetNextCharInName(const string &name, char &nextchar)
{
  uint32_t namelen;

  //Check for exact match
  if(_name == name)
  {
    nextchar = ' ';
    return true;
  }

  //Get length of string
  namelen = name.length();

  //Check for name too long
  if(namelen >= _name.length())
    return false;

  //Check for name starting with string
  if(_name.compare(0, namelen, name) == 0)
  {
    //Give caller the next character in the name and indicate match
    nextchar = _name.at(namelen);
    return true;
  }

  //Indicate no match
  return false;
}

HCParameter *HCParameter::GetNext(void)
{
  return _next;
}

void HCParameter::SetNext(HCParameter *next)
{
  _next = next;
}

void HCParameter::PrintNotReadable(void)
{
  std::cout << TC_YELLOW << _name;
  std::cout << " is not readable";
  std::cout << TC_RESET << "\n";
}

uint8_t HCParameter::GetType(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return T_CALL;
}

bool HCParameter::IsReadable(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::IsWritable(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::IsSavable(void)
{
  return false;
}

bool HCParameter::IsATable(void)
{
  return false;
}

bool HCParameter::IsAList(void)
{
  return false;
}

bool HCParameter::HasValEnums(void)
{
  return false;
}

void HCParameter::PrintVal(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
}

void HCParameter::PrintConfig(const string &path, ostream &st)
{
}

void HCParameter::PrintInfo(ostream &st)
{
  st << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET;
}

void HCParameter::SaveInfo(ofstream &, uint32_t, uint16_t)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
}

int HCParameter::Call(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::CallTbl(uint32_t)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetBool(bool &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  val = false;
  return ERR_TYPE;
}

int HCParameter::SetBool(bool)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetBoolTbl(uint32_t, bool &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  val = false;
  return ERR_TYPE;
}

int HCParameter::SetBoolTbl(uint32_t, bool)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetStr(string &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  val.clear();
  return ERR_TYPE;
}

int HCParameter::SetStr(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetStrLit(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetStrTbl(uint32_t, string &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  val.clear();
  return ERR_TYPE;
}

int HCParameter::SetStrTbl(uint32_t, const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetStrLitTbl(uint32_t, const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddStr(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddStrLit(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubStr(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubStrLit(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(int8_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(int16_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(int32_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(int64_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(uint8_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(uint16_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(uint32_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetInt(uint64_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const int8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const int16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const int32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const int64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const uint8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const uint16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const uint32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetInt(const uint64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, int8_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, int16_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, int32_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, int64_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, uint8_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, uint16_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, uint32_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetIntTbl(uint32_t eid, uint64_t &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const int8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const int16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const int32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const int64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const uint8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const uint16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const uint32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetIntTbl(uint32_t eid, const uint64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const int8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const int16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const int32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const int64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const uint8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const uint16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const uint32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::AddInt(const uint64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const int8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const int16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const int32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const int64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const uint8_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const uint16_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const uint32_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SubInt(const uint64_t val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetFlt(float &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetFlt(double &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetFlt(const float val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetFlt(const double val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetFltTbl(uint32_t eid, float &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::GetFltTbl(uint32_t eid, double &val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetFltTbl(uint32_t eid, const float val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::SetFltTbl(uint32_t eid, const double val)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::Upload(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

int HCParameter::Download(const string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return ERR_TYPE;
}

bool HCParameter::CallCell(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::CallCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::GetCell(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::SetCell(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::GetCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::SetCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::GetCellLst(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::AddCell(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::SubCell(HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::ReadCell(uint32_t, uint16_t, HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

bool HCParameter::WriteCell(uint32_t, HCCell *, HCCell *)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

uint32_t HCParameter::GetNumEIDs(void)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return 0;
}

bool HCParameter::EIDStrToNum(const string &, uint32_t &num)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  num = 0;
  return false;
}

bool HCParameter::EIDNumToStr(uint32_t, string &str)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  str.clear();
  return false;
}

bool HCParameter::GetValEnumStr(uint32_t, string &)
{
  cout << TC_RED << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'" << TC_RESET << "\n";
  return false;
}

HCEIDEnum::HCEIDEnum()
{
  //Initialize member variables
  _num = 0;
  _str = "";
}

HCEIDEnum::HCEIDEnum(uint32_t num, const string &str)
{
  //Initialize member variables
  _num = num;
  _str = str;
}
