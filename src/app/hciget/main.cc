// HC iget application
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

#include "hcclient.hh"
#include "hcutility.hh"
#include "str.hh"
#include "udpsocket.hh"
#include <iostream>
#include <stdio.h>

using namespace std;

void Usage(const char *appname)
{
  cout << "Usage: " << appname << " <SERVER IP ADDRESS> <SERVER PORT> <PATHNAME> <EID> <NLD FILE NAME>" << endl;
}

int main(int argc, char **argv)
{
  uint16_t port;
  UDPSocket *dev;
  HCContainer *top;
  HCClient *cli;
  string pathname;
  uint32_t eid;
  FILE *nldfile;
  uint16_t pid;
  uint8_t type;
  int8_t int8val;
  int16_t int16val;
  int32_t int32val;
  int64_t int64val;
  uint8_t uint8val;
  uint16_t uint16val;
  uint32_t uint32val;
  uint64_t uint64val;
  float floatval;
  double doubleval;
  bool boolval;
  string stringval;
  int ierr;

  //Check for wrong number of arguments
  if(argc != 6)
  {
    Usage(argv[0]);
    return -1;
  }

  //Convert server port number to integer and check for error
  if(!StringConvert(argv[2], port))
  {
    cout << "Invalid port number (" << argv[2] << ")" << endl;
    Usage(argv[0]);
    return -1;
  }

  //Convert EID to integer form and check for error
  if(!StringConvert(argv[4], eid))
  {
    cout << "Invalid EID (" << argv[4] << ")" << endl;
    Usage(argv[0]);
    return -1;
  }

  //Create device
  dev = new UDPSocket(0, argv[1], port);

  //Create dummy top
  top = new HCContainer("");

  //Create client
  cli = new HCClient(dev, top, 1000000);

  //Turn path name into c++ string
  pathname = argv[3];

  //Open NLD file and check for error
  if((nldfile = fopen(argv[5], "rb")) == NULL)
  {
    cout << "Can't open NLD file " << argv[5] << endl;
    return -1;
  }

  //Lookup PID and type and check for error
  if(!HCUtility::NLDLookup(nldfile, pathname, pid, type))
  {
    cout << "Can't find entry for " << pathname << " in " << argv[5] << endl;
    return -1;
  }

  //Close NLD file
  fclose(nldfile);

  //Transaction depends on type code
  switch(type)
  {
  case HCParameter::TYPE_INT8:
    if((ierr = cli->IGet(pid, eid, int8val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << (int16_t)int8val << endl;

    break;
  case HCParameter::TYPE_INT16:
    if((ierr = cli->IGet(pid, eid, int16val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << int16val << endl;

    break;
  case HCParameter::TYPE_INT32:
    if((ierr = cli->IGet(pid, eid, int32val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << int32val << endl;

    break;
  case HCParameter::TYPE_INT64:
    if((ierr = cli->IGet(pid, eid, int64val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << int64val << endl;

    break;
  case HCParameter::TYPE_UINT8:
    if((ierr = cli->IGet(pid, eid, uint8val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << (uint16_t)uint8val << endl;

    break;
  case HCParameter::TYPE_UINT16:
    if((ierr = cli->IGet(pid, eid, uint16val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << uint16val << endl;

    break;
  case HCParameter::TYPE_UINT32:
    if((ierr = cli->IGet(pid, eid, uint32val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << uint32val << endl;

    break;
  case HCParameter::TYPE_UINT64:
    if((ierr = cli->IGet(pid, eid, uint64val)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << uint64val << endl;

    break;
  case HCParameter::TYPE_FLOAT:
    if((ierr = cli->IGet(pid, eid, floatval)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << floatval << endl;

    break;
  case HCParameter::TYPE_DOUBLE:
    if((ierr = cli->IGet(pid, eid, doubleval)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << doubleval << endl;

    break;
  case HCParameter::TYPE_BOOL:
    if((ierr = cli->IGet(pid, eid, boolval)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << boolval << endl;

    break;
  case HCParameter::TYPE_STRING:
    if((ierr = cli->IGet(pid, eid, stringval)) != ERR_NONE)
      cout << ErrToString(ierr) << endl;
    else
      cout << stringval << endl;

    break;
  default:
    cout << ErrToString(ERR_TYPE) << endl;
    break;
  }

  //Cleanup
  delete cli;
  delete top;
  delete dev;

  return 0;
}
