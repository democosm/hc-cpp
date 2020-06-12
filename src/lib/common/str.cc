// String functions
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

#define __STDC_LIMIT_MACROS
#include "str.hh"
#include "error.hh"
#include <arpa/inet.h>
#include <cassert>
#include <stdint.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

bool StrExp(const string &str, const string &exp, size_t strind, size_t expind)
{
  size_t strsiz;
  size_t expsiz;

  //Get size of expression
  expsiz = exp.size();

  //Everything matches empty expression
  if(expind >= expsiz)
    return true;

  //Get size of string
  strsiz = str.size();

  //Consume character loop
  while(true)
  {
    //Check for end of string
    if(strind >= strsiz)
    {
      //Check for end of expression
      if(expind >= expsiz)
        return true;

      //End of string, but expression expects more
      return false;
    }

    //End of expression, but string expects more
    if(expind >= expsiz)
      return false;

    //Check for no match and no wildcard in expression
    if((str.at(strind) != exp.at(expind)) && (exp.at(expind) != '*') && (exp.at(expind) != '?'))
      return false;

    //Check for asterisk in expression
    if(exp.at(expind) == '*')
    {
      //Move forward in expression
      expind++;

      //Check for asterisk as last character in expression
      if(expind >= expsiz)
        return true;

      //Check for match after asterisk moving forward in string
      while(strind < strsiz)
      {
        //Move forward in string
        strind++;

        //Recurse
        if(StrExp(str, exp, strind, expind))
          return true;
      }

      //No match after asterisk
      return false;
    }

    //Move forward in string and expression
    strind++;
    expind++;
  }

  return false;
}

bool StringIPv4Convert(const string &str, uint32_t &val)
{
  struct sockaddr_in sa;

  //Initialize value to zero
  val = 0;

  //Convert string to struct
  if(inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 1)
    return false;

  //Return value
  val = ntohl(sa.sin_addr.s_addr);
  return true;
}

bool StringConvert(const string &str, bool &val)
{
  //Check for false
  if((str == "0") || (str == "false"))
  {
    val = false;
    return true;
  }

  //Check for true
  if((str == "1") || (str == "true"))
  {
    val = true;
    return true;
  }

  //Error
  return false;
}

bool StringConvert(const string &str, int8_t &val)
{
  intmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoimax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if((temp < INT8_MIN) || (temp > INT8_MAX))
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, int16_t &val)
{
  intmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoimax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if((temp < INT16_MIN) || (temp > INT16_MAX))
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, int32_t &val)
{
  intmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoimax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if((temp < INT32_MIN) || (temp > INT32_MAX))
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, int64_t &val)
{
  intmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoimax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if((temp < INT64_MIN) || (temp > INT64_MAX))
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, uint8_t &val)
{
  uintmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoumax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if(temp > UINT8_MAX)
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, uint16_t &val)
{
  uintmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoumax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if(temp > UINT16_MAX)
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, uint32_t &val)
{
  uintmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoumax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if(temp > UINT32_MAX)
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, uint64_t &val)
{
  uintmax_t temp;
  char *end;

  //Initialize value to zero
  val = 0;

  //Convert the value
  temp = strtoumax(str.c_str(), &end, 0);

  //Check for error
  if(*end != '\0')
    return false;

  //Check for value out of range
  if(temp > UINT64_MAX)
    return false;

  //Set value for return
  val = temp;
  return true;
}

bool StringConvert(const string &str, float &val)
{
  char *end;

  //Convert the value
  val = strtof(str.c_str(), &end);

  //Check for error
  if(*end != '\0')
    return false;

  return true;
}

bool StringConvert(const string &str, double &val)
{
  char *end;

  //Convert the value
  val = strtod(str.c_str(), &end);

  //Check for error
  if(*end != '\0')
    return false;

  return true;
}

bool StringConvert(const string &str, string &val)
{
  val = str;
  return true;
}

bool StringConvert(const string &str, float &val0, float &val1, float &val2)
{
  //Convert the value and check for error
  if(sscanf(str.c_str(), " %f , %f , %f ", &val0, &val1, &val2) != 3)
    return false;

  return true;
}

bool StringConvert(const string &str, double &val0, double &val1, double &val2)
{
  //Convert the value and check for error
  if(sscanf(str.c_str(), " %lf , %lf , %lf ", &val0, &val1, &val2) != 3)
    return false;

  return true;
}

bool StringConvert(const char *str, bool &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, int8_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, int16_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, int32_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, int64_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, uint8_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, uint16_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, uint32_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, uint64_t &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, float &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, double &val)
{
  string temp = str;
  return StringConvert(temp, val);
}

bool StringConvert(const char *str, string &val)
{
  val = str;
  return true;
}

bool StringConvert(const char *str, float &val0, float &val1, float &val2)
{
  string temp = str;
  return StringConvert(temp, val0, val1, val2);
}

bool StringConvert(const char *str, double &val0, double &val1, double &val2)
{
  string temp = str;
  return StringConvert(temp, val0, val1, val2);
}

void StringPrint(bool val, string &str)
{
  str = val ? "true" : "false";
}

void StringPrint(int8_t val, string &str)
{
  stringstream ss;
  ss << (int16_t)val;
  str = ss.str();
}

void StringPrint(int16_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(int32_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(int64_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(uint8_t val, string &str)
{
  stringstream ss;
  ss << (uint16_t)val;
  str = ss.str();
}

void StringPrint(uint16_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(uint32_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(uint64_t val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(float val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(double val, string &str)
{
  stringstream ss;
  ss << val;
  str = ss.str();
}

void StringPrint(float val0, float val1, float val2, string &str)
{
  stringstream ss;
  ss << val0 << ", " << val1 << ", " << val2;
  str = ss.str();
}

void StringPrint(double val0, double val1, double val2, string &str)
{
  stringstream ss;
  ss << val0 << ", " << val1 << ", " << val2;
  str = ss.str();
}
