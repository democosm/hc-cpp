// HC aggregator
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

#include "hcaggregator.hh"
#include "str.hh"
#include <cassert>
#include <iostream>
#include <string.h>

using namespace std;
using namespace tinyxml2;

HCAggregator::HCAggregator(const string& filename)
{
  XMLDocument doc;

  //Create top container
  _topcont = new HCContainer("");

  //Initialize connection array information
  _conn = 0;
  _conncount = 0;

  //Initialize query server information
  _qsrvdev = 0;
  _qsrv = 0;

  //Initialize server information
  _srvdev = 0;
  _srv = 0;

  //Parse file and check for error
  if((doc.LoadFile(filename.c_str())) != 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Error parsing file (" << filename << ')' << "\n";
    return;
  }

  //Parse server from DOM and check for error
  if((_srv = ParseServer(doc.FirstChildElement("server"))) == 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Error parsing server from DOM" << "\n";
    return;
  }

  //Add parameters to server starting at top container
  AddParamsToServer(_topcont);

  //Start server
  _srv->Start();
}

HCAggregator::~HCAggregator()
{
  uint32_t i;

  //Cleanup
  delete _srv;
  delete _srvdev;
  delete _qsrv;
  delete _qsrvdev;

  for(i=0; i<_conncount; i++)
    if(_conn[i] == 0)
      delete _conn[i];

  delete[] _conn;
  delete _topcont;
}

HCContainer* HCAggregator::GetTopCont(void)
{
  return _topcont;
}

void HCAggregator::AddParamsToServer(HCContainer* startcont)
{
  HCParameter* param;
  HCContainer* cont;

  //Check for server not created
  if(_srv == 0)
    return;

  //Loop through all parameters adding to server
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
    _srv->Add(param);

  //Loop through all containers recursively adding parameters to server
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
    AddParamsToServer(cont);
}

HCServer* HCAggregator::ParseServer(XMLElement* pelt)
{
  uint32_t i;
  XMLElement* elt;
  string name;
  uint16_t port;
  uint16_t qport;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Count number of connections
  _conncount = 0;
  for(elt = pelt->FirstChildElement("conn"); elt != 0; elt = elt->NextSiblingElement("conn"))
    _conncount++;

  //Check for no connections
  if(_conncount == 0)
    return 0;

  //Create connection array
  _conn = new HCConnection*[_conncount];

  //Parse all connections
  for(i = 0, elt = pelt->FirstChildElement("conn"); i < _conncount; i++, elt = elt->NextSiblingElement("conn"))
    _conn[i] = ParseConn(elt);

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return 0;

  //Parse port element and check for error
  if(!ParseValue(pelt, "port", port))
    return 0;

  //Check for optional query server
  if(ParseValue(pelt, "qport", qport))
  {
    //Create query server device
    _qsrvdev = new UDPDevice(qport);

    //Create query server
    _qsrv = new HCQServer(_qsrvdev, _topcont);
  }

  //Create server device
  _srvdev = new UDPDevice(port);

  //Create server
  return new HCServer(_srvdev, _topcont, name, __DATE__ " " __TIME__);
}

HCConnection* HCAggregator::ParseConn(XMLElement* pelt)
{
  string name;
  uint32_t timeout;
  Device* dev;
  XMLElement* elt;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Parse name element and check for error
  if(!ParseValue(pelt, "name", name))
    return 0;

  //Parse timeout element and check for error
  if(!ParseValue(pelt, "timeout", timeout))
    return 0;

  //Initialize device pointer to invalid
  dev = 0;

  //Look for various devices
  if((elt = pelt->FirstChildElement("udpsocket")) != 0)
    dev = ParseUDPSocket(elt);
  else if((elt = pelt->FirstChildElement("slipframer")) != 0)
    dev = ParseSLIPFramer(elt);

  //Check for device not found
  if(dev == 0)
    return 0;

  //Create connection
  return new HCConnection(dev, _topcont, name, timeout);
}

UDPDevice* HCAggregator::ParseUDPSocket(XMLElement* pelt)
{
  uint16_t port;
  string destipaddr;
  uint16_t destport;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Parse port element and check for error
  if(!ParseValue(pelt, "port", port))
    return 0;

  //Parse destination IP address element and check for error
  if(!ParseValue(pelt, "destipaddr", destipaddr))
    return 0;

  //Parse destination port element and check for error
  if(!ParseValue(pelt, "destport", destport))
    return 0;

  //Create UDP socket
  return new UDPDevice(port, 0, destipaddr.c_str(), destport);
}

SLIPFramer* HCAggregator::ParseSLIPFramer(XMLElement* pelt)
{
  uint32_t maxpldsiz;
  Device* dev;
  XMLElement* elt;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Parse max payload size element and check for error
  if(!ParseValue(pelt, "maxpldsiz", maxpldsiz))
    return 0;

  //Initialize device pointer to invalid
  dev = 0;

  //Look for various devices
  if((elt = pelt->FirstChildElement("tcpclient")) != 0)
    dev = ParseTCPClient(elt);
  else if((elt = pelt->FirstChildElement("tlsclient")) != 0)
    dev = ParseTLSClient(elt);

  //Check for device not found
  if(dev == 0)
    return 0;

  //Create SLIP framer
  return new SLIPFramer(dev, maxpldsiz);
}

TCPClient* HCAggregator::ParseTCPClient(XMLElement* pelt)
{
  uint16_t port;
  string srvipaddr;
  uint16_t srvport;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Parse port element and check for error
  if(!ParseValue(pelt, "port", port))
    return 0;

  //Parse server IP address element and check for error
  if(!ParseValue(pelt, "srvipaddr", srvipaddr))
    return 0;

  //Parse server port element and check for error
  if(!ParseValue(pelt, "srvport", srvport))
    return 0;

  //Create TCP client
  return new TCPClient(port, srvipaddr.c_str(), srvport);
}

TLSClient* HCAggregator::ParseTLSClient(XMLElement* pelt)
{
  uint16_t port;
  string srvipaddr;
  uint16_t srvport;
  string authstring;

  //Check for null parent element
  if(pelt == 0)
    return 0;

  //Parse port element and check for error
  if(!ParseValue(pelt, "port", port))
    return 0;

  //Parse server IP address element and check for error
  if(!ParseValue(pelt, "srvipaddr", srvipaddr))
    return 0;

  //Parse server port element and check for error
  if(!ParseValue(pelt, "srvport", srvport))
    return 0;

  //Parse authorization string element and check for error
  if(!ParseValue(pelt, "authstring", authstring))
    return 0;

  //Create TLS client
  return new TLSClient(port, srvipaddr.c_str(), srvport, authstring.c_str());
}

bool HCAggregator::ParseValue(XMLElement* pelt, const char* name, string& val)
{
  const char* text;

  //Check for null parent element
  if(pelt == 0)
    return false;

  //Find element with matching name and check for error
  if((text = pelt->FirstChildElement(name)->GetText()) == 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Could not find element \"" << name << "\"\n";
    return false;
  }

  //Return value
  val = text;
  return true;
}

bool HCAggregator::ParseValue(XMLElement* pelt, const char* name, uint16_t& val)
{
  const char* text;

  //Check for null parent element
  if(pelt == 0)
    return false;

  //Find element with matching name and check for error
  if((text = pelt->FirstChildElement(name)->GetText()) == 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Could not find element \"" << name << "\"\n";
    return false;
  }

  //Convert value to integer
  return StringConvert(text, val);
}

bool HCAggregator::ParseValue(XMLElement* pelt, const char* name, uint32_t& val)
{
  const char* text;

  //Check for null parent element
  if(pelt == 0)
    return false;

  //Find element with matching name and check for error
  if((text = pelt->FirstChildElement(name)->GetText()) == 0)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Could not find element \"" << name << "\"\n";
    return false;
  }

  //Convert value to integer
  return StringConvert(text, val);
}
