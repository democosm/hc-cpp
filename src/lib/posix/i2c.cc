// I2C driver
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
#include "i2c.hh"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

I2C::I2C(const char *name)
{
  //Open I2C device and check for error
  if((_fd = open(name, O_RDWR)) < 0)
    printf("Could not open I2C device `%s`\n", name);

  //Create mutex
  _mutex = new Mutex();
}

I2C::~I2C()
{
  //Delete mutex
  delete _mutex;

  //Close I2C device
  close(_fd);
}

int I2C::Get(uint8_t devaddr, uint8_t regaddr, uint8_t &val)
{
  //Begin mutual exclusion
  _mutex->Wait();

  //Set device address and check for error
  if(ioctl(_fd, I2C_SLAVE, devaddr) < 0)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Write device address and register address and check for error
  if(write(_fd, &regaddr, 1) != 1)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Read register value and check for error
  if(read(_fd, &val, 1) != 1)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //End mutual exclusion
  _mutex->Give();

  return ERR_NONE;
}

int I2C::Set(uint8_t devaddr, uint8_t regaddr, uint8_t val)
{
  uint8_t buf[2];

  //Begin mutual exclusion
  _mutex->Wait();

  //Set device address and check for error
  if(ioctl(_fd, I2C_SLAVE, devaddr) < 0)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Write device address, register address and register value and check for error
  buf[0] = regaddr;
  buf[1] = val;
  if(write(_fd, buf, 2) != 2)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //End mutual exclusion
  _mutex->Give();

  return ERR_NONE;
}
