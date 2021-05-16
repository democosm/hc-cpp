// PCA9685 PWM driver
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

#pragma once

#include "bits.hh"
#include "bus.hh"
#include "hccontainer.hh"
#include "hcserver.hh"
#include "register.hh"
#include "retsiger.hh"
#include <inttypes.h>

class PCA9685
{
public:
  PCA9685(Bus* bus, uint32_t pwmfreq);
  virtual ~PCA9685();
  void RegisterInterface(const char* contname, HCContainer* pcont, HCServer* srv);
  int GetPWMDutyCycle(uint32_t id, double& val);
  int SetPWMDutyCycle(uint32_t id, double val);

private:
  struct Mode1
  {
    Bits8* _restart;
    Bits8* _extclk;
    Bits8* _ai;
    Bits8* _sleep;
    Bits8* _sub1;
    Bits8* _sub2;
    Bits8* _sub3;
    Bits8* _allcall;
  };

  struct Mode2
  {
    Bits8* _invrt;
    Bits8* _och;
    Bits8* _outdrv;
    Bits8* _outne;
  };

  struct LED
  {
    Retsiger16* _oncount;
    Retsiger16* _offcount;
  };

  Mode1 _mode1;
  Mode2 _mode2;
  Register8* _subadr1;
  Register8* _subadr2;
  Register8* _subadr3;
  Register8* _allcalladr;
  LED _led[16];
  LED _ledall;
  Register8* _prescale;
};
