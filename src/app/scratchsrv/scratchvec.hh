// Vector scratch pad class
//
// Copyright 2020 Democosm
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

#ifndef _SCRATCHVEC_HH_
#define _SCRATCHVEC_HH_

#include "hcparameter.hh"
#include <inttypes.h>
#include <iostream>
#include <vector>

template <typename T>
class ScratchVec
{
public:
  ScratchVec(const uint32_t tablesize)
  {
    uint32_t eid;

    //Initialize member variables
    HCParameter::DefaultVal(_val0);
    HCParameter::DefaultVal(_val1);
    HCParameter::DefaultVal(_val2);

    for(eid=0; eid<tablesize; eid++)
    {
      _table0.push_back(_val0);
      _table1.push_back(_val1);
      _table2.push_back(_val2);
    }
  }

  ~ScratchVec()
  {
  }

  int Print(void)
  {
    //Print value
    std::cout << "Value = \"" << _val0 << ", " << _val1 << ", " << _val2 << "\"" << "\n";
    return ERR_NONE;
  }

  int TablePrint(uint32_t eid)
  {
    //Print something
    std::cout << "Table[" << eid << "] = " << _table0[eid] << ", " << _table1[eid] << ", " << _table2[eid] << "\n";
    return ERR_NONE;
  }

  int Get(T &val0, T &val1, T &val2)
  {
    //Get value
    val0 = _val0;
    val1 = _val1;
    val2 = _val2;
    return ERR_NONE;
  }

  int Set(const T val0, const T val1, const T val2)
  {
    //Set value
    _val0 = val0;
    _val1 = val1;
    _val2 = val2;
    return ERR_NONE;
  }

  int TableGet(uint32_t eid, T &val0, T &val1, T &val2)
  {
    //Check for EID out of range
    if(eid >= _table0.size())
    {
      HCParameter::DefaultVal(val0, val1, val2);
      return ERR_EID;
    }

    //Get value
    val0 = _table0[eid];
    val1 = _table1[eid];
    val2 = _table2[eid];
    return ERR_NONE;
  }

  int TableSet(uint32_t eid, const T val0, const T val1, const T val2)
  {
    //Check for EID out of range
    if(eid >= _table0.size())
      return ERR_EID;

    //Set value
    _table0[eid] = val0;
    _table1[eid] = val1;
    _table2[eid] = val2;
    return ERR_NONE;
  }

private:
  T _val0;
  T _val1;
  T _val2;
  std::vector<T> _table0;
  std::vector<T> _table1;
  std::vector<T> _table2;
};

typedef ScratchVec<float> ScratchVecFloat;
typedef ScratchVec<double> ScratchVecDouble;

#endif
