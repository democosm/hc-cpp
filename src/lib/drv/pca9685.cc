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
#include "hcparameter.hh"
#include "pca9685.hh"
#include <cassert>
#include <sstream>

using namespace std;

PCA9685::PCA9685(Bus *bus)
{
  uint32_t i;

  //Assert valid arguments
  assert(bus != 0);

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
    _led[i]._onl = new Reg8(bus, i*4+6);
    _led[i]._onfull = new Bits8(bus, i*4+7, 0x10);
    _led[i]._onh = new Bits8(bus, i*4+7, 0x0F);
    _led[i]._offl = new Reg8(bus, i*4+8);
    _led[i]._offfull = new Bits8(bus, i*4+9, 0x10);
    _led[i]._offh = new Bits8(bus, i*4+9, 0x0F);
  }

  _ledall._onl = new Reg8(bus, 0xFA);
  _ledall._onfull = new Bits8(bus, 0xFB, 0x10);
  _ledall._onh = new Bits8(bus, 0xFB, 0x0F);
  _ledall._offl = new Reg8(bus, 0xFC);
  _ledall._offfull = new Bits8(bus, 0xFD, 0x10);
  _ledall._offh = new Bits8(bus, 0xFD, 0x0F);
  _prescale = new Reg8(bus, 0xFE);
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
    delete _led[i]._onl;
    delete _led[i]._onfull;
    delete _led[i]._onh;
    delete _led[i]._offl;
    delete _led[i]._offfull;
    delete _led[i]._offh;
  }

  delete _ledall._onl;
  delete _ledall._onfull;
  delete _ledall._onh;
  delete _ledall._offl;
  delete _ledall._offfull;
  delete _ledall._offh;
  delete _prescale;
}

void PCA9685::RegisterInterface(HCContainer *cont, HCServer *srv)
{
  HCContainer *drvcont;
  HCContainer *regcont;
  HCContainer *namecont;
  HCParameter *param;
  uint32_t i;
  ostringstream tempname;

  drvcont = new HCContainer("pca9685");
  cont->Add(drvcont);

  regcont = new HCContainer("reg");
  drvcont->Add(regcont);

  namecont = new HCContainer("mode1");
  regcont->Add(namecont);
  param = new HCUnsigned8<Bits8>("restart", _mode1._restart, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("extclk", _mode1._extclk, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("ai", _mode1._ai, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("sleep", _mode1._sleep, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("sub1", _mode1._sub1, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("sub2", _mode1._sub2, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("sub3", _mode1._sub3, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("allcall", _mode1._allcall, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);

  namecont = new HCContainer("mode2");
  regcont->Add(namecont);
  param = new HCUnsigned8<Bits8>("invrt", _mode2._invrt, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("och", _mode2._och, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("outdrv", _mode2._outdrv, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("outne", _mode2._outne, &Bits8::Get, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);

  param = new HCUnsigned8<Reg8>("subadr1", _subadr1, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Reg8>("subadr2", _subadr2, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Reg8>("subadr3", _subadr3, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Reg8>("allcalladr", _allcalladr, &Reg8::Get, 0);
  regcont->Add(param);
  srv->Add(param);

  for(i=0; i<16; i++)
  {
    tempname.str("");
    tempname << "led" << i;
    namecont = new HCContainer(tempname.str());
    regcont->Add(namecont);
    param = new HCUnsigned8<Reg8>("onl", _led[i]._onl, &Reg8::Get, &Reg8::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUnsigned8<Bits8>("onfull", _led[i]._onfull, &Bits8::Get, &Bits8::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUnsigned8<Bits8>("onh", _led[i]._onh, &Bits8::Get, &Bits8::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUnsigned8<Reg8>("offl", _led[i]._offl, &Reg8::Get, &Reg8::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUnsigned8<Bits8>("offfull", _led[i]._offfull, &Bits8::Get, &Bits8::Set);
    namecont->Add(param);
    srv->Add(param);
    param = new HCUnsigned8<Bits8>("offh", _led[i]._offh, &Bits8::Get, &Bits8::Set);
    namecont->Add(param);
    srv->Add(param);
  }

  namecont = new HCContainer("ledall");
  regcont->Add(namecont);
  param = new HCUnsigned8<Reg8>("onl", _ledall._onl, 0, &Reg8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("onfull", _ledall._onfull, 0, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("onh", _ledall._onh, 0, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Reg8>("offl", _ledall._offl, 0, &Reg8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("offfull", _ledall._offfull, 0, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);
  param = new HCUnsigned8<Bits8>("offh", _ledall._offh, 0, &Bits8::Set);
  namecont->Add(param);
  srv->Add(param);

  param = new HCUnsigned8<Reg8>("prescale", _prescale, &Reg8::Get, &Reg8::Set);
  regcont->Add(param);
  srv->Add(param);
}
