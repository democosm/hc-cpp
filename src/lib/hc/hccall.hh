// HC call
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
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
//Call client stub
//-----------------------------------------------------------------------------
class HCCallCli
{
public:
  HCCallCli(HCClient* cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCCallCli()
  {
  }

  int Call(void)
  {
    //Delegate to client
    return _cli->Call(_pid);
  }

  int ICall(uint32_t eid)
  {
    //Delegate to client
    return _cli->ICall(_pid, eid);
  }

private:
  HCClient* _cli;
  uint16_t _pid;
};

//-----------------------------------------------------------------------------
//Call
//-----------------------------------------------------------------------------
template <class C>
class HCCall : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*CallMethod)(void);

public:
  HCCall(const std::string& name, C* object, CallMethod method)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert((object != 0) && (method != 0));

    //Initialize member variables
    _object = object;
    _method = method;
  }

  virtual ~HCCall()
  {
  }

  virtual uint8_t GetType(void)
  {
    return TypeCode();
  }

  virtual bool IsReadable(void)
  {
    return false;
  }

  virtual bool IsWritable(void)
  {
    if(_method == 0)
      return false;

    return true;
  }

  virtual void PrintVal(void)
  {
    std::cout << TC_GREEN << _name << "()" << TC_RESET << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    st << _name;
    st << "\n  Type: " << TypeString();
  }

  virtual void SaveInfo(std::ofstream& file, uint32_t indent, uint16_t pid)
  {
    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString() << ">\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "</" << TypeString() << ">\n";
  }

  virtual int Call(void)
  {
    //Check for null method
    if(_method == 0)
      return ERR_ACCESS;

    //Call the call method
    return (_object->*_method)();
  }

  virtual bool CallCell(HCCell* icell, HCCell* ocell)
  {
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_method != 0)
    {
      //Call method
      lerr = (_object->*_method)();
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
  CallMethod _method;
};

//-----------------------------------------------------------------------------
//Call table
//-----------------------------------------------------------------------------
template <class C>
class HCCallTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*CallMethod)(uint32_t);

public:
  HCCallTable(const std::string& name, C* object, CallMethod method, uint32_t size, const HCEIDEnum* eidenums)
  : HCParameter(name)
  {
    //Delegate to init method
    Init(name, object, method, size, eidenums);
  }

  HCCallTable(const std::string& name, C* object, CallMethod method, uint32_t size)
  : HCParameter(name)
  {
    //Delegate to init method
    Init(name, object, method, size, 0);
  }

  void Init(const std::string& name, C* object, CallMethod method, uint32_t size, const HCEIDEnum* eidenums)
  {
    //Assert valid arguments
    assert((object != 0) && (method != 0) && (size != 0));

    //Initialize member variables
    _object = object;
    _method = method;
    _size = size;
    _eidenums = eidenums;
  }

  virtual ~HCCallTable()
  {
  }

  virtual uint8_t GetType(void)
  {
    return TypeCode();
  }

  virtual bool IsReadable(void)
  {
    return false;
  }

  virtual bool IsWritable(void)
  {
    if(_method == 0)
      return false;

    return true;
  }

  virtual bool IsATable(void)
  {
    return true;
  }

  virtual void PrintVal(void)
  {
    std::cout << TC_GREEN << _name << "(0-" << (_size-1) << ')' << TC_RESET << "\n";
  }

  virtual void PrintInfo(std::ostream& st=std::cout)
  {
    uint32_t i;

    st << _name;
    st << "\n  Type: " << TypeString() << "t";
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
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<" << TypeString() << "t>\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << "\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << "\n";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </eidenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</" << TypeString() << "t>\n";
  }

  virtual int CallTbl(uint32_t eid)
  {
    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_method == 0)
      return ERR_ACCESS;

    //Call the call method
    return (_object->*_method)(eid);
  }

  virtual bool CallCellTbl(uint32_t eid, HCCell* icell, HCCell* ocell)
  {
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Check for valid method
    if(_method != 0)
    {
      //Call method
      lerr = (_object->*_method)(eid);
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
  CallMethod _method;
  uint32_t _size;
  const HCEIDEnum* _eidenums;
};
