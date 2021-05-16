// Event
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

#include "event.hh"
#include "error.hh"
#include <errno.h>
#include <time.h>

Event::Event(bool signalled)
{
  //Initialize member variables
  _initcount = signalled ? 1 : 0;
  _count = _initcount;

  //Initialize mutex
  pthread_mutex_init(&_mutex, NULL);

  //Initialize cond lock attributes to use monotonic clock
  pthread_condattr_init(&_condattr);
  pthread_condattr_setclock(&_condattr, CLOCK_MONOTONIC);

  //Initialize cond lock
  pthread_cond_init(&_cond, &_condattr);
}

Event::~Event()
{
  //Destroy cond lock
  pthread_cond_destroy(&_cond);

  //Destroy cond lock attributes
  pthread_condattr_destroy(&_condattr);

  //Destroy mutex
  pthread_mutex_destroy(&_mutex);
}

int Event::Clear(void)
{
  //Lock mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Clear count
  _count = 0;

  //Unlock mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Event::Wait(uint32_t msecs)
{
  struct timespec timeout;
  int result;

  //Lock mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Check for timeout specified as none, finite or infinite
  if(msecs == WAIT_NONE)
  {
    //Check for count of 0
    if(_count == 0)
    {
      //Unlock mutex
      pthread_mutex_unlock(&_mutex);

      //Return error
      return ERR_UNSPEC;
    }

    //Set count to 0
    _count = 0;
  }
  else if(msecs == WAIT_INF)
  {
    //Check for count of 0
    if(_count == 0)
    {
      //Wait for cond lock and check for error
      if(pthread_cond_wait(&_cond, &_mutex) != 0)
      {
        //Unlock mutex
        pthread_mutex_unlock(&_mutex);

        //Return error
        return ERR_UNSPEC;
      }
    }

    //Set count to 0
    _count = 0;
  }
  else
  {
    //Check for count of zero
    if(_count == 0)
    {
      //Calculate absolute time for cond wait timeout
      clock_gettime(CLOCK_MONOTONIC, &timeout);
      timeout.tv_sec += msecs / 1000;
      timeout.tv_nsec += (msecs % 1000) * 1000000;
      if(timeout.tv_nsec >= 1000000000)
      {
        timeout.tv_sec += timeout.tv_nsec / 1000000000;
        timeout.tv_nsec = timeout.tv_nsec % 1000000000;
      }

      //Wait for cond lock with timeout
      result = pthread_cond_timedwait(&_cond, &_mutex, &timeout);

      //Check for timeout or other error
      if(result == ETIMEDOUT)
      {
        //Unlock mutex
        pthread_mutex_unlock(&_mutex);

        //Return timeout
        return ERR_TIMEOUT;
      }
      else if(result != 0)
      {
        //Unlock mutex
        pthread_mutex_unlock(&_mutex);

        //Return timeout
        return ERR_UNSPEC;
      }
    }

    //Set count to 0
    _count = 0;
  }

  //Unlock mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Event::Signal(void)
{
  //Lock mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Set count to 1
  _count = 1;

  //Signal cond lock and check for error
  if(pthread_cond_signal(&_cond) != 0)
  {
    //Unlock mutex
    pthread_mutex_unlock(&_mutex);

    //Return timeout
    return ERR_UNSPEC;
  }

  //Unlock mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Event::Reset(void)
{
  //Lock mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Set count to initial value
  _count = _initcount;

  //Unlock mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}
