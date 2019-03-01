// HC file
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

#ifndef _HCFILE_HH_
#define _HCFILE_HH_

#include "const.hh"
#include "error.hh"
#include "hcclient.hh"
#include "hcparameter.hh"
#include <cassert>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <string>

//File client stub
class HCFileCli
{
public:
  HCFileCli(HCClient *cli, uint16_t pid)
  {
    //Assert valid arguments
    assert(cli != 0);

    //Initialize member variables
    _cli = cli;
    _pid = pid;
  }

  virtual ~HCFileCli()
  {
    //Cleanup
  }

  int Read(uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len)
  {
    //Delegate to client
    return _cli->Read(_pid, offset, val, maxlen, len);
  }

  int Write(uint32_t offset, uint8_t *val, uint16_t len)
  {
    //Delegate to client
    return _cli->Write(_pid, offset, val, len);
  }

private:
  HCClient *_cli;
  uint16_t _pid;
};

//File
template <class C>
class HCFile : public HCParameter
{
  //Method signatures
  typedef int (C::*ReadMethod)(uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len);
  typedef int (C::*WriteMethod)(uint32_t offset, uint8_t *val, uint16_t len);

public:
  //File buffer size (pid, offset, length, error code = 9 bytes)
  static const uint32_t BUFFSER_SIZE = HCCell::PAYLOAD_MAX - 9;

public:
  HCFile(const std::string &name, C *object, ReadMethod readmethod, WriteMethod writemethod)
  : HCParameter(name)
  {
    //Assert valid arguments
    assert(object != 0);

    //Initialize member variables
    _object = object;
    _readmethod = readmethod;
    _writemethod = writemethod;
    _buffer = new uint8_t[BUFFSER_SIZE];
  }

  virtual ~HCFile()
  {
    //Cleanup
  }

  virtual uint8_t GetType(void)
  {
    return TYPE_FILE;
  }

  virtual bool IsReadable(void)
  {
    if(_readmethod == 0)
      return false;

    return true;
  }

  virtual bool IsWritable(void)
  {
    if(_writemethod == 0)
      return false;

    return true;
  }

  virtual void PrintVal(void)
  {
    //Check for null method
    if(_readmethod == 0)
    {
      //Print value as not readable
      PrintNotReadable();

      return;
    }

    //Print value
    std::cout << TC_GREEN << _name << " = FILETYPE" << TC_RESET << "\n";
  }

  virtual void PrintInfo(std::ostream &st=std::cout)
  {
    st << _name;
    st << "\n  Type: file";
    st << "\n  Access: " << (_readmethod == 0 ? "" : "R") << (_writemethod == 0 ? "" : "W");
  }

  virtual void SaveInfo(std::ofstream &file, uint32_t indent, uint16_t pid)
  {
    //Generate XML information
    file << std::string(indent, ' ') << "<file>" << "\n";
    file << std::string(indent, ' ') << "  <pid>" << pid << "</pid>" << "\n";
    file << std::string(indent, ' ') << "  <name>" << _name << "</name>" << "\n";
    file << std::string(indent, ' ') << "  <acc>" << (_readmethod == 0 ? "" : "R") << (_writemethod == 0 ? "" : "W") << "</acc>" << "\n";
    file << std::string(indent, ' ') << "</file>" << "\n";
  }

  virtual int Upload(const std::string &val)
  {
    FILE *file;
    uint32_t offset;
    uint16_t len;
    int ierr;

    //Check for null method
    if(_writemethod == 0)
      return ERR_ACCESS;

    //Open local file and check for error
    if((file = fopen(val.c_str(), "r")) == NULL)
      return ERR_UNSPEC;

    //Transfer file piece by piece
    while(true)
    {
      //Get offset of local file
      offset = ftell(file);

      //Read part of local file
      len = fread(_buffer, 1, BUFFSER_SIZE, file);

      //Check for done
      if(len == 0)
        break;

      //Write to remote file
      ierr = (_object->*_writemethod)(offset, _buffer, len);

      //If timeout, retry read part of remote file
      if(ierr == ERR_TIMEOUT)
        ierr = (_object->*_writemethod)(offset, _buffer, len);

      //If timeout, retry read part of remote file
      if(ierr == ERR_TIMEOUT)
        ierr = (_object->*_writemethod)(offset, _buffer, len);

      //Check for error
      if(ierr != ERR_NONE)
      {
        //Close info file
        fclose(file);
        return ierr;
      }
    }

    //Close local file
    fclose(file);

    return ERR_NONE;
  }

  virtual int Download(const std::string &val)
  {
    FILE *file;
    uint16_t len;
    int ierr;

    //Check for null method
    if(_readmethod == 0)
      return ERR_ACCESS;

    //Open local file and check for error
    if((file = fopen(val.c_str(), "w")) == NULL)
      return ERR_UNSPEC;

    //Transfer file piece by piece
    while(true)
    {
      //Read part of remote file
      ierr = (_object->*_readmethod)((uint32_t)ftell(file), _buffer, BUFFSER_SIZE, len);

      //If timeout, retry read part of remote file
      if(ierr == ERR_TIMEOUT)
        ierr = (_object->*_readmethod)((uint32_t)ftell(file), _buffer, BUFFSER_SIZE, len);

      //If timeout, retry read part of remote file
      if(ierr == ERR_TIMEOUT)
        ierr = (_object->*_readmethod)((uint32_t)ftell(file), _buffer, BUFFSER_SIZE, len);

      //Check for error
      if(ierr != ERR_NONE)
      {
        //Close info file
        fclose(file);
        return ierr;
      }

      //Write to local file
      fwrite(_buffer, 1, len, file);

      //Check for done
      if(len < BUFFSER_SIZE)
        break;
    }

    //Close local file
    fclose(file);

    return ERR_NONE;
  }

  virtual bool ReadCell(uint32_t offset, uint16_t maxlen, HCCell *icell, HCCell *ocell)
  {
    uint16_t len;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Reduce maximum length as necessary
    if(maxlen > BUFFSER_SIZE)
      maxlen = BUFFSER_SIZE;

    //Check for valid method
    if(_readmethod != 0)
    {
      //Call read method
      lerr = (_object->*_readmethod)(offset, _buffer, maxlen, len);

      //Write value to outbound cell and check for error
      if(!ocell->Write(_buffer, len))
        return false;

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

  virtual bool WriteCell(uint32_t offset, HCCell *icell, HCCell *ocell)
  {
    uint16_t len;
    int lerr;

    //Assert valid arguments
    assert((icell != 0) && (ocell != 0));

    //Get value from inbound cell and check for error
    if(!icell->Read(_buffer, BUFFSER_SIZE, len))
      return false;

    //Check for valid method
    if(_writemethod != 0)
    {
      //Call write method
      lerr = (_object->*_writemethod)(offset, _buffer, len);
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
  ReadMethod _readmethod;
  WriteMethod _writemethod;
  uint8_t *_buffer;
};

#endif //_HCFILE_HH_
