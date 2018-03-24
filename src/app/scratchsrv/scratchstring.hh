// String scratch pad class
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

#ifndef _SCRATCHSTRING_HH_
#define _SCRATCHSTRING_HH_

#include "hcparameter.hh"
#include <inttypes.h>
#include <string>
#include <vector>

using namespace std;

class ScratchString
{
public:
  ScratchString(const uint32_t tablesize, const uint32_t listmaxsize)
  {
    uint32_t eid;

    //Initialize member variables
    HCParameter::DefaultVal(_val);

    for(eid=0; eid<tablesize; eid++)
      _table.push_back(_val);

    _listmaxsize = listmaxsize;
  }

  ~ScratchString()
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

  int Get(string &val)
  {
    //Get value
    val = _val;
    return ERR_NONE;
  }

  int Set(const string &val)
  {
    //Set value
    _val = val;
    return ERR_NONE;
  }

  int TableGet(uint32_t eid, string &val)
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

  int TableSet(uint32_t eid, const string &val)
  {
    //Check for EID out of range
    if(eid >= _table.size())
      return ERR_EID;

    //Set value
    _table[eid] = val;
    return ERR_NONE;
  }

  int ListGet(uint32_t eid, string &val)
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

  int ListAdd(const string &val)
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

  int ListSub(const string &val)
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
  string _val;
  vector<string> _table;
  vector<string> _list;
  uint32_t _listmaxsize;
};

#endif //_SCRATCHSTRING_HH_
