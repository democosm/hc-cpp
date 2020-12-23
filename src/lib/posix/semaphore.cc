// Semaphore
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

#include "semaphore.hh"
#include "error.hh"
#include <errno.h>
#include <time.h>
#include <cassert>

#define SEMAPHORE_CNT_MAX 1000000

Semaphore::Semaphore(uint32_t initcount)
{
  //Assert valid arguments
  assert(initcount <= SEMAPHORE_CNT_MAX);

  //Initialize member variables
  _initcount = initcount;
  _count = initcount;

  //Initialize the mutex
  pthread_mutex_init(&_mutex, NULL);

  //Initialize the cond lock
  pthread_cond_init(&_cond, NULL);
}

Semaphore::~Semaphore()
{
  //Destroy the mutex
  pthread_mutex_destroy(&_mutex);

  //Destroy the cond lock
  pthread_cond_destroy(&_cond);
}

int Semaphore::Clear(void)
{
  //Lock the mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Clear the count
  _count = 0;

  //Unlock the mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Semaphore::Wait(uint32_t usecs)
{
  struct timespec timeout;
  int result;

  //Lock the mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Check for timeout specified as none, finite or infinite
  if(usecs == WAIT_NONE)
  {
    //Check for count of 0
    if(_count == 0)
    {
      //Unlock the mutex
      pthread_mutex_unlock(&_mutex);

      //Return error
      return ERR_UNSPEC;
    }

    //Decrement count
    _count--;
  }
  else if(usecs == WAIT_INF)
  {
    //Check for count of 0
    if(_count == 0)
    {
      //Wait for cond lock and check for error
      if(pthread_cond_wait(&_cond, &_mutex) != 0)
      {
        //Unlock the mutex
        pthread_mutex_unlock(&_mutex);

        //Return error
        return ERR_UNSPEC;
      }
    }

    //The count should never be 0 here
    assert(_count != 0);

    //Decrement count
    _count--;
  }
  else
  {
    //Check count
    if(_count == 0)
    {
      //Calculate absolute time for cond wait timeout
      clock_gettime(CLOCK_MONOTONIC, &timeout);
      timeout.tv_sec += usecs/1000000;
      timeout.tv_nsec += (usecs%1000000)*1000;
      if(timeout.tv_nsec >= 1000000000)
      {
        timeout.tv_sec += timeout.tv_nsec/1000000000;
        timeout.tv_nsec = timeout.tv_nsec%1000000000;
      }

      //Wait for cond lock with timeout
      result = pthread_cond_timedwait(&_cond, &_mutex, &timeout);

      //Check for timeout or other error
      if(result == ETIMEDOUT)
      {
        //Unlock the mutex
        pthread_mutex_unlock(&_mutex);

        //Return timeout
        return ERR_TIMEOUT;
      }
      else if(result != 0)
      {
        //Unlock the mutex
        pthread_mutex_unlock(&_mutex);

        //Return timeout
        return ERR_UNSPEC;
      }
    }

    //The count should never be 0 here
    assert(_count != 0);

    //Decrement count
    _count--;
  }

  //Unlock the mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Semaphore::Give(void)
{
  //Lock the mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Increment count
  if(++_count > SEMAPHORE_CNT_MAX)
    _count = SEMAPHORE_CNT_MAX;

  //Signal cond lock and check for error
  if(pthread_cond_signal(&_cond) != 0)
  {
    //Unlock the mutex
    pthread_mutex_unlock(&_mutex);

    //Return timeout
    return ERR_UNSPEC;
  }

  //Unlock the mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}

int Semaphore::Reset(void)
{
  //Lock the mutex and check for error
  if(pthread_mutex_lock(&_mutex) != 0)
    return ERR_UNSPEC;

  //Set count to initial value
  _count = _initcount;

  //Unlock the mutex and check for error
  if(pthread_mutex_unlock(&_mutex) != 0)
    return ERR_UNSPEC;

  return ERR_NONE;
}
