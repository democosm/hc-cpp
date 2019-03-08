// PCA9685 PWM driver
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

#include "error.hh"
#include "ftoi.hh"
#include "hcfloat.hh"
#include "hcinteger.hh"
#include "hcparameter.hh"
#include "pca9685.hh"
#include <cassert>
#include <sstream>

using namespace std;

PCA9685::PCA9685(Bus *bus, uint32_t pwmfreq)
{
  uint32_t i;
  uint8_t prescale;
  uint8_t tval;

  //Assert valid arguments
  assert((bus != 0) && (pwmfreq >= 24) && (pwmfreq <=1526));

  //Initialize registers and bits
  _mode1._restart = new Bits8(bus, 0, 0x80);
  _mode1._extclk = new Bits8(bus, 0, 0x40);
  _mode1._ai = new Bits8(bus, 0, 0x20);
  _mode1._sleep = new Bits8(bus, 0, 0x10);
  _mode1._sub1 = new Bits8(bus, 0, 0x08);
  _mode1._sub2 = new Bits8(bus, 0, 0x04);
  _mode1._sub3 = new Bits8(bus, 0, 0x02);
  _mode1._allcall = new Bits8(bus, 0, 0x01);
  _mode2._invrt = new Bits8(bus, 1, 0x10);
  _mode2._och = new Bits8(bus, 1, 0x08);
  _mode2._outdrv = new Bits8(bus, 1, 0x04);
  _mode2._outne = new Bits8(bus, 1, 0x03);
  _subadr1 = new Reg8(bus, 2);
  _subadr2 = new Reg8(bus, 3);
  _subadr3 = new Reg8(bus, 4);
  _allcalladr = new Reg8(bus, 5);

  for(i=0; i<16; i++)
  {
    _led[i]._oncnt = new LReg16(bus, i*4+6);
    _led[i]._offcnt = new LReg16(bus, i*4+8);
  }

  _ledall._oncnt = new LReg16(bus, 0xFA);
  _ledall._offcnt = new LReg16(bus, 0xFC);
  _prescale = new Reg8(bus, 0xFE);

  //Force into sleep mode without restarting (prescale can only be set in sleep mode)
  tval = 0x10;
  bus->Set(0, &tval, 1);

  //Set prescale value to give us desired PWM frequency
  FToU(25000000.0/4096.0/(double)pwmfreq, prescale);
  prescale -= 1;
  _prescale->Set(prescale);

  //Pull out of sleep mode, wait 50ms and then restart
  tval = 0x00;
  bus->Set(0, &tval, 1);
  ThreadSleep(50000);
  tval = 0x80;
  bus->Set(0, &tval, 1);

  //Ensure address auto increment is set to allow multi-byte register read/write
  _mode1._ai->Set(1);

  //Drive outputs
  _mode2._outdrv->Set(1);
}

PCA9685::~PCA9685()
{
  uint32_t i;

  //Cleanup
  delete _mode1._restart;
  delete _mode1._extclk;
  delete _mode1._ai;
  delete _mode1._sleep;
  delete _mode1._sub1;
  delete _mode1._sub2;
  delete _mode1._sub3;
  delete _mode1._allcall;
  delete _mode2._invrt;
  delete _mode2._och;
  delete _mode2._outdrv;
  delete _mode2._outne;
  delete _subadr1;
  delete _subadr2;
  delete _subadr3;
  delete _allcalladr;

  for(i=0; i<16; i++)
  {
    delete _led[i]._oncnt;
    delete _led[i]._offcnt;
  }

  delete _ledall._oncnt;
  delete _ledall._offcnt;
  delete _prescale;
}

void PCA9685::RegisterInterface(const char *contname, HCContainer *pcont, HCServer *srv)
{
  HCContainer *cont;
  HCContainer *regcont;
  HCContainer *namecont;
  HCParameter *param;
  uint32_t i;
  ostringstream tempname;

  cont = new HCContainer(contname);
  pcont->Add(cont);

  param = new HCFlt64Table<PCA9685>("pwmdutycycle", this, &PCA9685::GetPWMDutyCycle, &PCA9685::SetPWMDutyCycle, 16, 0, 100.0);
  cont->Add(param);
  srv->Add(param);

  regcont = new HCContainer("reg");
  cont->Add(regcont);

  namecont = new HCContainer("mode1");
  regcont->Add(namecont);
  param = new HCUns8<Bits8>("restart", _mode1._restart, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("extclk", _mode1._extclk, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("ai", _mode1._ai, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("sleep", _mode1._sleep, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("sub1", _mode1._sub1, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("sub2", _mode1._sub2, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("sub3", _mode1._sub3, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("allcall", _mode1._allcall, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);

  namecont = new HCContainer("mode2");
  regcont->Add(namecont);
  param = new HCUns8<Bits8>("invrt", _mode2._invrt, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("och", _mode2._och, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("outdrv", _mode2._outdrv, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns8<Bits8>("outne", _mode2._outne, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);

  param = new HCUns8<Reg8>("subadr1", _subadr1, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUns8<Reg8>("subadr2", _subadr2, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUns8<Reg8>("subadr3", _subadr3, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUns8<Reg8>("allcalladr", _allcalladr, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);

  for(i=0; i<16; i++)
  {
    tempname.str("");
    tempname << "led" << i;
    namecont = new HCContainer(tempname.str());
    regcont->Add(namecont);
    param = new HCUns16<LReg16>("oncnt", _led[i]._oncnt, &LReg16::Get, &LReg16::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUns16<LReg16>("offcnt", _led[i]._offcnt, &LReg16::Get, &LReg16::Set);
    namecont->Add(param);
    srv->Add(param);
  }

  namecont = new HCContainer("ledall");
  regcont->Add(namecont);
  param = new HCUns16<LReg16>("oncnt", _ledall._oncnt, 0, &LReg16::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUns16<LReg16>("offcnt", _ledall._offcnt, 0, &LReg16::Set);
  namecont->Add(param);
  srv->Add(param);

  param = new HCUns8<Reg8>("prescale", _prescale, &Reg8::Get, &Reg8::Set);
  regcont->Add(param);
  srv->Add(param);
}

int PCA9685::GetPWMDutyCycle(uint32_t id, double &val)
{
  int lerr;
  uint16_t offcnt;

  //Check for invalid ID
  if(id >= 16)
  {
    val = 0.0;
    return ERR_EID;
  }

  //Get off count value
  if((lerr = _led[id]._offcnt->Get(offcnt)) != ERR_NONE)
  {
    val = 0.0;
    return lerr;
  }

  //Check for off full set
  if((offcnt & 0x1000) != 0)
  {
    val = 0.0;
    return ERR_NONE;
  }

  //Mask off unused bits (including off full)
  offcnt &= ~0xF000;

  //Calculate PWM duty cycle from off count
  val = (double)offcnt/4095.0;
  return ERR_NONE;
}

int PCA9685::SetPWMDutyCycle(uint32_t id, double val)
{
  uint16_t offcnt;

  //Check for invalid ID
  if(id >= 16)
  {
    val = 0.0;
    return ERR_EID;
  }

  //Check for range error
  if((val < 0.0) || (val > 1.0))
    return ERR_RANGE;

  //Check for 0 percent duty cycle desired
  if(val == 0.0)
    return _led[id]._offcnt->Set(0x1000);

  //Calculate off count value
  if(!FToU(4095.0*val, offcnt))
    return ERR_RANGE;

  //Set off count value
  return _led[id]._offcnt->Set(offcnt);
}
