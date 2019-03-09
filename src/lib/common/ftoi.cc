// Floating point to integer conversion functions
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

#include "ftoi.hh"
#include <limits>
#include <math.h>

template <typename T> bool FToI(const double fval, T &ival)
{
  int64_t temp;

  //Round and cast to integer
  temp = (int64_t)round(fval);

  //Check for value outside of range of desired integer type
  if(temp > std::numeric_limits<T>::max())
  {
    ival = std::numeric_limits<T>::max();
    return false;
  }
  else if(temp < std::numeric_limits<T>::min())
  {
    ival = std::numeric_limits<T>::min();
    return false;
  }

  //Return casted value
  ival = (T)temp;
  return true;
}

template bool FToI<int8_t>(const double fval, int8_t &ival);
template bool FToI<int16_t>(const double fval, int16_t &ival);
template bool FToI<int32_t>(const double fval, int32_t &ival);
template bool FToI<int64_t>(const double fval, int64_t &ival);

template <typename T> bool FToU(const double fval, T &ival)
{
  uint64_t temp;

  //Round and cast to integer
  temp = (uint64_t)round(fval);

  //Check for value outside of range of desired integer type
  if(temp > std::numeric_limits<T>::max())
  {
    ival = std::numeric_limits<T>::max();
    return false;
  }
  else if(temp < std::numeric_limits<T>::min())
  {
    ival = std::numeric_limits<T>::min();
    return false;
  }

  //Return casted value
  ival = (T)temp;
  return true;
}

template bool FToU<uint8_t>(const double fval, uint8_t &ival);
template bool FToU<uint16_t>(const double fval, uint16_t &ival);
template bool FToU<uint32_t>(const double fval, uint32_t &ival);
template bool FToU<uint64_t>(const double fval, uint64_t &ival);
