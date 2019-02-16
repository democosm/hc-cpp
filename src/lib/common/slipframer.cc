// SLIP framer
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

#include "error.hh"
#include "slipframer.hh"
#include <cassert>

SLIPFramer::SLIPFramer(Device *lowdev, uint32_t maxpldsiz)
: Device()
{
  //Assert valid arguments
  assert((lowdev != 0) && (maxpldsiz > 0));

  //Initialize member variables
  _lowdev = lowdev;
  _maxpldsiz = maxpldsiz;
  _txbuf = new uint8_t[_maxpldsiz*2 + 2];
}

SLIPFramer::~SLIPFramer()
{
  //Cleanup
  delete[] _txbuf;
  delete _lowdev;
}

uint32_t SLIPFramer::Read(void *buf, uint32_t maxlen)
{
  uint8_t ch;
  uint32_t rcnt;
  uint32_t rbufind;
  int rxmode;

  //Assert valid arguments
  assert((buf != 0) && (maxlen > 0));

  //Initialize state variables
  rxmode = RX_MODE_NORMAL;
  rbufind = 0;

  //Read a byte at a time
  while((rcnt = _lowdev->Read(&ch, 1)) == 1)
  {
    //Process depending on mode
    switch(rxmode)
    {
    case RX_MODE_NORMAL:
      //Check for END byte
      if(ch == BYTE_END)
      {
        //Check for any data received before END byte
        if(rbufind != 0)
          return rbufind;

        //Just read more data
        break;
      }

      //Check for escape byte
      if(ch == BYTE_ESC)
      {
        //Go to escape mode
        rxmode = RX_MODE_ESCAPE;
        break;
      }

      //Check for not enough room
      if(rbufind >= maxlen)
      {
        //Reset state machine
        rbufind = 0;
        rxmode = RX_MODE_NORMAL;
        break;
      }

      //Buffer received byte
      *((uint8_t *)buf + rbufind++) = ch;

      break;
    case RX_MODE_ESCAPE:
      //Check for not enough room
      if(rbufind >= maxlen)
      {
        //Reset state machine
        rbufind = 0;
        rxmode = RX_MODE_NORMAL;
        break;
      }

      //Decode escaped byte
      switch(ch)
      {
      case BYTE_ESC_END:
        //Buffer decoded END byte and go back to normal mode
        *((uint8_t *)buf + rbufind++) = BYTE_END;
        rxmode = RX_MODE_NORMAL;
        break;
      case BYTE_ESC_ESC:
        //Buffer decoded ESC byte and go back to normal mode
        *((uint8_t *)buf + rbufind++) = BYTE_ESC;
        rxmode = RX_MODE_NORMAL;
        break;
      default:
        //Reset state machine on invalid escape sequence
        rbufind = 0;
        rxmode = RX_MODE_NORMAL;
        break;
      }

      break;
    default:
      //Reset state machine on invalid state
      rbufind = 0;
      rxmode = RX_MODE_NORMAL;
      break;
    }
  }

  //An error occurred in lower level device read
  return rbufind;
}

uint32_t SLIPFramer::Write(const void *buf, uint32_t len)
{
  uint32_t i;
  uint32_t byteind;
  uint8_t ch;

  //Assert valid arguments
  assert((buf != 0) && (len > 0) && (len <= _maxpldsiz));

  //Zero the byte index
  byteind = 0;

  //First byte is always END byte
  _txbuf[byteind++] = BYTE_END;

  //Loop through all bytes to be sent
  for(i=0; i<len; i++)
  {
    //Get byte
    ch = *((uint8_t *)buf + i);

    //Put data into transmit buffer
    switch(ch)
    {
    case BYTE_END:
      //Byte stuff END byte
      _txbuf[byteind++] = BYTE_ESC;
      _txbuf[byteind++] = BYTE_ESC_END;
      break;
    case BYTE_ESC:
      //Byte stuff ESC byte
      _txbuf[byteind++] = BYTE_ESC;
      _txbuf[byteind++] = BYTE_ESC_ESC;
      break;
    default:
      //No need for byte stuffing
      _txbuf[byteind++] = ch;
    }
  }

  //Last byte is always END
  _txbuf[byteind++] = BYTE_END;

  //Write transmit message to the lower device and check for error
  if(_lowdev->Write(_txbuf, byteind) != byteind)
    return 0;

  return len;
}
