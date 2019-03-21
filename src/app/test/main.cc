// HC test application
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

#include "crc.hh"
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
HCContainer *srvtopcont;
HCServer *srv;
HCParameter *param;
UDPSocket *clidev;
HCContainer *clitopcont;
HCClient *cli;

TEST(HC, ServerInit)
{
  uint32_t u32val;
  uint32_t methcrc;
  uint32_t filecrc;
  string sval;
  bool bval;

  //Verify methods return correct initial state
  ASSERT_EQ(ERR_NONE, srv->GetName(sval));
  ASSERT_EQ("Scratch", sval);
  ASSERT_EQ(ERR_NONE, srv->GetDebug(bval));
  ASSERT_FALSE(bval);
  ASSERT_EQ(ERR_NONE, srv->GetSendErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetRecvErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetCellErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetOpCodeErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetPIDErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetIntErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, srv->GetGoodXactCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);

  //Get server CRC by method and compare to file CRC
  ASSERT_EQ(ERR_NONE, srv->GetInfoFileCRC(methcrc));
  filecrc = CRC32File(".server-Scratch.xml");
  ASSERT_EQ(methcrc, filecrc);
}

TEST(HC, ClientInit)
{
  uint32_t u32val;

  //Verify all error counts in client are zero
  ASSERT_EQ(ERR_NONE, cli->GetSendErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetRecvErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetTransactionErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetCellErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetOpCodeErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetTimeoutErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetPIDErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetTypeErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetEIDErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetOffsetErrCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
  ASSERT_EQ(ERR_NONE, cli->GetGoodXactCnt(u32val));
  ASSERT_EQ(u32val, (uint32_t)0);
}

TEST(HC, ClientInfoDownload)
{
  string srvname;
  string cliname;
  string srvver;
  string cliver;
  uint32_t srvcrc;
  uint32_t clicrc;

  //Get server name directly, and from client and test for equal
  ASSERT_EQ(ERR_NONE, srv->GetName(srvname));
  ASSERT_EQ(ERR_NONE, cli->Get(HCServer::PID_NAME, cliname));
  ASSERT_EQ(cliname, srvname);

  //Get server version directly, and from client and test for equal
  ASSERT_EQ(ERR_NONE, srv->GetVersion(srvver));
  ASSERT_EQ(ERR_NONE, cli->Get(HCServer::PID_VERSION, cliver));
  ASSERT_EQ(cliver, srvver);

  //Download SIF file
  ASSERT_EQ(ERR_NONE, cli->DownloadSIF(HCServer::PID_INFOFILE, ".client-Scratch.xml"));

  //Get server SIF CRC and compare to client CRC
  ASSERT_EQ(ERR_NONE, cli->Get(HCServer::PID_INFOFILECRC, srvcrc));
  clicrc = CRC32File(".client-Scratch.xml");
  ASSERT_EQ(clicrc, srvcrc);
}

TEST(HC, ControlStrings)
{
  string testval;
  string srvval;
  string clival;

  testval = "Hello world!";

  //Set scratch string from client
  ASSERT_EQ(ERR_NONE, cli->Set(4, testval));

  //Get scratch string directly, and from client and test for equal
  ASSERT_EQ(ERR_NONE, scratch->GetString(srvval));
  ASSERT_EQ(ERR_NONE, cli->Get(4, clival));
  ASSERT_EQ(srvval, testval);
  ASSERT_EQ(clival, testval);
}

int main(int argc, char **argv)
{
  int result;

  //Create scratch object
  scratch = new Scratch();

  //Create server device
  srvdev = new UDPSocket(1500);

  //Create server top container
  srvtopcont = new HCContainer("");

  //Create server
  srv = new HCServer(srvdev, srvtopcont, "Scratch", __DATE__ " " __TIME__);

  //Add parameters
  param = new HCString<Scratch>("string", scratch, &Scratch::GetString, &Scratch::SetString);
  srvtopcont->Add(param);
  srv->Add(param);

  //Start server
  srv->Start();

  //Create client device
  clidev = new UDPSocket(0, "127.0.0.1", 1500);

  //Create client top container
  clitopcont = new HCContainer("");

  //Create client
  cli = new HCClient(clidev, clitopcont, 100000);

  //Initialize and run all tests
  testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  //Cleanup
  delete srv;
  delete srvtopcont;
  delete srvdev;
  delete scratch;

  return result;
}
