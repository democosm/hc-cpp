// PI server application
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

#include "hcboolean.hh"
#include "hccall.hh"
#include "hcconsole.hh"
#include "hccontainer.hh"
#include "hcparameter.hh"
#include "hcserver.hh"
#include "hcfloatingpoint.hh"
#include "piserver.hh"
#include "udpsocket.hh"
#include <inttypes.h>

using namespace std;

int main(int argc, char **argv)
{
  PIServer *pisrv;
  HCContainer *topcont;
  UDPSocket *srvdev;
  HCServer *srv;
  HCConsole *hccons;
  HCParameter *param;

  //Create PI server object
  pisrv = new PIServer();

  //Create top container
  topcont = new HCContainer("");

  //Create server device
  srvdev = new UDPSocket(1500);

  //Create server
  srv = new HCServer(srvdev, topcont, "Raspberry PI Server", __DATE__" "__TIME__);

  //Add parameters
  param = new HCFloat<PIServer>("temperature", pisrv, &PIServer::GetTemperature, 0);
  topcont->Add(param);
  srv->Add(param);
  param = new HCBooleanTable<PIServer>("relayon", pisrv, &PIServer::GetRelayOn, &PIServer::SetRelayOn, 8);
  topcont->Add(param);
  srv->Add(param);
  param = new HCCallTable<PIServer>("pulserelayhigh", pisrv, &PIServer::PulseRelayHigh, 8);
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
  delete pisrv;

  //Return success
  return 0;
}
