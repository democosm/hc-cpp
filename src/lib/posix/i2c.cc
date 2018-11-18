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
#include <cassert>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

I2C::I2C(const char *name)
{
  //Assert valid arguments
  assert(name != 0);

  //Create mutex
  _mutex = new Mutex();

  //Open device
  if((_fd = open(name, O_RDWR)) < 0)
    printf("Could not open I2C device `%s`\n", name);
}

I2C::~I2C()
{
  //Close device
  close(_fd);

  //Delete mutex
  delete _mutex;
}

int I2C::Get(uint8_t devaddr, uint8_t regaddr, uint8_t *data, uint32_t len)
{
  //Assert valid arguments
  assert((data != 0) && (len > 0) && (len < 1000));

  //Begin mutual exclusion
  _mutex->Wait();

  //Set device address
  if(ioctl(_fd, I2C_SLAVE, devaddr) < 0)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Read transactions start with a write transaction
  if(write(_fd, &regaddr, 1) != 1)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Perform read transaction
  if(read(_fd, data, len) != (ssize_t)len)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //End mutual exclusion
  _mutex->Give();

  return ERR_NONE;
}

int I2C::Set(uint8_t devaddr, uint8_t regaddr, uint8_t *data, uint32_t len)
{
  uint8_t buf[len+1];

  //Assert valid arguments
  assert((data != 0) && (len > 0) && (len < 1000));

  //Begin mutual exclusion
  _mutex->Wait();

  //Set device address
  if(ioctl(_fd, I2C_SLAVE, devaddr) < 0)
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //Write register address to buffer
  buf[0] = regaddr;

  //Write data to buffer
  memcpy(buf+1, data, len);

  //Perform write transaction
  if(write(_fd, buf, len+1) != (ssize_t)(len+1))
  {
    //End mutual exclusion
    _mutex->Give();

    return ERR_UNSPEC;
  }

  //End mutual exclusion
  _mutex->Give();

  return ERR_NONE;
}
