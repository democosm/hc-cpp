// HC cell
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

#pragma once

#include <inttypes.h>
#include <string>

class HCCell
{
public:
  //Opcodes
  static const uint8_t OPCODE_CALL_CMD = 0x00;
  static const uint8_t OPCODE_CALL_STS = 0x01;
  static const uint8_t OPCODE_GET_CMD = 0x02;
  static const uint8_t OPCODE_GET_STS = 0x03;
  static const uint8_t OPCODE_SET_CMD = 0x04;
  static const uint8_t OPCODE_SET_STS = 0x05;
  static const uint8_t OPCODE_ICALL_CMD = 0x06;
  static const uint8_t OPCODE_ICALL_STS = 0x07;
  static const uint8_t OPCODE_IGET_CMD = 0x08;
  static const uint8_t OPCODE_IGET_STS = 0x09;
  static const uint8_t OPCODE_ISET_CMD = 0x0A;
  static const uint8_t OPCODE_ISET_STS = 0x0B;
  static const uint8_t OPCODE_ADD_CMD = 0x0C;
  static const uint8_t OPCODE_ADD_STS = 0x0D;
  static const uint8_t OPCODE_SUB_CMD = 0x0E;
  static const uint8_t OPCODE_SUB_STS = 0x0F;
  static const uint8_t OPCODE_READ_CMD = 0x10;
  static const uint8_t OPCODE_READ_STS = 0x11;
  static const uint8_t OPCODE_WRITE_CMD = 0x12;
  static const uint8_t OPCODE_WRITE_STS = 0x13;

  //Cell overhead
  static const uint32_t OVERHEAD = 3;

  //Maximum payload size (see message payload max)
  static const uint32_t PAYLOAD_MAX = 1397;

public:
  HCCell();
  ~HCCell();
  void Reset(uint8_t opcode);
  uint8_t GetOpCode(void);
  uint32_t Serialize(uint8_t* serbuf, uint32_t maxlen);
  uint32_t Deserialize(uint8_t* serbuf, uint32_t len);
  bool Read(bool& val);
  bool Write(bool val);
  bool Read(std::string& val);
  bool Write(const std::string& val);
  bool Read(int8_t& val);
  bool Write(int8_t val);
  bool Read(int16_t& val);
  bool Write(int16_t val);
  bool Read(int32_t& val);
  bool Write(int32_t val);
  bool Read(int64_t& val);
  bool Write(int64_t val);
  bool Read(uint8_t& val);
  bool Write(uint8_t val);
  bool Read(uint16_t& val);
  bool Write(uint16_t val);
  bool Read(uint32_t& val);
  bool Write(uint32_t val);
  bool Read(uint64_t& val);
  bool Write(uint64_t val);
  template <typename T> bool Read(T* val, uint16_t maxlen, uint16_t& len);
  template <typename T> bool Write(const T* val, uint16_t len);
  bool Read(float& val);
  bool Write(float val);
  bool Read(double& val);
  bool Write(double val);
  bool Read(float& val0, float& val1);
  bool Write(float val0, float val1);
  bool Read(double& val0, double& val1);
  bool Write(double val0, double val1);
  bool Read(float& val0, float& val1, float& val2);
  bool Write(float val0, float val1, float val2);
  bool Read(double& val0, double& val1, double& val2);
  bool Write(double val0, double val1, double val2);
  void Print(const std::string& extra);

private:
  uint8_t* _buffer;
  uint8_t* _payload;
  uint32_t _readindex;
  uint8_t _opcode;
  uint32_t _payloadlength;
};
