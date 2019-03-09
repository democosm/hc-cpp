// HC boolean
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

#ifndef _HCBOOLEAN_HH_
#define _HCBOOLEAN_HH_

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
//Boolean enumeration
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//Boolean client stub
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//Boolean template
//-----------------------------------------------------------------------------
template <class C>
class HCBoolean : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(bool &val);
  typedef int (C::*SetMethod)(const bool);

public:
  HCBoolean(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, const HCBooleanEnum *valenums)
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
      std::cout << std::boolalpha << val << std::noboolalpha;
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
        std::cout << std::boolalpha << val << std::noboolalpha;
        std::cout << " = ";
        std::cout << "\"" << _valenums[i]._str << "\"";
        std::cout << " !" << ErrToString(lerr);
        std::cout << TC_RESET << "\n";
  
        return;
      }
    }

    //Print value indicate that no value enum found
    std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
    std::cout << " = ";
    std::cout << std::boolalpha << val << std::noboolalpha;
    std::cout << " = ";
    std::cout << TC_MAGENTA << "\"\"" << (lerr == ERR_NONE ? TC_GREEN : TC_RED);
    std::cout << " !" << ErrToString(lerr);
    std::cout << TC_RESET << "\n";
  }

  virtual void PrintConfig(const std::string &path, std::ostream &st=std::cout)
  {
    bool val;
    uint32_t i;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Call get method
    if((_object->*_getmethod)(val) != ERR_NONE)
      return;

    //Check for no value enums
    if(_valenums == 0)
    {
      //Print value
      st << path;
      st << _name;
      st << " = ";
      st << std::boolalpha << val << std::noboolalpha;
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

    //Print value no value enum found
    st << path;
    st << _name;
    st << " = ";
    st << std::boolalpha << val << std::noboolalpha;
    st << "\n";
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: bool";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");

    //Print value enumeration information if it exists
    if(_valenums != 0)
    {
      st << "\n  Value Enums:";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        st << "\n    " << (_valenums[i]._num ? "true" : "false") << "," << _valenums[i]._str;
    }
  }

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<bool>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>" << "\n";

      for(i=0; _valenums[i]._str.length() != 0; i++)
        file << std::string(indent, ' ') << "    <eq>" << _valenums[i]._num << "," << _valenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </valenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</bool>" << "\n";
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

//-----------------------------------------------------------------------------
//Boolean template (savable)
//-----------------------------------------------------------------------------
template <class C>
class HCBooleanS : public HCBoolean<C>
{
public:
  HCBooleanS(const std::string &name, C *object, int (C::*getmethod)(bool &), int (C::*setmethod)(const bool), const HCBooleanEnum *valenums)
  : HCBoolean<C>(name, object, getmethod, setmethod, valenums)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from boolean templates
//-----------------------------------------------------------------------------
template <class C>
class HCBool : public HCBoolean<C>
{
public:
  HCBool(const std::string &name, C *object, int (C::*getmethod)(bool &), int (C::*setmethod)(const bool))
  : HCBoolean<C>(name, object, getmethod, setmethod, 0)
  {
  }

  HCBool(const std::string &name, C *object, int (C::*getmethod)(bool &), int (C::*setmethod)(const bool), const HCBooleanEnum *valenums)
  : HCBoolean<C>(name, object, getmethod, setmethod, valenums)
  {
  }
};

template <class C>
class HCBoolS : public HCBooleanS<C>
{
public:
  HCBoolS(const std::string &name, C *object, int (C::*getmethod)(bool &), int (C::*setmethod)(const bool))
  : HCBooleanS<C>(name, object, getmethod, setmethod, 0)
  {
  }

  HCBoolS(const std::string &name, C *object, int (C::*getmethod)(bool &), int (C::*setmethod)(const bool), const HCBooleanEnum *valenums)
  : HCBooleanS<C>(name, object, getmethod, setmethod, valenums)
  {
  }
};

//-----------------------------------------------------------------------------
//Boolean table template
//-----------------------------------------------------------------------------
template <class C>
class HCBooleanTable : public HCParameter
{
public:
  //Method signatures
  typedef int (C::*GetMethod)(uint32_t, bool &);
  typedef int (C::*SetMethod)(uint32_t, const bool);

public:
  HCBooleanTable(const std::string &name, C *object, GetMethod getmethod, SetMethod setmethod, uint32_t size, const HCEIDEnum *eidenums, const HCBooleanEnum *valenums)
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
          std::cout << std::boolalpha << val << std::noboolalpha;
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
            std::cout << std::boolalpha << val << std::noboolalpha;
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
          }
          else
          {
            //Print value (show EID enum string)
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[\"" << eidstr << "\"]";
            std::cout << " = ";
            std::cout << std::boolalpha << val << std::noboolalpha;
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
          if(!ValToEnumStr(val, valstr))
          {
            //Print value (indicate no value enum string found)
            std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
            std::cout << "[" << eid << "]";
            std::cout << " = ";
            std::cout << std::boolalpha << val << std::noboolalpha;
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
            std::cout << std::boolalpha << val << std::noboolalpha;
            std::cout << " = ";
            std::cout << "\"" << valstr << "\"";
            std::cout << " !" << ErrToString(lerr);
            std::cout << TC_RESET << "\n";
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
              std::cout << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << _name;
              std::cout << "[" << TC_MAGENTA << eid << (lerr == ERR_NONE ? TC_GREEN : TC_RED) << "]";
              std::cout << " = ";
              std::cout << std::boolalpha << val << std::noboolalpha;
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
              std::cout << std::boolalpha << val << std::noboolalpha;
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
              std::cout << std::boolalpha << val << std::noboolalpha;
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
              std::cout << std::boolalpha << val << std::noboolalpha;
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

  virtual void PrintConfig(const std::string &path, std::ostream &st=std::cout)
  {
    bool val;
    uint32_t eid;
    std::string valstr;
    std::string eidstr;

    //Check for not saveable
    if((_getmethod == 0) || (_setmethod == 0))
      return;

    //Loop through all elements
    for(eid=0; eid<_size; eid++)
    {
      //Call get method
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
          st << std::boolalpha << val << std::noboolalpha;
          st << "\n";
        }
        else
        {
          //Convert EID to enum string and check for error
          if(!EIDNumToStr(eid, eidstr))
          {
            //Print value no EID enum string found
            st << path;
            st << _name;
            st << "[" << eid << "]";
            st << " = ";
            st << std::boolalpha << val << std::noboolalpha;
            st << "\n";
          }
          else
          {
            //Print value (show EID enum string)
            st << path;
            st << _name;
            st << "[\"" << eidstr << "\"]";
            st << " = ";
            st << std::boolalpha << val << std::noboolalpha;
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
          if(!ValToEnumStr(val, valstr))
          {
            //Print value (indicate no value enum string found)
            st << path;
            st << _name;
            st << "[" << eid << "]";
            st << " = ";
            st << std::boolalpha << val << std::noboolalpha;
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
          if(!ValToEnumStr(val, valstr))
          {
            //Convert EID to enum string and check for error
            if(!EIDNumToStr(eid, eidstr))
            {
              //Print value (indicate no value or EID enum string found)
              st << path;
              st << _name;
              st << "[" << eid << "]";
              st << " = ";
              st << std::boolalpha << val << std::noboolalpha;
              st << "\n";
            }
            else
            {
              //Print value (show EID enum string, indicate no value enum string found)
              st << path;
              st << _name;
              st << "[\"" << eidstr << "\"]";
              st << " = ";
              st << std::boolalpha << val << std::noboolalpha;
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

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    uint32_t i;

    //Print information
    st << _name;
    st << "\n  Type: boolt";
    st << "\n  Access: " << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W");
    st << "\n  Savable: " << (IsSavable() ? "Yes" : "No");
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

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    uint32_t i;

    //Generate XML information
    file << std::string(indent, ' ') << "<boolt>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_getmethod == 0 ? "" : "R") << (_setmethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "  <sav>" << (IsSavable() ? "Yes" : "No") << "</sav>" << "\n";
    file << std::string(indent, ' ') << "  <size>" << _size << "</size>" << "\n";

    if(_eidenums != 0)
    {
      file << std::string(indent, ' ') << "  <eidenums>" << "\n";

      for(i=0; _eidenums[i]._str[0] != '\0'; i++)
        file << std::string(indent, ' ') << "    <eq>" << _eidenums[i]._num << "," << _eidenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </eidenums>" << "\n";
    }

    if(_valenums != 0)
    {
      file << std::string(indent, ' ') << "  <valenums>" << "\n";

      for(i=0; _valenums[i]._str[0] != '\0'; i++)
        file << std::string(indent, ' ') << "    <eq>" << _valenums[i]._num << "," << _valenums[i]._str << "</eq>" << "\n";

      file << std::string(indent, ' ') << "  </valenums>" << "\n";
    }

    file << std::string(indent, ' ') << "</boolt>" << "\n";
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

//-----------------------------------------------------------------------------
//Boolean table template (savable)
//-----------------------------------------------------------------------------
template <class C>
class HCBooleanTableS : public HCBooleanTable<C>
{
public:
  HCBooleanTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCEIDEnum *eidenums, const HCBooleanEnum *valenums)
  : HCBooleanTable<C>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }

  virtual bool IsSavable(void)
  {
    return true;
  }
};

//-----------------------------------------------------------------------------
//Classes derived from boolean table templates
//-----------------------------------------------------------------------------
template <class C>
class HCBoolTable : public HCBooleanTable<C>
{
public:
  HCBoolTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size)
  : HCBooleanTable<C>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCBoolTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCBooleanEnum *valenums)
  : HCBooleanTable<C>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCBoolTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCEIDEnum *eidenums)
  : HCBooleanTable<C>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCBoolTable(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCEIDEnum *eidenums, const HCBooleanEnum *valenums)
  : HCBooleanTable<C>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

template <class C>
class HCBoolTableS : public HCBooleanTableS<C>
{
public:
  HCBoolTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size)
  : HCBooleanTableS<C>(name, object, getmethod, setmethod, size, 0, 0)
  {
  }

  HCBoolTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCBooleanEnum *valenums)
  : HCBooleanTableS<C>(name, object, getmethod, setmethod, size, 0, valenums)
  {
  }

  HCBoolTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCEIDEnum *eidenums)
  : HCBooleanTableS<C>(name, object, getmethod, setmethod, size, eidenums, 0)
  {
  }

  HCBoolTableS(const std::string &name, C *object, int (C::*getmethod)(uint32_t, bool &), int (C::*setmethod)(uint32_t, const bool), uint32_t size, const HCEIDEnum *eidenums, const HCBooleanEnum *valenums)
  : HCBooleanTableS<C>(name, object, getmethod, setmethod, size, eidenums, valenums)
  {
  }
};

#endif //_HCBOOLEAN_HH_
