// IPv4 functions
//
// Copyright 2021 Democosm
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

#include <arpa/inet.h>
#include <cassert>

bool IPv4AddrStrToInt(const char* addrstr, uint32_t& addrint)
{
  struct sockaddr_in sa;

  //Assert valid arguments
  assert(addrstr != 0);

  //Initialize integer value to zero
  addrint = 0;

  //Convert string to struct
  if(inet_pton(AF_INET, addrstr, &(sa.sin_addr)) != 1)
    return false;

  //Return integer value
  addrint = ntohl(sa.sin_addr.s_addr);
  return true;
}

bool IPv4AddrIsUnicast(const uint32_t addr)
{
  //Check for address in valid 
  if((addr > 0x00FFFFFF) && (addr < 0xE0000000))
    return true;

  //Not a good unicast address
  return false;
}

bool IPv4AddrIsUnicast(const char* addr)
{
  uint32_t addrint;

  //Convert to integer form
  if(!IPv4AddrStrToInt(addr, addrint))
    return false;

  return IPv4AddrIsUnicast(addrint);
}
