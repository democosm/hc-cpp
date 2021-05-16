// SPI driver
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

#include "error.hh"
#include "spi.hh"
#include <cassert>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

SPI::SPI(const char* name, uint32_t speed)
{
  uint32_t temp;

  //Assert valid arguments
  assert((name != 0) && (speed >= 1000) && (speed < 5000000));

  //Create mutex
  _mutex = new Mutex();

  //Open device
  if((_fd = open(name, O_RDWR)) < 0)
    printf("Could not open SPI device `%s`\n", name);

  //Configure write mode
  temp = SPI_MODE_0;
  if(ioctl(_fd, SPI_IOC_WR_MODE, &temp) < 0)
    printf("Could not set write mode\n");

  //Configure read mode
  temp = SPI_MODE_0;
  if(ioctl(_fd, SPI_IOC_RD_MODE, &temp) < 0)
    printf("Could not set read mode\n");

  //Configure write bits per word
  temp = 8;
  if(ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &temp) < 0)
    printf("Could not set write bits per word\n");

  //Configure read bits per word
  temp = 8;
  if(ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &temp) < 0)
    printf("Could not set read bits per word\n");

  //Cache speed
  _speed = speed;

  //Configure write max speed
  if(ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) < 0)
    printf("Could not set write max speed\n");

  //Configure read max speed
  if(ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) < 0)
    printf("Could not set read max speed\n");
}

SPI::~SPI()
{
  //Close device
  close(_fd);

  //Delete mutex
  delete _mutex;
}

uint32_t SPI::Transfer(uint8_t* wdata, uint8_t* rdata, uint32_t len)
{
  struct spi_ioc_transfer xfer[len];
  uint32_t i;

  //Silly, but set up one SPI transfer for each byte
  for(i=0; i<len; i++)
  {
    memset(&xfer[i], 0, sizeof(struct spi_ioc_transfer));
    xfer[i].tx_buf = (uint64_t)(wdata + i);
    xfer[i].rx_buf = (uint64_t)(rdata + i);
    xfer[i].len = 1;
    xfer[i].delay_usecs = 0;
    xfer[i].bits_per_word = 8;
    xfer[i].speed_hz = _speed;
    xfer[i].cs_change = 0;
  }

  //Perform SPI transfers
  if(ioctl(_fd, SPI_IOC_MESSAGE(len), &xfer) < 0)
    return 0;

  return len;
}
