// Thread
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

#pragma once

#include "error.hh"
#include "mutex.hh"
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <cassert>

int ThreadSleep(uint32_t usecs);

template <class T>

class Thread
{
public:
  //Signature of method to spawn as thread
  typedef void (T::*ThreadMethod)(void);

public:
  Thread(T *object, ThreadMethod method)
  {
    //Assert valid arguments
    assert((object != 0) && (method != 0));

    //Initialize object and method pointers
    _object = object;
    _method = method;

    //Create the mutex
    _mutex = new Mutex();

    //Indicate not started
    _started = false;
  }

  ~Thread()
  {
    //Stop the thread if it isn't already
    if(_started != 0)
      pthread_kill(_threadid, 0);

    //Destroy the mutex
    delete _mutex;
  }

  int Start(void)
  {
    int result;

    //Begin mutual exclusion
    _mutex->Wait();

    //Check for already started
    if(_started)
    {
      //End mutual exclusion
      _mutex->Give();
      return ERR_INVALID;
    }

    //Start the thread
    if((result = pthread_create(&_threadid, NULL, (void *(*)(void *))&Thread<T>::Wrapper, this)) != 0)
    {
      //End mutual exclusion
      _mutex->Give();
      return ERR_UNSPEC;
    }

    //Indicate started
    _started = true;

    //End mutual exclusion
    _mutex->Give();

    return ERR_NONE;
  }

  static void *Wrapper(Thread<T> *thread)
  {
    //Assert valid arguments
    assert(thread != 0);

    //Call the thread method
    ((thread->_object)->*(thread->_method))();

    //Indicate stopped
    thread->_started = 0;

    return NULL;
  }

private:
  T *_object;
  ThreadMethod _method;
  Mutex *_mutex;
  bool _started;
  pthread_t _threadid;
};
