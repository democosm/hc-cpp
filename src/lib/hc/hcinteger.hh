// HC integer templates
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

#include "const.hh"
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

//-----------------------------------------------------------------------------
//Integer enumeration template
//-----------------------------------------------------------------------------
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

  HCIntegerEnum(T num, const std::string& str)
  {
    //Initialize member variables
    _num = num;
    _str = str;
  }

  HCIntegerEnum(tinyxml2::XMLElement* pelt)
  {
    tinyxml2::XMLElement* elt;
    std::string numstr;
    std::string eqstr;
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
        if((seppos = eqstr.find(',')) == std::string::npos)
        {
          std::cout << __FILE__ << ' ' << __LINE__ << " - Delimiter not found in equality (" << eqstr << ")\n";
          return;
        }
        numstr = eqstr.substr(0, seppos);

        //Convert number and check for error
        if(!StringConvert(numstr.c_str(), _num))
        {
          std::cout << __FILE__ << ' ' << __LINE__ << " - Error converting enumeration number (" << numstr << ")\n";
          return;
        }

        //Copy string
        _str = eqstr.substr(seppos+1, eqstr.length()-seppos);
      }
    }
  }

public:
  T _num;
  std::string _str;
};

//-----------------------------------------------------------------------------
//Integer enumerations
//-----------------------------------------------------------------------------
typedef HCIntegerEnum<int8_t> HCInt8Enum;
typedef HCIntegerEnum<int16_t> HCInt16Enum;
typedef HCIntegerEnum<int32_t> HCInt32Enum;
typedef HCIntegerEnum<int64_t> HCInt64Enum;
typedef HCIntegerEnum<uint8_t> HCUns8Enum;
typedef HCIntegerEnum<uint16_t> HCUns16Enum;
typedef HCIntegerEnum<uint32_t> HCUns32Enum;
typedef HCIntegerEnum<uint64_t> HCUns64Enum;

//-----------------------------------------------------------------------------
//Integer client stub template
//-----------------------------------------------------------------------------
template <class T>
class HCIntegerCli
{
public:
  HCIntegerCli(HCClient* cli, uint16_t pid)
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

  int Get(T& val)
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

  int IGet(uint32_t eid, T& val)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val);
  }

  int ISet(uint32_t eid, const T val)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val);
  }

  int Get(T* val, uint16_t maxlen, uint16_t& len)
  {
    //Delegate to client
    return _cli->Get(_pid, val, maxlen, len);
  }

  int Set(const T* val, uint16_t len)
  {
    //Delegate to client
    return _cli->Set(_pid, val, len);
  }

private:
  HCClient* _cli;
  uint16_t _pid;
};

//-----------------------------------------------------------------------------
//Integer client stubs
//-----------------------------------------------------------------------------
typedef HCIntegerCli<int8_t> HCInt8Cli;
typedef HCIntegerCli<int16_t> HCInt16Cli;
typedef HCIntegerCli<int32_t> HCInt32Cli;
typedef HCIntegerCli<int64_t> HCInt64Cli;
typedef HCIntegerCli<uint8_t> HCUns8Cli;
typedef HCIntegerCli<uint16_t> HCUns16Cli;
typedef HCIntegerCli<uint32_t> HCUns32Cli;
typedef HCIntegerCli<uint64_t> HCUns64Cli;

//-----------------------------------------------------------------------------
//Integer template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCInteger : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T&);
  typedef int (C::*SetMethod)(const T);

public:
  HCInteger(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, const HCIntegerEnum<T>* valenums)
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
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val);

    //Check for no value enums
    if(_valenums == 0)
    {
      //Print value
      std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
      std::cout << " = ";
      std::cout << PrintCast(val);
      std::cout << " = ";
      std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
      std::cout << " !" << ErrToString(lerr);
      std::cout << TC_RESET << "\n";

      return;
    }

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._num == val)
      {
        //Print value
        std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
        std::cout << " = ";
        std::cout << PrintCast(val);
        std::cout << " = ";
        std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
        std::cout << " = ";
        std::cout << "\"" << _valenums[i]._str << "\"";
        std::cout << " !" << ErrToString(lerr);
        std::cout << TC_RESET << "\n";

        return;
      }
    }

    //Print value indicate that no enum found
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";
    std::cout << PrintCast(val);
    std::cout << " = ";
    std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
    std::cout << " = ";
    std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val;
    uint32_t i;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Get value
    if((_object->*_getmethod)(val) != ERR_NONE)
      return;

    //Check for no value enums
    if(_valenums == 0)
    {
      //Print value
      st << path;
      st << _name;
      st << " = ";
      st << PrintCast(val);
      st << "\n";

      return;
    }

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._num == val)
      {
        //Print value
        st << path;
        st << _name;
        st << " = ";
        st << "\"" << _valenums[i]._str << "\"";
        st << "\n";

        return;
      }
    }

    //Print value indicate that no enum found
    st << path;
    st << _name;
    st << " = ";
    st << PrintCast(val);
    st << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");

    //Print enum information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << ">\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << std::string(indent, ' ') << "  </valenums>\n";
    }

    file << std::string(indent, ' ') << "</" << HCParameter::TypeString(dummy) << ">\n";
  }

  virtual int GetInt(T& val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val);
  }

  virtual int SetInt(const T val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val);
  }

  virtual int GetStr(std::string& val)
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

  virtual int SetStr(const std::string& val)
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

  virtual int SetStrLit(const std::string& val)
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

  virtual bool GetCell(HCCell* icell, HCCell* ocell)
  {
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(val);
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

  virtual bool SetCell(HCCell* icell, HCCell* ocell)
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

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool GetValEnumStr(uint32_t ind, std::string& str)
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
  C* _object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  const HCIntegerEnum<T>* _valenums;
};

//-----------------------------------------------------------------------------
//Integer template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerS : public HCInteger<C, T>
{
public:
  HCIntegerS(const std::string& name, C* object, int (C::*getmethod)(T&), int (C::*setmethod)(const T), const HCIntegerEnum<T>* valenums)
  : HCInteger<C, T>(name, object, getmethod, setmethod, valenums)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from integer templates
//-----------------------------------------------------------------------------
template <class C>
class HCInt8 : public HCInteger<C, int8_t>
{
public:
  HCInt8(const std::string& name, C* object, int (C::*getmethod)(int8_t&), int (C::*setmethod)(const int8_t), const HCInt8Enum* valenums)
  : HCInteger<C, int8_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt8(const std::string& name, C* object, int (C::*getmethod)(int8_t&), int (C::*setmethod)(const int8_t))
  : HCInteger<C, int8_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt8S : public HCIntegerS<C, int8_t>
{
public:
  HCInt8S(const std::string& name, C* object, int (C::*getmethod)(int8_t&), int (C::*setmethod)(const int8_t), const HCInt8Enum* valenums)
  : HCIntegerS<C, int8_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt8S(const std::string& name, C* object, int (C::*getmethod)(int8_t&), int (C::*setmethod)(const int8_t))
  : HCIntegerS<C, int8_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt16 : public HCInteger<C, int16_t>
{
public:
  HCInt16(const std::string& name, C* object, int (C::*getmethod)(int16_t&), int (C::*setmethod)(const int16_t), const HCInt16Enum* valenums)
  : HCInteger<C, int16_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt16(const std::string& name, C* object, int (C::*getmethod)(int16_t&), int (C::*setmethod)(const int16_t))
  : HCInteger<C, int16_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt16S : public HCIntegerS<C, int16_t>
{
public:
  HCInt16S(const std::string& name, C* object, int (C::*getmethod)(int16_t&), int (C::*setmethod)(const int16_t), const HCInt16Enum* valenums)
  : HCIntegerS<C, int16_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt16S(const std::string& name, C* object, int (C::*getmethod)(int16_t&), int (C::*setmethod)(const int16_t))
  : HCIntegerS<C, int16_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt32 : public HCInteger<C, int32_t>
{
public:
  HCInt32(const std::string& name, C* object, int (C::*getmethod)(int32_t&), int (C::*setmethod)(const int32_t), const HCInt32Enum* valenums)
  : HCInteger<C, int32_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt32(const std::string& name, C* object, int (C::*getmethod)(int32_t&), int (C::*setmethod)(const int32_t))
  : HCInteger<C, int32_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt32S : public HCIntegerS<C, int32_t>
{
public:
  HCInt32S(const std::string& name, C* object, int (C::*getmethod)(int32_t&), int (C::*setmethod)(const int32_t), const HCInt32Enum* valenums)
  : HCIntegerS<C, int32_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt32S(const std::string& name, C* object, int (C::*getmethod)(int32_t&), int (C::*setmethod)(const int32_t))
  : HCIntegerS<C, int32_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt64 : public HCInteger<C, int64_t>
{
public:
  HCInt64(const std::string& name, C* object, int (C::*getmethod)(int64_t&), int (C::*setmethod)(const int64_t), const HCInt64Enum* valenums)
  : HCInteger<C, int64_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt64(const std::string& name, C* object, int (C::*getmethod)(int64_t&), int (C::*setmethod)(const int64_t))
  : HCInteger<C, int64_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCInt64S : public HCIntegerS<C, int64_t>
{
public:
  HCInt64S(const std::string& name, C* object, int (C::*getmethod)(int64_t&), int (C::*setmethod)(const int64_t), const HCInt64Enum* valenums)
  : HCIntegerS<C, int64_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCInt64S(const std::string& name, C* object, int (C::*getmethod)(int64_t&), int (C::*setmethod)(const int64_t))
  : HCIntegerS<C, int64_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns8 : public HCInteger<C, uint8_t>
{
public:
  HCUns8(const std::string& name, C* object, int (C::*getmethod)(uint8_t&), int (C::*setmethod)(const uint8_t), const HCUns8Enum* valenums)
  : HCInteger<C, uint8_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns8(const std::string& name, C* object, int (C::*getmethod)(uint8_t&), int (C::*setmethod)(const uint8_t))
  : HCInteger<C, uint8_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns8S : public HCIntegerS<C, uint8_t>
{
public:
  HCUns8S(const std::string& name, C* object, int (C::*getmethod)(uint8_t&), int (C::*setmethod)(const uint8_t), const HCUns8Enum* valenums)
  : HCIntegerS<C, uint8_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns8S(const std::string& name, C* object, int (C::*getmethod)(uint8_t&), int (C::*setmethod)(const uint8_t))
  : HCIntegerS<C, uint8_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns16 : public HCInteger<C, uint16_t>
{
public:
  HCUns16(const std::string& name, C* object, int (C::*getmethod)(uint16_t&), int (C::*setmethod)(const uint16_t), const HCUns16Enum* valenums)
  : HCInteger<C, uint16_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns16(const std::string& name, C* object, int (C::*getmethod)(uint16_t&), int (C::*setmethod)(const uint16_t))
  : HCInteger<C, uint16_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns16S : public HCIntegerS<C, uint16_t>
{
public:
  HCUns16S(const std::string& name, C* object, int (C::*getmethod)(uint16_t&), int (C::*setmethod)(const uint16_t), const HCUns16Enum* valenums)
  : HCIntegerS<C, uint16_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns16S(const std::string& name, C* object, int (C::*getmethod)(uint16_t&), int (C::*setmethod)(const uint16_t))
  : HCIntegerS<C, uint16_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns32 : public HCInteger<C, uint32_t>
{
public:
  HCUns32(const std::string& name, C* object, int (C::*getmethod)(uint32_t&), int (C::*setmethod)(const uint32_t), const HCUns32Enum* valenums)
  : HCInteger<C, uint32_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns32(const std::string& name, C* object, int (C::*getmethod)(uint32_t&), int (C::*setmethod)(const uint32_t))
  : HCInteger<C, uint32_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns32S : public HCIntegerS<C, uint32_t>
{
public:
  HCUns32S(const std::string& name, C* object, int (C::*getmethod)(uint32_t&), int (C::*setmethod)(const uint32_t), const HCUns32Enum* valenums)
  : HCIntegerS<C, uint32_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns32S(const std::string& name, C* object, int (C::*getmethod)(uint32_t&), int (C::*setmethod)(const uint32_t))
  : HCIntegerS<C, uint32_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns64 : public HCInteger<C, uint64_t>
{
public:
  HCUns64(const std::string& name, C* object, int (C::*getmethod)(uint64_t&), int (C::*setmethod)(const uint64_t), const HCUns64Enum* valenums)
  : HCInteger<C, uint64_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns64(const std::string& name, C* object, int (C::*getmethod)(uint64_t&), int (C::*setmethod)(const uint64_t))
  : HCInteger<C, uint64_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

template <class C>
class HCUns64S : public HCIntegerS<C, uint64_t>
{
public:
  HCUns64S(const std::string& name, C* object, int (C::*getmethod)(uint64_t&), int (C::*setmethod)(const uint64_t), const HCUns64Enum* valenums)
  : HCIntegerS<C, uint64_t>(name, object, getmethod, setmethod, valenums)
  {
  }

  HCUns64S(const std::string& name, C* object, int (C::*getmethod)(uint64_t&), int (C::*setmethod)(const uint64_t))
  : HCIntegerS<C, uint64_t>(name, object, getmethod, setmethod, 0)
  {
  }
};

//-----------------------------------------------------------------------------
//Integer table template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T&);
  typedef int (C::*SetMethod)(uint32_t, const T);

public:
  HCIntegerTable(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum* eidenums, const HCIntegerEnum<T>* valenums)
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
    std::string valstr;
    std::string eidstr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

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
          std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
          std::cout << "[" << eid << "]";
          std::cout << " = ";
          std::cout << PrintCast(val);
          std::cout << " = ";
          std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
        else
        {
          //Convert EID to enum string and check for error
          if(!EIDNumToStr(eid, eidstr))
          {
            //Print value (indicate no EID enum string found)
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[" << TC_MAGENTA << eid << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << "]";
            std::cout << " = ";
            std::cout << PrintCast(val);
            std::cout << " = ";
            std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
          }
          else
          {
            //Print value (show EID enum string)
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[\"" << eidstr << "\"]";
            std::cout << " = ";
            std::cout << PrintCast(val);
            std::cout << " = ";
            std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
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
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[" << eid << "]";
            std::cout << " = ";
            std::cout << PrintCast(val);
            std::cout << " = ";
            std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
            std::cout << " = ";
            std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
          }
          else
          {
            //Print value (show value enum string)
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[" << eid << "]";
            std::cout << " = ";
            std::cout << PrintCast(val);
            std::cout << " = ";
            std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
            std::cout << " = ";
            std::cout << "\"" << valstr << "\"";
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
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
              std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
              std::cout << "[" << TC_MAGENTA << eid << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << "]";
              std::cout << " = ";
              std::cout << PrintCast(val);
              std::cout << " = ";
              std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
              std::cout << " = ";
              std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
              std::cout << " !" << ErrToString(lerr);
              std::cout << TC_RESET << "\n";
            }
            else
            {
              //Print value (show EID enum string, indicate no value enum string found)
              std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
              std::cout << "[\"" << eidstr << "\"]";
              std::cout << " = ";
              std::cout << PrintCast(val);
              std::cout << " = ";
              std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
              std::cout << " = ";
              std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
              std::cout << " !" << ErrToString(lerr);
              std::cout << TC_RESET << "\n";
            }
          }
          else
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no EID enum string found, show value enum string)
              std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
              std::cout << "[" << TC_MAGENTA << eid << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << "]";
              std::cout << " = ";
              std::cout << PrintCast(val);
              std::cout << " = ";
              std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
              std::cout << " = ";
              std::cout << "\"" << valstr << "\"";
              std::cout << " !" << ErrToString(lerr);
              std::cout << TC_RESET << "\n";
            }
            else
            {
              //Print value (show EID enum string and value enum string)
              std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
              std::cout << "[\"" << eidstr << "\"]";
              std::cout << " = ";
              std::cout << PrintCast(val);
              std::cout << " = ";
              std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
              std::cout << " = ";
              std::cout << "\"" << valstr << "\"";
              std::cout << " !" << ErrToString(lerr);
              std::cout << TC_RESET << "\n";
            }
          }
        }
      }
    }
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val;
    uint32_t eid;
    std::string valstr;
    std::string eidstr;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Loop through all elements
    for(eid=0; eid<_size; eid++)
    {
      //Get value
      if((_object->*_getmethod)(eid, val) != ERR_NONE)
        continue;

      //Check for no value enums
      if(_valenums == 0)
      {
        //Check for no EID enums
        if(_eidenums == 0)
        {
          //Print value
          st << path;
          st << _name;
          st << "[" << eid << "]";
          st << " = ";
          st << PrintCast(val);
          st << "\n";
        }
        else
        {
          //Convert EID to enum string and check for error
          if(!EIDNumToStr(eid, eidstr))
          {
            //Print value (indicate no EID enum string found)
            st << path;
            st << _name;
            st << "[" << eid << "]";
            st << " = ";
            st << PrintCast(val);
            st << "\n";
          }
          else
          {
            //Print value (show EID enum string)
            st << path;
            st << _name;
            st << "[\"" << eidstr << "\"]";
            st << " = ";
            st << PrintCast(val);
            st << "\n";
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
            st << path;
            st << _name;
            st << "[" << eid << "]";
            st << " = ";
            st << PrintCast(val);
            st << "\n";
          }
          else
          {
            //Print value (show value enum string)
            st << path;
            st << _name;
            st << "[" << eid << "]";
            st << " = ";
            st << "\"" << valstr << "\"";
            st << "\n";
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
              st << path;
              st << _name;
              st << "[" << eid << "]";
              st << " = ";
              st << PrintCast(val);
              st << "\n";
            }
            else
            {
              //Print value (show EID enum string, indicate no value enum string found)
              st << path;
              st << _name;
              st << "[\"" << eidstr << "\"]";
              st << " = ";
              st << PrintCast(val);
              st << "\n";
            }
          }
          else
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no EID enum string found, show value enum string)
              st << path;
              st << _name;
              st << "[" << eid << "]";
              st << " = ";
              st << "\"" << valstr << "\"";
              st << "\n";
            }
            else
            {
              //Print value (show EID enum string and value enum string)
              st << path;
              st << _name;
              st << "[\"" << eidstr << "\"]";
              st << " = ";
              st << "\"" << valstr << "\"";
              st << "\n";
            }
          }
        }
      }
    }
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy) << "t";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
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

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << "t>\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>\n";

      file << std::string(indent, ' ') << "  </eidenums>\n";
    }

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << std::string(indent, ' ') << "  </valenums>\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy) << "t>\n";
  }

  virtual int GetIntTbl(uint32_t eid, T& val)
  {
    //Check for EID out of range
    if(eid >= _size)
    {
      DefaultVal(val);
      return ERR_EID;
    }

    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val);
  }

  virtual int SetIntTbl(uint32_t eid, const T val)
  {
    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(eid, val);
  }

  virtual int GetStrTbl(uint32_t eid, std::string& val)
  {
    T nval;
    int lerr;
    uint32_t i;

    //Check for EID out of range
    if(eid >= _size)
    {
      val.clear();
      return ERR_EID;
    }

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

  virtual int SetStrTbl(uint32_t eid, const std::string& val)
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

  virtual int SetStrLitTbl(uint32_t eid, const std::string& val)
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

  virtual bool GetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);
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

  virtual bool SetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
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

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual uint32_t GetNumEIDs(void)
  {
    return _size;
  }

  virtual bool EIDStrToNum(const std::string& str, uint32_t& num)
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

  virtual bool EIDNumToStr(uint32_t num, std::string& str)
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

  virtual bool GetValEnumStr(uint32_t ind, std::string& str)
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

  bool ValNumToStr(T num, std::string& str)
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
  C* _object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  uint32_t _size;
  const HCIntegerEnum<T>* _valenums;
  const HCEIDEnum* _eidenums;
};

//-----------------------------------------------------------------------------
//Integer table template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerTableS : public HCIntegerTable<C, T>
{
public:
  HCIntegerTableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, T&), int (C::*setmethod)(uint32_t, const T), uint32_t size, const HCEIDEnum* eidenums, const HCIntegerEnum<T>* valenums)
  : HCIntegerTable<C, T>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from integer table templates
//-----------------------------------------------------------------------------
template <class C>
class HCInt8Table : public HCIntegerTable<C, int8_t>
{
public:
  HCInt8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size)
  : HCIntegerTable<C, int8_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCInt8Enum* valenums)
  : HCIntegerTable<C, int8_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, int8_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt8Enum* valenums)
  : HCIntegerTable<C, int8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt8TableS : public HCIntegerTableS<C, int8_t>
{
public:
  HCInt8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size)
  : HCIntegerTableS<C, int8_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCInt8Enum* valenums)
  : HCIntegerTableS<C, int8_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, int8_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*setmethod)(uint32_t, const int8_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt8Enum* valenums)
  : HCIntegerTableS<C, int8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt16Table : public HCIntegerTable<C, int16_t>
{
public:
  HCInt16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size)
  : HCIntegerTable<C, int16_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCInt16Enum* valenums)
  : HCIntegerTable<C, int16_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, int16_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt16Enum* valenums)
  : HCIntegerTable<C, int16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt16TableS : public HCIntegerTableS<C, int16_t>
{
public:
  HCInt16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size)
  : HCIntegerTableS<C, int16_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCInt16Enum* valenums)
  : HCIntegerTableS<C, int16_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, int16_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*setmethod)(uint32_t, const int16_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt16Enum* valenums)
  : HCIntegerTableS<C, int16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt32Table : public HCIntegerTable<C, int32_t>
{
public:
  HCInt32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size)
  : HCIntegerTable<C, int32_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCInt32Enum* valenums)
  : HCIntegerTable<C, int32_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, int32_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt32Enum* valenums)
  : HCIntegerTable<C, int32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt32TableS : public HCIntegerTableS<C, int32_t>
{
public:
  HCInt32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size)
  : HCIntegerTableS<C, int32_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCInt32Enum* valenums)
  : HCIntegerTableS<C, int32_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, int32_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*setmethod)(uint32_t, const int32_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt32Enum* valenums)
  : HCIntegerTableS<C, int32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt64Table : public HCIntegerTable<C, int64_t>
{
public:
  HCInt64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size)
  : HCIntegerTable<C, int64_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCInt64Enum* valenums)
  : HCIntegerTable<C, int64_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, int64_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt64Enum* valenums)
  : HCIntegerTable<C, int64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCInt64TableS : public HCIntegerTableS<C, int64_t>
{
public:
  HCInt64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size)
  : HCIntegerTableS<C, int64_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCInt64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCInt64Enum* valenums)
  : HCIntegerTableS<C, int64_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCInt64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, int64_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCInt64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*setmethod)(uint32_t, const int64_t), uint32_t size, const HCEIDEnum* eidenums, const HCInt64Enum* valenums)
  : HCIntegerTableS<C, int64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns8Table : public HCIntegerTable<C, uint8_t>
{
public:
  HCUns8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size)
  : HCIntegerTable<C, uint8_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCUns8Enum* valenums)
  : HCIntegerTable<C, uint8_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, uint8_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns8Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns8Enum* valenums)
  : HCIntegerTable<C, uint8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns8TableS : public HCIntegerTableS<C, uint8_t>
{
public:
  HCUns8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size)
  : HCIntegerTableS<C, uint8_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCUns8Enum* valenums)
  : HCIntegerTableS<C, uint8_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, uint8_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns8TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*setmethod)(uint32_t, const uint8_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns8Enum* valenums)
  : HCIntegerTableS<C, uint8_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns16Table : public HCIntegerTable<C, uint16_t>
{
public:
  HCUns16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size)
  : HCIntegerTable<C, uint16_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCUns16Enum* valenums)
  : HCIntegerTable<C, uint16_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, uint16_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns16Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns16Enum* valenums)
  : HCIntegerTable<C, uint16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns16TableS : public HCIntegerTableS<C, uint16_t>
{
public:
  HCUns16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size)
  : HCIntegerTableS<C, uint16_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCUns16Enum* valenums)
  : HCIntegerTableS<C, uint16_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, uint16_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns16TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*setmethod)(uint32_t, const uint16_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns16Enum* valenums)
  : HCIntegerTableS<C, uint16_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns32Table : public HCIntegerTable<C, uint32_t>
{
public:
  HCUns32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size)
  : HCIntegerTable<C, uint32_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCUns32Enum* valenums)
  : HCIntegerTable<C, uint32_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, uint32_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns32Enum* valenums)
  : HCIntegerTable<C, uint32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns32TableS : public HCIntegerTableS<C, uint32_t>
{
public:
  HCUns32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size)
  : HCIntegerTableS<C, uint32_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCUns32Enum* valenums)
  : HCIntegerTableS<C, uint32_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, uint32_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*setmethod)(uint32_t, const uint32_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns32Enum* valenums)
  : HCIntegerTableS<C, uint32_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns64Table : public HCIntegerTable<C, uint64_t>
{
public:
  HCUns64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size)
  : HCIntegerTable<C, uint64_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCUns64Enum* valenums)
  : HCIntegerTable<C, uint64_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTable<C, uint64_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns64Enum* valenums)
  : HCIntegerTable<C, uint64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCUns64TableS : public HCIntegerTableS<C, uint64_t>
{
public:
  HCUns64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size)
  : HCIntegerTableS<C, uint64_t>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCUns64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCUns64Enum* valenums)
  : HCIntegerTableS<C, uint64_t>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCUns64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCEIDEnum* eidenums)
  : HCIntegerTableS<C, uint64_t>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCUns64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*setmethod)(uint32_t, const uint64_t), uint32_t size, const HCEIDEnum* eidenums, const HCUns64Enum* valenums)
  : HCIntegerTableS<C, uint64_t>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

//-----------------------------------------------------------------------------
//Integer list template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerList : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T&);
  typedef int (C::*AddMethod)(const T);
  typedef int (C::*SubMethod)(const T);

public:
  HCIntegerList(const std::string& name, C* object, GetMethod getmethod, AddMethod addmethod, SubMethod submethod, uint32_t maxsize, const HCIntegerEnum<T>* valenums)
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
      //Print value as not readable
      PrintNotReadable();

      return;
    }

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
        std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
        std::cout << "[" << eid << "]";
        std::cout << " = ";
        std::cout << PrintCast(val);
        std::cout << " = ";
        std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
        std::cout << " !" << ErrToString(lerr);
        std::cout << TC_RESET << "\n";
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
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[" << eid << "]";
            std::cout << " = ";
            std::cout << PrintCast(val);
            std::cout << " = ";
            std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
            std::cout << " = ";
            std::cout << "\"" << _valenums[i]._str << "\"";
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";

            break;
          }
        }

        //Print value indicate that no enum found
        if(_valenums[i]._str.length() == 0)
        {
          std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
          std::cout << "[" << eid << "]";
          std::cout << " = ";
          std::cout << PrintCast(val);
          std::cout << " = ";
          std::cout << "0x" << std::hex << PrintCast(val) << std::dec;
          std::cout << " = ";
          std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
      }
    }

    //Check for no values in list
    if(eid == 0)
    {
      std::cout << TC_GREEN << _name;
      std::cout << "[]";
      std::cout << TC_RESET << "\n";
    }
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val;
    uint32_t i;
    uint32_t eid;

    //Check for not saveable
    if((_getmethod == 0) || (_addmethod == 0))
      return;

    //Loop through all elements
    for(eid=0; eid<_maxsize; eid++)
    {
      //Get value
      if((_object->*_getmethod)(eid, val) != ERR_NONE)
        break;

      //Check for no value enums
      if(_valenums == 0)
      {
        //Print value
        st << path;
        st << _name;
        st << " < ";
        st << PrintCast(val);
        st << "\n";
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
            st << path;
            st << _name;
            st << " < ";
            st << "\"" << _valenums[i]._str << "\"";
            st << "\n";

            break;
          }
        }

        //Print value indicate that no enum found
        if(_valenums[i]._str.length() == 0)
        {
          st << path;
          st << _name;
          st << " < ";
          st << PrintCast(val);
          st << "\n";
        }
      }
    }
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy) << "l";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << ((_addmethod == 0) && (_submethod == 0) ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Max Size: " << _maxsize;

    //Print enum information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << "l>\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (((_addmethod == 0) || (_submethod == 0)) ? "" : "W") << "</acc>\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <maxsize>" << _maxsize << "</maxsize>\n";

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << PrintCast(_valenums[i]._num) << "," << _valenums[i]._str << "</eq>\n";

      file << std::string(indent, ' ') << "  </valenums>\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy) << "l>\n";
  }

  virtual int GetIntTbl(uint32_t eid, T& val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val);
  }

  virtual int AddInt(const T val)
  {
    //Check for null method
    if(_addmethod == 0)
      return ERR_ACCESS;

    //Call add method
    return (_object->*_addmethod)(val);
  }

  virtual int SubInt(const T val)
  {
    //Check for null method
    if(_submethod == 0)
      return ERR_ACCESS;

    //Call sub method
    return (_object->*_submethod)(val);
  }

  virtual int GetStrTbl(uint32_t eid, std::string& val)
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

  virtual int AddStr(const std::string& val)
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

  virtual int AddStrLit(const std::string& val)
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

  virtual int SubStr(const std::string& val)
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

  virtual int SubStrLit(const std::string& val)
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

  virtual bool GetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    T val;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);
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

  virtual bool AddCell(HCCell* icell, HCCell* ocell)
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

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SubCell(HCCell* icell, HCCell* ocell)
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

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool GetValEnumStr(uint32_t ind, std::string& str)
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
  C* _object;
  GetMethod _getmethod;
  AddMethod _addmethod;
  SubMethod _submethod;
  uint32_t _maxsize;
  const HCIntegerEnum<T>* _valenums;
};

//-----------------------------------------------------------------------------
//Integer list template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerListS : public HCIntegerList<C, T>
{
public:
  HCIntegerListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, T&), int (C::*addmethod)(const T), int (C::*submethod)(const T), uint32_t maxsize, const HCIntegerEnum<T>* valenums)
  : HCIntegerList<C, T>(name, object, getmethod, addmethod, submethod, maxsize, valenums)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from integer list templates
//-----------------------------------------------------------------------------
template <class C>
class HCInt8List : public HCIntegerList<C, int8_t>
{
public:
  HCInt8List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*addmethod)(const int8_t), int (C::*submethod)(const int8_t), uint32_t maxlen)
  : HCIntegerList<C, int8_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt8List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*addmethod)(const int8_t), int (C::*submethod)(const int8_t), uint32_t maxlen, const HCInt8Enum* valenums)
  : HCIntegerList<C, int8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt8ListS : public HCIntegerListS<C, int8_t>
{
public:
  HCInt8ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*addmethod)(const int8_t), int (C::*submethod)(const int8_t), uint32_t maxlen)
  : HCIntegerListS<C, int8_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt8ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int8_t&), int (C::*addmethod)(const int8_t), int (C::*submethod)(const int8_t), uint32_t maxlen, const HCInt8Enum* valenums)
  : HCIntegerListS<C, int8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt16List : public HCIntegerList<C, int16_t>
{
public:
  HCInt16List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*addmethod)(const int16_t), int (C::*submethod)(const int16_t), uint32_t maxlen)
  : HCIntegerList<C, int16_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt16List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*addmethod)(const int16_t), int (C::*submethod)(const int16_t), uint32_t maxlen, const HCInt16Enum* valenums)
  : HCIntegerList<C, int16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt16ListS : public HCIntegerListS<C, int16_t>
{
public:
  HCInt16ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*addmethod)(const int16_t), int (C::*submethod)(const int16_t), uint32_t maxlen)
  : HCIntegerListS<C, int16_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt16ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int16_t&), int (C::*addmethod)(const int16_t), int (C::*submethod)(const int16_t), uint32_t maxlen, const HCInt16Enum* valenums)
  : HCIntegerListS<C, int16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt32List : public HCIntegerList<C, int32_t>
{
public:
  HCInt32List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*addmethod)(const int32_t), int (C::*submethod)(const int32_t), uint32_t maxlen)
  : HCIntegerList<C, int32_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt32List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*addmethod)(const int32_t), int (C::*submethod)(const int32_t), uint32_t maxlen, const HCInt32Enum* valenums)
  : HCIntegerList<C, int32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt32ListS : public HCIntegerListS<C, int32_t>
{
public:
  HCInt32ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*addmethod)(const int32_t), int (C::*submethod)(const int32_t), uint32_t maxlen)
  : HCIntegerListS<C, int32_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt32ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int32_t&), int (C::*addmethod)(const int32_t), int (C::*submethod)(const int32_t), uint32_t maxlen, const HCInt32Enum* valenums)
  : HCIntegerListS<C, int32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt64List : public HCIntegerList<C, int64_t>
{
public:
  HCInt64List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*addmethod)(const int64_t), int (C::*submethod)(const int64_t), uint32_t maxlen)
  : HCIntegerList<C, int64_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt64List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*addmethod)(const int64_t), int (C::*submethod)(const int64_t), uint32_t maxlen, const HCInt64Enum* valenums)
  : HCIntegerList<C, int64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCInt64ListS : public HCIntegerListS<C, int64_t>
{
public:
  HCInt64ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*addmethod)(const int64_t), int (C::*submethod)(const int64_t), uint32_t maxlen)
  : HCIntegerListS<C, int64_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCInt64ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, int64_t&), int (C::*addmethod)(const int64_t), int (C::*submethod)(const int64_t), uint32_t maxlen, const HCInt64Enum* valenums)
  : HCIntegerListS<C, int64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns8List : public HCIntegerList<C, uint8_t>
{
public:
  HCUns8List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*addmethod)(const uint8_t), int (C::*submethod)(const uint8_t), uint32_t maxlen)
  : HCIntegerList<C, uint8_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns8List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*addmethod)(const uint8_t), int (C::*submethod)(const uint8_t), uint32_t maxlen, const HCUns8Enum* valenums)
  : HCIntegerList<C, uint8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns8ListS : public HCIntegerListS<C, uint8_t>
{
public:
  HCUns8ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*addmethod)(const uint8_t), int (C::*submethod)(const uint8_t), uint32_t maxlen)
  : HCIntegerListS<C, uint8_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns8ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint8_t&), int (C::*addmethod)(const uint8_t), int (C::*submethod)(const uint8_t), uint32_t maxlen, const HCUns8Enum* valenums)
  : HCIntegerListS<C, uint8_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns16List : public HCIntegerList<C, uint16_t>
{
public:
  HCUns16List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*addmethod)(const uint16_t), int (C::*submethod)(const uint16_t), uint32_t maxlen)
  : HCIntegerList<C, uint16_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns16List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*addmethod)(const uint16_t), int (C::*submethod)(const uint16_t), uint32_t maxlen, const HCUns16Enum* valenums)
  : HCIntegerList<C, uint16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns16ListS : public HCIntegerListS<C, uint16_t>
{
public:
  HCUns16ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*addmethod)(const uint16_t), int (C::*submethod)(const uint16_t), uint32_t maxlen)
  : HCIntegerListS<C, uint16_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns16ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint16_t&), int (C::*addmethod)(const uint16_t), int (C::*submethod)(const uint16_t), uint32_t maxlen, const HCUns16Enum* valenums)
  : HCIntegerListS<C, uint16_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns32List : public HCIntegerList<C, uint32_t>
{
public:
  HCUns32List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*addmethod)(const uint32_t), int (C::*submethod)(const uint32_t), uint32_t maxlen)
  : HCIntegerList<C, uint32_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns32List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*addmethod)(const uint32_t), int (C::*submethod)(const uint32_t), uint32_t maxlen, const HCUns32Enum* valenums)
  : HCIntegerList<C, uint32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns32ListS : public HCIntegerListS<C, uint32_t>
{
public:
  HCUns32ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*addmethod)(const uint32_t), int (C::*submethod)(const uint32_t), uint32_t maxlen)
  : HCIntegerListS<C, uint32_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns32ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint32_t&), int (C::*addmethod)(const uint32_t), int (C::*submethod)(const uint32_t), uint32_t maxlen, const HCUns32Enum* valenums)
  : HCIntegerListS<C, uint32_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns64List : public HCIntegerList<C, uint64_t>
{
public:
  HCUns64List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*addmethod)(const uint64_t), int (C::*submethod)(const uint64_t), uint32_t maxlen)
  : HCIntegerList<C, uint64_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns64List(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*addmethod)(const uint64_t), int (C::*submethod)(const uint64_t), uint32_t maxlen, const HCUns64Enum* valenums)
  : HCIntegerList<C, uint64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

template <class C>
class HCUns64ListS : public HCIntegerListS<C, uint64_t>
{
public:
  HCUns64ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*addmethod)(const uint64_t), int (C::*submethod)(const uint64_t), uint32_t maxlen)
  : HCIntegerListS<C, uint64_t>(name, object, getmethod, addmethod, submethod, maxlen, 0)
  {
  }

  HCUns64ListS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, uint64_t&), int (C::*addmethod)(const uint64_t), int (C::*submethod)(const uint64_t), uint32_t maxlen, const HCUns64Enum* valenums)
  : HCIntegerListS<C, uint64_t>(name, object, getmethod, addmethod, submethod, maxlen, valenums)
  {
  }
};

//-----------------------------------------------------------------------------
//Integer array template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerArray : public HCParameter
{
public:
  //Method signatures
  typedef int (C::* GetMethod)(T*, uint16_t maxlen, uint16_t& len);
  typedef int (C::* SetMethod)(const T*, uint16_t len);

public:
  HCIntegerArray(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, uint8_t base=16)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert((object != 0) && ((base == 8) || (base == 10) || (base == 16)));

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _base = base;
  }

  virtual ~HCIntegerArray()
  {
  }

  virtual uint8_t GetType(void)
  {
    T* val = 0;
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

  virtual void PrintVal(void)
  {
    T val[1000];
    uint16_t len;
    int lerr;
    uint16_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val, sizeof(val), len);

    //Print value
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";

    for(i = 0; i < len; i++)
    {
      if(i != 0)
        std::cout << ',';

      if(_base == 16)
        std::cout << std::hex << PrintCast(val[i]) << std::dec;
      else if(_base == 10)
        std::cout << PrintCast(val[i]);
      else if(_base == 8)
        std::cout << std::oct << PrintCast(val[i]) << std::dec;
    }

    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val[1000];
    uint16_t len;
    uint16_t i;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Get value
    if((_object->*_getmethod)(val, sizeof(val), len) != ERR_NONE)
      return;

    //Print value
    st << path;
    st << _name;
    st << " = ";

    for(i = 0; i < len; i++)
    {
      if(i != 0)
        st << ',';

      if(_base == 16)
        st << std::hex << PrintCast(val[i]) << std::dec;
      else if(_base == 10)
        st << PrintCast(val[i]);
      else if(_base == 8)
        st << std::oct << PrintCast(val[i]) << std::dec;
    }

    st << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T* dummy = 0;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T* dummy = 0;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << ">\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "</" << TypeString(dummy) << ">\n";
  }

  virtual int GetIntArr(T* val, uint16_t maxlen, uint16_t& len)
  {
    //Assert valid arguments
    assert((val != 0) && (maxlen != 0));

    //Check for null method
    if(_getmethod == 0)
    {
      len = 0;
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val, maxlen, len);
  }

  virtual int SetIntArr(const T* val, uint16_t len)
  {
    //Assert valid arguments
    assert(val != 0);

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val, len);
  }

  virtual int GetStr(std::string& val)
  {
    T nval[1000];
    uint16_t len;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method and check for error
    if((lerr = (_object->*_getmethod)(nval, sizeof(nval), len)) != ERR_NONE)
    {
      val.clear();
      return lerr;
    }

    //Convert to string
    StringPrint(nval, len, val, _base);
    return ERR_NONE;
  }

  virtual int SetStr(const std::string& val)
  {
    T nval[1000];
    uint16_t len;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval, sizeof(nval), len, _base))
      return ERR_UNSPEC;

    return (_object->*_setmethod)(nval, len);
  }

  virtual int SetStrLit(const std::string& val)
  {
    return SetStr(val);
  }

  virtual bool GetCell(HCCell* icell, HCCell* ocell)
  {
    T val[1000];
    uint16_t len;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(val, sizeof(val), len);
    }
    else
    {
      //Access error
      len = 0;
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val, len))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCell(HCCell* icell, HCCell* ocell)
  {
    uint8_t type;
    T val[1000];
    uint16_t len;
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

      //Write type error code to outbound cell and check for error
      if(!ocell->Write(ERR_TYPE))
        return false;

      return true;
    }

    //Get value from inbound cell and check for error
    if(!icell->Read(val, sizeof(val), len))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(val, len);
    }
    else
    {
      //Access error
      lerr = ERR_ACCESS;
    }

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

private:
  C* _object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  uint8_t _base;
};

//-----------------------------------------------------------------------------
//Integer array template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCIntegerArrayS : public HCIntegerArray<C, T>
{
public:
  HCIntegerArrayS(const std::string& name, C* object, int (C::*getmethod)(T*, uint16_t, uint16_t&), int (C::*setmethod)(const T*, uint16_t))
  : HCIntegerArray<C, T>(name, object, getmethod, setmethod)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from integer array templates
//-----------------------------------------------------------------------------
template <class C>
class HCInt8Array : public HCIntegerArray<C, int8_t>
{
public:
  HCInt8Array(const std::string& name, C* object, int (C::*getmethod)(int8_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int8_t*, uint16_t))
  : HCIntegerArray<C, int8_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt8ArrayS : public HCIntegerArrayS<C, int8_t>
{
public:
  HCInt8ArrayS(const std::string& name, C* object, int (C::*getmethod)(int8_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int8_t*, uint16_t))
  : HCIntegerArrayS<C, int8_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt16Array : public HCIntegerArray<C, int16_t>
{
public:
  HCInt16Array(const std::string& name, C* object, int (C::*getmethod)(int16_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int16_t*, uint16_t))
  : HCIntegerArray<C, int16_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt16ArrayS : public HCIntegerArrayS<C, int16_t>
{
public:
  HCInt16ArrayS(const std::string& name, C* object, int (C::*getmethod)(int16_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int16_t*, uint16_t))
  : HCIntegerArrayS<C, int16_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt32Array : public HCIntegerArray<C, int32_t>
{
public:
  HCInt32Array(const std::string& name, C* object, int (C::*getmethod)(int32_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int32_t*, uint16_t))
  : HCIntegerArray<C, int32_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt32ArrayS : public HCIntegerArrayS<C, int32_t>
{
public:
  HCInt32ArrayS(const std::string& name, C* object, int (C::*getmethod)(int32_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int32_t*, uint16_t))
  : HCIntegerArrayS<C, int32_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt64Array : public HCIntegerArray<C, int64_t>
{
public:
  HCInt64Array(const std::string& name, C* object, int (C::*getmethod)(int64_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int64_t*, uint16_t))
  : HCIntegerArray<C, int64_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCInt64ArrayS : public HCIntegerArrayS<C, int64_t>
{
public:
  HCInt64ArrayS(const std::string& name, C* object, int (C::*getmethod)(int64_t*, uint16_t, uint16_t&), int (C::*setmethod)(const int64_t*, uint16_t))
  : HCIntegerArrayS<C, int64_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns8Array : public HCIntegerArray<C, uint8_t>
{
public:
  HCUns8Array(const std::string& name, C* object, int (C::*getmethod)(uint8_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint8_t*, uint16_t))
  : HCIntegerArray<C, uint8_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns8ArrayS : public HCIntegerArrayS<C, uint8_t>
{
public:
  HCUns8ArrayS(const std::string& name, C* object, int (C::*getmethod)(uint8_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint8_t*, uint16_t))
  : HCIntegerArrayS<C, uint8_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns16Array : public HCIntegerArray<C, uint16_t>
{
public:
  HCUns16Array(const std::string& name, C* object, int (C::*getmethod)(uint16_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint16_t*, uint16_t))
  : HCIntegerArray<C, uint16_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns16ArrayS : public HCIntegerArrayS<C, uint16_t>
{
public:
  HCUns16ArrayS(const std::string& name, C* object, int (C::*getmethod)(uint16_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint16_t*, uint16_t))
  : HCIntegerArrayS<C, uint16_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns32Array : public HCIntegerArray<C, uint32_t>
{
public:
  HCUns32Array(const std::string& name, C* object, int (C::*getmethod)(uint32_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint32_t*, uint16_t))
  : HCIntegerArray<C, uint32_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns32ArrayS : public HCIntegerArrayS<C, uint32_t>
{
public:
  HCUns32ArrayS(const std::string& name, C* object, int (C::*getmethod)(uint32_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint32_t*, uint16_t))
  : HCIntegerArrayS<C, uint32_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns64Array : public HCIntegerArray<C, uint64_t>
{
public:
  HCUns64Array(const std::string& name, C* object, int (C::*getmethod)(uint64_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint64_t*, uint16_t))
  : HCIntegerArray<C, uint64_t>(name, object, getmethod, setmethod)
  {
  }
};

template <class C>
class HCUns64ArrayS : public HCIntegerArrayS<C, uint64_t>
{
public:
  HCUns64ArrayS(const std::string& name, C* object, int (C::*getmethod)(uint64_t*, uint16_t, uint16_t&), int (C::*setmethod)(const uint64_t*, uint16_t))
  : HCIntegerArrayS<C, uint64_t>(name, object, getmethod, setmethod)
  {
  }
};
