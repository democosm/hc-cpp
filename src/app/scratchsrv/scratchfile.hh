// File scratch pad class
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

#ifndef _SCRATCHFILE_HH_
#define _SCRATCHFILE_HH_

#include "hcparameter.hh"
#include <inttypes.h>
#include <string>

using namespace std;

class ScratchFile
{
public:
  ScratchFile(const char *filename)
  {
    //Initialize member variables
    _filename = filename;
  }

  ~ScratchFile()
  {
  }

  int Read(uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len)
  {
    FILE *file;

    //Open file and check for error
    if((file = fopen(_filename.c_str(), "r")) == NULL)
    {
      //Indicate zero bytes read
      len = 0;
      return ERR_ACCESS;
    }

    //Seek to offset
    if(fseek(file, offset, SEEK_SET) < 0)
    {
      //Close file
      fclose(file);

      //Indicate zero bytes read
      len = 0;
      return ERR_RANGE;
    }

    //Read from file
    len = fread(val, 1, maxlen, file);

    //Close file
    fclose(file);
    return ERR_NONE;
  }

  int Write(uint32_t offset, uint8_t *val, uint16_t len)
  {
    FILE *file;

    //Open file and check for error
    if((file = fopen(_filename.c_str(), "w")) == NULL)
      return ERR_ACCESS;

    //Seek to offset
    if(fseek(file, offset, SEEK_SET) < 0)
    {
      //Close file
      fclose(file);
      return ERR_RANGE;
    }

    //Write to file
    fwrite(val, 1, len, file);

    //Close file
    fclose(file);
    return ERR_NONE;
  }

private:
  string _filename;
};

#endif //_SCRATCHFILE_HH_