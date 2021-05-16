// HC query application
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

#include "event.hh"
#include "str.hh"
#include "thread.hh"
#include "udpdevice.hh"
#include <cassert>
#include <iostream>
#include <string.h>

using namespace std;

class Transactor
{
public:
  Transactor(Device* dev, const char* querystring)
  {
    //Assert valid arguments
    assert((dev != 0) && (querystring != 0));

    //Initialize cache member variables
    _dev = dev;
    _querystring = querystring;

    //Create event
    _event = new Event();

    //Create and start read thread
    _readthread = new Thread<Transactor>(this, &Transactor::ReadThread);
    _readthread->Start();
  }

  ~Transactor()
  {
    //Cleanup
    delete _readthread;
    delete _event;
  }

  void Transact(void)
  {
    uint32_t len;

    //Get length of query string
    len = strlen(_querystring);

    //Check for bad query string length
    if(len < 3)
    {
      cout << "Bad query string length (" << len << ")" << "\n";
      return;
    }

    //Write query string to device
    if(_dev->Write(_querystring, len) != len)
    {
      cout << "Error sending query string" << "\n";
      return;
    }

    //Wait for event
    if(_event->Wait(1000) != ERR_NONE)
      cout << "Timed out waiting for response" << "\n";
  }

private:
  void ReadThread(void)
  {
    char readbuf[65536];
    uint32_t readcount;
    uint32_t i;

    //Go forever
    while(true)
    {
      //Read from device
      if((readcount = _dev->Read(readbuf, sizeof(readbuf)-1)) < 3)
      {
        ThreadSleep(1000);
        continue;
      }

      //Check for transaction match
      for(i=1; i<4; i++)
      {
        if((_querystring[i] == readbuf[i]) && (readbuf[i] == ','))
	{
          //Null terminate read data and print
          readbuf[readcount] = '\0';
          cout << readbuf << "\n";

          //Signal event
          _event->Signal();
	}
      }
    }
  }

private:
  Device* _dev;
  const char* _querystring;
  Event* _event;
  Thread<Transactor>* _readthread;
};

void Usage(const char* appname)
{
  cout << "Usage: " << appname << " <SERVER IP ADDRESS> <SERVER PORT> <QUERY STRING>" << "\n";
}

int main(int argc, char** argv)
{
  uint16_t port;
  UDPDevice* dev;
  Transactor* transactor;

  //Check for wrong number of arguments
  if(argc != 4)
  {
    Usage(argv[0]);
    return -1;
  }

  //Convert server port number to integer and check for error
  if(!StringConvert(argv[2], port))
  {
    cout << "Invalid port number " << argv[2] << "\n";
    Usage(argv[0]);
    return -1;
  }

  //Create device
  dev = new UDPDevice(0, 0, argv[1], port);

  //Create transactor and perform transaction
  transactor = new Transactor(dev, argv[3]);
  transactor->Transact();

  //Cleanup
  delete transactor;
  delete dev;

  return 0;
}
