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

#ifndef _QUEUE_HH_
#define _QUEUE_HH_

#include "mutex.hh"
#include "semaphore.hh"
#include <inttypes.h>

class QueueBuffer
{
public:
  QueueBuffer(uint32_t size);
  ~QueueBuffer();
  void Reset(void);
  uint32_t Read(void *buf, uint32_t maxlen);
  uint32_t Write(const void *buf, uint32_t len);

private:
  uint32_t _head;
  uint32_t _size;
  uint8_t *_buffer;
};

class Queue
{
public:
  Queue(uint32_t bufcnt, uint32_t bufsiz);
  ~Queue();
  void Reset(void);
  uint32_t Read(void *buf, uint32_t maxlen, uint32_t usecs);
  uint32_t Write(const void *buf, uint32_t len, uint32_t usecs);

private:
  Mutex *_mutex;
  Semaphore *_emptysem;
  Semaphore *_fullsem;
  uint32_t _wrbufind;
  uint32_t _rdbufind;
  uint32_t _bufcnt;
  uint32_t _bufsiz;
  QueueBuffer **_bufarray;
  QueueBuffer *_writebuf;
  QueueBuffer *_readbuf;
};

#endif //_QUEUE_HH_
