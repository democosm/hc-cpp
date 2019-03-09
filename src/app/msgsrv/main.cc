// Example text message HC server application
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

#include "error.hh"
#include "str.hh"
#include "device.hh"
#include "text.hh"
#include "hcconsole.hh"
#include "hccontainer.hh"
#include "hcparameter.hh"
#include "hcserver.hh"
#include "hcstring.hh"
#include "udpsocket.hh"
#include <getopt.h>
#include <inttypes.h>
#include <iostream>
#include <cassert>

using namespace std;

int main(int argc, char **argv)
{
  Text *text;
  HCContainer *topcont;
  UDPSocket *srvdev;
  HCServer *srv;
  HCConsole *hccons;
  uint16_t port;
  HCParameter *param;

  //Set argument default values
  port = 1500;

  //Check for invalid number of arguments
  if(argc > 2)
  {
    cout << "Usage: " << argv[0] << " <UDP PORT NUMBER (defaults to 1500)>" << "\n";
    return -1;
  }

  //Check for port specified
  if(argc == 2)
  {
    //Convert port to integer and check for error
    if(!StringConvert(argv[1], port))
    {
      cout << "Error converting port number (" << argv[1] << ")" << "\n";
      return -1;
    }
  }

  //Create text object
  text = new Text();

  //Create top container
  topcont = new HCContainer("");

  //Create server device
  srvdev = new UDPSocket(port);

  //Create server
  srv = new HCServer(srvdev, topcont, "Msg", __DATE__ " " __TIME__);

  //Add parameters
  param = new HCString<Text>("message", text, &Text::GetMessage, &Text::SetMessage); 
  topcont->Add(param);
  srv->Add(param);

  //Start HC server
  srv->Start();

  //Create HC console
  hccons = new HCConsole(topcont);

  //Run console in context of this thread
  hccons->Run();

  //Cleanup
  delete hccons;
  delete srv;
  delete srvdev;
  delete topcont;
  delete text;

  //Return success
  return 0;
}
