// Bus interface
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

#include "bus.hh"
#include "error.hh"
#include "order.hh"

Bus::Bus()
{
  //Create mutex
  _mutex = new Mutex();
}

Bus::~Bus()
{
  //Delete mutex
  delete _mutex;
}

int Bus::Get(uint32_t addr, uint8_t* data, uint32_t len)
{
  return ERR_NOIMP;
}

int Bus::Set(uint32_t addr, uint8_t* data, uint32_t len)
{
  return ERR_NOIMP;
}

template <typename T> int Bus::Get(uint32_t addr, T& val)
{
  int err;
  T buf;

  //Get data using bus
  _mutex->Wait();
  err = Get(addr, (uint8_t*)&buf, sizeof(T));
  _mutex->Give();

  //Check for error
  if(err != ERR_NONE)
  {
    val = 0;
    return err;
  }

  //Convert to host byte order
  val = NetToHost(buf);

  return ERR_NONE;
}

template int Bus::Get<uint8_t>(uint32_t addr, uint8_t& val);
template int Bus::Get<uint16_t>(uint32_t addr, uint16_t& val);
template int Bus::Get<uint32_t>(uint32_t addr, uint32_t& val);
template int Bus::Get<uint64_t>(uint32_t addr, uint64_t& val);

template <typename T> int Bus::Set(uint32_t addr, const T val)
{
  int err;
  T buf;

  //Convert to network byte order
  buf = HostToNet(val);

  //Set data using bus
  _mutex->Wait();
  err = Set(addr, (uint8_t*)&buf, sizeof(T));
  _mutex->Give();

  return err;
}

template int Bus::Set<uint8_t>(uint32_t addr, const uint8_t val);
template int Bus::Set<uint16_t>(uint32_t addr, const uint16_t val);
template int Bus::Set<uint32_t>(uint32_t addr, const uint32_t val);
template int Bus::Set<uint64_t>(uint32_t addr, const uint64_t val);

void Bus::Reserve(void)
{
  //Begin mutual exclusion
  _mutex->Wait();
}

void Bus::Release(void)
{
  //End mutual exclusion
  _mutex->Give();
}
