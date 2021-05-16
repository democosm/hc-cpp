// Simulated bus
//
// Copyright 2021 Democosm
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
#include "simbus.hh"
#include <cassert>
#include <string.h>

SimBus::SimBus(uint32_t size)
: Bus()
{
  //Cache arguments
  _size = size;

  //Allocate RAM for dummy registers
  _buf = new uint8_t[_size];

  //Zero buffer contents
  memset(_buf, 0, _size);
}

SimBus::~SimBus()
{
  //Deallocate memory for dummy registers
  delete[] _buf;
}

int SimBus::Get(uint32_t addr, uint8_t* data, uint32_t len)
{
  //Assert valid arguments
  assert((data != 0) && (len > 0));

  //Check for reading outside of buffer bounds
  if((addr + len) > _size)
    return ERR_OVERFLOW;

  //Copy data from buffer
  memcpy(data, _buf + addr, len);
  return ERR_NONE;
}

int SimBus::Set(uint32_t addr, uint8_t* data, uint32_t len)
{
  //Assert valid arguments
  assert((data != 0) && (len > 0));

  //Check for writing outside of buffer bounds
  if((addr + len) > _size)
    return ERR_OVERFLOW;

  //Copy data to buffer
  memcpy(_buf + addr, data, len);
  return ERR_NONE;
}
