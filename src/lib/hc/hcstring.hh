// HC string
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

#ifndef _HCSTRING_HH_
#define _HCSTRING_HH_

#include "error.hh"
#include "hcclient.hh"
#include "hcparameter.hh"
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//String client stub
class HCStringCli
{
public:
  HCStringCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCStringCli()
  {
    //Cleanup
  }

  int Get(std::string &val)
  {
    //Delegate to client
    return _cli->Get(_pid, val);
  }

  int Set(const std::string &val)
  {
    //Delegate to client
    return _cli->Set(_pid, val);
  }

  int Add(const std::string &val)
  {
    //Delegate to client
    return _cli->Add(_pid, val);
  }

  int Sub(const std::string &val)
  {
    //Delegate to client
    return _cli->Sub(_pid, val);
  }

  int IGet(uint32_t eid, std::string &val)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val);
  }

  int ISet(uint32_t eid, const std::string &val)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val);
  }

private:
  HCClient *_cli;
  uint16_t _pid;
};

//String
template <class C>
class HCString : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(std::string &);
  typedef int (C::*SetMethod)(const std::string &);

public:
  HCString(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
  }

  virtual ~HCString()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_STRING;
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

  virtual void PrintVal(void)
  {
    std::string val;
    int lerr;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value
      std::cout << _name << " !" << ErrToString(ERR_ACCESS) << std::endl;
      return;
    }

    //Call get method
    lerr = (_object->*_getmethod)(val);

    //Print value
    std::cout << _name << " = \"" << val << "\" !" << ErrToString(lerr) << std::endl;
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    st << _name;
    st << "\n  Type: str";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
  }

  virtual void SaveXML(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    //Generate XML information
    file << std::string(indent, ' ') << "<str>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "</str>" << std::endl;
  }

  virtual int GetStr(std::string &val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val);
  }

  virtual int SetStr(const std::string &val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val);
  }

  virtual int SetStrLit(const std::string &val)
  {
    //Delegate to normal set string method
    return SetStr(val);
  }

  virtual bool GetCell(HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    std::string val;
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
    std::string val;
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
};

//String table
template <class C>
class HCStringTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, std::string &);
  typedef int (C::*SetMethod)(uint32_t, const std::string &);

public:
  HCStringTable(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums=0)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _getmethod = getmethod;
    _setmethod = setmethod;
    _size = size;
    _eidenums = eidenums;
  }

  virtual ~HCStringTable()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_STRING;
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

  virtual void PrintVal(void)
  {
    std::string val;
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
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);

      //Check for no EID enums
      if(_eidenums == 0)
      {
        //Print value
        std::cout << " [" << eid << "] = \"" << val << "\" !" << ErrToString(lerr) << std::endl;
      }
      else
      {
        //Convert EID to enum string and check for error
        if(!EIDNumToStr(eid, eidstr))
        {
          //Print value (indicate no EID enum string found)
          std::cout << " [\e[31m" << eid << "\e[0m] = \"" << val << "\" !" << ErrToString(lerr) << std::endl;
        }
        else
        {
          //Print value (show EID enum string)
          std::cout << " [\"" << eidstr << "\"] = \"" << val << "\" !" << ErrToString(lerr) << std::endl;
        }
      }
    }
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    uint32_t i;

    st << _name;
    st << "\n  Type: strt";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }
  }

  virtual void SaveXML(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<strt>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << std::endl;

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << std::endl;

      for(i=0; _eidenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << std::endl;

      file << std::string(indent, ' ') << "  </eidenums>" << std::endl;
    }

    file << std::string(indent, ' ') << "</strt>" << std::endl;
  }

  virtual int GetStrTbl(uint32_t eid, std::string &val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val);
  }

  virtual int SetStrTbl(uint32_t eid, const std::string &val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(eid, val);
  }

  virtual int SetStrLitTbl(uint32_t eid, const std::string &val)
  {
    //Delegate to normal set string method
    return SetStrTbl(eid, val);
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    std::string val;
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
    std::string val;
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
  uint32_t _size;
  const HCEIDEnum *_eidenums;
};

//String list
template <class C>
class HCStringList : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, std::string &);
  typedef int (C::*AddMethod)(const std::string &);
  typedef int (C::*SubMethod)(const std::string &);

public:
  HCStringList(const std::string &name, C *object, GetMethod getmethod, AddMethod addmethod, SubMethod submethod, uint32_t maxsize)
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
  }

  virtual ~HCStringList()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_STRING;
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

  virtual void PrintVal(void)
  {
    std::string val;
    int lerr;
    uint32_t eid;

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
    for(eid=0; eid<_maxsize; eid++)
    {
      //Call get method
      lerr = (_object->*_getmethod)(eid, val);

      //Stop on error
      if(lerr != ERR_NONE)
        break;

      //Print value
      std::cout << " [" << eid << "] = \"" << val << "\" !" << ErrToString(lerr) << std::endl;
    }
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    st << _name;
    st << "\n  Type: strl";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << ((_addmethod == 0) && (_submethod == 0) ? "" : "W");
    st << "\n  Max Size: " << _maxsize;
  }

  virtual void SaveXML(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    //Generate XML information
    file << std::string(indent, ' ') << "<strl>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (((_addmethod == 0) || (_submethod == 0)) ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "  <maxsize>" << _maxsize << "</maxsize>" << std::endl;
    file << std::string(indent, ' ') << "</strl>" << std::endl;
  }

  virtual int GetStrTbl(uint32_t eid, std::string &val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      val.clear();
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val);
  }

  virtual int AddStr(const std::string &val)
  {
    //Check for null method
    if(_addmethod == 0)
      return ERR_ACCESS;

    //Call add method
    return (_object->*_addmethod)(val);
  }

  virtual int AddStrLit(const std::string &val)
  {
    //Delegate to normal add string method
    return AddStr(val);
  }

  virtual int SubStr(const std::string &val)
  {
    //Check for null method
    if(_submethod == 0)
      return ERR_ACCESS;

    //Call subtract method
    return (_object->*_submethod)(val);
  }

  virtual int SubStrLit(const std::string &val)
  {
    //Delegate to normal subtract string method
    return SubStr(val);
  }

  virtual bool GetCellTbl(uint32_t eid, HCCell *icell, HCCell *ocell)
  {
    uint8_t type;
    std::string val;
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
    std::string val;
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
    std::string val;
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

private:
  C *_object;
  GetMethod _getmethod;
  AddMethod _addmethod;
  SubMethod _submethod;
  uint32_t _maxsize;
};

#endif //_HCSTRING_HH_
