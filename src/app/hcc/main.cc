// HC console application
//
// Copyright 2021 Democosm
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

#include "hcconnection.hh"
#include "hcconsole.hh"
#include "ipv4.hh"
#include "udpdevice.hh"
#include <argp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//Version information and bug report address
const char* argp_program_version = "1.0";
const char* argp_program_bug_address = "<democosm@gmail.com>";

//Argp keys
#define ARGP_KEY_ADDRESS 'a'
#define ARGP_KEY_PORT 'p'

//Option descriptions
static struct argp_option Optdesc[] =
{
  {"address", ARGP_KEY_ADDRESS, "IP ADDRESS", 0, "Specify IP address of HC server (default is 127.0.0.1)"},
  {"port", ARGP_KEY_PORT, "UDP PORT", 0, "Specify UDP port of HC server (default is 1500)"},
  { 0 }
};

//Application and argument descriptions
static char Appdesc[] = "HC console application.";
static char Argdesc[] = "";

//Storage for argument values
struct Argstore
{
  std::string address;
  uint16_t port;
};

//Parse a single option
static error_t ParseOption(int key, char* arg, struct argp_state* state)
{
  //Get input argument from state
  struct Argstore* argstore = (struct Argstore*)state->input;

  //Parse option based on key values
  switch(key)
  {
  case ARGP_KEY_ADDRESS:
    if(!IPv4AddrIsUnicast(arg))
    {
      printf("HC server IP address must be IPv4 unicast\n");
      exit(-1);
    }

    argstore->address = arg;
    break;
  case ARGP_KEY_PORT:
    if(!StringConvert(arg, argstore->port))
    {
      printf("Invalid HC server UDP port number (%s)\n", arg);
      exit(-1);
    }

    break;
  case ARGP_KEY_ARG:
    //Check for too many non-option arguments
    if(state->arg_num >= 0)
      argp_usage(state);

    break;
  case ARGP_KEY_END:
    //Check for not enough non-option arguments
    if(state->arg_num < 0)
      argp_usage(state);

    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

int main(int argc, char** argv)
{
  struct Argstore argstore;
  static struct argp argp = {Optdesc, ParseOption, Argdesc, Appdesc};
  HCContainer* topcont;
  Device* conndev;
  HCConnection* conn;
  HCConsole* hccons;

  //Initialize arguments
  argstore.address = "127.0.0.1";
  argstore.port = 1500;

  //Parse arguments
  argp_parse(&argp, argc, argv, 0, 0, &argstore);

  //Create top container
  topcont = new HCContainer("");

  //Create HC console
  hccons = new HCConsole(topcont);

  //Run console in context of this thread
  hccons->Run();

  //Create connection
  conndev = new UDPDevice(0, 0, argstore.address.c_str(), argstore.port);
  conn = new HCConnection(conndev, topcont, "conn", 100000);

  //Cleanup
  delete hccons;
  delete conn; //FIXME: conn destructor deletes conndev and shouldn't
  delete topcont;

  return 0;
}
