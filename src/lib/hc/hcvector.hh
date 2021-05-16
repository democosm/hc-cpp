// HC vector
//
// Copyright 2020 Democosm
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
//Vec2 client stub template
//-----------------------------------------------------------------------------
template <class T>
class HCVec2Cli
{
public:
  HCVec2Cli(HCClient* cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCVec2Cli()
  {
    //Cleanup
  }

  int Get(T& val0, T& val1)
  {
    //Delegate to client
    return _cli->Get(_pid, val0, val1);
  }

  int Set(const T val0, const T val1)
  {
    //Delegate to client
    return _cli->Set(_pid, val0, val1);
  }

  int IGet(uint32_t eid, T& val0, T& val1)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val0, val1);
  }

  int ISet(uint32_t eid, const T val0, const T val1)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val0, val1);
  }

private:
  HCClient* _cli;
  uint16_t _pid;
};

//-----------------------------------------------------------------------------
//Vec2 client stubs
//-----------------------------------------------------------------------------
typedef HCVec2Cli<float> HCV2F32Cli;
typedef HCVec2Cli<double> HCV2F64Cli;

//-----------------------------------------------------------------------------
//Vec2 template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec2 : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T&, T&);
  typedef int (C::*SetMethod)(const T, const T);

public:
  HCVec2(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, T scale0, T scale1)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale0 = scale0;
    _scale1 = scale1;
  }

  virtual ~HCVec2()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val, val, val);
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
    T val0;
    T val1;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val0, val1);
    val0 *= _scale0;
    val1 *= _scale1;

    //Print value
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";
    std::cout << val0 << ", " << val1;
    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val0;
    T val1;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Get value
    if((_object->*_getmethod)(val0, val1) != ERR_NONE)
      return;

    val0 *= _scale0;
    val1 *= _scale1;

    //Print value
    st << path;
    st << _name;
    st << " = ";
    st << val0 << ", " << val1;
    st << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T dummy;

    st << _name;
    st << "\n  Type: " << TypeString(dummy, dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale0: " << _scale0;
    st << "\n  Scale1: " << _scale1;
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy, dummy) << ">" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl0>" << _scale0 << "</scl0>" << "\n";
    file << std::string(indent, ' ') << "  <scl1>" << _scale1 << "</scl1>" << "\n";
    file << std::string(indent, ' ') << "</" << TypeString(dummy, dummy) << ">" << "\n";
  }

  virtual int GetVec(T& val0, T& val1)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val0, val1);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val0, val1);
  }

  virtual int SetVec(const T val0, const T val1)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val0, val1);
  }

  virtual int GetStr(std::string& val)
  {
    T nval0;
    T nval1;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    lerr = (_object->*_getmethod)(nval0, nval1);
    nval0 *= _scale0;
    nval1 *= _scale1;

    //Convert to string
    StringPrint(nval0, nval1, val);
    return lerr;
  }

  virtual int SetStr(const std::string& val)
  {
    T nval0;
    T nval1;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval0, nval1))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(nval0 / _scale0, nval1 / _scale1);
  }

  virtual int SetStrLit(const std::string& val)
  {
    return SetStr(val);
  }

  virtual bool GetCell(HCCell* icell, HCCell* ocell)
  {
    T val0;
    T val1;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(val0, val1);
    }
    else
    {
      //Access error
      DefaultVal(val0, val1);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val0, val1)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val0, val1))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCell(HCCell* icell, HCCell* ocell)
  {
    uint8_t type;
    T val0;
    T val1;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val0, val1))
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
    if(!icell->Read(val0, val1))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(val0, val1);
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
  T _scale0;
  T _scale1;
};

//-----------------------------------------------------------------------------
//Vec2 template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec2S : public HCVec2<C, T>
{
public:
  HCVec2S(const std::string& name, C* object, int (C::*getmethod)(T&, T&), int (C::*setmethod)(const T, const T), T scale0, T scale1)
  : HCVec2<C, T>(name, object, getmethod, setmethod, scale0, scale1)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from vec2 templates
//-----------------------------------------------------------------------------
template <class C>
class HCV2F32 : public HCVec2<C, float>
{
public:
  HCV2F32(const std::string& name, C* object, int (C::*getmethod)(float&, float&), int (C::*setmethod)(const float, const float))
  : HCVec2<C, float>(name, object, getmethod, setmethod, 1.0, 1.0)
  {
  }

  HCV2F32(const std::string& name, C* object, int (C::*getmethod)(float&, float&), int (C::*setmethod)(const float, const float), float scale0, float scale1)
  : HCVec2<C, float>(name, object, getmethod, setmethod, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F32S : public HCVec2S<C, float>
{
public:
  HCV2F32S(const std::string& name, C* object, int (C::*getmethod)(float&, float&), int (C::*setmethod)(const float, const float))
  : HCVec2S<C, float>(name, object, getmethod, setmethod, 1.0, 1.0)
  {
  }

  HCV2F32S(const std::string& name, C* object, int (C::*getmethod)(float&, float&), int (C::*setmethod)(const float, const float), float scale0, float scale1)
  : HCVec2S<C, float>(name, object, getmethod, setmethod, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F64 : public HCVec2<C, double>
{
public:
  HCV2F64(const std::string& name, C* object, int (C::*getmethod)(double&, double&), int (C::*setmethod)(const double, const double))
  : HCVec2<C, double>(name, object, getmethod, setmethod, 1.0, 1.0)
  {
  }

  HCV2F64(const std::string& name, C* object, int (C::*getmethod)(double&, double&), int (C::*setmethod)(const double, const double), double scale0, double scale1)
  : HCVec2<C, double>(name, object, getmethod, setmethod, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F64S : public HCVec2S<C, double>
{
public:
  HCV2F64S(const std::string& name, C* object, int (C::*getmethod)(double&, double&), int (C::*setmethod)(const double, const double))
  : HCVec2S<C, double>(name, object, getmethod, setmethod, 1.0, 1.0)
  {
  }

  HCV2F64S(const std::string& name, C* object, int (C::*getmethod)(double&, double&), int (C::*setmethod)(const double, const double), double scale0, double scale1)
  : HCVec2S<C, double>(name, object, getmethod, setmethod, scale0, scale1)
  {
  }
};

//-----------------------------------------------------------------------------
//Vec2 table template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec2Table : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T&, T&);
  typedef int (C::*SetMethod)(uint32_t, const T, const T);

public:
  HCVec2Table(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum* eidenums, T scale0, T scale1)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale0 = scale0;
    _scale1 = scale1;
    _size = size;
    _eidenums = eidenums;
  }

  virtual ~HCVec2Table()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val, val, val);
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
    T val0;
    T val1;
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
      lerr = (_object->*_getmethod)(eid, val0, val1);
      val0 *= _scale0;
      val1 *= _scale1;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
        std::cout << "[" << eid << "]";
        std::cout << " = ";
        std::cout << val0 << ", " << val1;
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
          std::cout << val0 << ", " << val1;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
          std::cout << "[\"" << eidstr << "\"]";
          std::cout << " = ";
          std::cout << val0 << ", " << val1;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
      }
    }
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val0;
    T val1;
    uint32_t eid;
    std::string eidstr;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Loop through all elements
    for(eid=0; eid<_size; eid++)
    {
      //Get value
      if((_object->*_getmethod)(eid, val0, val1) != ERR_NONE)
        continue;

      val0 *= _scale0;
      val1 *= _scale1;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        st << path;
        st << _name;
        st << "[" << eid << "]";
        st << " = ";
        st << val0 << ", " << val1;
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
          st << val0 << ", " << val1;
          st << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          st << path;
          st << _name;
          st << "[\"" << eidstr << "\"]";
          st << " = ";
          st << val0 << ", " << val1;
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
    st << "\n  Type: " << TypeString(dummy, dummy) << "t";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale0: " << _scale0;
    st << "\n  Scale1: " << _scale1;
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy, dummy) << "t>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl0>" << _scale0 << "</scl0>" << "\n";
    file << std::string(indent, ' ') << "  <scl1>" << _scale1 << "</scl1>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << "\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << "\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </eidenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy, dummy) << "t>" << "\n";
  }

  virtual int GetVecTbl(uint32_t eid, T& val0, T& val1)
  {
    //Check for EID out of range
    if(eid >= _size)
    {
      DefaultVal(val0, val1);
      return ERR_EID;
    }

    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val0, val1);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val0, val1);
  }

  virtual int SetVecTbl(uint32_t eid, const T val0, const T val1)
  {
    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(eid, val0, val1);
  }

  virtual int GetStrTbl(uint32_t eid, std::string& val)
  {
    T nval0;
    T nval1;
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
    lerr = (_object->*_getmethod)(eid, nval0, nval1);
    nval0 *= _scale0;
    nval1 *= _scale1;

    //Convert to string
    StringPrint(nval0, nval1, val);
    return lerr;
  }

  virtual int SetStrTbl(uint32_t eid, const std::string& val)
  {
    T nval0;
    T nval1;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval0, nval1))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(eid, nval0 / _scale0, nval1 / _scale1);
  }

  virtual int SetStrLitTbl(uint32_t eid, const std::string& val)
  {
    return SetStrTbl(eid, val);
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    T val0;
    T val1;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val0, val1);
    }
    else
    {
      //Access error
      DefaultVal(val0, val1);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val0, val1)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val0, val1))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    uint8_t type;
    T val0;
    T val1;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val0, val1))
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
    if(!icell->Read(val0, val1))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(eid, val0, val1);
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

private:
  C* _object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  T _scale0;
  T _scale1;
  uint32_t _size;
  const HCEIDEnum* _eidenums;
};

//-----------------------------------------------------------------------------
//Vec2 table template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec2TableS : public HCVec2Table<C, T>
{
public:
  HCVec2TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, T&, T&), int (C::*setmethod)(uint32_t, const T, const T), uint32_t size, const HCEIDEnum* eidenums, T scale0, T scale1)
  : HCVec2Table<C, T>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from Vec2 table templates
//-----------------------------------------------------------------------------
template <class C>
class HCV2F32Table : public HCVec2Table<C, float>
{
public:
  HCV2F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size)
  : HCVec2Table<C, float>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0)
  {
  }

  HCV2F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, float scale0, float scale1)
  : HCVec2Table<C, float>(name, object, getmethod, setmethod, size, 0, scale0, scale1)
  {
  }

  HCV2F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec2Table<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0)
  {
  }

  HCV2F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, const HCEIDEnum* eidenums, float scale0, float scale1)
  : HCVec2Table<C, float>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F32TableS : public HCVec2TableS<C, float>
{
public:
  HCV2F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size)
  : HCVec2TableS<C, float>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0)
  {
  }

  HCV2F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, float scale0, float scale1)
  : HCVec2TableS<C, float>(name, object, getmethod, setmethod, size, 0, scale0, scale1)
  {
  }

  HCV2F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec2TableS<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0)
  {
  }

  HCV2F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&), int (C::*setmethod)(uint32_t, const float, const float), uint32_t size, const HCEIDEnum* eidenums, float scale0, float scale1)
  : HCVec2TableS<C, float>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F64Table : public HCVec2Table<C, double>
{
public:
  HCV2F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size)
  : HCVec2Table<C, double>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0)
  {
  }

  HCV2F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, double scale0, double scale1)
  : HCVec2Table<C, double>(name, object, getmethod, setmethod, size, 0, scale0, scale1)
  {
  }

  HCV2F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec2Table<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0)
  {
  }

  HCV2F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, const HCEIDEnum* eidenums, double scale0, double scale1)
  : HCVec2Table<C, double>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1)
  {
  }
};

template <class C>
class HCV2F64TableS : public HCVec2TableS<C, double>
{
public:
  HCV2F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size)
  : HCVec2TableS<C, double>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0)
  {
  }

  HCV2F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, double scale0, double scale1)
  : HCVec2TableS<C, double>(name, object, getmethod, setmethod, size, 0, scale0, scale1)
  {
  }

  HCV2F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec2TableS<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0)
  {
  }

  HCV2F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&), int (C::*setmethod)(uint32_t, const double, const double), uint32_t size, const HCEIDEnum* eidenums, double scale0, double scale1)
  : HCVec2TableS<C, double>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1)
  {
  }
};

//-----------------------------------------------------------------------------
//Vec3 client stub template
//-----------------------------------------------------------------------------
template <class T>
class HCVec3Cli
{
public:
  HCVec3Cli(HCClient* cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCVec3Cli()
  {
    //Cleanup
  }

  int Get(T& val0, T& val1, T& val2)
  {
    //Delegate to client
    return _cli->Get(_pid, val0, val1, val2);
  }

  int Set(const T val0, const T val1, const T val2)
  {
    //Delegate to client
    return _cli->Set(_pid, val0, val1, val2);
  }

  int IGet(uint32_t eid, T& val0, T& val1, T& val2)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val0, val1, val2);
  }

  int ISet(uint32_t eid, const T val0, const T val1, const T val2)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val0, val1, val2);
  }

private:
  HCClient* _cli;
  uint16_t _pid;
};

//-----------------------------------------------------------------------------
//Vec3 client stubs
//-----------------------------------------------------------------------------
typedef HCVec3Cli<float> HCV3F32Cli;
typedef HCVec3Cli<double> HCV3F64Cli;

//-----------------------------------------------------------------------------
//Vec3 template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec3 : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T&, T&, T&);
  typedef int (C::*SetMethod)(const T, const T, const T);

public:
  HCVec3(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, T scale0, T scale1, T scale2)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale0 = scale0;
    _scale1 = scale1;
    _scale2 = scale2;
  }

  virtual ~HCVec3()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val, val, val);
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
    T val0;
    T val1;
    T val2;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val0, val1, val2);
    val0 *= _scale0;
    val1 *= _scale1;
    val2 *= _scale2;

    //Print value
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";
    std::cout << val0 << ", " << val1 << ", " << val2;
    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val0;
    T val1;
    T val2;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Get value
    if((_object->*_getmethod)(val0, val1, val2) != ERR_NONE)
      return;

    val0 *= _scale0;
    val1 *= _scale1;
    val2 *= _scale2;

    //Print value
    st << path;
    st << _name;
    st << " = ";
    st << val0 << ", " << val1 << ", " << val2;
    st << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    T dummy;

    st << _name;
    st << "\n  Type: " << TypeString(dummy, dummy, dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale0: " << _scale0;
    st << "\n  Scale1: " << _scale1;
    st << "\n  Scale2: " << _scale2;
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy, dummy, dummy) << ">" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl0>" << _scale0 << "</scl0>" << "\n";
    file << std::string(indent, ' ') << "  <scl1>" << _scale1 << "</scl1>" << "\n";
    file << std::string(indent, ' ') << "  <scl2>" << _scale2 << "</scl2>" << "\n";
    file << std::string(indent, ' ') << "</" << TypeString(dummy, dummy, dummy) << ">" << "\n";
  }

  virtual int GetVec(T& val0, T& val1, T& val2)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val0, val1, val2);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val0, val1, val2);
  }

  virtual int SetVec(const T val0, const T val1, const T val2)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val0, val1, val2);
  }

  virtual int GetStr(std::string& val)
  {
    T nval0;
    T nval1;
    T nval2;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    lerr = (_object->*_getmethod)(nval0, nval1, nval2);
    nval0 *= _scale0;
    nval1 *= _scale1;
    nval2 *= _scale2;

    //Convert to string
    StringPrint(nval0, nval1, nval2, val);
    return lerr;
  }

  virtual int SetStr(const std::string& val)
  {
    T nval0;
    T nval1;
    T nval2;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval0, nval1, nval2))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(nval0 / _scale0, nval1 / _scale1, nval2 / _scale2);
  }

  virtual int SetStrLit(const std::string& val)
  {
    return SetStr(val);
  }

  virtual bool GetCell(HCCell* icell, HCCell* ocell)
  {
    T val0;
    T val1;
    T val2;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(val0, val1, val2);
    }
    else
    {
      //Access error
      DefaultVal(val0, val1, val2);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val0, val1, val2)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val0, val1, val2))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCell(HCCell* icell, HCCell* ocell)
  {
    uint8_t type;
    T val0;
    T val1;
    T val2;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val0, val1, val2))
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
    if(!icell->Read(val0, val1, val2))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(val0, val1, val2);
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
  T _scale0;
  T _scale1;
  T _scale2;
};

//-----------------------------------------------------------------------------
//Vec3 template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec3S : public HCVec3<C, T>
{
public:
  HCVec3S(const std::string& name, C* object, int (C::*getmethod)(T&, T&, T&), int (C::*setmethod)(const T, const T, const T), T scale0, T scale1, T scale2)
  : HCVec3<C, T>(name, object, getmethod, setmethod, scale0, scale1, scale2)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from vec3 templates
//-----------------------------------------------------------------------------
template <class C>
class HCV3F32 : public HCVec3<C, float>
{
public:
  HCV3F32(const std::string& name, C* object, int (C::*getmethod)(float&, float&, float&), int (C::*setmethod)(const float, const float, const float))
  : HCVec3<C, float>(name, object, getmethod, setmethod, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32(const std::string& name, C* object, int (C::*getmethod)(float&, float&, float&), int (C::*setmethod)(const float, const float, const float), float scale0, float scale1, float scale2)
  : HCVec3<C, float>(name, object, getmethod, setmethod, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F32S : public HCVec3S<C, float>
{
public:
  HCV3F32S(const std::string& name, C* object, int (C::*getmethod)(float&, float&, float&), int (C::*setmethod)(const float, const float, const float))
  : HCVec3S<C, float>(name, object, getmethod, setmethod, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32S(const std::string& name, C* object, int (C::*getmethod)(float&, float&, float&), int (C::*setmethod)(const float, const float, const float), float scale0, float scale1, float scale2)
  : HCVec3S<C, float>(name, object, getmethod, setmethod, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F64 : public HCVec3<C, double>
{
public:
  HCV3F64(const std::string& name, C* object, int (C::*getmethod)(double&, double&, double&), int (C::*setmethod)(const double, const double, const double))
  : HCVec3<C, double>(name, object, getmethod, setmethod, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64(const std::string& name, C* object, int (C::*getmethod)(double&, double&, double&), int (C::*setmethod)(const double, const double, const double), double scale0, double scale1, double scale2)
  : HCVec3<C, double>(name, object, getmethod, setmethod, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F64S : public HCVec3S<C, double>
{
public:
  HCV3F64S(const std::string& name, C* object, int (C::*getmethod)(double&, double&, double&), int (C::*setmethod)(const double, const double, const double))
  : HCVec3S<C, double>(name, object, getmethod, setmethod, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64S(const std::string& name, C* object, int (C::*getmethod)(double&, double&, double&), int (C::*setmethod)(const double, const double, const double), double scale0, double scale1, double scale2)
  : HCVec3S<C, double>(name, object, getmethod, setmethod, scale0, scale1, scale2)
  {
  }
};

//-----------------------------------------------------------------------------
//Vec3 table template
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec3Table : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T&, T&, T&);
  typedef int (C::*SetMethod)(uint32_t, const T, const T, const T);

public:
  HCVec3Table(const std::string& name, C* object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum* eidenums, T scale0, T scale1, T scale2)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _scale0 = scale0;
    _scale1 = scale1;
    _scale2 = scale2;
    _size = size;
    _eidenums = eidenums;
  }

  virtual ~HCVec3Table()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    T val;
    return TypeCode(val, val, val);
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
    T val0;
    T val1;
    T val2;
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
      lerr = (_object->*_getmethod)(eid, val0, val1, val2);
      val0 *= _scale0;
      val1 *= _scale1;
      val2 *= _scale2;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
        std::cout << "[" << eid << "]";
        std::cout << " = ";
        std::cout << val0 << ", " << val1 << ", " << val2;
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
          std::cout << val0 << ", " << val1 << ", " << val2;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
          std::cout << "[\"" << eidstr << "\"]";
          std::cout << " = ";
          std::cout << val0 << ", " << val1 << ", " << val2;
          std::cout << " !" << ErrToString(lerr);
          std::cout << TC_RESET << "\n";
        }
      }
    }
  }

  virtual void PrintConfig(const std::string& path, std::ostream& st=std::cout)
  {
    T val0;
    T val1;
    T val2;
    uint32_t eid;
    std::string eidstr;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Loop through all elements
    for(eid=0; eid<_size; eid++)
    {
      //Get value
      if((_object->*_getmethod)(eid, val0, val1, val2) != ERR_NONE)
        continue;

      val0 *= _scale0;
      val1 *= _scale1;
      val2 *= _scale2;

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        st << path;
        st << _name;
        st << "[" << eid << "]";
        st << " = ";
        st << val0 << ", " << val1 << ", " << val2;
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
          st << val0 << ", " << val1 << ", " << val2;
          st << "\n";
        }
        else
        {
          //Print value (show EID enum string)
          st << path;
          st << _name;
          st << "[\"" << eidstr << "\"]";
          st << " = ";
          st << val0 << ", " << val1 << ", " << val2;
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
    st << "\n  Type: " << TypeString(dummy, dummy, dummy) << "t";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
    st << "\n  Scale0: " << _scale0;
    st << "\n  Scale1: " << _scale1;
    st << "\n  Scale2: " << _scale2;
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    T dummy;
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy, dummy, dummy) << "t>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <scl0>" << _scale0 << "</scl0>" << "\n";
    file << std::string(indent, ' ') << "  <scl1>" << _scale1 << "</scl1>" << "\n";
    file << std::string(indent, ' ') << "  <scl2>" << _scale2 << "</scl2>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << "\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << "\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </eidenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy, dummy, dummy) << "t>" << "\n";
  }

  virtual int GetVecTbl(uint32_t eid, T& val0, T& val1, T& val2)
  {
    //Check for EID out of range
    if(eid >= _size)
    {
      DefaultVal(val0, val1, val2);
      return ERR_EID;
    }

    //Check for null method
    if(_getmethod == 0)
    {
      DefaultVal(val0, val1, val2);
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val0, val1, val2);
  }

  virtual int SetVecTbl(uint32_t eid, const T val0, const T val1, const T val2)
  {
    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(eid, val0, val1, val2);
  }

  virtual int GetStrTbl(uint32_t eid, std::string& val)
  {
    T nval0;
    T nval1;
    T nval2;
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
    lerr = (_object->*_getmethod)(eid, nval0, nval1, nval2);
    nval0 *= _scale0;
    nval1 *= _scale1;
    nval2 *= _scale2;

    //Convert to string
    StringPrint(nval0, nval1, nval2, val);
    return lerr;
  }

  virtual int SetStrTbl(uint32_t eid, const std::string& val)
  {
    T nval0;
    T nval1;
    T nval2;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval0, nval1, nval2))
      return ERR_UNSPEC;

    //Set value
    return (_object->*_setmethod)(eid, nval0 / _scale0, nval1 / _scale1, nval2 / _scale2);
  }

  virtual int SetStrLitTbl(uint32_t eid, const std::string& val)
  {
    return SetStrTbl(eid, val);
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    T val0;
    T val1;
    T val2;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_getmethod != 0)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val0, val1, val2);
    }
    else
    {
      //Access error
      DefaultVal(val0, val1, val2);
      lerr = ERR_ACCESS;
    }

    //Write type code to outbound cell and check for error
    if(!ocell->Write(TypeCode(val0, val1, val2)))
      return false;

    //Write value to outbound cell and check for error
    if(!ocell->Write(val0, val1, val2))
      return false;

    //Write error code to outbound cell and check for error
    if(!ocell->Write((int8_t)lerr))
      return false;

    return true;
  }

  virtual bool SetCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    uint8_t type;
    T val0;
    T val1;
    T val2;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get type code from inbound cell and check for error
    if(!icell->Read(type))
      return false;

    //Check for incorrect type
    if(type != TypeCode(val0, val1, val2))
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
    if(!icell->Read(val0, val1, val2))
      return false;

    //Check for valid method
    if(_setmethod != 0)
    {
      //Call set method
      lerr = (_object->*_setmethod)(eid, val0, val1, val2);
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

private:
  C* _object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  T _scale0;
  T _scale1;
  T _scale2;
  uint32_t _size;
  const HCEIDEnum* _eidenums;
};

//-----------------------------------------------------------------------------
//Vec3 table template (savable)
//-----------------------------------------------------------------------------
template <class C, class T>
class HCVec3TableS : public HCVec3Table<C, T>
{
public:
  HCVec3TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, T&, T&, T&), int (C::*setmethod)(uint32_t, const T, const T, const T), uint32_t size, const HCEIDEnum* eidenums, T scale0, T scale1, T scale2)
  : HCVec3Table<C, T>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1, scale2)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from vec3 table templates
//-----------------------------------------------------------------------------
template <class C>
class HCV3F32Table : public HCVec3Table<C, float>
{
public:
  HCV3F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size)
  : HCVec3Table<C, float>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, float scale0, float scale1, float scale2)
  : HCVec3Table<C, float>(name, object, getmethod, setmethod, size, 0, scale0, scale1, scale2)
  {
  }

  HCV3F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec3Table<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, const HCEIDEnum* eidenums, float scale0, float scale1, float scale2)
  : HCVec3Table<C, float>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F32TableS : public HCVec3TableS<C, float>
{
public:
  HCV3F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size)
  : HCVec3TableS<C, float>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, float scale0, float scale1, float scale2)
  : HCVec3TableS<C, float>(name, object, getmethod, setmethod, size, 0, scale0, scale1, scale2)
  {
  }

  HCV3F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec3TableS<C, float>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0, 1.0)
  {
  }

  HCV3F32TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, float&, float&, float&), int (C::*setmethod)(uint32_t, const float, const float, const float), uint32_t size, const HCEIDEnum* eidenums, float scale0, float scale1, float scale2)
  : HCVec3TableS<C, float>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F64Table : public HCVec3Table<C, double>
{
public:
  HCV3F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size)
  : HCVec3Table<C, double>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, double scale0, double scale1, double scale2)
  : HCVec3Table<C, double>(name, object, getmethod, setmethod, size, 0, scale0, scale1, scale2)
  {
  }

  HCV3F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec3Table<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64Table(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, const HCEIDEnum* eidenums, double scale0, double scale1, double scale2)
  : HCVec3Table<C, double>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1, scale2)
  {
  }
};

template <class C>
class HCV3F64TableS : public HCVec3TableS<C, double>
{
public:
  HCV3F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size)
  : HCVec3TableS<C, double>(name, object, getmethod, setmethod, size, 0, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, double scale0, double scale1, double scale2)
  : HCVec3TableS<C, double>(name, object, getmethod, setmethod, size, 0, scale0, scale1, scale2)
  {
  }

  HCV3F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, const HCEIDEnum* eidenums)
  : HCVec3TableS<C, double>(name, object, getmethod, setmethod, size, eidenums, 1.0, 1.0, 1.0)
  {
  }

  HCV3F64TableS(const std::string& name, C* object, int (C::*getmethod)(uint32_t, double&, double&, double&), int (C::*setmethod)(uint32_t, const double, const double, const double), uint32_t size, const HCEIDEnum* eidenums, double scale0, double scale1, double scale2)
  : HCVec3TableS<C, double>(name, object, getmethod, setmethod, size, eidenums, scale0, scale1, scale2)
  {
  }
};
