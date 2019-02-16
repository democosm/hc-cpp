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
  bool boolval;
  string strval;

  //Read value depending on type
  switch(type)
  {
  case TYPE_INT8:
    return cell->Read(i8val);
  case TYPE_INT16:
    return cell->Read(i16val);
  case TYPE_INT32:
    return cell->Read(i32val);
  case TYPE_INT64:
    return cell->Read(i64val);
  case TYPE_UINT8:
    return cell->Read(u8val);
  case TYPE_UINT16:
    return cell->Read(u16val);
  case TYPE_UINT32:
    return cell->Read(u32val);
  case TYPE_UINT64:
    return cell->Read(u64val);
  case TYPE_FLOAT:
    return cell->Read(f32val);
  case TYPE_DOUBLE:
    return cell->Read(f64val);
  case TYPE_BOOL:
    return cell->Read(boolval);
  case TYPE_STRING:
    return cell->Read(strval);
  }

  //Error
  return false;
}

uint8_t HCParameter::TypeCode(const int8_t &)
{
  return HCParameter::TYPE_INT8;
}

const string HCParameter::TypeString(const int8_t &)
{
  return "s8";
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
  return HCParameter::TYPE_INT16;
}

const string HCParameter::TypeString(const int16_t &)
{
  return "s16";
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
  return HCParameter::TYPE_INT32;
}

const string HCParameter::TypeString(const int32_t &)
{
  return "s32";
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
  return HCParameter::TYPE_INT64;
}

const string HCParameter::TypeString(const int64_t &)
{
  return "s64";
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
  return HCParameter::TYPE_UINT8;
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
  return HCParameter::TYPE_UINT16;
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
  return HCParameter::TYPE_UINT32;
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
  return HCParameter::TYPE_UINT64;
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
  return HCParameter::TYPE_FLOAT;
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
  return HCParameter::TYPE_DOUBLE;
}

const string HCParameter::TypeString(const double &)
{
  return "f64";
}

void HCParameter::DefaultVal(double &val)
{
  val = 0.0;
}

uint8_t HCParameter::TypeCode(const bool &)
{
  return HCParameter::TYPE_BOOL;
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
  return HCParameter::TYPE_STRING;
}

const string HCParameter::TypeString(const string &)
{
  return "str";
}

void HCParameter::DefaultVal(string &val)
{
  val = "";
}

int HCParameter::HandleGetPIDError(HCCell *icell, HCCell *ocell)
{
  uint8_t type;
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
  bool bval;
  string sval;

  //Assert valid arguments
  assert((icell != 0) && (ocell != 0));

  //Get type code from inbound cell and check for error
  if(!icell->Read(type))
    return false;

  //Write type code to outbound cell and check for error
  if(!ocell->Write(type))
    return false;

  //Write default value to outbound cell and check for error
  switch(type)
  {
  case TYPE_CALL:
    break;
  case TYPE_INT8:
    DefaultVal(i8val);
    if(!ocell->Write(i8val))
      return false;

    break;
  case TYPE_INT16:
    DefaultVal(i16val);
    if(!ocell->Write(i16val))
      return false;

    break;
  case TYPE_INT32:
    DefaultVal(i32val);
    if(!ocell->Write(i32val))
      return false;

    break;
  case TYPE_INT64:
    DefaultVal(i64val);
    if(!ocell->Write(i64val))
      return false;

    break;
  case TYPE_UINT8:
    DefaultVal(u8val);
    if(!ocell->Write(u8val))
      return false;

    break;
  case TYPE_UINT16:
    DefaultVal(u16val);
    if(!ocell->Write(u16val))
      return false;

    break;
  case TYPE_UINT32:
    DefaultVal(u32val);
    if(!ocell->Write(u32val))
      return false;

    break;
  case TYPE_UINT64:
    DefaultVal(u64val);
    if(!ocell->Write(u64val))
      return false;

    break;
  case TYPE_FLOAT:
    DefaultVal(f32val);
    if(!ocell->Write(f32val))
      return false;

    break;
  case TYPE_DOUBLE:
    DefaultVal(f64val);
    if(!ocell->Write(f64val))
      return false;

    break;
  case TYPE_BOOL:
    DefaultVal(bval);
    if(!ocell->Write(bval))
      return false;

    break;
  case TYPE_STRING:
    DefaultVal(sval);
    if(!ocell->Write(sval))
      return false;

    break;
  default:
    return false;
  }

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

uint32_t HCParameter::SaveNLD(ofstream &file, uint16_t pid)
{
  uint8_t namelen;
  uint8_t type;
  uint32_t datalen;

  //Write name to file
  namelen = _name.length();
  file.write((const char *)&namelen, sizeof(namelen));
  file.write(_name.c_str(), namelen);

  //Write data length to file
  datalen = sizeof(pid) + sizeof(type);
  file.write((const char *)&datalen, sizeof(datalen));

  //Write PID and type to file
  file.write((const char *)&pid, sizeof(pid));
  type = GetType();
  file.write((const char *)&type, sizeof(type));

  //Return number of bytes written
  return 1 + namelen + sizeof(datalen) + datalen;
}

uint8_t HCParameter::GetType(void)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return TYPE_CALL;
}

bool HCParameter::IsReadable(void)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::IsWritable(void)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
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
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
}

void HCParameter::PrintInfo(ostream &st)
{
  st << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m";
}

void HCParameter::SaveInfo(ofstream &, uint32_t, uint16_t)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
}

int HCParameter::Call(void)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::CallTbl(uint32_t)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::GetBool(bool &val)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  val = false;
  return ERR_TYPE;
}

int HCParameter::SetBool(bool)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::GetBoolTbl(uint32_t, bool &val)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  val = false;
  return ERR_TYPE;
}

int HCParameter::SetBoolTbl(uint32_t, bool)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::GetStr(string &val)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  val.clear();
  return ERR_TYPE;
}

int HCParameter::SetStr(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::SetStrLit(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::GetStrTbl(uint32_t, string &val)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  val.clear();
  return ERR_TYPE;
}

int HCParameter::SetStrTbl(uint32_t, const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::SetStrLitTbl(uint32_t, const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::AddStr(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::AddStrLit(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::SubStr(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::SubStrLit(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::Upload(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

int HCParameter::Download(const string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return ERR_TYPE;
}

bool HCParameter::CallCell(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::CallCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::GetCell(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::SetCell(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::GetCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::SetCellTbl(uint32_t, HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::GetCellLst(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::AddCell(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::SubCell(HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::ReadCell(uint32_t, uint16_t, HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

bool HCParameter::WriteCell(uint32_t, HCCell *, HCCell *)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return false;
}

uint32_t HCParameter::GetNumEIDs(void)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  return 0;
}

bool HCParameter::EIDStrToNum(const string &, uint32_t &num)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  num = 0;
  return false;
}

bool HCParameter::EIDNumToStr(uint32_t, string &str)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
  str.clear();
  return false;
}

bool HCParameter::GetValEnumStr(uint32_t, string &)
{
  cout << "\e[31m" << _name << " does not override method '" << __PRETTY_FUNCTION__ << "'\e[0m" << endl;
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
