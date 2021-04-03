// Class for controlling GPIO pins of a raspberry pi 3
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

#include "pigpio.hh"
#include "error.hh"
#include "thread.hh"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

PiGPIO::PiGPIO(uint8_t num, bool output, bool initval)
{
  char filename[200];
  FILE *file;

  //Cache number
  _num = num;

  //Open export file and check for error
  if((file = fopen ("/sys/class/gpio/export", "w")) == NULL)
  {
    printf("%s - Unable to open export file\n", __FUNCTION__);
    return;
  }

  //Write number to file to get control files to show up
  fprintf(file, "%" PRIu8 "\n", num);

  //Close file
  fclose(file);

  //Takes a while for control files to show up
  ThreadSleep(100000);

  //Determine direction file name
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu8 "/direction", _num);

  //Open file and check for error
  if((file = fopen(filename, "w")) == NULL)
  {
    printf("%s - Unable to open direction file for GPIO%" PRIu8 "\n", __FUNCTION__, _num);
    return;
  }

  //Write value to file
  fprintf(file, "%s\n", output ? "out" : "in");

  //Close file
  fclose(file);

  //Set initial value
  SetValue(initval);
}

PiGPIO::~PiGPIO()
{
  char filename[200];
  FILE *file;

  //Determine direction file name
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu8 "/direction", _num);

  //Open file and check for error
  if((file = fopen(filename, "w")) == NULL)
  {
    printf("%s - Unable to open direction file for GPIO%" PRIu8 "\n", __FUNCTION__, _num);
    return;
  }

  //Write value to file
  fprintf(file, "%s\n", "in");

  //Close file
  fclose(file);

  //Open export file and check for error
  if((file = fopen ("/sys/class/gpio/unexport", "w")) == NULL)
  {
    printf("%s - Unable to open unexport file\n", __FUNCTION__);
    return;
  }

  //Write number to file
  fprintf(file, "%" PRIu8 "\n", _num);

  //Close file
  fclose(file);
}

int PiGPIO::GetValue(bool &val)
{
  char filename[200];
  FILE *file;
  uint8_t valint;

  //Determine value file name
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu8 "/value", _num);

  //Open file and check for error
  if((file = fopen(filename, "r")) == NULL)
  {
    printf("%s - Unable to open value file for GPIO%" PRIu8 "\n", __FUNCTION__, _num);
    return ERR_ACCESS;
  }

  //Read relay value from file
  if(fscanf(file, "%" SCNu8, &valint) != 1)
  {
    //Close file and return error
    fclose(file);
    return ERR_UNSPEC;
  }

  //Close file
  fclose(file);

  //Convert to bool
  val = valint == 1 ? true : false;

  return ERR_NONE;
}

int PiGPIO::SetValue(bool val)
{
  char filename[200];
  FILE *file;

  //Determine value file name
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu8 "/value", _num);

  //Open file and check for error
  if((file = fopen(filename, "w")) == NULL)
  {
    printf("%s - Unable to open value file for GPIO%" PRIu8 "\n", __FUNCTION__, _num);
    return ERR_ACCESS;
  }

  //Write value to file
  fprintf(file, "%s\n", val ? "1" : "0");

  //Close file
  fclose(file);

  return ERR_NONE;
}
