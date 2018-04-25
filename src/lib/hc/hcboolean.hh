// HC boolean
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

#ifndef _HCBOOLEAN_HH_
#define _HCBOOLEAN_HH_

#include "error.hh"
#include "hcclient.hh"
#include "hcparameter.hh"
#include "str.hh"
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//Boolean enumeration
struct HCBooleanEnum
{
  HCBooleanEnum()
  {
    //Initialize member variables
    _num = false;
    _str = "";
  }

  HCBooleanEnum(bool num, const std::string &str)
  {
    //Initialize member variables
    _num = num;
    _str = str;
  }

  bool _num;
  std::string _str;
};

//Boolean client stub
class HCBooleanCli
{
public:
  HCBooleanCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCBooleanCli()
  {
  }

  int Get(bool &val)
  {
    //Delegate to client
    return _cli->Get(_pid, val);
  }

  int Set(const bool val)
  {
    //Delegate to client
    return _cli->Set(_pid, val);
  }

  int IGet(uint32_t eid, bool &val)
  {
    //Delegate to client
    return _cli->IGet(_pid, eid, val);
  }

  int ISet(uint32_t eid, const bool val)
  {
    //Delegate to client
    return _cli->ISet(_pid, eid, val);
  }

private:
  HCClient *_cli;
  uint16_t _pid;
};

//Boolean
template <class C>
class HCBoolean : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(bool &val);
  typedef int (C::*SetMethod)(const bool);

public:
  HCBoolean(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, const HCBooleanEnum *valenums=0)
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

  virtual ~HCBoolean()
  {
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_BOOL;
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
    bool val;
    int lerr;
    uint32_t i;

    //Check for null method
    if(_getmethod == 0)
    {
      //Print value
      std::cout << _name << " !" << ErrToString(ERR_ACCESS) << std::endl;
      return;
    }

    //Get value
    lerr = (_object->*_getmethod)(val);

    //Check for no value enums
    if(_valenums == 0)
    {
      //Print value
      std::cout << _name << " = " << std::boolalpha << val << std::noboolalpha << " !" << ErrToString(lerr) << std::endl;
      return;
    }

    //Loop through value enums
    for(i=0; _valenums[i]._str.length() != 0; i++)
    {
      //Check for match
      if(_valenums[i]._num == val)
      {
        //Print value
        std::cout << _name << " = " << std::boolalpha << val << std::noboolalpha << " = \"" << _valenums[i]._str << "\" !" << ErrToString(lerr) << std::endl;
        return;
      }
    }

    //Print value indicate that no value enum found
    std::cout << _name << " = " << std::boolalpha << val << std::noboolalpha << "\e[31m\"\"\e[0m !" << ErrToString(lerr) << std::endl;
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: bool";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");

    //Print value enumeration information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << (_valenums[i]._num ? "true" : "false") << "," << _valenums[i]._str;
    }
  }

  virtual void SaveXML(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<bool>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>" << std::endl;

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _valenums[i]._num << "," << _valenums[i]._str << "</eq>" << std::endl;

      file << std::string(indent, ' ') << "  </valenums>" << std::endl;
    }

    file << std::string(indent, ' ') << "</bool>" << std::endl;
  }

  virtual int GetBool(bool &val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      val = false;
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(val);
  }

  virtual int SetBool(bool val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(val);
  }

  virtual int GetStr(std::string &val)
  {
    bool nval;
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

      //Not found
      val.clear();
      return ERR_RANGE;
    }

    //Convert to string
    val = nval ? "true" : "false";
    return ERR_NONE;
  }

  virtual int SetStr(const std::string &val)
  {
    bool nval;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_RANGE;

    //Set value
    return (_object->*_setmethod)(nval);
  }

  virtual int SetStrLit(const std::string &val)
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
    bool val;
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
    bool val;
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

  virtual bool GetValEnumStr(uint32_t ind, std::string &str)
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
  const HCBooleanEnum *_valenums;
};

//Boolean table
template <class C>
class HCBooleanTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, bool &);
  typedef int (C::*SetMethod)(uint32_t, const bool);

public:
  HCBooleanTable(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums=0, const HCBooleanEnum *valenums=0)
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

  virtual ~HCBooleanTable()
  {
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_BOOL;
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
    bool val;
    int lerr;
    uint32_t eid;
    std::string valstr;
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

      //Check for no value enums
      if(_valenums == 0)
      {
        //Check for no EID enums
        if(_eidenums == 0)
        {
          //Print value
          std::cout << " [" << eid << "] = " << std::boolalpha << val << std::noboolalpha << " !" << ErrToString(lerr) << std::endl;
        }
        else
        {
          //Convert EID to enum string and check for error
          if(!EIDNumToStr(eid, eidstr))
          {
            //Print value (indicate no EID enum string found)
            std::cout << " [\e[31m" << eid << "\e[0m] = " << std::boolalpha << val << std::noboolalpha << " !" << ErrToString(lerr) << std::endl;
          }
          else
          {
            //Print value (show EID enum string)
            std::cout << " [\"" << eidstr << "\"] = " << std::boolalpha << val << std::noboolalpha << " !" << ErrToString(lerr) << std::endl;
          }
        }
      }
      else
      {
        //Check for no EID enums
        if(_eidenums == 0)
        {
          //Convert value to enum string and check for error
          if(!ValToEnumStr(val, valstr))
          {
            //Print value (indicate no value enum string found)
            std::cout << " [" << eid << "] = " << std::boolalpha << val << std::noboolalpha << " = \e[31m\"\"\e[0m" << " !" << ErrToString(lerr) << std::endl;
          }
          else
          {
            //Print value (show value enum string)
            std::cout << " [" << eid << "] = " << std::boolalpha << val << std::noboolalpha << " = \"" << valstr << "\" !" << ErrToString(lerr) << std::endl;
          }
        }
        else
        {
          //Convert value to enum string and check for error
          if(!ValToEnumStr(val, valstr))
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no value or EID enum string found)
              std::cout << " [\e[31m" << eid << "\e[0m] = " << std::boolalpha << val << std::noboolalpha << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << std::endl;
            }
            else
            {
              //Print value (show EID enum string, indicate no value enum string found)
              std::cout << " [\"" << eidstr << "\"] = " << std::boolalpha << val << std::noboolalpha << " = \e[31m\"\"\e[0m !" << ErrToString(lerr) << std::endl;
            }
          }
          else
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no EID enum string found, show value enum string)
              std::cout << " [\e[31m" << eid << "\e[0m] = " << std::boolalpha << val << std::noboolalpha << " = \"" << valstr << "\" !" << ErrToString(lerr) << std::endl;
            }
            else
            {
              //Print value (show EID enum string and value enum string)
              std::cout << " [\"" << eidstr << "\"] = " << std::boolalpha << val << std::noboolalpha << " = \"" << valstr << "\" !" << ErrToString(lerr) << std::endl;
            }
          }
        }
      }
    }
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: boolt";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Size: " << _size;

    //Print EID enumeration information if it exists
    if(_eidenums != 0)
    {
      st << "\n  EID Enums:";

      for(i=0; _eidenums[i]._str[0] != '\0'; i++)
        st << "\n    " << _eidenums[i]._num << ',' << _eidenums[i]._str;
    }

    //Print value enumeration information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str[0] != '\0'; i++)
        st << "\n    " << (_valenums[i]._num ? "true" : "false") << "," << _valenums[i]._str;
    }
  }

  virtual void SaveXML(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<boolt>" << std::endl;
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << std::endl;
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << std::endl;
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << std::endl;
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << std::endl;

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << std::endl;

      for(i=0; _eidenums[i]._str[0] != '\0'; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << std::endl;

      file << std::string(indent, ' ') << "  </eidenums>" << std::endl;
    }

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>" << std::endl;

      for(i=0; _valenums[i]._str[0] != '\0'; i++)
        file << std::string(indent, ' ') << "    <eq>" << _valenums[i]._num << "," << _valenums[i]._str << "</eq>" << std::endl;

      file << std::string(indent, ' ') << "  </valenums>" << std::endl;
    }

    file << std::string(indent, ' ') << "</boolt>" << std::endl;
  }

  virtual int GetBoolTbl(uint32_t eid, bool &val)
  {
    //Check for null method
    if(_getmethod == 0)
    {
      val = false;
      return ERR_ACCESS;
    }

    //Call get method
    return (_object->*_getmethod)(eid, val);
  }

  virtual int SetBoolTbl(uint32_t eid, bool val)
  {
    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Call set method
    return (_object->*_setmethod)(eid, val);
  }

  virtual int GetStrTbl(uint32_t eid, std::string &val)
  {
    bool nval;
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

      //Not found
      val.clear();
      return ERR_RANGE;
    }

    //Convert to string
    val = nval ? "true" : "false";
    return ERR_NONE;
  }

  virtual int SetStrTbl(uint32_t eid, const std::string &val)
  {
    bool nval;
    uint32_t i;

    //Check for EID out of range
    if(eid >= _size)
      return ERR_EID;

    //Check for null method
    if(_setmethod == 0)
      return ERR_ACCESS;

    //Convert string value to native value and check for error
    if(!StringConvert(val, nval))
      return ERR_RANGE;

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

    //Set value
    return (_object->*_setmethod)(eid, nval);
  }

  virtual int SetStrLitTbl(uint32_t eid, const std::string &val)
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
    bool val;
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
    bool val;
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

  virtual bool ValToEnumStr(bool num, std::string &str)
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

  virtual bool GetValEnumStr(uint32_t ind, std::string &str)
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
  uint32_t _size;
  const HCBooleanEnum *_valenums;
  const HCEIDEnum *_eidenums;
};

#endif //_HCBOOLEAN_HH_
