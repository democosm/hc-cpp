// Example HC server application
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

#include "device.hh"
#include "error.hh"
#include "hcboolean.hh"
#include "hccall.hh"
#include "hcconsole.hh"
#include "hccontainer.hh"
#include "hcfile.hh"
#include "hcfloat.hh"
#include "hcinteger.hh"
#include "hcqserver.hh"
#include "hcserver.hh"
#include "hcstring.hh"
#include "hcvector.hh"
#include "scratchfile.hh"
#include "scratch.hh"
#include "scratchstring.hh"
#include "scratchvec.hh"
#include "slipframer.hh"
#include "str.hh"
#include "tcpserver.hh"
#include "thread.hh"
#include "tlsserver.hh"
#include "udpdevice.hh"
#include <cassert>
#include <getopt.h>
#include <inttypes.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define TABLE_SIZE 3
#define LIST_MAX_SIZE 10

//Description strings
static const char *Appversion = "2.0.0 " __DATE__ " " __TIME__;
static const char *Appbugaddress = "<democosm@gmail.com>";
static const char Appdoc[] = "Creates a scratch pad HC server that contains all basic HC types.";

//Application options
static struct option Longopts[] =
{
  { "tcp", 0, NULL, 't' },
  { "tls", 0, NULL, 's' },
  { "port", required_argument, NULL, 'p' },
  { "qport", required_argument, NULL, 'q' },
  { "daemon", 0, NULL, 'd' },
  { NULL, 0, NULL, 0 }
};

//Storage for parsed arguments
struct Args
{
  bool tcp;
  bool tls;
  uint16_t port;
  uint16_t qport;
  bool daemon;
};

void Usage()
{
  //Print usage information
  cout << "Example HC server" << "\n";
  cout << "Version: " << Appversion << "\n";
  cout << "Report bugs to: " << Appbugaddress << "\n";
  cout << Appdoc << "\n";
  cout << "[-t, --tcp] Use TCP for transport protocol" << "\n";
  cout << "[-s, --tls] Use TLS for transport protocol" << "\n";
  cout << "[-p, --port] <PORT> Port number used for server (defaults to 1500)" << "\n";
  cout << "[-q, --qport] <PORT> Port number used for query server (defaults to 5555)" << "\n";
  cout << "[-d, --daemon] Spawn in background mode" << "\n";
}

bool ParseOptions(int argc, char **argv, struct Args* args)
{
  int ch;
  bool valid;

  //Loop through all options or until invalid option reached
  valid = true;
  while(((ch = getopt_long(argc, argv, "p", Longopts, NULL)) != -1) && valid)
  {
    switch(ch)
    {
    case 't':
      args->tcp = true;
      break;
    case 's':
      args->tls = true;
      break;
    case 'p':
      //Convert port number and check for error
      if(!StringConvert(optarg, args->port))
      {
        valid = false;
        cout << "Invalid port number (" << optarg << ")" << "\n";
        Usage();
        break;
      }

      break;
    case 'q':
      //Convert port number and check for error
      if(!StringConvert(optarg, args->qport))
      {
        valid = false;
        cout << "Invalid query port number (" << optarg << ")" << "\n";
        Usage();
        break;
      }

      break;
    case 'd':
      args->daemon = true;
      break;
    default:
      valid = false;
      Usage();
      break;
    }
  }

  argc -= optind;
  argv += optind;

  return valid;
}

void Add(HCContainer *cont, HCContainer *parent)
{
  //Assert valid arguments
  assert((cont != 0) && (parent != 0));

  //Add container to parent container
  parent->Add(cont);
}

void Add(HCParameter *param, HCContainer *parent, HCServer *srv)
{
  //Assert valid arguments
  assert((param != 0) && (parent != 0));

  //Add parameter to parent container
  parent->Add(param);

  //Add parameter to server if valid
  if(srv != 0)
    srv->Add(param);
}

static const HCBooleanEnum Boolenums[] =
{
  HCBooleanEnum(false, "Off"),
  HCBooleanEnum(true, "On"),
  HCBooleanEnum()
};

static const HCInt8Enum I8enums[] =
{
  HCInt8Enum(0, "Zero"),
  HCInt8Enum(1, "One"),
  HCInt8Enum(3, "Three"),
  HCInt8Enum(5, "Five"),
  HCInt8Enum(7, "Seven"),
  HCInt8Enum()
};

static const HCInt16Enum I16enums[] =
{
  HCInt16Enum(0, "Zero"),
  HCInt16Enum(1, "One"),
  HCInt16Enum(3, "Three"),
  HCInt16Enum(5, "Five"),
  HCInt16Enum(7, "Seven"),
  HCInt16Enum()
};

static const HCInt32Enum I32enums[] =
{
  HCInt32Enum(0, "Zero"),
  HCInt32Enum(1, "One"),
  HCInt32Enum(3, "Three"),
  HCInt32Enum(5, "Five"),
  HCInt32Enum(7, "Seven"),
  HCInt32Enum()
};

static const HCInt64Enum I64enums[] =
{
  HCInt64Enum(0, "Zero"),
  HCInt64Enum(1, "One"),
  HCInt64Enum(3, "Three"),
  HCInt64Enum(5, "Five"),
  HCInt64Enum(7, "Seven"),
  HCInt64Enum()
};

static const HCUns8Enum U8enums[] =
{
  HCUns8Enum(0, "Zero"),
  HCUns8Enum(1, "One"),
  HCUns8Enum(3, "Three"),
  HCUns8Enum(5, "Five"),
  HCUns8Enum(7, "Seven"),
  HCUns8Enum()
};

static const HCUns16Enum U16enums[] =
{
  HCUns16Enum(0, "Zero"),
  HCUns16Enum(1, "One"),
  HCUns16Enum(3, "Three"),
  HCUns16Enum(5, "Five"),
  HCUns16Enum(7, "Seven"),
  HCUns16Enum()
};

static const HCUns32Enum U32enums[] =
{
  HCUns32Enum(0, "Zero"),
  HCUns32Enum(1, "One"),
  HCUns32Enum(3, "Three"),
  HCUns32Enum(5, "Five"),
  HCUns32Enum(7, "Seven"),
  HCUns32Enum()
};

static const HCUns64Enum U64enums[] =
{
  HCUns64Enum(0, "Zero"),
  HCUns64Enum(1, "One"),
  HCUns64Enum(3, "Three"),
  HCUns64Enum(5, "Five"),
  HCUns64Enum(7, "Seven"),
  HCUns64Enum()
};

static const HCEIDEnum Eidenums[] =
{
  HCEIDEnum(0, "Zero"),
  HCEIDEnum(1, "One"),
  HCEIDEnum(2, "Two"),
  HCEIDEnum()
};

int main(int argc, char **argv)
{
  ScratchBool *scratchbool;
  ScratchString *scratchstr;
  ScratchFile *scratchfile;
  ScratchI8 *scratchi8;
  ScratchI16 *scratchi16;
  ScratchI32 *scratchi32;
  ScratchI64 *scratchi64;
  ScratchU8 *scratchu8;
  ScratchU16 *scratchu16;
  ScratchU32 *scratchu32;
  ScratchU64 *scratchu64;
  ScratchF32 *scratchf32;
  ScratchF64 *scratchf64;
  ScratchV2F32 *scratchv2f32;
  ScratchV2F64 *scratchv2f64;
  ScratchV3F32 *scratchv3f32;
  ScratchV3F64 *scratchv3f64;
  HCContainer *topcont;
  HCContainer *cont;
  Device *srvdev;
  TCPServer *tcpsrv;
  TLSServer *tlssrv;
  HCServer *srv;
  Device *qsrvdev;
  HCQServer *qsrv;
  HCConsole *hccons;
  struct Args args;

  //Set argument default values
  args.tcp = false;
  args.tls = false;
  args.port = 1500;
  args.qport = 5555;
  args.daemon = false;

  //Parse arguments
  if(!ParseOptions(argc, argv, &args))
    return -1;

  //Check for daemon mode
  if(args.daemon)
    if(daemon(1, 1) != 0)
      return -1;

  //Initialize TLS
  SSL_load_error_strings();	
  OpenSSL_add_ssl_algorithms();

  //Create scratch objects
  scratchbool = new ScratchBool(TABLE_SIZE, LIST_MAX_SIZE);
  scratchstr = new ScratchString(TABLE_SIZE, LIST_MAX_SIZE);
  scratchfile = new ScratchFile("scratchfile");
  scratchi8 = new ScratchI8(TABLE_SIZE, LIST_MAX_SIZE);
  scratchi16 = new ScratchI16(TABLE_SIZE, LIST_MAX_SIZE);
  scratchi32 = new ScratchI32(TABLE_SIZE, LIST_MAX_SIZE);
  scratchi64 = new ScratchI64(TABLE_SIZE, LIST_MAX_SIZE);
  scratchu8 = new ScratchU8(TABLE_SIZE, LIST_MAX_SIZE);
  scratchu16 = new ScratchU16(TABLE_SIZE, LIST_MAX_SIZE);
  scratchu32 = new ScratchU32(TABLE_SIZE, LIST_MAX_SIZE);
  scratchu64 = new ScratchU64(TABLE_SIZE, LIST_MAX_SIZE);
  scratchf32 = new ScratchF32(TABLE_SIZE, LIST_MAX_SIZE);
  scratchf64 = new ScratchF64(TABLE_SIZE, LIST_MAX_SIZE);
  scratchv2f32 = new ScratchV2F32(TABLE_SIZE);
  scratchv2f64 = new ScratchV2F64(TABLE_SIZE);
  scratchv3f32 = new ScratchV3F32(TABLE_SIZE);
  scratchv3f64 = new ScratchV3F64(TABLE_SIZE);

  //Create top container
  topcont = new HCContainer("");

  //Check protocol to use
  if(args.tcp)
  {
    //Create server device
    tcpsrv = new TCPServer(args.port);
    srvdev = new SLIPFramer(tcpsrv, 2000 + 2);
  }
  else if(args.tls)
  {
    //Create server device
    tlssrv = new TLSServer(args.port, "cert.pem", "key.pem", 0xB6FE1F4A); //CRC32 of "democosm:hcpass"
    srvdev = new SLIPFramer(tlssrv, 2000 + 2);
  }
  else
  {
    //Create server device
    srvdev = new UDPDevice(args.port);
  }

  //Create server
  srv = new HCServer(srvdev, topcont, "Scratch", __DATE__ " " __TIME__);

  //Add parameters
  cont = new HCContainer("bool");
  Add(cont, topcont);
  Add(new HCCall<ScratchBool>("print", scratchbool, &ScratchBool::Print), cont, srv);
  Add(new HCCallTable<ScratchBool>("printt", scratchbool, &ScratchBool::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchBool>("printtee", scratchbool, &ScratchBool::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCBool<ScratchBool>("val", scratchbool, &ScratchBool::Get, &ScratchBool::Set), cont, srv);
  Add(new HCBool<ScratchBool>("valro", scratchbool, &ScratchBool::Get, 0), cont, srv);
  Add(new HCBool<ScratchBool>("valwo", scratchbool, 0, &ScratchBool::Set), cont, srv);
  Add(new HCBoolS<ScratchBool>("vale", scratchbool, &ScratchBool::Get, &ScratchBool::Set, Boolenums), cont, srv);
  Add(new HCBoolTable<ScratchBool>("table", scratchbool, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tablero", scratchbool, &ScratchBool::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tablewo", scratchbool, 0, &ScratchBool::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tableve", scratchbool, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE, 0, Boolenums), cont, srv);
  Add(new HCBoolTableS<ScratchBool>("tableeeve", scratchbool, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE, Eidenums, Boolenums), cont, srv);

  cont = new HCContainer("str");
  Add(cont, topcont);
  Add(new HCCall<ScratchString>("print", scratchstr, &ScratchString::Print), cont, srv);
  Add(new HCCallTable<ScratchString>("printt", scratchstr, &ScratchString::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchString>("printtee", scratchstr, &ScratchString::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCStrS<ScratchString>("val", scratchstr, &ScratchString::Get, &ScratchString::Set), cont, srv);
  Add(new HCStr<ScratchString>("valro", scratchstr, &ScratchString::Get, 0), cont, srv);
  Add(new HCStr<ScratchString>("valwo", scratchstr, 0, &ScratchString::Set), cont, srv);
  Add(new HCStrTable<ScratchString>("table", scratchstr, &ScratchString::TableGet, &ScratchString::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCStrTable<ScratchString>("tablero", scratchstr, &ScratchString::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCStrTable<ScratchString>("tablewo", scratchstr, 0, &ScratchString::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCStrTableS<ScratchString>("tableee", scratchstr, &ScratchString::TableGet, &ScratchString::TableSet, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCStrListS<ScratchString>("list", scratchstr, &ScratchString::ListGet, &ScratchString::ListAdd, &ScratchString::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCStrList<ScratchString>("listro", scratchstr, &ScratchString::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCStrList<ScratchString>("listwo", scratchstr, 0, &ScratchString::ListAdd, &ScratchString::ListSub, LIST_MAX_SIZE), cont, srv);

  cont = new HCContainer("file");
  Add(cont, topcont);
  Add(new HCFile<ScratchFile>("scratchfile", scratchfile, &ScratchFile::Read, &ScratchFile::Write), cont, srv);

  cont = new HCContainer("i8");
  Add(cont, topcont);
  Add(new HCCall<ScratchI8>("print", scratchi8, &ScratchI8::Print), cont, srv);
  Add(new HCCallTable<ScratchI8>("printt", scratchi8, &ScratchI8::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchI8>("printtee", scratchi8, &ScratchI8::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt8<ScratchI8>("val", scratchi8, &ScratchI8::Get, &ScratchI8::Set), cont, srv);
  Add(new HCInt8<ScratchI8>("valro", scratchi8, &ScratchI8::Get, 0), cont, srv);
  Add(new HCInt8<ScratchI8>("valwo", scratchi8, 0, &ScratchI8::Set), cont, srv);
  Add(new HCInt8S<ScratchI8>("vale", scratchi8, &ScratchI8::Get, &ScratchI8::Set, I8enums), cont, srv);
  Add(new HCInt8Table<ScratchI8>("table", scratchi8, &ScratchI8::TableGet, &ScratchI8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchI8>("tablero", scratchi8, &ScratchI8::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchI8>("tablewo", scratchi8, 0, &ScratchI8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchI8>("tableve", scratchi8, &ScratchI8::TableGet, &ScratchI8::TableSet, TABLE_SIZE, 0, I8enums), cont, srv);
  Add(new HCInt8TableS<ScratchI8>("tableeeve", scratchi8, &ScratchI8::TableGet, &ScratchI8::TableSet, TABLE_SIZE, Eidenums, I8enums), cont, srv);
  Add(new HCInt8List<ScratchI8>("list", scratchi8, &ScratchI8::ListGet, &ScratchI8::ListAdd, &ScratchI8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8List<ScratchI8>("listro", scratchi8, &ScratchI8::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8List<ScratchI8>("listwo", scratchi8, 0, &ScratchI8::ListAdd, &ScratchI8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8ListS<ScratchI8>("listve", scratchi8, &ScratchI8::ListGet, &ScratchI8::ListAdd, &ScratchI8::ListSub, LIST_MAX_SIZE, I8enums), cont, srv);

  cont = new HCContainer("i16");
  Add(cont, topcont);
  Add(new HCCall<ScratchI16>("print", scratchi16, &ScratchI16::Print), cont, srv);
  Add(new HCCallTable<ScratchI16>("printt", scratchi16, &ScratchI16::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchI16>("printtee", scratchi16, &ScratchI16::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt16<ScratchI16>("val", scratchi16, &ScratchI16::Get, &ScratchI16::Set), cont, srv);
  Add(new HCInt16<ScratchI16>("valro", scratchi16, &ScratchI16::Get, 0), cont, srv);
  Add(new HCInt16<ScratchI16>("valwo", scratchi16, 0, &ScratchI16::Set), cont, srv);
  Add(new HCInt16S<ScratchI16>("vale", scratchi16, &ScratchI16::Get, &ScratchI16::Set, I16enums), cont, srv);
  Add(new HCInt16Table<ScratchI16>("table", scratchi16, &ScratchI16::TableGet, &ScratchI16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchI16>("tablero", scratchi16, &ScratchI16::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchI16>("tablewo", scratchi16, 0, &ScratchI16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchI16>("tableve", scratchi16, &ScratchI16::TableGet, &ScratchI16::TableSet, TABLE_SIZE, 0, I16enums), cont, srv);
  Add(new HCInt16TableS<ScratchI16>("tableeeve", scratchi16, &ScratchI16::TableGet, &ScratchI16::TableSet, TABLE_SIZE, Eidenums, I16enums), cont, srv);
  Add(new HCInt16List<ScratchI16>("list", scratchi16, &ScratchI16::ListGet, &ScratchI16::ListAdd, &ScratchI16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16List<ScratchI16>("listro", scratchi16, &ScratchI16::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16List<ScratchI16>("listwo", scratchi16, 0, &ScratchI16::ListAdd, &ScratchI16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16ListS<ScratchI16>("listve", scratchi16, &ScratchI16::ListGet, &ScratchI16::ListAdd, &ScratchI16::ListSub, LIST_MAX_SIZE, I16enums), cont, srv);

  cont = new HCContainer("i32");
  Add(cont, topcont);
  Add(new HCCall<ScratchI32>("print", scratchi32, &ScratchI32::Print), cont, srv);
  Add(new HCCallTable<ScratchI32>("printt", scratchi32, &ScratchI32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchI32>("printtee", scratchi32, &ScratchI32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt32<ScratchI32>("val", scratchi32, &ScratchI32::Get, &ScratchI32::Set), cont, srv);
  Add(new HCInt32<ScratchI32>("valro", scratchi32, &ScratchI32::Get, 0), cont, srv);
  Add(new HCInt32<ScratchI32>("valwo", scratchi32, 0, &ScratchI32::Set), cont, srv);
  Add(new HCInt32S<ScratchI32>("vale", scratchi32, &ScratchI32::Get, &ScratchI32::Set, I32enums), cont, srv);
  Add(new HCInt32Table<ScratchI32>("table", scratchi32, &ScratchI32::TableGet, &ScratchI32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchI32>("tablero", scratchi32, &ScratchI32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchI32>("tablewo", scratchi32, 0, &ScratchI32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchI32>("tableve", scratchi32, &ScratchI32::TableGet, &ScratchI32::TableSet, TABLE_SIZE, 0, I32enums), cont, srv);
  Add(new HCInt32TableS<ScratchI32>("tableeeve", scratchi32, &ScratchI32::TableGet, &ScratchI32::TableSet, TABLE_SIZE, Eidenums, I32enums), cont, srv);
  Add(new HCInt32List<ScratchI32>("list", scratchi32, &ScratchI32::ListGet, &ScratchI32::ListAdd, &ScratchI32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32List<ScratchI32>("listro", scratchi32, &ScratchI32::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32List<ScratchI32>("listwo", scratchi32, 0, &ScratchI32::ListAdd, &ScratchI32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32ListS<ScratchI32>("listve", scratchi32, &ScratchI32::ListGet, &ScratchI32::ListAdd, &ScratchI32::ListSub, LIST_MAX_SIZE, I32enums), cont, srv);

  cont = new HCContainer("i64");
  Add(cont, topcont);
  Add(new HCCall<ScratchI64>("print", scratchi64, &ScratchI64::Print), cont, srv);
  Add(new HCCallTable<ScratchI64>("printt", scratchi64, &ScratchI64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchI64>("printtee", scratchi64, &ScratchI64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt64<ScratchI64>("val", scratchi64, &ScratchI64::Get, &ScratchI64::Set), cont, srv);
  Add(new HCInt64<ScratchI64>("valro", scratchi64, &ScratchI64::Get, 0), cont, srv);
  Add(new HCInt64<ScratchI64>("valwo", scratchi64, 0, &ScratchI64::Set), cont, srv);
  Add(new HCInt64S<ScratchI64>("vale", scratchi64, &ScratchI64::Get, &ScratchI64::Set, I64enums), cont, srv);
  Add(new HCInt64Table<ScratchI64>("table", scratchi64, &ScratchI64::TableGet, &ScratchI64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchI64>("tablero", scratchi64, &ScratchI64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchI64>("tablewo", scratchi64, 0, &ScratchI64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchI64>("tableve", scratchi64, &ScratchI64::TableGet, &ScratchI64::TableSet, TABLE_SIZE, 0, I64enums), cont, srv);
  Add(new HCInt64TableS<ScratchI64>("tableeeve", scratchi64, &ScratchI64::TableGet, &ScratchI64::TableSet, TABLE_SIZE, Eidenums, I64enums), cont, srv);
  Add(new HCInt64List<ScratchI64>("list", scratchi64, &ScratchI64::ListGet, &ScratchI64::ListAdd, &ScratchI64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64List<ScratchI64>("listro", scratchi64, &ScratchI64::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64List<ScratchI64>("listwo", scratchi64, 0, &ScratchI64::ListAdd, &ScratchI64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64ListS<ScratchI64>("listve", scratchi64, &ScratchI64::ListGet, &ScratchI64::ListAdd, &ScratchI64::ListSub, LIST_MAX_SIZE, I64enums), cont, srv);

  cont = new HCContainer("u8");
  Add(cont, topcont);
  Add(new HCCall<ScratchU8>("print", scratchu8, &ScratchU8::Print), cont, srv);
  Add(new HCCallTable<ScratchU8>("printt", scratchu8, &ScratchU8::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU8>("printtee", scratchu8, &ScratchU8::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns8<ScratchU8>("val", scratchu8, &ScratchU8::Get, &ScratchU8::Set), cont, srv);
  Add(new HCUns8<ScratchU8>("valro", scratchu8, &ScratchU8::Get, 0), cont, srv);
  Add(new HCUns8<ScratchU8>("valwo", scratchu8, 0, &ScratchU8::Set), cont, srv);
  Add(new HCUns8S<ScratchU8>("vale", scratchu8, &ScratchU8::Get, &ScratchU8::Set, U8enums), cont, srv);
  Add(new HCUns8Table<ScratchU8>("table", scratchu8, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tablero", scratchu8, &ScratchU8::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tablewo", scratchu8, 0, &ScratchU8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tableve", scratchu8, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE, 0, U8enums), cont, srv);
  Add(new HCUns8TableS<ScratchU8>("tableeeve", scratchu8, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE, Eidenums, U8enums), cont, srv);
  Add(new HCUns8List<ScratchU8>("list", scratchu8, &ScratchU8::ListGet, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8List<ScratchU8>("listro", scratchu8, &ScratchU8::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8List<ScratchU8>("listwo", scratchu8, 0, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8ListS<ScratchU8>("listve", scratchu8, &ScratchU8::ListGet, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE, U8enums), cont, srv);

  cont = new HCContainer("u16");
  Add(cont, topcont);
  Add(new HCCall<ScratchU16>("print", scratchu16, &ScratchU16::Print), cont, srv);
  Add(new HCCallTable<ScratchU16>("printt", scratchu16, &ScratchU16::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU16>("printtee", scratchu16, &ScratchU16::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns16<ScratchU16>("val", scratchu16, &ScratchU16::Get, &ScratchU16::Set), cont, srv);
  Add(new HCUns16<ScratchU16>("valro", scratchu16, &ScratchU16::Get, 0), cont, srv);
  Add(new HCUns16<ScratchU16>("valwo", scratchu16, 0, &ScratchU16::Set), cont, srv);
  Add(new HCUns16S<ScratchU16>("vale", scratchu16, &ScratchU16::Get, &ScratchU16::Set, U16enums), cont, srv);
  Add(new HCUns16Table<ScratchU16>("table", scratchu16, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tablero", scratchu16, &ScratchU16::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tablewo", scratchu16, 0, &ScratchU16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tableve", scratchu16, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE, 0, U16enums), cont, srv);
  Add(new HCUns16TableS<ScratchU16>("tableeeve", scratchu16, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE, Eidenums, U16enums), cont, srv);
  Add(new HCUns16List<ScratchU16>("list", scratchu16, &ScratchU16::ListGet, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16List<ScratchU16>("listro", scratchu16, &ScratchU16::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16List<ScratchU16>("listwo", scratchu16, 0, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16ListS<ScratchU16>("listve", scratchu16, &ScratchU16::ListGet, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE, U16enums), cont, srv);

  cont = new HCContainer("u32");
  Add(cont, topcont);
  Add(new HCCall<ScratchU32>("print", scratchu32, &ScratchU32::Print), cont, srv);
  Add(new HCCallTable<ScratchU32>("printt", scratchu32, &ScratchU32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU32>("printtee", scratchu32, &ScratchU32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns32<ScratchU32>("val", scratchu32, &ScratchU32::Get, &ScratchU32::Set), cont, srv);
  Add(new HCUns32<ScratchU32>("valro", scratchu32, &ScratchU32::Get, 0), cont, srv);
  Add(new HCUns32<ScratchU32>("valwo", scratchu32, 0, &ScratchU32::Set), cont, srv);
  Add(new HCUns32S<ScratchU32>("vale", scratchu32, &ScratchU32::Get, &ScratchU32::Set, U32enums), cont, srv);
  Add(new HCUns32Table<ScratchU32>("table", scratchu32, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tablero", scratchu32, &ScratchU32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tablewo", scratchu32, 0, &ScratchU32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tableve", scratchu32, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE, 0, U32enums), cont, srv);
  Add(new HCUns32TableS<ScratchU32>("tableeeve", scratchu32, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE, Eidenums, U32enums), cont, srv);
  Add(new HCUns32List<ScratchU32>("list", scratchu32, &ScratchU32::ListGet, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32List<ScratchU32>("listro", scratchu32, &ScratchU32::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32List<ScratchU32>("listwo", scratchu32, 0, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32ListS<ScratchU32>("listve", scratchu32, &ScratchU32::ListGet, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE, U32enums), cont, srv);

  cont = new HCContainer("u64");
  Add(cont, topcont);
  Add(new HCCall<ScratchU64>("print", scratchu64, &ScratchU64::Print), cont, srv);
  Add(new HCCallTable<ScratchU64>("printt", scratchu64, &ScratchU64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU64>("printtee", scratchu64, &ScratchU64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns64<ScratchU64>("val", scratchu64, &ScratchU64::Get, &ScratchU64::Set), cont, srv);
  Add(new HCUns64<ScratchU64>("valro", scratchu64, &ScratchU64::Get, 0), cont, srv);
  Add(new HCUns64<ScratchU64>("valwo", scratchu64, 0, &ScratchU64::Set), cont, srv);
  Add(new HCUns64S<ScratchU64>("vale", scratchu64, &ScratchU64::Get, &ScratchU64::Set, U64enums), cont, srv);
  Add(new HCUns64TableS<ScratchU64>("table", scratchu64, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tablero", scratchu64, &ScratchU64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tablewo", scratchu64, 0, &ScratchU64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tableve", scratchu64, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE, 0, U64enums), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tableeeve", scratchu64, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE, Eidenums, U64enums), cont, srv);
  Add(new HCUns64List<ScratchU64>("list", scratchu64, &ScratchU64::ListGet, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64List<ScratchU64>("listro", scratchu64, &ScratchU64::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64List<ScratchU64>("listwo", scratchu64, 0, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64ListS<ScratchU64>("listve", scratchu64, &ScratchU64::ListGet, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE, U64enums), cont, srv);

  cont = new HCContainer("f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchF32>("print", scratchf32, &ScratchF32::Print), cont, srv);
  Add(new HCCallTable<ScratchF32>("printt", scratchf32, &ScratchF32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchF32>("printtee", scratchf32, &ScratchF32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCFlt32S<ScratchF32>("val", scratchf32, &ScratchF32::Get, &ScratchF32::Set), cont, srv);
  Add(new HCFlt32<ScratchF32>("valro", scratchf32, &ScratchF32::Get, 0), cont, srv);
  Add(new HCFlt32<ScratchF32>("valwo", scratchf32, 0, &ScratchF32::Set), cont, srv);
  Add(new HCFlt32Table<ScratchF32>("table", scratchf32, &ScratchF32::TableGet, &ScratchF32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt32Table<ScratchF32>("tablero", scratchf32, &ScratchF32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCFlt32Table<ScratchF32>("tablewo", scratchf32, 0, &ScratchF32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt32TableS<ScratchF32>("tableee", scratchf32, &ScratchF32::TableGet, &ScratchF32::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchF64>("print", scratchf64, &ScratchF64::Print), cont, srv);
  Add(new HCCallTable<ScratchF64>("printt", scratchf64, &ScratchF64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchF64>("printtee", scratchf64, &ScratchF64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCFlt64S<ScratchF64>("val", scratchf64, &ScratchF64::Get, &ScratchF64::Set), cont, srv);
  Add(new HCFlt64<ScratchF64>("valro", scratchf64, &ScratchF64::Get, 0), cont, srv);
  Add(new HCFlt64<ScratchF64>("valwo", scratchf64, 0, &ScratchF64::Set), cont, srv);
  Add(new HCFlt64Table<ScratchF64>("table", scratchf64, &ScratchF64::TableGet, &ScratchF64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt64Table<ScratchF64>("tablero", scratchf64, &ScratchF64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCFlt64Table<ScratchF64>("tablewo", scratchf64, 0, &ScratchF64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt64TableS<ScratchF64>("tableee", scratchf64, &ScratchF64::TableGet, &ScratchF64::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v2f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchV2F32>("print", scratchv2f32, &ScratchV2F32::Print), cont, srv);
  Add(new HCCallTable<ScratchV2F32>("printt", scratchv2f32, &ScratchV2F32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV2F32>("printtee", scratchv2f32, &ScratchV2F32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV2F32S<ScratchV2F32>("val", scratchv2f32, &ScratchV2F32::Get, &ScratchV2F32::Set), cont, srv);
  Add(new HCV2F32<ScratchV2F32>("valro", scratchv2f32, &ScratchV2F32::Get, 0), cont, srv);
  Add(new HCV2F32<ScratchV2F32>("valwo", scratchv2f32, 0, &ScratchV2F32::Set), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("table", scratchv2f32, &ScratchV2F32::TableGet, &ScratchV2F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("tablero", scratchv2f32, &ScratchV2F32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("tablewo", scratchv2f32, 0, &ScratchV2F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F32TableS<ScratchV2F32>("tableee", scratchv2f32, &ScratchV2F32::TableGet, &ScratchV2F32::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v2f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchV2F64>("print", scratchv2f64, &ScratchV2F64::Print), cont, srv);
  Add(new HCCallTable<ScratchV2F64>("printt", scratchv2f64, &ScratchV2F64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV2F64>("printtee", scratchv2f64, &ScratchV2F64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV2F64S<ScratchV2F64>("val", scratchv2f64, &ScratchV2F64::Get, &ScratchV2F64::Set), cont, srv);
  Add(new HCV2F64<ScratchV2F64>("valro", scratchv2f64, &ScratchV2F64::Get, 0), cont, srv);
  Add(new HCV2F64<ScratchV2F64>("valwo", scratchv2f64, 0, &ScratchV2F64::Set), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("table", scratchv2f64, &ScratchV2F64::TableGet, &ScratchV2F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("tablero", scratchv2f64, &ScratchV2F64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("tablewo", scratchv2f64, 0, &ScratchV2F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F64TableS<ScratchV2F64>("tableee", scratchv2f64, &ScratchV2F64::TableGet, &ScratchV2F64::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v3f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchV3F32>("print", scratchv3f32, &ScratchV3F32::Print), cont, srv);
  Add(new HCCallTable<ScratchV3F32>("printt", scratchv3f32, &ScratchV3F32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV3F32>("printtee", scratchv3f32, &ScratchV3F32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV3F32S<ScratchV3F32>("val", scratchv3f32, &ScratchV3F32::Get, &ScratchV3F32::Set), cont, srv);
  Add(new HCV3F32<ScratchV3F32>("valro", scratchv3f32, &ScratchV3F32::Get, 0), cont, srv);
  Add(new HCV3F32<ScratchV3F32>("valwo", scratchv3f32, 0, &ScratchV3F32::Set), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("table", scratchv3f32, &ScratchV3F32::TableGet, &ScratchV3F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("tablero", scratchv3f32, &ScratchV3F32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("tablewo", scratchv3f32, 0, &ScratchV3F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F32TableS<ScratchV3F32>("tableee", scratchv3f32, &ScratchV3F32::TableGet, &ScratchV3F32::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v3f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchV3F64>("print", scratchv3f64, &ScratchV3F64::Print), cont, srv);
  Add(new HCCallTable<ScratchV3F64>("printt", scratchv3f64, &ScratchV3F64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV3F64>("printtee", scratchv3f64, &ScratchV3F64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV3F64S<ScratchV3F64>("val", scratchv3f64, &ScratchV3F64::Get, &ScratchV3F64::Set), cont, srv);
  Add(new HCV3F64<ScratchV3F64>("valro", scratchv3f64, &ScratchV3F64::Get, 0), cont, srv);
  Add(new HCV3F64<ScratchV3F64>("valwo", scratchv3f64, 0, &ScratchV3F64::Set), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("table", scratchv3f64, &ScratchV3F64::TableGet, &ScratchV3F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("tablero", scratchv3f64, &ScratchV3F64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("tablewo", scratchv3f64, 0, &ScratchV3F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F64TableS<ScratchV3F64>("tableee", scratchv3f64, &ScratchV3F64::TableGet, &ScratchV3F64::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  //Start HC server
  srv->Start();

  //Create query server
  qsrvdev = new UDPDevice(args.qport);
  qsrv = new HCQServer(qsrvdev, topcont);

  //Just loop if in daemon mode otherwise run console
  if(args.daemon)
  {
    while(true)
      ThreadSleep(1000000);
  }
  else
  {
    //Create HC console
    hccons = new HCConsole(topcont);

    //Run console in context of this thread
    hccons->Run();

    //Cleanup
    delete hccons;
  }

  //Cleanup
  delete qsrv;
  delete qsrvdev;
  delete srv;
  delete srvdev;
  delete topcont;
  delete scratchbool;
  delete scratchstr;
  delete scratchfile;
  delete scratchi8;
  delete scratchi16;
  delete scratchi32;
  delete scratchi64;
  delete scratchu8;
  delete scratchu16;
  delete scratchu32;
  delete scratchu64;
  delete scratchf32;
  delete scratchf64;
  delete scratchv2f32;
  delete scratchv2f64;
  delete scratchv3f32;
  delete scratchv3f64;

  //Cleanup TLS
  EVP_cleanup();

  //Return success
  return 0;
}
