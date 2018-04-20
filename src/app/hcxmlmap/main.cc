// HC XML to map file application
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

#include "hcparameter.hh"
#include "tinyxml2.hh"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace tinyxml2;
using namespace std;

uint8_t TypeCode(const char *val)
{
  //Convert string to type code
  if(strncmp(val, "call", 4) == 0)
    return HCParameter::TYPE_CALL;
  else if(strncmp(val, "s8", 2) == 0)
    return HCParameter::TYPE_INT8;
  else if(strncmp(val, "s16", 3) == 0)
    return HCParameter::TYPE_INT16;
  else if(strncmp(val, "s32", 3) == 0)
    return HCParameter::TYPE_INT32;
  else if(strncmp(val, "s64", 3) == 0)
    return HCParameter::TYPE_INT64;
  else if(strncmp(val, "u8", 2) == 0)
    return HCParameter::TYPE_UINT8;
  else if(strncmp(val, "u16", 3) == 0)
    return HCParameter::TYPE_UINT16;
  else if(strncmp(val, "u32", 3) == 0)
    return HCParameter::TYPE_UINT32;
  else if(strncmp(val, "u64", 3) == 0)
    return HCParameter::TYPE_UINT64;
  else if(strncmp(val, "f32", 3) == 0)
    return HCParameter::TYPE_FLOAT;
  else if(strncmp(val, "f64", 3) == 0)
    return HCParameter::TYPE_DOUBLE;
  else if(strncmp(val, "bool", 4) == 0)
    return HCParameter::TYPE_BOOL;
  else if(strcmp(val, "str") == 0)
    return HCParameter::TYPE_STRING;
  else if(strcmp(val, "file") == 0)
    return HCParameter::TYPE_FILE;

  //No type code
  return 0xFF;
}

uint32_t WriteString(const char *val, FILE *file)
{
  uint8_t len;

  //Check for string length too long
  if(strlen(val) > 255)
  {
    printf("%s - String too long\n", __FUNCTION__);
    exit(-1);
  }

  //Write string length and check for error
  len = strlen(val);
  if(fwrite(&len, 1, 1, file) != 1)
  {
    printf("%s - Error writing to file\n", __FUNCTION__);
    exit(-1);
  }

  //Write string and check for error
  if(fwrite(val, 1, len, file) != len)
  {
    printf("%s - Error writing to file\n", __FUNCTION__);
    exit(-1);
  }

  //Return length
  return (uint32_t)len + 1;
}

uint32_t WriteU8(uint8_t val, FILE *file)
{
  //Write value and check for error
  if(fwrite(&val, 1, sizeof(val), file) < sizeof(val))
  {
    printf("%s - Error writing to file\n", __FUNCTION__);
    exit(-1);
  }

  //Return length
  return (uint32_t)sizeof(val);
}

uint32_t WriteU16(uint16_t val, FILE *file)
{
  //Write value and check for error
  if(fwrite(&val, 1, sizeof(val), file) < sizeof(val))
  {
    printf("%s - Error writing to file\n", __FUNCTION__);
    exit(-1);
  }

  //Return length
  return (uint32_t)sizeof(val);
}

uint32_t WriteU32(uint32_t val, FILE *file)
{
  //Write value and check for error
  if(fwrite(&val, 1, sizeof(val), file) < sizeof(val))
  {
    printf("%s - Error writing to file\n", __FUNCTION__);
    exit(-1);
  }

  //Return length
  return (uint32_t)sizeof(val);
}

uint32_t WriteParameter(XMLElement *parent, FILE *file)
{
  XMLElement *elt;
  const char *name;
  const char *pid;

  //Initialize all fields to invalid
  name = 0;
  pid = 0;

  //Loop through all child elements
  for(elt = parent->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name
    if(strcmp(elt->Name(), "name") == 0)
      name = elt->GetText();
    else if(strcmp(elt->Name(), "pid") == 0)
      pid = elt->GetText();
  }

  //Check for a field not found (must not be a parameter)
  if((name == 0) || (pid == 0))
  {
    printf("%s - Name or PID not found\n", __FUNCTION__);
    exit(-1);
  }

  //Write name
  WriteString(name, file);

  //Write length
  WriteU32((uint32_t)(sizeof(uint16_t) + sizeof(uint8_t)), file);

  //Write PID and update length
  WriteU16((uint16_t)strtoul(pid, NULL, 0), file);

  //Write type and update length
  WriteU8(TypeCode(parent->Name()), file);

  //Return total length
  return (uint32_t)(strlen(name) + 1 + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));
}

uint32_t WriteContainer(XMLElement *parent, FILE *file)
{
  XMLElement *elt;
  const char *name;
  uint32_t len;
  long lenpos;

  //Initialize all fields to invalid
  name = 0;

  //Loop through all child elements
  for(elt = parent->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check name
    if(strcmp(elt->Name(), "name") == 0)
      name = elt->GetText();
  }

  //Check for a field not found (must not be a container)
  if(name == 0)
  {
    printf("%s - Name not found\n", __FUNCTION__);
    exit(-1);
  }

  //Write name
  WriteString(name, file);

  //Remember where we are in the file and seek ahead for storing length later
  lenpos = ftell(file);
  fseek(file, sizeof(uint32_t), SEEK_CUR);

  //Initialize length
  len = 0;

  //Write containers and parameters
  for(elt = parent->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check for container else parameter
    if(strcmp(elt->Name(), "cont") == 0)
      len += WriteContainer(elt, file);
    else if(strncmp(elt->Name(), "call", 4) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s8", 2) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s16", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s32", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s64", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u8", 2) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u16", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u32", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u64", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "f32", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "f64", 3) == 0)
      len += WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "bool", 4) == 0)
      len += WriteParameter(elt, file);
    else if(strcmp(elt->Name(), "str") == 0)
      len += WriteParameter(elt, file);
    else if(strcmp(elt->Name(), "file") == 0)
      len += WriteParameter(elt, file);
  }

  //Seek back to where we need to put length code in
  fseek(file, lenpos, SEEK_SET);

  //Write length
  WriteU32(len, file);

  //Seek back to end of file
  fseek(file, 0, SEEK_END);

  //Return total length
  return (uint32_t)(strlen(name) + 1 + sizeof(uint32_t) + len);
}

void WriteServer(XMLElement *parent, FILE *file)
{
  XMLElement *elt;

  //Write containers and parameters
  for(elt = parent->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
  {
    //Check for container else parameter
    if(strcmp(elt->Name(), "cont") == 0)
      WriteContainer(elt, file);
    else if(strncmp(elt->Name(), "call", 4) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s8", 2) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s16", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s32", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "s64", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u8", 2) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u16", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u32", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "u64", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "f32", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "f64", 3) == 0)
      WriteParameter(elt, file);
    else if(strncmp(elt->Name(), "bool", 4) == 0)
      WriteParameter(elt, file);
    else if(strcmp(elt->Name(), "str") == 0)
      WriteParameter(elt, file);
    else if(strcmp(elt->Name(), "file") == 0)
      WriteParameter(elt, file);
  }
}

void Usage(const char *appname)
{
  cout << "Usage: " << appname << " <XML FILE NAME> <MAP FILE NAME>" << endl;
}

int main(int argc, char **argv)
{
  XMLDocument doc;
  FILE *mapfile;

  //Check for wrong number of arguments
  if(argc != 3)
  {
    Usage(argv[0]);
    return -1;
  }

  //Parse XML file into document object model and check for error
  doc.LoadFile(argv[1]);

  //Check for error
  if(doc.ErrorID() != 0)
  {
    printf("Error loading file '%s'\n", "test.xml");
    return -1;
  }

  //Open map file and check for error
  if((mapfile = fopen(argv[2], "w")) == NULL)
  {
    printf("Error opening file '%s'\n", argv[2]);
    return -1;
  }

  //Write server contents to map file
  WriteServer(doc.FirstChildElement("server"), mapfile);

  //Close map file
  fclose(mapfile);

  return 0;
}
