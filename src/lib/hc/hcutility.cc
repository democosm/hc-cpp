// HC utility
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

#include "hcutility.hh"
#include <cassert>

using namespace std;

HCContainer* HCUtility::GetCont(const string& name, HCContainer* startcont, size_t index)
{
  string nodename;
  size_t nextindex;
  HCContainer* cont;

  //Assert valid arguments
  assert(startcont != 0);

  //Loop through directory names in path
  while((nextindex = name.find('/', index)) != string::npos)
  {
    //Extract node name
    nodename = name.substr(index, nextindex-index);

    //Update index
    index = nextindex+1;

    //Check for special strings first
    if((nodename == "") || (nodename == "."))
    {
      //Continue to next iteration
      continue;
    }
    else if(nodename == "..")
    {
      //If no parent, then continue to next iteration
      if(startcont->GetParent() == 0)
      {
        //Continue to next iteration
        continue;
      }

      //Recurse
      return GetCont(name, startcont->GetParent(), index);
    }
    else
    {
      //Loop through all containers
      for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
      {
        //Check for exact match
        if(cont->IsNamed(nodename))
        {
          //Recurse
          return GetCont(name, cont, index);
        }
      }
    }

    //Container not found
    return 0;
  }

  //Extract node name
  nodename = name.substr(index, name.length()-index);

  //Check for special strings first
  if((nodename == "") || (nodename == "."))
  {
    return startcont;
  }
  else if(nodename == "..")
  {
    //If no parent, then return start container
    if(startcont->GetParent() == 0)
      return startcont;

    //Return parent
    return startcont->GetParent();
  }

  //Loop through all containers and return one with matching name
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
  {
    //Check for exact match
    if(cont->IsNamed(nodename))
      return cont;
  }

  //Not found
  return 0;
}

HCParameter* HCUtility::GetParam(const string& name, HCContainer* startcont, size_t index)
{
  string nodename;
  HCContainer* cont;
  HCParameter* param;
  size_t nextindex;

  //Assert valid arguments
  assert(startcont != 0);

  //Loop through directory names in path
  while((nextindex = name.find('/', index)) != string::npos)
  {
    //Extract node name
    nodename = name.substr(index, nextindex-index);

    //Update index
    index = nextindex+1;

    //Check for special strings first
    if((nodename == "") || (nodename == "."))
    {
      //Continue to next iteration
      continue;
    }
    else if(nodename == "..")
    {
      //If no parent, then continue to next iteration
      if(startcont->GetParent() == 0)
      {
        //Continue to next iteration
        continue;
      }

      //Recurse
      return GetParam(name, startcont->GetParent(), index);
    }
    else
    {
      //Loop through all containers
      for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
      {
        //Check for exact match
        if(cont->IsNamed(nodename))
        {
          //Recurse
          return GetParam(name, cont, index);
        }
      }
    }

    //Container not found
    return 0;
  }

  //Extract node name
  nodename = name.substr(index, name.length()-index);

  //Loop through all parameters
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
  {
    //Check for exact match
    if(param->IsNamed(nodename))
      return param;
  }

  //Not found
  return 0;
}
