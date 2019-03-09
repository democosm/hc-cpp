// PCA9685 servo driver
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

#include "const.hh"
#include "error.hh"
#include "hccall.hh"
#include "hcfloat.hh"
#include "hcparameter.hh"
#include "pca9685servo.hh"
#include <cassert>

PCA9685Servo::PCA9685Servo(PCA9685 *pca9685, uint32_t chanid, double dutymin, double dutymax)
{
  //Assert valid arguments
  assert((pca9685 != 0) && (chanid < 16) && (dutymin < dutymax) && (dutymin >= 0.0) & (dutymax <= 1.0));

  //Initialize cache variables
  _pca9685 = pca9685;
  _chanid = chanid;
  _dutymin = dutymin;
  _dutymax = dutymax;
}

PCA9685Servo::~PCA9685Servo()
{
}

void PCA9685Servo::RegisterInterface(const char *contname, HCContainer *pcont, HCServer *srv)
{
  HCContainer *cont;
  HCParameter *param;

  cont = new HCContainer(contname);
  pcont->Add(cont);

  param = new HCFlt64<PCA9685Servo>("angle", this, &PCA9685Servo::GetAngle, &PCA9685Servo::SetAngle, 180.0/PI);
  cont->Add(param);
  srv->Add(param);
  param = new HCCall<PCA9685Servo>("slewtest", this, &PCA9685Servo::SlewTest);
  cont->Add(param);
  srv->Add(param);
}

int PCA9685Servo::GetAngle(double &val)
{
  int err;
  double duty;

  //Get PWM duty cycle for servo channel
  if((err = _pca9685->GetPWMDutyCycle(_chanid, duty)) != ERR_NONE)
    return err;

  //Convert duty cycle to angle
  if(duty <= _dutymin)
    val = -HALFPI;
  else if(duty >= _dutymax)
    val = HALFPI;
  else
    val = (duty - _dutymin)/(_dutymax - _dutymin)*PI - HALFPI;

  return ERR_NONE;
}

int PCA9685Servo::SetAngle(double val)
{
  double duty;

  //Check for range error
  if((val < -HALFPI) || (val > HALFPI))
    return ERR_RANGE;

  //Convert angle to duty cycle
  duty = (val + HALFPI)/PI*(_dutymax - _dutymin) + _dutymin;

  //Set duty cycle for servo channel
  return _pca9685->SetPWMDutyCycle(_chanid, duty);
}

int PCA9685Servo::SlewTest(void)
{
  uint32_t i;

  for(i=0; i<20; i++)
  {
    SetAngle(PI*(double)i/19.0 - HALFPI);
    ThreadSleep(50000);
  }

  for(i=0; i<20; i++)
  {
    SetAngle(HALFPI - PI*(double)i/19.0);
    ThreadSleep(50000);
  }

  SetAngle(0.0);

  return ERR_NONE;
}
