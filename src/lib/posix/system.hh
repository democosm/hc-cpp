// System
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

#pragma once

#include <inttypes.h>
#include <stdio.h>

#define SC_QUIET_BEG "("
#define SC_QUIET_END ") > /dev/null 2>&1"

int SystemExecute(const char* fmt, ...);
void SystemPrint(uint8_t detail, const char* fmt, ...);
bool SystemGetPrintEnable(void);
void SystemSetPrintEnable(bool enable);
uint8_t SystemGetMaxPrintDetail(void);
void SystemSetMaxPrintDetail(uint8_t detail);
const char* SystemGetPrintLogFile(void);
void SystemSetPrintLogFile(const char* filename);

class System
{
public:
  System();
  ~System();
  int GetPrintEnable(bool& val);
  int SetPrintEnable(const bool val);
  int GetMaxPrintDetail(uint8_t& val);
  int SetMaxPrintDetail(const uint8_t val);
  int GetPrintLogFile(std::string& val);
  int SetPrintLogFile(const std::string& val);
};
