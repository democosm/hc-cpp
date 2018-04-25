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

#ifndef _HCCONTAINER_HH_
#define _HCCONTAINER_HH_

#include "hcnode.hh"
#include "hcparameter.hh"
#include <string>

class HCContainer : public HCNode
{
public:
  HCContainer(const std::string &name);
  virtual ~HCContainer();
  bool GetNextCharInName(const std::string &name, char &nextchar);
  void PrintPath(void);
  void PrintInfo(std::ostream &st=std::cout);
  HCContainer *GetParent(void);
  void SetParent(HCContainer *node);
  HCContainer *GetNext(void);
  void SetNext(HCContainer *node);
  void Add(HCContainer *cont);
  void Add(HCParameter *param);
  HCContainer *GetFirstSubCont(void);
  HCParameter *GetFirstSubParam(void);

private:
  HCContainer *_parent;
  HCContainer *_next;
  HCContainer *_firstsubcont;
  HCParameter *_firstsubparam;
};

#endif //_HCCONTAINER_HH_
