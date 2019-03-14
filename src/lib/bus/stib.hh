// Bits with wrong byte ordering
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

#ifndef _STIB_HH_
#define _STIB_HH_

#include "bus.hh"
#include "order.hh"
#include <cassert>
#include <inttypes.h>

template <class T>
class Stib
{
public:
  Stib(Bus *bus, uint32_t addr, T mask)
  {
    //Assert valid arguments
    assert((bus != 0) && (mask != 0));

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
    _mask = mask;

    //Determine shift ammount from mask
    for(_shift=0; _shift<sizeof(T)*8; _shift++)
      if(((_mask >> _shift) & (T)1) != 0)
        break;
  }

  virtual ~Stib()
  {
  }

  int Get(T &val)
  {
    int terr;
    uint8_t buf[sizeof(T)];

    //Get data using bus
    _bus->Reserve();
    terr = _bus->Get(_addr, buf, sizeof(T));
    _bus->Release();

    //Check for error
    if(terr != ERR_NONE)
    {
      val = 0;
      return terr;
    }

    //Read data from buffer
    val = WrongToHost(*((T *)buf));

    //Mask and right justify
    val = (val & _mask) >> _shift;

    return ERR_NONE;
  }

  int Set(T val)
  {
    int terr;
    uint8_t buf[sizeof(T)];
    T tval;

    //Reserve bus
    _bus->Reserve();

    //Get data using bus
    terr = _bus->Get(_addr, buf, sizeof(T));

    //Check for error
    if(terr != ERR_NONE)
    {
      //Release bus
      _bus->Release();

      return terr;
    }

    //Read data from buffer
    tval = WrongToHost(*((T *)buf));

    //Shift and mask in new value
    val = (tval & ~_mask) | (val << _shift);

    //Write data to buffer
    *((T *)buf) = HostToWrong(val);

    //Set data using bus
    terr = _bus->Set(_addr, buf, sizeof(T));

    //Release bus
    _bus->Release();

    return terr;
  }

private:
  Bus *_bus;
  uint32_t _addr;
  T _mask;
  uint8_t _shift;
};

//Types derived from template
typedef Stib<uint16_t> Stib16;
typedef Stib<uint32_t> Stib32;
typedef Stib<uint64_t> Stib64;

#endif //_STIB_HH_
