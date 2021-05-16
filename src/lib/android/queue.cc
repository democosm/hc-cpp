// Queue
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

#include "queue.hh"
#include "error.hh"
#include <cassert>

QueueBuffer::QueueBuffer(uint32_t size)
{
  //Assert valid arguments
  assert(size > 0);

  //Initialize member variables
  _buffer = new uint8_t[size];
  _size = size;
  _head = 0;
}

QueueBuffer::~QueueBuffer()
{
  //Deallocate memory for the queue buffer
  delete[] _buffer;
}

void QueueBuffer::Reset(void)
{
  //Reset the head
  _head = 0;
}

uint32_t QueueBuffer::Read(void* buf, uint32_t maxlen)
{
  uint32_t i;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Check for overflow
  if((maxlen < _head) || (_head == 0))
    return 0;

  //Copy remaining data
  for(i=0; i<_head; i++)
    *((uint8_t*)buf + i) = *(_buffer + i);

  //Reset head
  _head = 0;

  return i;
}

uint32_t QueueBuffer::Write(const void* buf, uint32_t len)
{
  uint32_t i;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Check for overflow
  if((_head != 0) || (len > _size))
    return 0;

  //Copy data
  for(i=0; i<len; i++)
    *(_buffer + i) = *((uint8_t*)buf + i);

  //Adjust head
  _head = len;

  return len;
}

Queue::Queue(uint32_t bufcount, uint32_t bufsiz)
{
  uint32_t i;

  //Assert valid arguments
  assert((bufcount > 0) && (bufsiz > 0));

  //Initialize member variables
  _bufarray = new QueueBuffer*[bufcount];
  _bufcount = bufcount;
  _bufsiz = bufsiz;

  for(i=0; i<_bufcount; i++)
    _bufarray[i] = new QueueBuffer(_bufsiz);

  _wrbufind = 0;
  _rdbufind = 0;
  _writebuf = _bufarray[_wrbufind];
  _readbuf = _bufarray[_rdbufind];
  _mutex = new Mutex();
  _emptysem = new Semaphore(bufcount);
  _fullsem = new Semaphore(0);
}

Queue::~Queue()
{
  uint32_t i;

  //Destroy the mutex and synchronizing semaphores
  delete _mutex;
  delete _emptysem;
  delete _fullsem;

  //Destroy the buffers
  for(i=0; i<_bufcount; i++)
    delete _bufarray[i];

  //Deallocate memory for the buffer array
  delete[] _bufarray;
}

void Queue::Reset(void)
{
  uint32_t i;

  //Reset the buffers
  for(i=0; i<_bufcount; i++)
    _bufarray[i]->Reset();

  //Reset queue fields
  _wrbufind = 0;
  _rdbufind = 0;
  _writebuf = _bufarray[_wrbufind];
  _readbuf = _bufarray[_rdbufind];

  //Reset the mutex and synchronizing semaphores
  _mutex->Reset();
  _emptysem->Reset();
  _fullsem->Reset();
}

uint32_t Queue::Read(void* buf, uint32_t maxlen, uint32_t msecs)
{
  uint32_t retval;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Wait for a full buffer
  if(_fullsem->Wait(msecs) != ERR_NONE)
    return 0;

  //Begin mutual exclusion
  _mutex->Wait();

  //Read from the current read buffer
  retval = _readbuf->Read(buf, maxlen);

  //Advance the read buffer
  if(++_rdbufind >= _bufcount)
    _rdbufind = 0;

  _readbuf = _bufarray[_rdbufind];

  //End mutual exclusion
  _mutex->Give();

  //Give the empty semaphore
  _emptysem->Give();

  return retval;
}

uint32_t Queue::Write(const void* buf, uint32_t len, uint32_t msecs)
{
  uint32_t retval;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Check for overflow
  if(len > _bufsiz)
    return 0;

  //Wait for an empty buffer
  if(_emptysem->Wait(msecs) != ERR_NONE)
    return 0;

  //Begin mutual exclusion
  _mutex->Wait();

  //Write to the current write buffer
  retval = _writebuf->Write(buf, len);

  //Advance the write buffer
  if(++_wrbufind >= _bufcount)
    _wrbufind = 0;

  _writebuf = _bufarray[_wrbufind];

  //End mutual exclusion
  _mutex->Give();

  //Give the full semaphore
  _fullsem->Give();

  return retval;
}
