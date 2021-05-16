// Physical memory bus
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

#include "error.hh"
#include "membus.hh"
#include <cassert>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

MemBus::MemBus(uint32_t physbase, uint32_t size)
: Bus()
{
  //Cache arguments
  _size = size;

  //Open memory device
  _memfd = open("/dev/mem", O_RDWR);

  //Map physical address space to virtual memory
  if((_virtbase = (uint8_t*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, _memfd, physbase)) == MAP_FAILED)
    printf("MemBus - Error mapping physical memory 0x%X to virtual space\n", physbase);
}

MemBus::~MemBus()
{
  //Cleanup
  if(_virtbase != MAP_FAILED)
    munmap((void*)_virtbase, _size);

  close(_memfd);
}

int MemBus::Get(uint32_t addr, uint8_t* data, uint32_t len)
{
  //Assert valid arguments
  assert((data != 0) && (len > 0));

  //Check for no virtual mapping
  if(_virtbase == MAP_FAILED)
    return ERR_ACCESS;

  //Check for reading outside of buffer bounds
  if((addr + len) > _size)
    return ERR_OVERFLOW;

  //Copy data from virtual memory
  memcpy(data, _virtbase + addr, len);
  return ERR_NONE;
}

int MemBus::Set(uint32_t addr, uint8_t* data, uint32_t len)
{
  //Assert valid arguments
  assert((data != 0) && (len > 0));

  //Check for no virtual mapping
  if(_virtbase == MAP_FAILED)
    return ERR_ACCESS;

  //Check for writing outside of buffer bounds
  if((addr + len) > _size)
    return ERR_OVERFLOW;

  //Copy data to virtual memory
  memcpy(_virtbase + addr, data, len);
  return ERR_NONE;
}
