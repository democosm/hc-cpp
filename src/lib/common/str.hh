// String functions
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

#ifndef _STR_HH_
#define _STR_HH_

#include <inttypes.h>
#include <string>

bool StrExp(const std::string &str, const std::string &exp, size_t strind=0, size_t expind=0);
bool StringIPv4Convert(const std::string &str, uint32_t &val);
bool StringConvert(const std::string &str, bool &val);
bool StringConvert(const std::string &str, int8_t &val);
bool StringConvert(const std::string &str, int16_t &val);
bool StringConvert(const std::string &str, int32_t &val);
bool StringConvert(const std::string &str, int64_t &val);
bool StringConvert(const std::string &str, uint8_t &val);
bool StringConvert(const std::string &str, uint16_t &val);
bool StringConvert(const std::string &str, uint32_t &val);
bool StringConvert(const std::string &str, uint64_t &val);
bool StringConvert(const std::string &str, float &val);
bool StringConvert(const std::string &str, double &val);
bool StringConvert(const std::string &str, std::string &val);
bool StringConvert(const char *str, bool &val);
bool StringConvert(const char *str, int8_t &val);
bool StringConvert(const char *str, int16_t &val);
bool StringConvert(const char *str, int32_t &val);
bool StringConvert(const char *str, int64_t &val);
bool StringConvert(const char *str, uint8_t &val);
bool StringConvert(const char *str, uint16_t &val);
bool StringConvert(const char *str, uint32_t &val);
bool StringConvert(const char *str, uint64_t &val);
bool StringConvert(const char *str, float &val);
bool StringConvert(const char *str, double &val);
bool StringConvert(const char *str, std::string &val);
void StringPrint(bool val, std::string &str);
void StringPrint(int8_t val, std::string &str);
void StringPrint(int16_t val, std::string &str);
void StringPrint(int32_t val, std::string &str);
void StringPrint(int64_t val, std::string &str);
void StringPrint(uint8_t val, std::string &str);
void StringPrint(uint16_t val, std::string &str);
void StringPrint(uint32_t val, std::string &str);
void StringPrint(uint64_t val, std::string &str);
void StringPrint(float val, std::string &str);
void StringPrint(double val, std::string &str);

#endif //_STR_HH_

