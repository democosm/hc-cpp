// CRC functions
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

#include "order.hh"
#include <arpa/inet.h>
#include <sys/param.h>

#define BSWAP16(n) ((x << 8) | \
                    (x >> 8))

#define BSWAP32(n) ((n << 24) | \
                   ((n << 8) & 0x00FF0000) | \
                   ((n >> 8) & 0x0000FF00) | \
                    (n >> 24))

#define BSWAP64(n) ((n << 56) | \
                   ((n << 32) & 0x00FF000000000000) | \
                   ((n << 16) & 0x0000FF0000000000) | \
                   ((n << 8) & 0x000000FF00000000) | \
                   ((n >> 8) & 0x00000000FF000000) | \
                   ((n >> 16) & 0x0000000000FF0000) | \
                   ((n >> 32) & 0x000000000000FF00) | \
                    (n >> 56))

uint8_t HostToNet(uint8_t n)
{
  return n; 
}

uint8_t NetToHost(uint8_t n)
{
  return n; 
}

uint16_t HostToNet(uint16_t n)
{
  return htons(n); 
}

uint16_t NetToHost(uint16_t n)
{
  return ntohs(n); 
}

uint32_t HostToNet(uint32_t n)
{
  return htonl(n); 
}

uint32_t NetToHost(uint32_t n)
{
  return ntohl(n); 
}

uint64_t HostToNet(uint64_t n)
{
#if __BYTE_ORDER == __BIG_ENDIAN
  return n; 
#else
  return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

uint64_t NetToHost(uint64_t n)
{
#if __BYTE_ORDER == __BIG_ENDIAN
  return n; 
#else
  return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
#endif
}

uint16_t HostToWrong(uint16_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n;
#else
  return BSWAP16(n); 
#endif
}

uint16_t WrongToHost(uint16_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n;
#else
  return BSWAP16(n); 
#endif
}

uint32_t HostToWrong(uint32_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n;
#else
  return BSWAP32(n); 
#endif
}

uint32_t WrongToHost(uint32_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n;
#else
  return BSWAP32(n); 
#endif
}

uint64_t HostToWrong(uint64_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n; 
#else
  return BSWAP64(n); 
#endif
}

uint64_t WrongToHost(uint64_t n)
{
#if __BYTE_ORDER != __BIG_ENDIAN
  return n; 
#else
  return BSWAP64(n); 
#endif
}
