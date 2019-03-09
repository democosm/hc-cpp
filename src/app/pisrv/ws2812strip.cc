// WS2812 LED strip driver
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

#include "error.hh"
#include "thread.hh"
#include "ws2812strip.hh"
#include <cassert>
#include <string.h>

WS2812Strip::WS2812Strip(SPI *spi, uint32_t count)
{
  //Assert valid arguments
  assert((spi != 0) && (count <= 300));

  //Cache arguments
  _spi = spi;
  _count = count;

  //Create color buffer
  _colorbuf = new uint8_t[_count*3];
  memset(_colorbuf, 0, _count*3);
}

WS2812Strip::~WS2812Strip()
{
  //Delete color buffer
  delete[] _colorbuf;
}

int WS2812Strip::GetColor(uint32_t id, uint32_t &val)
{
  uint32_t index;

  //Check for id out of range
  if(id >= _count)
    return ERR_RANGE;

  //Calculate index into color array
  index = id*3;

  //Set value for return
  val = ((uint32_t)_colorbuf[index] << 16) | ((uint32_t)_colorbuf[index+1] << 8) | (uint32_t)_colorbuf[index+2];

  return ERR_NONE;
}

int WS2812Strip::SetColor(uint32_t id, uint32_t val)
{
  uint32_t index;

  //Check for id out of range
  if(id >= _count)
    return ERR_RANGE;

  //Calculate index into color array
  index = id*3;

  //Set value for return
  _colorbuf[index] = (uint8_t)val >> 16;
  _colorbuf[index] = (uint8_t)val >> 8;
  _colorbuf[index] = (uint8_t)val;

  return ERR_NONE;
}

int WS2812Strip::Update(void)
{
  //Must wait at least 500us before performing new update so chips know this is a new sequence
  ThreadSleep(500);

  //Write color buffer to strip
  _spi->Transfer(_colorbuf, 0, _count*3);

  return ERR_NONE;
}
