// Pipe
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

#include "pipe.hh"
#include "error.hh"
#include <cassert>

Pipe::Pipe(uint32_t size)
{
  //Assert valid arguments
  assert(size > 0);

  //Initialize member variables
  _head = 0;
  _tail = 0;
  _full = 0;
  _size = size;
  _buffer = new uint8_t[size];
  _mutex = new Mutex();
  _wrevt = new Event();
  _rdevt = new Event();
}

Pipe::~Pipe()
{
  //Reset head, tail and full count (paranoia)
  _head = 0;
  _tail = 0;
  _full = 0;

  //Destroy the mutex and synchronizing events
  delete _mutex;
  delete _rdevt;
  delete _wrevt;

  //Deallocate memory for the pipe
  delete[] _buffer;
}

void Pipe::Reset(void)
{
  //Reset head, tail and full count
  _head = 0;
  _tail = 0;
  _full = 0;

  //Reset the mutex and synchronizing events
  _mutex->Reset();
  _rdevt->Reset();
  _wrevt->Reset();
}

uint32_t Pipe::Read(void *buf, uint32_t maxlen, uint32_t usecs)
{
  uint32_t i, amtread;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Begin mutual exclusion
  _mutex->Wait();

  //Wait for data to become available
  while(_full == 0)
  {
    //End mutual exclusion
    _mutex->Give();

    //Wait for write event
    if(_wrevt->Wait(usecs) != ERR_NONE)
      return 0;

    //Begin mutual exclusion
    _mutex->Wait();
  }

  //Limit the ammount of bytes to be read
  if(maxlen < _full)
    amtread = maxlen;
  else
    amtread = _full;

  //Copy the data
  for(i=0; i<amtread; i++)
  {
    *((uint8_t *)buf + i) = *(_buffer + _tail++);

    //Check for rollover
    if(_tail >= _size)
      _tail = 0;
  }

  //Update full count
  _full -= amtread;

  //End mutual exclusion
  _mutex->Give();

  //Signal the read event
  _rdevt->Signal();

  return amtread;
}

uint32_t Pipe::Write(void *buf, uint32_t len, uint32_t usecs)
{
  uint32_t i, empty;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Check for overflow
  if(len > _size)
    return 0;

  //Begin mutual exclusion
  _mutex->Wait();

  //Calculate empty space
  empty = _size - _full;

  //Wait for free space
  while(len > empty)
  {
    //End mutual exclusion
    _mutex->Give();

    //Wait for read event
    if(_rdevt->Wait(usecs) != ERR_NONE)
      return 0;

    //Begin mutual exclusion
    _mutex->Wait();

    //Recalculate empty space
    empty = _size - _full;
  }

  //Copy the data
  for(i=0; i<len; i++)
  {
    *(_buffer + _head++) = *((uint8_t *)buf + i);

    //Check for rollover
    if(_head >= _size)
      _head = 0;
  }

  //Update full count
  _full += len;

  //End mutual exclusion
  _mutex->Give();

  //Signal the write event
  _wrevt->Signal();

  return len;
}
