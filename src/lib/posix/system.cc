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

#include "error.hh"
#include "system.hh"
#include <cassert>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

bool Systemprintenable = false;
uint8_t Systemmaxprintdetail = 0;
char Systemprintlogfilename[100] = {};
FILE* Systemprintlogfile = NULL;

System::System()
{
}

System::~System()
{
}

int System::GetPrintEnable(bool& val)
{
  val = SystemGetPrintEnable();
  return ERR_NONE;
}

int System::SetPrintEnable(const bool val)
{
  SystemSetPrintEnable(val);
  return ERR_NONE;
}

int System::GetMaxPrintDetail(uint8_t& val)
{
  val = SystemGetMaxPrintDetail();
  return ERR_NONE;
}

int System::SetMaxPrintDetail(const uint8_t val)
{
  //Check for range error
  if(val > 10)
    return ERR_RANGE;

  SystemSetMaxPrintDetail(val);
  return ERR_NONE;
}

int System::GetPrintLogFile(string& val)
{
  val = SystemGetPrintLogFile();
  return ERR_NONE;
}

int System::SetPrintLogFile(const string& val)
{
  if(val.length() == 0)
    SystemSetPrintLogFile(0);
  else
    SystemSetPrintLogFile(val.c_str());

  return ERR_NONE;
}

int SystemExecute(const char *fmt, ...)
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

void SystemPrint(uint8_t detail, const char *fmt, ...)
{
  va_list args;

  //Assert valid arguments
  assert((detail <= 10) && (fmt != 0));

  //Check for output disabled
  if(!Systemprintenable)
    return;

  //Filter out details beyond max level
  if(detail > Systemmaxprintdetail)
    return;

  //Start var args
  va_start(args, fmt);

  //Send to file or console
  if(Systemprintlogfile == NULL)
    vprintf(fmt, args);
  else
    vfprintf(Systemprintlogfile, fmt, args);

  //End var args
  va_end(args);
}

bool SystemGetPrintEnable(void)
{
  //Return print enable
  return Systemprintenable;
}

void SystemSetPrintEnable(bool enable)
{
  //Set print enable
  Systemprintenable = enable;
}

uint8_t SystemGetMaxPrintDetail(void)
{
  //Return maximum print detail
  return Systemmaxprintdetail;
}

void SystemSetMaxPrintDetail(uint8_t detail)
{
  //Assert valid arguments
  assert(detail <= 10);

  //Set maximum print detail
  Systemmaxprintdetail = detail;
}

const char* SystemGetPrintLogFile(void)
{
  //Return print log file name
  return Systemprintlogfilename;
}

void SystemSetPrintLogFile(const char* filename)
{
  FILE* temp;

  //Close file if open
  if(Systemprintlogfile != NULL)
  {
    //Temp variable used to minimize reentrancy issues without needing a mutex
    temp = Systemprintlogfile;
    Systemprintlogfile = NULL;
    Systemprintlogfilename[0] = '\0';
    fclose(temp);
  }

  //Open log file if name provided
  if(filename != 0)
  {
    if((Systemprintlogfile = fopen(filename, "w")) == NULL)
    {
      printf("SysPrint - Can't open log file '%s'\n", filename);
      return;
    }

    //Cache file name
    strncpy(Systemprintlogfilename, filename, sizeof(Systemprintlogfilename));
    Systemprintlogfilename[sizeof(Systemprintlogfilename) - 1] = '\0';
  }
}
