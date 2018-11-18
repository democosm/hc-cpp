// HC test application
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

#include "device.hh"
#include "scratch.hh"
#include "hccontainer.hh"
#include "hcparameter.hh"
#include "hcserver.hh"
#include "hcstring.hh"
#include "udpsocket.hh"
#include "gtest.h"
#include <stdio.h>

using namespace std;

Scratch *scratch;
UDPSocket *srvdev;
HCContainer *topcont;
HCServer *srv;
HCParameter *param;

TEST(HC, Blah)
{
  string sval;
  bool bval;

  ASSERT_EQ(ERR_NONE, srv->GetName(sval));
  ASSERT_EQ("Scratch", sval);
  ASSERT_EQ(ERR_NONE, srv->GetDebug(bval));
  ASSERT_FALSE(bval);
}

int main(int argc, char **argv)
{
  int result;

  //Create scratch object
  scratch = new Scratch();

  //Create server device
  srvdev = new UDPSocket(1500);

  //Create top container
  topcont = new HCContainer("");

  //Create server
  srv = new HCServer(srvdev, topcont, "Scratch", __DATE__ " " __TIME__);

  //Add parameters
  param = new HCString<Scratch>("string", scratch, &Scratch::GetString, &Scratch::SetString);
  topcont->Add(param);
  srv->Add(param);

  //Start server
  srv->Start();

  //Initialize and run all tests
  testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  //Cleanup
  delete srv;
  delete topcont;
  delete srvdev;
  delete scratch;

  return result;
}
