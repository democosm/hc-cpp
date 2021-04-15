// System
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

#include "system.hh"
#include <cassert>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Sysprintmaxdetail = 10;
FILE* Sysprintlogfile = NULL;

int SysCmd(const char *fmt, ...)
{
  char cmdstr[1000];
  int retval;
  va_list args;

  //Assert valid arguments
  assert(fmt != 0);

  //Start var args
  va_start(args, fmt);

  //Format command string
  retval = vsnprintf(cmdstr, sizeof(cmdstr), fmt, args);

  //End var args
  va_end(args);

  //Check for error formatting command string
  if(retval <= 0)
    return -1;

  //Ensure that command string is null terminated
  cmdstr[sizeof(cmdstr) - 1] = '\0';

  //Execute command through system function
  retval = system(cmdstr);

  //Check for exiting abnormally
  if(WIFEXITED(retval) == 0)
    return -1;

  //Return exit status from command
  return WEXITSTATUS(retval);
}

void SysPrint(int detail, const char *fmt, ...)
{
  va_list args;

  //Assert valid arguments
  assert((detail >= 0) && (detail <= 10) && (fmt != 0));

  //Filter out details beyond max level
  if(detail > Sysprintmaxdetail)
    return;

  //Start var args
  va_start(args, fmt);

  //Send to file or console
  if(Sysprintlogfile == NULL)
    vprintf(fmt, args);
  else
    vfprintf(Sysprintlogfile, fmt, args);

  //End var args
  va_end(args);
}

void SysPrintSetMaxDetail(int val)
{
  //Assert valid arguments
  assert((val >= -1) && (val <= 10));

  //Set maximum print detail
  Sysprintmaxdetail = val;
}

void SysPrintSetLogFile(const char* filename)
{
  FILE* temp;

  //Close file if open
  if(Sysprintlogfile != NULL)
  {
    //Temp variable used to minimize reentrancy issues without needing a mutex
    temp = Sysprintlogfile;
    Sysprintlogfile = NULL;
    fclose(temp);
  }

  //Open log file if name provided
  if(filename != 0)
    if((Sysprintlogfile = fopen(filename, "w")) == NULL)
      printf("SysPrint - Can't open log file '%s'\n", filename);
}
