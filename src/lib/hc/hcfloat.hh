// HC floating point
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
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
//Floating point client stub template
//-----------------------------------------------------------------------------
template <class T>
class HCFloatCli
{
public:
  HCFloatCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCFloatCli()
  {
    //Cleanup
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

//-----------------------------------------------------------------------------
//Floating point client stubs
//-----------------------------------------------------------------------------
typedef HCFloatCli<float> HCFlt32Cli;
typedef HCFloatCli<double> HCFlt64Cli;

//-----------------------------------------------------------------------------
//Floating point template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCFloat : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T &);
  typedef int (C::*SetMethod)(const T);

public:
  HCFloat(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, T scale)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale = scale;
  }

  virtual ~HCFloat()
  {
    //Cleanup
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

  virtual void PrintVal(void)
  {
    T val;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val);
    val *= _scale;

    //Print value
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";
    std::cout << val;
    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string &path, std::ostream &st=std::cout)
  {
    T val;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Get value
    if((_object->*_getmethod)(val) != ERR_NONE)
      return;

    val *= _scale;

    //Print value
    st << path;
    st << _name;
    st << " = ";
    st << val;
    st << "\n";
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    T dummy;

    st << _name;
    st << "\n  Type: " << TypeString(dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale: " << _scale;
  }

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << ">" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl>" << _scale << "</scl>" << "\n";
    file << std::string(indent, ' ') << "</" << TypeString(dummy) << ">" << "\n";
  }

  virtual int GetFlt(T &val)
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

  virtual int SetFlt(const T val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val);
  }

  virtual int GetStr(std::string &val)
  {
    T nval;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    lerr = (_object->*_getmethod)(nval);
    nval *= _scale;

    //Convert to string
    StringPrint(nval, val);
    return lerr;
  }

  virtual int SetStr(const std::string &val)
  {
    T nval;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(nval / _scale);
  }

  virtual int SetStrLit(const std::string &val)
  {
    return SetStr(val);
  }

  virtual bool GetCell(HCCell *icell, HCCell *ocell)
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

private:
  C *_object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  T _scale;
};

//-----------------------------------------------------------------------------
//Floating point template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCFloatS : public HCFloat<C, T>
{
public:
  HCFloatS(const std::string &name, C *object, int (C::*getmethod)(T &), int (C::*setmethod)(const T), T scale)
  : HCFloat<C, T>(name, object, getmethod, setmethod, scale)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from floating point templates
//-----------------------------------------------------------------------------
template <class C>
class HCFlt32 : public HCFloat<C, float>
{
public:
  HCFlt32(const std::string &name, C *object, int (C::*getmethod)(float &), int (C::*setmethod)(const float))
  : HCFloat<C, float>(name, object, getmethod, setmethod, 1.0)
  {
  }

  HCFlt32(const std::string &name, C *object, int (C::*getmethod)(float &), int (C::*setmethod)(const float), float scale)
  : HCFloat<C, float>(name, object, getmethod, setmethod, scale)
  {
  }
};

template <class C>
class HCFlt32S : public HCFloatS<C, float>
{
public:
  HCFlt32S(const std::string &name, C *object, int (C::*getmethod)(float &), int (C::*setmethod)(const float))
  : HCFloatS<C, float>(name, object, getmethod, setmethod, 1.0)
  {
  }

  HCFlt32S(const std::string &name, C *object, int (C::*getmethod)(float &), int (C::*setmethod)(const float), float scale)
  : HCFloatS<C, float>(name, object, getmethod, setmethod, scale)
  {
  }
};

template <class C>
class HCFlt64 : public HCFloat<C, double>
{
public:
  HCFlt64(const std::string &name, C *object, int (C::*getmethod)(double &), int (C::*setmethod)(const double))
  : HCFloat<C, double>(name, object, getmethod, setmethod, 1.0)
  {
  }

  HCFlt64(const std::string &name, C *object, int (C::*getmethod)(double &), int (C::*setmethod)(const double), double scale)
  : HCFloat<C, double>(name, object, getmethod, setmethod, scale)
  {
  }
};

template <class C>
class HCFlt64S : public HCFloatS<C, double>
{
public:
  HCFlt64S(const std::string &name, C *object, int (C::*getmethod)(double &), int (C::*setmethod)(const double))
  : HCFloatS<C, double>(name, object, getmethod, setmethod, 1.0)
  {
  }

  HCFlt64S(const std::string &name, C *object, int (C::*getmethod)(double &), int (C::*setmethod)(const double), double scale)
  : HCFloatS<C, double>(name, object, getmethod, setmethod, scale)
  {
  }
};

//-----------------------------------------------------------------------------
//Floating point table template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCFloatTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T &);
  typedef int (C::*SetMethod)(uint32_t, const T);

public:
  HCFloatTable(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums, T scale)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale = scale;
    _size = size;
    _eidenums = eidenums;
  }

  virtual ~HCFloatTable()
  {
    //Cleanup
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

  virtual void PrintVal(void)
  {
    T val;
    int lerr;
    uint32_t eid;
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
      val *= _scale;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
        std::cout << "[" << eid << "]";
        std::cout << " = ";
        std::cout << val;
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
          std::cout << val;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
          std::cout << "[\"" << eidstr << "\"]";
          std::cout << " = ";
          std::cout << val;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
      }
    }
  }

  virtual void PrintConfig(const std::string &path, std::ostream &st=std::cout)
  {
    T val;
    uint32_t eid;
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

      val *= _scale;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        st << path;
        st << _name;
        st << "[" << eid << "]";
        st << " = ";
        st << val;
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
          st << val;
          st << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          st << path;
          st << _name;
          st << "[\"" << eidstr << "\"]";
          st << " = ";
          st << val;
          st << "\n";
        }
      }
    }
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    T dummy;
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: " << TypeString(dummy) << "t";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale: " << _scale;
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << "t>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl>" << _scale << "</scl>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << "\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << "\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </eidenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy) << "t>" << "\n";
  }

  virtual int GetFltTbl(uint32_t eid, T &val)
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

  virtual int SetFltTbl(uint32_t eid, const T val)
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

  virtual int GetStrTbl(uint32_t eid, std::string &val)
  {
    T nval;
    int lerr;

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

    //Call get method
    lerr = (_object->*_getmethod)(eid, nval);
    nval *= _scale;

    //Convert to string
    StringPrint(nval, val);
    return lerr;
  }

  virtual int SetStrTbl(uint32_t eid, const std::string &val)
  {
    T nval;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(eid, nval / _scale);
  }

  virtual int SetStrLitTbl(uint32_t eid, const std::string &val)
  {
    return SetStrTbl(eid, val);
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
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

  virtual bool EIDStrToNum(const std::string &str, uint32_t &num)
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

  virtual bool EIDNumToStr(uint32_t num, std::string &str)
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

private:
  C *_object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  T _scale;
  uint32_t _size;
  const HCEIDEnum *_eidenums;
};

//-----------------------------------------------------------------------------
//Floating point table template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCFloatTableS : public HCFloatTable<C, T>
{
public:
  HCFloatTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, T &), int (C::*setmethod)(uint32_t, const T), uint32_t size, const HCEIDEnum *eidenums, T scale)
  : HCFloatTable<C, T>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from floating point table templates
//-----------------------------------------------------------------------------
template <class C>
class HCFlt32Table : public HCFloatTable<C, float>
{
public:
  HCFlt32Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size)
  : HCFloatTable<C, float>(name, object, getmethod, setmethod, size, 0, 1.0)
  {
  }

  HCFlt32Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, float scale)
  : HCFloatTable<C, float>(name, object, getmethod, setmethod, size, 0, scale)
  {
  }

  HCFlt32Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, const HCEIDEnum *eidenums)
  : HCFloatTable<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0)
  {
  }

  HCFlt32Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, const HCEIDEnum *eidenums, float scale)
  : HCFloatTable<C, float>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};

template <class C>
class HCFlt32TableS : public HCFloatTableS<C, float>
{
public:
  HCFlt32TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size)
  : HCFloatTableS<C, float>(name, object, getmethod, setmethod, size, 0, 1.0)
  {
  }

  HCFlt32TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, float scale)
  : HCFloatTableS<C, float>(name, object, getmethod, setmethod, size, 0, scale)
  {
  }

  HCFlt32TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, const HCEIDEnum *eidenums)
  : HCFloatTableS<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0)
  {
  }

  HCFlt32TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, const HCEIDEnum *eidenums, float scale)
  : HCFloatTableS<C, float>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};

template <class C>
class HCFlt64Table : public HCFloatTable<C, double>
{
public:
  HCFlt64Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size)
  : HCFloatTable<C, double>(name, object, getmethod, setmethod, size, 0, 1.0)
  {
  }

  HCFlt64Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, double scale)
  : HCFloatTable<C, double>(name, object, getmethod, setmethod, size, 0, scale)
  {
  }

  HCFlt64Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, const HCEIDEnum *eidenums)
  : HCFloatTable<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0)
  {
  }

  HCFlt64Table(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, const HCEIDEnum *eidenums, double scale)
  : HCFloatTable<C, double>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};

template <class C>
class HCFlt64TableS : public HCFloatTableS<C, double>
{
public:
  HCFlt64TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size)
  : HCFloatTableS<C, double>(name, object, getmethod, setmethod, size, 0, 1.0)
  {
  }

  HCFlt64TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, double scale)
  : HCFloatTableS<C, double>(name, object, getmethod, setmethod, size, 0, scale)
  {
  }

  HCFlt64TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, const HCEIDEnum *eidenums)
  : HCFloatTableS<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0)
  {
  }

  HCFlt64TableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, const HCEIDEnum *eidenums, double scale)
  : HCFloatTableS<C, double>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};
