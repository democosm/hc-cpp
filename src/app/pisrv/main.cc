// PI server application
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

#include "hcboolean.hh"
#include "hccall.hh"
#include "hcconsole.hh"
#include "hccontainer.hh"
#include "hcinteger.hh"
#include "hcparameter.hh"
#include "hcqserver.hh"
#include "hcserver.hh"
#include "hcfloat.hh"
#include "i2c.hh"
#include "i2cbus.hh"
#include "pca9685.hh"
#include "pca9685servo.hh"
#include "piserver.hh"
#include "udpdevice.hh"
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>

using namespace std;

//Description strings
static const char *Appversion = "1.0.0 " __DATE__ " " __TIME__;
static const char *Appbugaddress = "<democosm@gmail.com>";
static const char Appdoc[] = "Creates a server to control the Raspberry Pi.";

//Application options
static struct option Longopts[] =
{
  { "daemon", 0, NULL, 'd' },
  { NULL, 0, NULL, 0 }
};

//Storage for parsed arguments
struct Args
{
  bool daemon;
};

void Usage()
{
  //Print usage information
  cout << "Raspberry Pi HC server" << "\n";
  cout << "Version: " << Appversion << "\n";
  cout << "Report bugs to: " << Appbugaddress << "\n";
  cout << Appdoc << "\n";
  cout << "[-d, --daemon] Spawn in background mode" << "\n";
}

bool ParseOptions(int argc, char **argv, struct Args* args)
{
  int ch;
  bool valid;

  //Loop through all options or until invalid option reached
  valid = true;
  while(((ch = getopt_long(argc, argv, "p", Longopts, NULL)) != -1) && valid)
  {
    switch(ch)
    {
    case 'd':
      args->daemon = true;
      break;
    default:
      valid = false;
      Usage();
      break;
    }
  }

  argc -= optind;
  argv += optind;

  return valid;
}

int main(int argc, char **argv)
{
//  I2C *i2c;
//  I2CBus *pca9685bus;
//  PCA9685 *pca9685;
//  PCA9685Servo *frontdoor;
//  PCA9685Servo *leftwindow;
//  PCA9685Servo *rightwindow;
//  PCA9685Servo *cellardoor;
  PIServer *pisrv;
  HCContainer *topcont;
  UDPDevice *srvdev;
  HCServer *srv;
  Device *qsrvdev;
  HCQServer *qsrv;
  HCConsole *hccons;
  HCParameter *param;
  struct Args args;

  //Set argument default values
  args.daemon = false;

  //Parse arguments
  if(!ParseOptions(argc, argv, &args))
    return -1;

  //Check for daemon mode
  if(args.daemon)
    if(daemon(1, 1) != 0)
      return -1;

  //Create I2C driver
//  i2c = new I2C("/dev/i2c-1");

  //Create PCA9685 bus
//  pca9685bus = new I2CBus(i2c, 0x6F);

  //Create PCA9685 PWM driver with 50Hz PWM frequency
//  pca9685 = new PCA9685(pca9685bus, 50);

  //Create PCA9685 servos
//  frontdoor = new PCA9685Servo(pca9685, 0, 0.027, 0.127);
//  leftwindow = new PCA9685Servo(pca9685, 1, 0.027, 0.127);
//  rightwindow = new PCA9685Servo(pca9685, 14, 0.027, 0.127);
//  cellardoor = new PCA9685Servo(pca9685, 15, 0.027, 0.127);

  //Create PI server object
  pisrv = new PIServer();

  //Create top container
  topcont = new HCContainer("");

  //Create server device
  srvdev = new UDPDevice(1500);

  //Create server
  srv = new HCServer(srvdev, topcont, "Pi", __DATE__ " " __TIME__);

  //Add parameters
//  pca9685->RegisterInterface("pca9685", topcont, srv);
//  frontdoor->RegisterInterface("frontdoor", topcont, srv);
//  leftwindow->RegisterInterface("leftwindow", topcont, srv);
//  rightwindow->RegisterInterface("rightwindow", topcont, srv);
//  cellardoor->RegisterInterface("cellardoor", topcont, srv);
  param = new HCFlt32<PIServer>("temperature", pisrv, &PIServer::GetTemperature, 0);
  topcont->Add(param);
  srv->Add(param);
  param = new HCUns8<PIServer>("cpuutilization", pisrv, &PIServer::GetCPUUtilization, 0);
  topcont->Add(param);
  srv->Add(param);
  param = new HCBoolTable<PIServer>("relayon", pisrv, &PIServer::GetRelayOn, &PIServer::SetRelayOn, 3);
  topcont->Add(param);
  srv->Add(param);
  param = new HCCallTable<PIServer>("pulserelayhigh", pisrv, &PIServer::PulseRelayHigh, 3);
  topcont->Add(param);
  srv->Add(param);

  //Start HC server
  srv->Start();

  //Create query server
  qsrvdev = new UDPDevice(1501);
  qsrv = new HCQServer(qsrvdev, topcont);

  //Just loop if in daemon mode otherwise run console
  if(args.daemon)
  {
    while(true)
      ThreadSleep(1000000);
  }
  else
  {
    //Create HC console
    hccons = new HCConsole(topcont);

    //Run console in context of this thread
    hccons->Run();

    //Cleanup
    delete hccons;
  }

  //Cleanup
  delete qsrv;
  delete qsrvdev;
  delete srv;
  delete srvdev;
  delete topcont;
  delete pisrv;
//  delete cellardoor;
//  delete rightwindow;
//  delete leftwindow;
//  delete frontdoor;
//  delete pca9685;
//  delete pca9685bus;
//  delete i2c;

  //Return success
  return 0;
}
