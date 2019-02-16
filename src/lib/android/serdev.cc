// Serial device
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

#include "serdev.hh"
#include "error.hh"
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>

using namespace std;

SerDev::SerDev(const char *name, uint32_t baud, int hwflowctl)
{
  struct termios opts;

  //Assert valid arguments
  assert((name != 0) && (baud > 0));

  //Open the serial device
  if((_fd = open(name, O_RDWR | O_NOCTTY)) < 0)
    cout << __FILE__ << ":" << __LINE__ << " - Error opening " << name << endl;

  //Remember old options
  tcgetattr(_fd, &_oldopts);

  //Zero out new options
  memset(&opts, 0, sizeof(opts));

  //Data bits etc...
  opts.c_cflag = CS8 | CLOCAL | CREAD;

  //Parity etc...
  opts.c_iflag = IGNPAR;
  opts.c_oflag = 0;
  opts.c_lflag = 0;

  //Inter character timer unused
  opts.c_cc[VTIME] = 0;

  //Block until at least this many bytes have arrived
  opts.c_cc[VMIN] = 1;

  //Baud rate
  switch(baud)
  {
  case 50:
    cfsetispeed(&opts, B50);
    cfsetospeed(&opts, B50);
    break;
  case 75:
    cfsetispeed(&opts, B75);
    cfsetospeed(&opts, B75);
    break;
  case 110:
    cfsetispeed(&opts, B110);
    cfsetospeed(&opts, B110);
    break;
  case 134:
    cfsetispeed(&opts, B134);
    cfsetospeed(&opts, B134);
    break;
  case 150:
    cfsetispeed(&opts, B150);
    cfsetospeed(&opts, B150);
    break;
  case 200:
    cfsetispeed(&opts, B200);
    cfsetospeed(&opts, B200);
    break;
  case 300:
    cfsetispeed(&opts, B300);
    cfsetospeed(&opts, B300);
    break;
  case 600:
    cfsetispeed(&opts, B600);
    cfsetospeed(&opts, B600);
    break;
  case 1200:
    cfsetispeed(&opts, B1200);
    cfsetospeed(&opts, B1200);
    break;
  case 1800:
    cfsetispeed(&opts, B1800);
    cfsetospeed(&opts, B1800);
    break;
  case 2400:
    cfsetispeed(&opts, B2400);
    cfsetospeed(&opts, B2400);
    break;
  case 4800:
    cfsetispeed(&opts, B4800);
    cfsetospeed(&opts, B4800);
    break;
  case 9600:
    cfsetispeed(&opts, B9600);
    cfsetospeed(&opts, B9600);
    break;
  case 19200:
    cfsetispeed(&opts, B19200);
    cfsetospeed(&opts, B19200);
    break;
  case 38400:
    cfsetispeed(&opts, B38400);
    cfsetospeed(&opts, B38400);
    break;
  case 57600:
    cfsetispeed(&opts, B57600);
    cfsetospeed(&opts, B57600);
    break;
  case 115200:
    cfsetispeed(&opts, B115200);
    cfsetospeed(&opts, B115200);
    break;
  case 230400:
    cfsetispeed(&opts, B230400);
    cfsetospeed(&opts, B230400);
    break;
  default:
    cout << __FILE__ << ":" << __LINE__ << " - Unsupported baud rate (" << baud << ")" << endl;
    break;
  }

  //Flow control
  if(hwflowctl == 0)
    opts.c_cflag &= ~CRTSCTS;
  else
    opts.c_cflag |= CRTSCTS;

  //Apply new options
  tcsetattr(_fd, TCSANOW, &opts);
}

SerDev::~SerDev()
{
  //Restore old options
  tcsetattr(_fd, TCSANOW, &_oldopts);

  //Close the port
  close(_fd);
}

uint32_t SerDev::Read(void *buf, uint32_t maxlen)
{
  ssize_t rlen;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Read from the port
  if((rlen = read(_fd, buf, maxlen)) <= 0)
    return 0;

  return (uint32_t)rlen;
}

uint32_t SerDev::Write(const void *buf, uint32_t len)
{
  ssize_t wlen;

  //Assert valid arguments
  assert((buf != 0) && (len > 0));

  //Write to the port
  if((wlen = write(_fd, buf, len)) <= 0)
    return 0;

  return (uint32_t)wlen;
}
