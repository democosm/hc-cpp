// Register with wrong byte ordering
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

#ifndef _RETSIGER_HH_
#define _RETSIGER_HH_

#include "bus.hh"
#include "order.hh"
#include <cassert>
#include <inttypes.h>

template <class T>
class Retsiger
{
public:
  Retsiger(Bus *bus, uint32_t addr)
  {
    //Assert valid arguments
    assert(bus != 0);

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
  }

  virtual ~Retsiger()
  {
  }

  int Get(T &val)
  {
    int terr;
    T buf;

    //Get data using bus
    _bus->Reserve();
    terr = _bus->Get(_addr, (uint8_t *)&buf, sizeof(T));
    _bus->Release();

    //Check for error
    if(terr != ERR_NONE)
    {
      val = 0;
      return terr;
    }

    //Read data from buffer
    val = WrongToHost(buf);

    return ERR_NONE;
  }

  int Set(T val)
  {
    int terr;
    T buf;

    //Write data to buffer
    buf = HostToWrong(val);

    //Set data using bus
    _bus->Reserve();
    terr = _bus->Set(_addr, (uint8_t *)&buf, sizeof(T));
    _bus->Release();

    return terr;
  }

private:
  Bus *_bus;
  uint32_t _addr;
};

//Types derived from template
typedef Retsiger<uint16_t> Retsiger16;
typedef Retsiger<uint32_t> Retsiger32;
typedef Retsiger<uint64_t> Retsiger64;

#endif
