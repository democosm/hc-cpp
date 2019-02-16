// HC floating point
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

#ifndef _HCFLOATINGPOINT_HH_
#define _HCFLOATINGPOINT_HH_

#include "error.hh"
#include "hcclient.hh"
#include "hcparameter.hh"
#include "str.hh"
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//Floating point client stub
template <class T>
class HCFloatingPointCli
{
public:
  HCFloatingPointCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCFloatingPointCli()
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

  int ISet(uint32_t eid, T val)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val);
  }

private:
  HCClient *_cli;
  uint16_t _pid;
};

//Types derived from client stub template
typedef HCFloatingPointCli<float> HCFloatCli;
typedef HCFloatingPointCli<double> HCDoubleCli;

//Floating point
template <class C, class T>
class HCFloatingPoint : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(T &);
  typedef int (C::*SetMethod)(const T);

public:
  HCFloatingPoint(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, T scale=1.0)
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

  virtual ~HCFloatingPoint()
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
      //Print value
      std::cout << _name << " !" << ErrToString(ERR_ACCESS) << std::endl;
      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val);
    val *= _scale;

    //Print value
    std::cout << _name << " = " << val << " !" << ErrToString(lerr) << std::endl;
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    T dummy;

    st << _name;
    st << "\n  Type: " << TypeString(dummy);
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Scale: " << _scale;
  }

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    T dummy;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << ">" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "  <scl>" << _scale << "</scl>" << std::endl;
    file << std::string(indent, ' ') << "</" << TypeString(dummy) << ">" << std::endl;
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

private:
  C *_object;
  GetMethod _getmethod;
  SetMethod _setmethod;
  T _scale;
};

//Classes derived from value template
template <class C>
class HCFloat : public HCFloatingPoint<C, float>
{
public:
  HCFloat(const std::string &name, C *object, int (C::*getmethod)(float &), int (C::*setmethod)(const float), float scale=1.0)
  : HCFloatingPoint<C, float>(name, object, getmethod, setmethod, scale)
  {
  }
};

template <class C>
class HCDouble : public HCFloatingPoint<C, double>
{
public:
  HCDouble(const std::string &name, C *object, int (C::*getmethod)(double &), int (C::*setmethod)(const double), double scale=1.0)
  : HCFloatingPoint<C, double>(name, object, getmethod, setmethod, scale)
  {
  }
};

//Floating point table
template <class C, class T>
class HCFloatingPointTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, T &);
  typedef int (C::*SetMethod)(uint32_t, const T);

public:
  HCFloatingPointTable(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums=0, T scale=1.0)
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

  virtual ~HCFloatingPointTable()
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
      //Print value
      std::cout << _name << " !" << ErrToString(ERR_ACCESS) << std::endl;
      return;
    }

    //Print name
    std::cout << _name << std::endl;

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
        std::cout << " [" << eid << "] = " << val << " !" << ErrToString(lerr) << std::endl;
      }
      else
      {
        //Convert EID to enum string and check for error
        if(!EIDNumToStr(eid, eidstr))
        {
          //Print value (indicate no EID enum string found)
          std::cout << " [\e[31m" << eid << "\e[0m] = " << val << " !" << ErrToString(lerr) << std::endl;
        }
        else
        {
          //Print value (show EID enum string)
          std::cout << " [\"" << eidstr << "\"] = " << val << " !" << ErrToString(lerr) << std::endl;
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
    file << std::string(indent, ' ') << "<" << TypeString(dummy) << "t>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "  <scl>" << _scale << "</scl>" << std::endl;
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << std::endl;

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << std::endl;

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << std::endl;

      file << std::string(indent, ' ') << "  </eidenums>" << std::endl;
    }

    file << std::string(indent, ' ') << "</" << TypeString(dummy) << "t>" << std::endl;
  }

  virtual int GetStrTbl(uint32_t eid, std::string &val)
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

//Classes derived from value template
template <class C>
class HCFloatTable : public HCFloatingPointTable<C, float>
{
public:
  HCFloatTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, float &), int (C::*setmethod)(uint32_t, const float), uint32_t size, const HCEIDEnum *eidenums=0, float scale=1.0)
  : HCFloatingPointTable<C, float>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};

template <class C>
class HCDoubleTable : public HCFloatingPointTable<C, double>
{
public:
  HCDoubleTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, double &), int (C::*setmethod)(uint32_t, const double), uint32_t size, const HCEIDEnum *eidenums=0, double scale=1.0)
  : HCFloatingPointTable<C, double>(name, object, getmethod, setmethod, size, eidenums, scale)
  {
  }
};

#endif //_HCFLOATINGPOINT_HH_
