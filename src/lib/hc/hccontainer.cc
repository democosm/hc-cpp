// HC container
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

#include "hccontainer.hh"
#include <inttypes.h>
#include <cassert>
#include <iostream>

HCContainer::HCContainer(const string &name)
: HCNode(name)
{
  //Initialize member variables
  _parent = 0;
  _next = 0;
  _firstsubcont = 0;
  _firstsubparam = 0;
}

HCContainer::~HCContainer()
{
  HCContainer *cont;
  HCContainer *nextcont;
  HCParameter *param;
  HCParameter *nextparam;

  //Cleanup sub containers
  for(cont=_firstsubcont; cont!=0; cont=nextcont)
  {
    //Get next container from current before deleting current
    nextcont = cont->GetNext();
    delete cont;
  }

  //Cleanup sub parameters
  for(param=_firstsubparam; param!=0; param=nextparam)
  {
    //Get next parameter from current before deleting current
    nextparam = param->GetNext();
    delete param;
  }
}

bool HCContainer::GetNextCharInName(const string &name, char &nextchar)
{
  uint32_t namelen;

  //Check for exact match
  if(_name == name)
  {
    nextchar = '/';
    return true;
  }

  //Get length of string
  namelen = name.length();

  //Check for name too long
  if(namelen >= _name.length())
    return false;

  //Check for name starting with string
  if(_name.compare(0, namelen, name) == 0)
  {
    //Give caller the next character in the name and indicate match
    nextchar = _name.at(namelen);
    return true;
  }

  //Indicate no match
  return false;
}

void HCContainer::PrintPath(void)
{
  //Check for at root of path
  if(_parent == 0)
  {
    //Print name and separator
    cout << _name << '/';
    return;
  }

  //Recurse
  _parent->PrintPath();

  //Print name and separator
  cout << _name << '/';
}

void HCContainer::PrintInfo(ostream &st)
{
  //Print information
  st << "Type: Cont";
}

HCContainer *HCContainer::GetParent(void)
{
  return _parent;
}

void HCContainer::SetParent(HCContainer *parent)
{
  _parent = parent;
}

HCContainer *HCContainer::GetNext(void)
{
  return _next;
}

void HCContainer::SetNext(HCContainer *next)
{
  _next = next;
}

void HCContainer::Add(HCContainer *cont)
{
  HCContainer *last;

  //Assert valid arguments
  assert(cont != 0);

  //Check for first container added
  if(_firstsubcont == 0)
  {
    _firstsubcont = cont;
  }
  else
  {
    //Find last container in list
    for(last=_firstsubcont; last!=0; last=last->GetNext())
      if(last->GetNext() == 0)
        break;

    //Add new container at end of list
    last->SetNext(cont);
  }

  //This container becomes parent to added container
  cont->SetParent(this);
}

void HCContainer::Add(HCParameter *param)
{
  HCParameter *last;

  //Assert valid arguments
  assert(param != 0);

  //Check for first parameter added
  if(_firstsubparam == 0)
  {
    _firstsubparam = param;
  }
  else
  {
    //Find last parameter in list
    for(last=_firstsubparam; last!=0; last=last->GetNext())
      if(last->GetNext() == 0)
        break;

    //Add new parameter at end of list
    last->SetNext(param);
  }
}

HCContainer *HCContainer::GetFirstSubCont(void)
{
  return _firstsubcont;
}

HCParameter *HCContainer::GetFirstSubParam(void)
{
  return _firstsubparam;
}
