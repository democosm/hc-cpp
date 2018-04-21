// HC integer templates
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

#ifndef _HCINTEGER_HH_
#define _HCINTEGER_HH_

#include "error.hh"
#include "hcclient.hh"
#include "hcparameter.hh"
#include "str.hh"
#include "tinyxml2.hh"
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

using namespace std;
using namespace tinyxml2;

//Integer enumeration
template <class T>
struct HCIntegerEnum
{
public:
  HCIntegerEnum()
  {
    //Initialize member variables
    _num = 0;
    _str = "";
  }

  HCIntegerEnum(T num, const string &str)
  {
    //Initialize member variables
    _num = num;
    _str = str;
  }

  HCIntegerEnum(XMLElement *pelt)
  {
    XMLElement *elt;
    string numstr;
    string eqstr;
    size_t seppos;

    //Initialize member variables
    _num = 0;
    _str = "";

    //Check for null parent objects
    if(pelt == 0)
      return;

    //Loop through all children and gather information
    for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
    {
      //Check for equality element node
      if(strcmp(elt->Name(), "eq") == 0)
      {
        //Get enumeration number string and check for error
        eqstr = elt->GetText();
        if((seppos = eqstr.find(',')) == string::npos)
        {
          cout << __FILE__ << ' ' << __LINE__ << " - Delimiter not found in equality (" << eqstr << ")\n";
          return;
        }
        numstr = eqstr.substr(0, seppos);

        //Convert number and check for error
        if(!StringConvert(numstr.c_str(), _num))
        {
          cout << __FILE__ << ' ' << __LINE__ << " - Error converting enumeration number (" << numstr << ")\n";
          return;
        }

        //Copy string
        _str = eqstr.substr(seppos+1, eqstr.length()-seppos);
      }
    }
  }

public:
  T _num;
  string _str;
};

//Types derived from enumeration template
typedef HCIntegerEnum<int8_t> HCSigned8Enum;
typedef HCIntegerEnum<int16_t> HCSigned16Enum;
typedef HCIntegerEnum<int32_t> HCSigned32Enum;
typedef HCIntegerEnum<int64_t> HCSigned64Enum;
typedef HCIntegerEnum<uint8_t> HCUnsigned8Enum;
typedef HCIntegerEnum<uint16_t> HCUnsigned16Enum;
typedef HCIntegerEnum<uint32_t> HCUnsigned32Enum;
typedef HCIntegerEnum<uint64_t> HCUnsigned64Enum;

//Integer client stub
template <class T>
class HCIntegerCli
{
public:
  HCIntegerCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCIntegerCli()
  {
  }

  int Get(T &val)
  {
    //Delegate to client
    return _cli->Get(_pid, val);
  }

  int Set(const T val)
  {
    //Delegate to client
    return _cli->Set(_pid, val);
  }

  int Add(const T val)
  {
    //Delegate to client
    return _cli->Add(_pid, val);
  }

  int Sub(const T val)
  {
    //Delegate to client
    return _cli->Sub(_pid, val);
  }

  int IGet(uint32_t eid, T &val)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val);
  }

  int ISet(uint32_t eid, const T val)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val);
  }

private:
  HCClient *_cli;
  uint16_t _pid;
};

//Types derived from client stub template
typedef HCIntegerCli<int8_t> HCSigned8Cli;
typedef HCIntegerCli<int16_t> HCSigned16Cli;
typedef HCIntegerCli<int32_t> HCSigned32Cli;
typedef HCIntegerCli<int64_t> HCSigned64Cli;
typedef HCIntegerCli<uint8_t> HCUnsigned8Cli;
typedef HCIntegerCli<uint16_t> HCUnsigned16Cli;
typedef HCIntegerCli<uint32_t> HCUnsigned32Cli;
typedef HCIntegerCli<uint64_t> HCUnsigned64Cli;

//Integer
template <class C, class T>
class HCInteger : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T &);
  typedef int (C::*SetMethod)(const T);

public:
  HCInteger(const string &name, C *object, GetMethod getmethod, SetMethod setmethod, const HCIntegerEnum<T> *valenums=0)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _valenums = valenums;
  }

  virtual ~HCInteger()
  {
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val);
  }

  virtual bool IsReadable(void)
  {
    if(_getmethod == 0)
      return false;

    return true;
  }

  virtual bool IsWritable(void)
  {
    if(_setmethod == 0)
      return false;

    return true;
  }

  virtual bool IsATable(void)
  {
    return false;
  }

  virtual bool IsAList(void)
  {
    return false;
  }

  virtual bool HasValEnums(void)
  {
    if(_valenums == 0)
      return false;

    return true;
  }

  virtual void PrintVal(void)
  {
    T val;
    int lerr;
    uint32_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value
      cout << _name << " !" << ErrToString(ERR_ACCESS) << "\n";
      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val);

    //Check for no value enums
    if(_valenums == 0)
    {
      //Print value
      cout << _name << " = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " !" << ErrToString(lerr) << "\n";
      return;
    }

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._num == val)
      {
        //Print value
        cout << _name << " = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \"" << _valenums[i]._str << "\" !" << ErrToString(lerr) << "\n";
        return;
      }
    }

    //Print value indicate that no enum found
    cout << _name << " = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << "\n";
  }

  virtual void PrintInfo(ostream &st=cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");

    //Print enum information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str;
    }
  }

  virtual void SaveXML(ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << string(indent, ' ') << "<" << TypeString(dummy) << ">\n";
    file << string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>\n";

    if(_valenums != 0)
    {
      file << string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << string(indent, ' ') << "  </valenums>\n";
    }

    file << string(indent, ' ') << "</" << HCParameter::TypeString(dummy) << ">\n";
  }

  virtual int GetStr(string &val)
  {
    T nval;
    int lerr;
    uint32_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method and check for error
    if((lerr = (_object->*_getmethod)(nval)) != ERR_NONE)
    {
      val.clear();
      return lerr;
    }

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Return enum value
          val = _valenums[i]._str;
          return ERR_NONE;
        }
      }

      val.clear();
      return ERR_RANGE;
    }

    //Convert to string
    StringPrint(nval, val);
    return ERR_NONE;
  }

  virtual int SetStr(const string &val)
  {
    T nval;
    uint32_t i;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Set value
          return (_object->*_setmethod)(nval);
        }
      }

      return ERR_RANGE;
    }

    //Set value
    return (_object->*_setmethod)(nval);
  }

  virtual int SetStrLit(const string &val)
  {
    uint32_t i;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Check for no value enums
    if(_valenums == 0)
      return ERR_TYPE;

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._str == val)
      {
        //Set value
        return (_object->*_setmethod)(_valenums[i]._num);
      }
    }

    return ERR_RANGE;
  }

  virtual bool GetCell(HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(val);

      //Check for incorrect type, report this type and value with type error
      if(type != TypeCode(val))
        lerr = ERR_TYPE;
    }
    else
    {
      //Access error
      DefaultVal(val);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCell(HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val))
    {
      //Skip value in inbound cell and check for error
      if(!SkipValue(icell, type))
        return false;

      //Write type code to outbound cell and check for error
      if(!ocell->Write(TypeCode(val)))
        return false;

      //Write type error code to outbound cell and check for error
      if(!ocell->Write(ERR_TYPE))
        return false;

      return true;
    }

    //Get value from inbound cell and check for error
    if(!icell->Read(val))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(val);
    }
    else
    {
      //Access error
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool GetValEnumStr(uint32_t ind, string &str)
  {
    uint32_t i;

    //Check for no value enums
    if(_valenums == 0)
    {
      str.clear();
      return false;
    }

    //Count value enums
    for(i=0; _valenums[i]._str.length() != 0; i++);

    //Check for index out of range
    if(ind >= i)
    {
      str.clear();
      return false;
    }

    //Return value enum string
    str = _valenums[ind]._str;
    return true;
  }

private:
  C *_object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  const HCIntegerEnum<T> *_valenums;
};

//Classes derived from value template
template <class C>
class HCSigned8 : public HCInteger<C, int8_t>
{
public:
  HCSigned8(const string &name, C *object, int (C::*getmethod)(int8_t &), int (C::*setmethod)(const int8_t), const HCSigned8Enum *valenums=0)
  : HCInteger<C, int8_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCSigned16 : public HCInteger<C, int16_t>
{
public:
  HCSigned16(const string &name, C *object, int (C::*getmethod)(int16_t &), int (C::*setmethod)(const int16_t), const HCSigned16Enum *valenums=0)
  : HCInteger<C, int16_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCSigned32 : public HCInteger<C, int32_t>
{
public:
  HCSigned32(const string &name, C *object, int (C::*getmethod)(int32_t &), int (C::*setmethod)(const int32_t), const HCSigned32Enum *valenums=0)
  : HCInteger<C, int32_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCSigned64 : public HCInteger<C, int64_t>
{
public:
  HCSigned64(const string &name, C *object, int (C::*getmethod)(int64_t &), int (C::*setmethod)(const int64_t), const HCSigned64Enum *valenums=0)
  : HCInteger<C, int64_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCUnsigned8 : public HCInteger<C, uint8_t>
{
public:
  HCUnsigned8(const string &name, C *object, int (C::*getmethod)(uint8_t &), int (C::*setmethod)(const uint8_t), const HCUnsigned8Enum *valenums=0)
  : HCInteger<C, uint8_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCUnsigned16 : public HCInteger<C, uint16_t>
{
public:
  HCUnsigned16(const string &name, C *object, int (C::*getmethod)(uint16_t &), int (C::*setmethod)(const uint16_t), const HCUnsigned16Enum *valenums=0)
  : HCInteger<C, uint16_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCUnsigned32 : public HCInteger<C, uint32_t>
{
public:
  HCUnsigned32(const string &name, C *object, int (C::*getmethod)(uint32_t &), int (C::*setmethod)(const uint32_t), const HCUnsigned32Enum *valenums=0)
  : HCInteger<C, uint32_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCUnsigned64 : public HCInteger<C, uint64_t>
{
public:
  HCUnsigned64(const string &name, C *object, int (C::*getmethod)(uint64_t &), int (C::*setmethod)(const uint64_t), const HCUnsigned64Enum *valenums=0)
  : HCInteger<C, uint64_t>(name, object, getmethod, setmethod, valenums)
  {
  }
};

//Integer table
template <class C, class T>
class HCIntegerTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T &);
  typedef int (C::*SetMethod)(uint32_t, const T);

public:
  HCIntegerTable(const string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums=0, const HCIntegerEnum<T> *valenums=0)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _size = size;
    _valenums = valenums;
    _eidenums = eidenums;
  }

  virtual ~HCIntegerTable()
  {
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val);
  }

  virtual bool IsReadable(void)
  {
    if(_getmethod == 0)
      return false;

    return true;
  }

  virtual bool IsWritable(void)
  {
    if(_setmethod == 0)
      return false;

    return true;
  }

  virtual bool IsATable(void)
  {
    return true;
  }

  virtual bool IsAList(void)
  {
    return false;
  }

  virtual bool HasValEnums(void)
  {
    if(_valenums == 0)
      return false;

    return true;
  }

  virtual void PrintVal(void)
  {
    T val;
    int lerr;
    uint32_t eid;
    string valstr;
    string eidstr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value
      cout << _name << " !" << ErrToString(ERR_ACCESS) << "\n";
      return;
    }

    //Print name
    cout << _name << "\n";

    //Loop through all elements
    for(eid=0; eid<_size; eid++)
    {
      //Get value
      lerr = (_object->*_getmethod)(eid, val);

      //Check for no value enums
      if(_valenums == 0)
      {
        //Check for no EID enums
        if(_eidenums == 0)
        {
          //Print value
          cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " !" << ErrToString(lerr) << "\n";
        }
        else
        {
          //Convert EID to enum string and check for error
          if(!EIDNumToStr(eid, eidstr))
          {
            //Print value (indicate no EID enum string found)
            cout << " [\e[31m" << eid << "\e[0m] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " !" << ErrToString(lerr) << "\n";
          }
          else
          {
            //Print value (show EID enum string)
            cout << " [\"" << eidstr << "\"] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " !" << ErrToString(lerr) << "\n";
          }
        }
      }
      else
      {
        //Check for no EID enums
        if(_eidenums == 0)
        {
          //Convert value to enum string and check for error
          if(!ValNumToStr(val, valstr))
          {
            //Print value (indicate no value enum string found)
            cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << "\n";
          }
          else
          {
            //Print value (show value enum string)
            cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \"" << valstr << "\" !" << ErrToString(lerr) << "\n";
          }
        }
        else
        {
          //Convert value to enum string and check for error
          if(!ValNumToStr(val, valstr))
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no value or EID enum string found)
              cout << " [\e[31m" << eid << "\e[0m] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << "\n";
            }
            else
            {
              //Print value (show EID enum string, indicate no value enum string found)
              cout << " [\"" << eidstr << "\"] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << "\n";
            }
          }
          else
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no EID enum string found, show value enum string)
              cout << " [\e[31m" << eid << "\e[0m] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \"" << valstr << "\" !" << ErrToString(lerr) << "\n";
            }
            else
            {
              //Print value (show EID enum string and value enum string)
              cout << " [\"" << eidstr << "\"] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \"" << valstr << "\" !" << ErrToString(lerr) << "\n";
            }
          }
        }
      }
    }
  }

  virtual void PrintInfo(ostream &st=cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy) << "t";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }

    //Print value enumeration information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str;
    }
  }

  virtual void SaveXML(ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << string(indent, ' ') << "<" << TypeString(dummy) << "t>\n";
    file << string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>\n";
    file << string(indent, ' ') << "  <size>" << _size << "</size>\n";

    if(_eidenums != 0)
    {
      file << string(indent, ' ') << "  <eidenums>\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>\n";

      file << string(indent, ' ') << "  </eidenums>\n";
    }

    if(_valenums != 0)
    {
      file << string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << string(indent, ' ') << "  </valenums>\n";
    }

    file << string(indent, ' ') << "</" << TypeString(dummy) << "t>\n";
  }

  virtual int GetStrTbl(uint32_t eid, string &val)
  {
    T nval;
    int lerr;
    uint32_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method and check for error
    if((lerr = (_object->*_getmethod)(eid, nval)) != ERR_NONE)
    {
      val.clear();
      return lerr;
    }

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Return enum value
          val = _valenums[i]._str;
          return ERR_NONE;
        }
      }

      //Return zero length string
      val.clear();
      return ERR_RANGE;
    }

    //Convert to string
    StringPrint(nval, val);
    return ERR_NONE;
  }

  virtual int SetStrTbl(uint32_t eid, const string &val)
  {
    T nval;
    uint32_t i;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Set value
          return (_object->*_setmethod)(eid, nval);
        }
      }

      return ERR_RANGE;
    }

    return (_object->*_setmethod)(eid, nval);
  }

  virtual int SetStrLitTbl(uint32_t eid, const string &val)
  {
    uint32_t i;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Check for no value enums
    if(_valenums == 0)
      return ERR_TYPE;

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._str == val)
      {
        //Set value
        return (_object->*_setmethod)(eid, _valenums[i]._num);
      }
    }

    return ERR_RANGE;
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);

      //Check for incorrect type, report this type and value with type error
      if(type != TypeCode(val))
        lerr = ERR_TYPE;
    }
    else
    {
      //Access error
      DefaultVal(val);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val))
    {
      //Skip value in inbound cell and check for error
      if(!SkipValue(icell, type))
        return false;

      //Write type code to outbound cell and check for error
      if(!ocell->Write(TypeCode(val)))
        return false;

      //Write type error code to outbound cell and check for error
      if(!ocell->Write(ERR_TYPE))
        return false;

      return true;
    }

    //Get value from inbound cell and check for error
    if(!icell->Read(val))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(eid, val);
    }
    else
    {
      //Access error
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual uint32_t GetNumEIDs(void)
  {
    return _size;
  }

  virtual bool EIDStrToNum(const string &str, uint32_t &num)
  {
    uint32_t i;

    //Check for no EID enums
    if(_eidenums == 0)
    {
      num = 0;
      return false;
    }

    //Loop through EID enums and return number of matching entry
    for(i=0; _eidenums[i]._str.length() != 0; i++)
    {
      if(_eidenums[i]._str == str)
      {
        num = _eidenums[i]._num;
        return true;
      }
    }

    //No matching entry found
    num = 0;
    return false;
  }

  virtual bool EIDNumToStr(uint32_t num, string &str)
  {
    uint32_t i;

    //Check for no EID enums
    if(_eidenums == 0)
    {
      str.clear();
      return false;
    }

    //Loop through EID enums and return string of matching entry
    for(i=0; _eidenums[i]._str.length() != 0; i++)
    {
      if(_eidenums[i]._num == num)
      {
        str = _eidenums[i]._str;
        return true;
      }
    }

    //No matching entry found
    str.clear();
    return false;
  }

  virtual bool GetValEnumStr(uint32_t ind, string &str)
  {
    uint32_t i;

    //Check for no value enums
    if(_valenums == 0)
    {
      str.clear();
      return false;
    }

    //Count value enums
    for(i=0; _valenums[i]._str.length() != 0; i++);

    //Check for index out of range
    if(ind >= i)
    {
      str.clear();
      return false;
    }

    //Return value enum string
    str = _valenums[ind]._str;
    return true;
  }

  bool ValNumToStr(T num, string &str)
  {
    uint32_t i;

    //Check for no value enums
    if(_valenums == 0)
    {
      str.clear();
      return false;
    }

    //Loop through value enums and return string of matching entry
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      if(_valenums[i]._num == num)
      {
        str = _valenums[i]._str;
        return true;
      }
    }

    //No matching entry found
    str.clear();
    return false;
  }

private:
  C *_object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  uint32_t _size;
  const HCIntegerEnum<T> *_valenums;
  const HCEIDEnum *_eidenums;
};

//Derived classes
template <class C>
class HCSigned8Table : public HCIntegerTable<C, int8_t>
{
public:
  HCSigned8Table(const string &name, C *object, int (C::*getmethod)(uint32_t, int8_t &), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCSigned8Enum *valenums=0)
  : HCIntegerTable<C, int8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCSigned16Table : public HCIntegerTable<C, int16_t>
{
public:
  HCSigned16Table(const string &name, C *object, int (C::*getmethod)(uint32_t, int16_t &), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCSigned16Enum *valenums=0)
  : HCIntegerTable<C, int16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCSigned32Table : public HCIntegerTable<C, int32_t>
{
public:
  HCSigned32Table(const string &name, C *object, int (C::*getmethod)(uint32_t, int32_t &), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCSigned32Enum *valenums=0)
  : HCIntegerTable<C, int32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCSigned64Table : public HCIntegerTable<C, int64_t>
{
public:
  HCSigned64Table(const string &name, C *object, int (C::*getmethod)(uint32_t, int64_t &), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCSigned64Enum *valenums=0)
  : HCIntegerTable<C, int64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUnsigned8Table : public HCIntegerTable<C, uint8_t>
{
public:
  HCUnsigned8Table(const string &name, C *object, int (C::*getmethod)(uint32_t, uint8_t &), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCUnsigned8Enum *valenums=0)
  : HCIntegerTable<C, uint8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUnsigned16Table : public HCIntegerTable<C, uint16_t>
{
public:
  HCUnsigned16Table(const string &name, C *object, int (C::*getmethod)(uint32_t, uint16_t &), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCUnsigned16Enum *valenums=0)
  : HCIntegerTable<C, uint16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUnsigned32Table : public HCIntegerTable<C, uint32_t>
{
public:
  HCUnsigned32Table(const string &name, C *object, int (C::*getmethod)(uint32_t, uint32_t &), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCUnsigned32Enum *valenums=0)
  : HCIntegerTable<C, uint32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUnsigned64Table : public HCIntegerTable<C, uint64_t>
{
public:
  HCUnsigned64Table(const string &name, C *object, int (C::*getmethod)(uint32_t, uint64_t &), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCEIDEnum *eidenums=0, const HCUnsigned64Enum *valenums=0)
  : HCIntegerTable<C, uint64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

//Integer list
template <class C, class T>
class HCIntegerList : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T &);
  typedef int (C::*AddMethod)(const T);
  typedef int (C::*SubMethod)(const T);

public:
  HCIntegerList(const string &name, C *object, GetMethod getmethod, AddMethod addmethod, SubMethod submethod, uint32_t maxsize, const HCIntegerEnum<T> *valenums=0)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _addmethod = addmethod;
    _submethod = submethod;
    _maxsize = maxsize;
    _valenums = valenums;
  }

  virtual ~HCIntegerList()
  {
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val);
  }

  virtual bool IsReadable(void)
  {
    if(_getmethod == 0)
      return false;

    return true;
  }

  virtual bool IsWritable(void)
  {
    if((_addmethod == 0) && (_submethod == 0))
      return false;

    return true;
  }

  virtual bool IsATable(void)
  {
    return false;
  }

  virtual bool IsAList(void)
  {
    return true;
  }

  virtual bool HasValEnums(void)
  {
    if(_valenums == 0)
      return false;

    return true;
  }

  virtual void PrintVal(void)
  {
    T val;
    int lerr;
    uint32_t i;
    uint32_t eid;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value
      cout << _name << " !" << ErrToString(ERR_ACCESS) << "\n";
      return;
    }

    //Print name
    cout << _name << "\n";

    //Loop through all elements
    for(eid=0; eid<_maxsize; eid++)
    {
      //Get value
      lerr = (_object->*_getmethod)(eid, val);

      //Stop on error
      if(lerr != ERR_NONE)
        break;

      //Check for no value enums
      if(_valenums == 0)
      {
        //Print value
        cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " !" << ErrToString(lerr) << "\n";
      }
      else
      {
        //Loop through enums
        for(i=0; _valenums[i]._str.length() != 0; i++)
        {
          //Check for match
          if(_valenums[i]._num == val)
          {
            //Print value
            cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \"" << _valenums[i]._str << "\" !" << ErrToString(lerr) << "\n";
            break;
          }
        }

        //Print value indicate that no enum found
        if(_valenums[i]._str.length() == 0)
          cout << " [" << eid << "] = " << PrintCast(val) << " = 0x" << hex << PrintCast(val) << dec << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << "\n";
      }
    }
  }

  virtual void PrintInfo(ostream &st=cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy) << "l";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << ((_addmethod == 0) && (_submethod == 0) ? "" : "W");
    st << "\n  Max Size: " << _maxsize;

    //Print enum information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str;
    }
  }

  virtual void SaveXML(ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << string(indent, ' ') << "<" << TypeString(dummy) << "l>\n";
    file << string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (((_addmethod == 0) || (_submethod == 0)) ? "" : "W") << "</acc>\n";
    file << string(indent, ' ') << "  <maxsize>" << _maxsize << "</maxsize>\n";

    if(_valenums != 0)
    {
      file << string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << string(indent, ' ') << "  </valenums>\n";
    }

    file << string(indent, ' ') << "</" << TypeString(dummy) << "l>\n";
  }

  virtual int GetStrTbl(uint32_t eid, string &val)
  {
    T nval;
    int lerr;
    uint32_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method and check for error
    if((lerr = (_object->*_getmethod)(eid, nval)) != ERR_NONE)
    {
      val.clear();
      return lerr;
    }

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Return enum value
          val = _valenums[i]._str;
          return ERR_NONE;
        }
      }

      val.clear();
      return ERR_RANGE;
    }

    //Convert to string
    StringPrint(nval, val);
    return ERR_NONE;
  }

  virtual int AddStr(const string &val)
  {
    T nval;
    uint32_t i;

    //Check for null method
    if(_addmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Add value
          return (_object->*_addmethod)(nval);
        }
      }

      return ERR_RANGE;
    }

    //Add value
    return (_object->*_addmethod)(nval);
  }

  virtual int AddStrLit(const string &val)
  {
    uint32_t i;

    //Check for null method
    if(_addmethod == 0)
      return ERR_ACCESS;

    //Check for no value enums
    if(_valenums == 0)
      return ERR_TYPE;

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._str == val)
      {
        //Add value
        return (_object->*_addmethod)(_valenums[i]._num);
      }
    }

    return ERR_RANGE;
  }

  virtual int SubStr(const string &val)
  {
    T nval;
    uint32_t i;

    //Check for null method
    if(_submethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Check for value enums
    if(_valenums != 0)
    {
      //Loop through enums
      for(i=0; _valenums[i]._str.length() != 0; i++)
      {
        //Check for match
        if(_valenums[i]._num == nval)
        {
          //Subtract value
          return (_object->*_submethod)(nval);
        }
      }

      return ERR_RANGE;
    }

    //Subtract value
    return (_object->*_submethod)(nval);
  }

  virtual int SubStrLit(const string &val)
  {
    uint32_t i;

    //Check for null method
    if(_submethod == 0)
      return ERR_ACCESS;

    //Check for no value enums
    if(_valenums == 0)
      return ERR_TYPE;

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._str == val)
      {
        //Subtract value
        return (_object->*_submethod)(_valenums[i]._num);
      }
    }

    return ERR_RANGE;
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);

      //Check for incorrect type, report this type and value with type error
      if(type != TypeCode(val))
        lerr = ERR_TYPE;
    }
    else
    {
      //Access error
      DefaultVal(val);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool AddCell(HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val))
    {
      //Skip value in inbound cell and check for error
      if(!SkipValue(icell, type))
        return false;

      //Write type code to outbound cell and check for error
      if(!ocell->Write(TypeCode(val)))
        return false;

      //Write type error code to outbound cell and check for error
      if(!ocell->Write(ERR_TYPE))
        return false;

      return true;
    }

    //Get value from inbound cell and check for error
    if(!icell->Read(val))
      return false;

    //Check for valid method
    if(_addmethod != 0)
    {
      //Call add method
      lerr = (_object->*_addmethod)(val);
    }
    else
    {
      //Access error
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SubCell(HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val))
    {
      //Skip value in inbound cell and check for error
      if(!SkipValue(icell, type))
        return false;

      //Write type code to outbound cell and check for error
      if(!ocell->Write(TypeCode(val)))
        return false;

      //Write type error code to outbound cell and check for error
      if(!ocell->Write(ERR_TYPE))
        return false;

      return true;
    }

    //Get value from inbound cell and check for error
    if(!icell->Read(val))
      return false;

    //Check for valid method
    if(_submethod != 0)
    {
      //Call subtract method
      lerr = (_object->*_submethod)(val);
    }
    else
    {
      //Access error
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool GetValEnumStr(uint32_t ind, string &str)
  {
    uint32_t i;

    //Check for no value enums
    if(_valenums == 0)
    {
      str.clear();
      return false;
    }

    //Count value enums
    for(i=0; _valenums[i]._str.length() != 0; i++);

    //Check for index out of range
    if(ind >= i)
    {
      str.clear();
      return false;
    }

    //Return value enum string
    str = _valenums[ind]._str;
    return true;
  }

private:
  C *_object;
  GetMethod _getmethod;
  AddMethod _addmethod;
  SubMethod _submethod;
  uint32_t _maxsize;
  const HCIntegerEnum<T> *_valenums;
};

//Derived classes
template <class C>
class HCSigned8List : public HCIntegerList<C, int8_t>
{
public:
  HCSigned8List(const string &name, C *object, int (C::*getmethod)(uint32_t, int8_t &), int (C::*addmethod)(const int8_t), int (C::*submethod)(const int8_t), uint32_t maxlen, const HCSigned8Enum *valenums=0)
  : HCIntegerList<C, int8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCSigned16List : public HCIntegerList<C, int16_t>
{
public:
  HCSigned16List(const string &name, C *object, int (C::*getmethod)(uint32_t, int16_t &), int (C::*addmethod)(const int16_t), int (C::*submethod)(const int16_t), uint32_t maxlen, const HCSigned16Enum *valenums=0)
  : HCIntegerList<C, int16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCSigned32List : public HCIntegerList<C, int32_t>
{
public:
  HCSigned32List(const string &name, C *object, int (C::*getmethod)(uint32_t, int32_t &), int (C::*addmethod)(const int32_t), int (C::*submethod)(const int32_t), uint32_t maxlen, const HCSigned32Enum *valenums=0)
  : HCIntegerList<C, int32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCSigned64List : public HCIntegerList<C, int64_t>
{
public:
  HCSigned64List(const string &name, C *object, int (C::*getmethod)(uint32_t, int64_t &), int (C::*addmethod)(const int64_t), int (C::*submethod)(const int64_t), uint32_t maxlen, const HCSigned64Enum *valenums=0)
  : HCIntegerList<C, int64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUnsigned8List : public HCIntegerList<C, uint8_t>
{
public:
  HCUnsigned8List(const string &name, C *object, int (C::*getmethod)(uint32_t, uint8_t &), int (C::*addmethod)(const uint8_t), int (C::*submethod)(const uint8_t), uint32_t maxlen, const HCUnsigned8Enum *valenums=0)
  : HCIntegerList<C, uint8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUnsigned16List : public HCIntegerList<C, uint16_t>
{
public:
  HCUnsigned16List(const string &name, C *object, int (C::*getmethod)(uint32_t, uint16_t &), int (C::*addmethod)(const uint16_t), int (C::*submethod)(const uint16_t), uint32_t maxlen, const HCUnsigned16Enum *valenums=0)
  : HCIntegerList<C, uint16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUnsigned32List : public HCIntegerList<C, uint32_t>
{
public:
  HCUnsigned32List(const string &name, C *object, int (C::*getmethod)(uint32_t, uint32_t &), int (C::*addmethod)(const uint32_t), int (C::*submethod)(const uint32_t), uint32_t maxlen, const HCUnsigned32Enum *valenums=0)
  : HCIntegerList<C, uint32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUnsigned64List : public HCIntegerList<C, uint64_t>
{
public:
  HCUnsigned64List(const string &name, C *object, int (C::*getmethod)(uint32_t, uint64_t &), int (C::*addmethod)(const uint64_t), int (C::*submethod)(const uint64_t), uint32_t maxlen, const HCUnsigned64Enum *valenums=0)
  : HCIntegerList<C, uint64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

#endif //_HCINTEGER_HH_
