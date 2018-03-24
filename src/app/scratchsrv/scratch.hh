// Scratch pad class
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

#ifndef _SCRATCH_HH_
#define _SCRATCH_HH_

#include "hcparameter.hh"
#include <inttypes.h>
#include <string>
#include <vector>

using namespace std;

template <typename T>
class Scratch
{
public:
  Scratch(const uint32_t tablesize, const uint32_t listmaxsize)
  {
    uint32_t eid;

    //Initialize member variables
    HCParameter::DefaultVal(_val);

    for(eid=0; eid<tablesize; eid++)
      _table.push_back(_val);

    _listmaxsize = listmaxsize;
  }

  ~Scratch()
  {
  }

  int Print(void)
  {
    //Print value
    cout << "Value = \"" << _val << "\"" << endl;
    return ERR_NONE;
  }

  int TablePrint(uint32_t eid)
  {
    //Print something
    cout << "Table[" << eid << "] = " << _table[eid] << endl;
    return ERR_NONE;
  }

  int Get(T &val)
  {
    //Get value
    val = _val;
    return ERR_NONE;
  }

  int Set(const T val)
  {
    //Set value
    _val = val;
    return ERR_NONE;
  }

  int TableGet(uint32_t eid, T &val)
  {
    //Check for EID out of range
    if(eid >= _table.size())
    {
      HCParameter::DefaultVal(val);
      return ERR_EID;
    }

    //Get value
    val = _table[eid];
    return ERR_NONE;
  }

  int TableSet(uint32_t eid, const T val)
  {
    //Check for EID out of range
    if(eid >= _table.size())
      return ERR_EID;

    //Set value
    _table[eid] = val;
    return ERR_NONE;
  }

  int ListGet(uint32_t eid, T &val)
  {
    //Check for EID out of range
    if(eid >= _list.size())
    {
      HCParameter::DefaultVal(val);
      return ERR_EID;
    }

    //Get value
    val = _list[eid];
    return ERR_NONE;
  }

  int ListAdd(const T val)
  {
    uint32_t i;

    //Check for list full
    if(_list.size() >= _listmaxsize)
      return ERR_UNSPEC;

    //Loop through existing entries looking for a match
    for(i=0; i<_list.size(); i++)
      if(_list[i] == val)
        break;

    //Check for matching entry found
    if(i < _list.size())
      return ERR_UNSPEC;

    //Add entry
    _list.push_back(val);
    return ERR_NONE;
  }

  int ListSub(const T val)
  {
    uint32_t i;

    //Check for list empty
    if(_list.size() == 0)
      return ERR_UNSPEC;

    //Loop through all entries
    for(i=0; i<_list.size(); i++)
    {
      //Check for match
      if(_list[i] == val)
      {
        _list.erase(_list.begin()+i);
        return ERR_NONE;
      }
    }

    //Entry not found
    return ERR_NOTFOUND;
  }

private:
  T _val;
  vector<T> _table;
  vector<T> _list;
  uint32_t _listmaxsize;
};

typedef Scratch<bool> ScratchBool;
typedef Scratch<int8_t> ScratchS8;
typedef Scratch<int16_t> ScratchS16;
typedef Scratch<int32_t> ScratchS32;
typedef Scratch<int64_t> ScratchS64;
typedef Scratch<uint8_t> ScratchU8;
typedef Scratch<uint16_t> ScratchU16;
typedef Scratch<uint32_t> ScratchU32;
typedef Scratch<uint64_t> ScratchU64;
typedef Scratch<float> ScratchFloat;
typedef Scratch<double> ScratchDouble;

#endif //_SCRATCH_HH_
