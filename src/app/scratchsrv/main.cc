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
static const char *Appversion = "1.1.0 " __DATE__ " " __TIME__;
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

static const HCInt8Enum S8enums[] =
{
  HCInt8Enum(0, "Zero"),
  HCInt8Enum(1, "One"),
  HCInt8Enum(3, "Three"),
  HCInt8Enum(5, "Five"),
  HCInt8Enum(7, "Seven"),
  HCInt8Enum()
};

static const HCInt16Enum S16enums[] =
{
  HCInt16Enum(0, "Zero"),
  HCInt16Enum(1, "One"),
  HCInt16Enum(3, "Three"),
  HCInt16Enum(5, "Five"),
  HCInt16Enum(7, "Seven"),
  HCInt16Enum()
};

static const HCInt32Enum S32enums[] =
{
  HCInt32Enum(0, "Zero"),
  HCInt32Enum(1, "One"),
  HCInt32Enum(3, "Three"),
  HCInt32Enum(5, "Five"),
  HCInt32Enum(7, "Seven"),
  HCInt32Enum()
};

static const HCInt64Enum S64enums[] =
{
  HCInt64Enum(0, "Zero"),
  HCInt64Enum(1, "One"),
  HCInt64Enum(3, "Three"),
  HCInt64Enum(5, "Five"),
  HCInt64Enum(7, "Seven"),
  HCInt64Enum()
};

static const HCBooleanEnum Boolenums[] =
{
  HCBooleanEnum(false, "Off"),
  HCBooleanEnum(true, "On"),
  HCBooleanEnum()
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
  ScratchBool *boolscratch;
  ScratchU8 *u8scratch;
  ScratchU16 *u16scratch;
  ScratchU32 *u32scratch;
  ScratchU64 *u64scratch;
  ScratchS8 *s8scratch;
  ScratchS16 *s16scratch;
  ScratchS32 *s32scratch;
  ScratchS64 *s64scratch;
  ScratchFloat *floatscratch;
  ScratchDouble *doublescratch;
  ScratchString *stringscratch;
  ScratchV2F32 *v2f32scratch;
  ScratchV2F64 *v2f64scratch;
  ScratchV3F32 *v3f32scratch;
  ScratchV3F64 *v3f64scratch;
  ScratchFile *filescratch;
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
  boolscratch = new ScratchBool(TABLE_SIZE, LIST_MAX_SIZE);
  u8scratch = new ScratchU8(TABLE_SIZE, LIST_MAX_SIZE);
  u16scratch = new ScratchU16(TABLE_SIZE, LIST_MAX_SIZE);
  u32scratch = new ScratchU32(TABLE_SIZE, LIST_MAX_SIZE);
  u64scratch = new ScratchU64(TABLE_SIZE, LIST_MAX_SIZE);
  s8scratch = new ScratchS8(TABLE_SIZE, LIST_MAX_SIZE);
  s16scratch = new ScratchS16(TABLE_SIZE, LIST_MAX_SIZE);
  s32scratch = new ScratchS32(TABLE_SIZE, LIST_MAX_SIZE);
  s64scratch = new ScratchS64(TABLE_SIZE, LIST_MAX_SIZE);
  floatscratch = new ScratchFloat(TABLE_SIZE, LIST_MAX_SIZE);
  doublescratch = new ScratchDouble(TABLE_SIZE, LIST_MAX_SIZE);
  stringscratch = new ScratchString(TABLE_SIZE, LIST_MAX_SIZE);
  v2f32scratch = new ScratchV2F32(TABLE_SIZE);
  v2f64scratch = new ScratchV2F64(TABLE_SIZE);
  v3f32scratch = new ScratchV3F32(TABLE_SIZE);
  v3f64scratch = new ScratchV3F64(TABLE_SIZE);
  filescratch = new ScratchFile("scratchfile");

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
  cont = new HCContainer("i8");
  Add(cont, topcont);
  Add(new HCCall<ScratchS8>("print", s8scratch, &ScratchS8::Print), cont, srv);
  Add(new HCCallTable<ScratchS8>("printt", s8scratch, &ScratchS8::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchS8>("printtee", s8scratch, &ScratchS8::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt8<ScratchS8>("val", s8scratch, &ScratchS8::Get, &ScratchS8::Set), cont, srv);
  Add(new HCInt8<ScratchS8>("valro", s8scratch, &ScratchS8::Get, 0), cont, srv);
  Add(new HCInt8<ScratchS8>("valwo", s8scratch, 0, &ScratchS8::Set), cont, srv);
  Add(new HCInt8S<ScratchS8>("vale", s8scratch, &ScratchS8::Get, &ScratchS8::Set, S8enums), cont, srv);
  Add(new HCInt8Table<ScratchS8>("table", s8scratch, &ScratchS8::TableGet, &ScratchS8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchS8>("tablero", s8scratch, &ScratchS8::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchS8>("tablewo", s8scratch, 0, &ScratchS8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt8Table<ScratchS8>("tableve", s8scratch, &ScratchS8::TableGet, &ScratchS8::TableSet, TABLE_SIZE, 0, S8enums), cont, srv);
  Add(new HCInt8TableS<ScratchS8>("tableeeve", s8scratch, &ScratchS8::TableGet, &ScratchS8::TableSet, TABLE_SIZE, Eidenums, S8enums), cont, srv);
  Add(new HCInt8List<ScratchS8>("list", s8scratch, &ScratchS8::ListGet, &ScratchS8::ListAdd, &ScratchS8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8List<ScratchS8>("listro", s8scratch, &ScratchS8::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8List<ScratchS8>("listwo", s8scratch, 0, &ScratchS8::ListAdd, &ScratchS8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt8ListS<ScratchS8>("listve", s8scratch, &ScratchS8::ListGet, &ScratchS8::ListAdd, &ScratchS8::ListSub, LIST_MAX_SIZE, S8enums), cont, srv);

  cont = new HCContainer("i16");
  Add(cont, topcont);
  Add(new HCCall<ScratchS16>("print", s16scratch, &ScratchS16::Print), cont, srv);
  Add(new HCCallTable<ScratchS16>("printt", s16scratch, &ScratchS16::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchS16>("printtee", s16scratch, &ScratchS16::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt16<ScratchS16>("val", s16scratch, &ScratchS16::Get, &ScratchS16::Set), cont, srv);
  Add(new HCInt16<ScratchS16>("valro", s16scratch, &ScratchS16::Get, 0), cont, srv);
  Add(new HCInt16<ScratchS16>("valwo", s16scratch, 0, &ScratchS16::Set), cont, srv);
  Add(new HCInt16S<ScratchS16>("vale", s16scratch, &ScratchS16::Get, &ScratchS16::Set, S16enums), cont, srv);
  Add(new HCInt16Table<ScratchS16>("table", s16scratch, &ScratchS16::TableGet, &ScratchS16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchS16>("tablero", s16scratch, &ScratchS16::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchS16>("tablewo", s16scratch, 0, &ScratchS16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt16Table<ScratchS16>("tableve", s16scratch, &ScratchS16::TableGet, &ScratchS16::TableSet, TABLE_SIZE, 0, S16enums), cont, srv);
  Add(new HCInt16TableS<ScratchS16>("tableeeve", s16scratch, &ScratchS16::TableGet, &ScratchS16::TableSet, TABLE_SIZE, Eidenums, S16enums), cont, srv);
  Add(new HCInt16List<ScratchS16>("list", s16scratch, &ScratchS16::ListGet, &ScratchS16::ListAdd, &ScratchS16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16List<ScratchS16>("listro", s16scratch, &ScratchS16::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16List<ScratchS16>("listwo", s16scratch, 0, &ScratchS16::ListAdd, &ScratchS16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt16ListS<ScratchS16>("listve", s16scratch, &ScratchS16::ListGet, &ScratchS16::ListAdd, &ScratchS16::ListSub, LIST_MAX_SIZE, S16enums), cont, srv);

  cont = new HCContainer("i32");
  Add(cont, topcont);
  Add(new HCCall<ScratchS32>("print", s32scratch, &ScratchS32::Print), cont, srv);
  Add(new HCCallTable<ScratchS32>("printt", s32scratch, &ScratchS32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchS32>("printtee", s32scratch, &ScratchS32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt32<ScratchS32>("val", s32scratch, &ScratchS32::Get, &ScratchS32::Set), cont, srv);
  Add(new HCInt32<ScratchS32>("valro", s32scratch, &ScratchS32::Get, 0), cont, srv);
  Add(new HCInt32<ScratchS32>("valwo", s32scratch, 0, &ScratchS32::Set), cont, srv);
  Add(new HCInt32S<ScratchS32>("vale", s32scratch, &ScratchS32::Get, &ScratchS32::Set, S32enums), cont, srv);
  Add(new HCInt32Table<ScratchS32>("table", s32scratch, &ScratchS32::TableGet, &ScratchS32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchS32>("tablero", s32scratch, &ScratchS32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchS32>("tablewo", s32scratch, 0, &ScratchS32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt32Table<ScratchS32>("tableve", s32scratch, &ScratchS32::TableGet, &ScratchS32::TableSet, TABLE_SIZE, 0, S32enums), cont, srv);
  Add(new HCInt32TableS<ScratchS32>("tableeeve", s32scratch, &ScratchS32::TableGet, &ScratchS32::TableSet, TABLE_SIZE, Eidenums, S32enums), cont, srv);
  Add(new HCInt32List<ScratchS32>("list", s32scratch, &ScratchS32::ListGet, &ScratchS32::ListAdd, &ScratchS32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32List<ScratchS32>("listro", s32scratch, &ScratchS32::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32List<ScratchS32>("listwo", s32scratch, 0, &ScratchS32::ListAdd, &ScratchS32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt32ListS<ScratchS32>("listve", s32scratch, &ScratchS32::ListGet, &ScratchS32::ListAdd, &ScratchS32::ListSub, LIST_MAX_SIZE, S32enums), cont, srv);

  cont = new HCContainer("i64");
  Add(cont, topcont);
  Add(new HCCall<ScratchS64>("print", s64scratch, &ScratchS64::Print), cont, srv);
  Add(new HCCallTable<ScratchS64>("printt", s64scratch, &ScratchS64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchS64>("printtee", s64scratch, &ScratchS64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCInt64<ScratchS64>("val", s64scratch, &ScratchS64::Get, &ScratchS64::Set), cont, srv);
  Add(new HCInt64<ScratchS64>("valro", s64scratch, &ScratchS64::Get, 0), cont, srv);
  Add(new HCInt64<ScratchS64>("valwo", s64scratch, 0, &ScratchS64::Set), cont, srv);
  Add(new HCInt64S<ScratchS64>("vale", s64scratch, &ScratchS64::Get, &ScratchS64::Set, S64enums), cont, srv);
  Add(new HCInt64Table<ScratchS64>("table", s64scratch, &ScratchS64::TableGet, &ScratchS64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchS64>("tablero", s64scratch, &ScratchS64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchS64>("tablewo", s64scratch, 0, &ScratchS64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCInt64Table<ScratchS64>("tableve", s64scratch, &ScratchS64::TableGet, &ScratchS64::TableSet, TABLE_SIZE, 0, S64enums), cont, srv);
  Add(new HCInt64TableS<ScratchS64>("tableeeve", s64scratch, &ScratchS64::TableGet, &ScratchS64::TableSet, TABLE_SIZE, Eidenums, S64enums), cont, srv);
  Add(new HCInt64List<ScratchS64>("list", s64scratch, &ScratchS64::ListGet, &ScratchS64::ListAdd, &ScratchS64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64List<ScratchS64>("listro", s64scratch, &ScratchS64::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64List<ScratchS64>("listwo", s64scratch, 0, &ScratchS64::ListAdd, &ScratchS64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCInt64ListS<ScratchS64>("listve", s64scratch, &ScratchS64::ListGet, &ScratchS64::ListAdd, &ScratchS64::ListSub, LIST_MAX_SIZE, S64enums), cont, srv);

  cont = new HCContainer("u8");
  Add(cont, topcont);
  Add(new HCCall<ScratchU8>("print", u8scratch, &ScratchU8::Print), cont, srv);
  Add(new HCCallTable<ScratchU8>("printt", u8scratch, &ScratchU8::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU8>("printtee", u8scratch, &ScratchU8::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns8<ScratchU8>("val", u8scratch, &ScratchU8::Get, &ScratchU8::Set), cont, srv);
  Add(new HCUns8<ScratchU8>("valro", u8scratch, &ScratchU8::Get, 0), cont, srv);
  Add(new HCUns8<ScratchU8>("valwo", u8scratch, 0, &ScratchU8::Set), cont, srv);
  Add(new HCUns8S<ScratchU8>("vale", u8scratch, &ScratchU8::Get, &ScratchU8::Set, U8enums), cont, srv);
  Add(new HCUns8Table<ScratchU8>("table", u8scratch, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tablero", u8scratch, &ScratchU8::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tablewo", u8scratch, 0, &ScratchU8::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns8Table<ScratchU8>("tableve", u8scratch, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE, 0, U8enums), cont, srv);
  Add(new HCUns8TableS<ScratchU8>("tableeeve", u8scratch, &ScratchU8::TableGet, &ScratchU8::TableSet, TABLE_SIZE, Eidenums, U8enums), cont, srv);
  Add(new HCUns8List<ScratchU8>("list", u8scratch, &ScratchU8::ListGet, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8List<ScratchU8>("listro", u8scratch, &ScratchU8::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8List<ScratchU8>("listwo", u8scratch, 0, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns8ListS<ScratchU8>("listve", u8scratch, &ScratchU8::ListGet, &ScratchU8::ListAdd, &ScratchU8::ListSub, LIST_MAX_SIZE, U8enums), cont, srv);

  cont = new HCContainer("u16");
  Add(cont, topcont);
  Add(new HCCall<ScratchU16>("print", u16scratch, &ScratchU16::Print), cont, srv);
  Add(new HCCallTable<ScratchU16>("printt", u16scratch, &ScratchU16::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU16>("printtee", u16scratch, &ScratchU16::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns16<ScratchU16>("val", u16scratch, &ScratchU16::Get, &ScratchU16::Set), cont, srv);
  Add(new HCUns16<ScratchU16>("valro", u16scratch, &ScratchU16::Get, 0), cont, srv);
  Add(new HCUns16<ScratchU16>("valwo", u16scratch, 0, &ScratchU16::Set), cont, srv);
  Add(new HCUns16S<ScratchU16>("vale", u16scratch, &ScratchU16::Get, &ScratchU16::Set, U16enums), cont, srv);
  Add(new HCUns16Table<ScratchU16>("table", u16scratch, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tablero", u16scratch, &ScratchU16::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tablewo", u16scratch, 0, &ScratchU16::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns16Table<ScratchU16>("tableve", u16scratch, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE, 0, U16enums), cont, srv);
  Add(new HCUns16TableS<ScratchU16>("tableeeve", u16scratch, &ScratchU16::TableGet, &ScratchU16::TableSet, TABLE_SIZE, Eidenums, U16enums), cont, srv);
  Add(new HCUns16List<ScratchU16>("list", u16scratch, &ScratchU16::ListGet, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16List<ScratchU16>("listro", u16scratch, &ScratchU16::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16List<ScratchU16>("listwo", u16scratch, 0, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns16ListS<ScratchU16>("listve", u16scratch, &ScratchU16::ListGet, &ScratchU16::ListAdd, &ScratchU16::ListSub, LIST_MAX_SIZE, U16enums), cont, srv);

  cont = new HCContainer("u32");
  Add(cont, topcont);
  Add(new HCCall<ScratchU32>("print", u32scratch, &ScratchU32::Print), cont, srv);
  Add(new HCCallTable<ScratchU32>("printt", u32scratch, &ScratchU32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU32>("printtee", u32scratch, &ScratchU32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns32<ScratchU32>("val", u32scratch, &ScratchU32::Get, &ScratchU32::Set), cont, srv);
  Add(new HCUns32<ScratchU32>("valro", u32scratch, &ScratchU32::Get, 0), cont, srv);
  Add(new HCUns32<ScratchU32>("valwo", u32scratch, 0, &ScratchU32::Set), cont, srv);
  Add(new HCUns32S<ScratchU32>("vale", u32scratch, &ScratchU32::Get, &ScratchU32::Set, U32enums), cont, srv);
  Add(new HCUns32Table<ScratchU32>("table", u32scratch, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tablero", u32scratch, &ScratchU32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tablewo", u32scratch, 0, &ScratchU32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns32Table<ScratchU32>("tableve", u32scratch, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE, 0, U32enums), cont, srv);
  Add(new HCUns32TableS<ScratchU32>("tableeeve", u32scratch, &ScratchU32::TableGet, &ScratchU32::TableSet, TABLE_SIZE, Eidenums, U32enums), cont, srv);
  Add(new HCUns32List<ScratchU32>("list", u32scratch, &ScratchU32::ListGet, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32List<ScratchU32>("listro", u32scratch, &ScratchU32::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32List<ScratchU32>("listwo", u32scratch, 0, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns32ListS<ScratchU32>("listve", u32scratch, &ScratchU32::ListGet, &ScratchU32::ListAdd, &ScratchU32::ListSub, LIST_MAX_SIZE, U32enums), cont, srv);

  cont = new HCContainer("u64");
  Add(cont, topcont);
  Add(new HCCall<ScratchU64>("print", u64scratch, &ScratchU64::Print), cont, srv);
  Add(new HCCallTable<ScratchU64>("printt", u64scratch, &ScratchU64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchU64>("printtee", u64scratch, &ScratchU64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCUns64<ScratchU64>("val", u64scratch, &ScratchU64::Get, &ScratchU64::Set), cont, srv);
  Add(new HCUns64<ScratchU64>("valro", u64scratch, &ScratchU64::Get, 0), cont, srv);
  Add(new HCUns64<ScratchU64>("valwo", u64scratch, 0, &ScratchU64::Set), cont, srv);
  Add(new HCUns64S<ScratchU64>("vale", u64scratch, &ScratchU64::Get, &ScratchU64::Set, U64enums), cont, srv);
  Add(new HCUns64TableS<ScratchU64>("table", u64scratch, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tablero", u64scratch, &ScratchU64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tablewo", u64scratch, 0, &ScratchU64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tableve", u64scratch, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE, 0, U64enums), cont, srv);
  Add(new HCUns64Table<ScratchU64>("tableeeve", u64scratch, &ScratchU64::TableGet, &ScratchU64::TableSet, TABLE_SIZE, Eidenums, U64enums), cont, srv);
  Add(new HCUns64List<ScratchU64>("list", u64scratch, &ScratchU64::ListGet, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64List<ScratchU64>("listro", u64scratch, &ScratchU64::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64List<ScratchU64>("listwo", u64scratch, 0, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCUns64ListS<ScratchU64>("listve", u64scratch, &ScratchU64::ListGet, &ScratchU64::ListAdd, &ScratchU64::ListSub, LIST_MAX_SIZE, U64enums), cont, srv);

  cont = new HCContainer("bool");
  Add(cont, topcont);
  Add(new HCCall<ScratchBool>("print", boolscratch, &ScratchBool::Print), cont, srv);
  Add(new HCCallTable<ScratchBool>("printt", boolscratch, &ScratchBool::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchBool>("printtee", boolscratch, &ScratchBool::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCBool<ScratchBool>("val", boolscratch, &ScratchBool::Get, &ScratchBool::Set), cont, srv);
  Add(new HCBool<ScratchBool>("valro", boolscratch, &ScratchBool::Get, 0), cont, srv);
  Add(new HCBool<ScratchBool>("valwo", boolscratch, 0, &ScratchBool::Set), cont, srv);
  Add(new HCBoolS<ScratchBool>("vale", boolscratch, &ScratchBool::Get, &ScratchBool::Set, Boolenums), cont, srv);
  Add(new HCBoolTable<ScratchBool>("table", boolscratch, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tablero", boolscratch, &ScratchBool::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tablewo", boolscratch, 0, &ScratchBool::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCBoolTable<ScratchBool>("tableve", boolscratch, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE, 0, Boolenums), cont, srv);
  Add(new HCBoolTableS<ScratchBool>("tableeeve", boolscratch, &ScratchBool::TableGet, &ScratchBool::TableSet, TABLE_SIZE, Eidenums, Boolenums), cont, srv);

  cont = new HCContainer("f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchFloat>("print", floatscratch, &ScratchFloat::Print), cont, srv);
  Add(new HCCallTable<ScratchFloat>("printt", floatscratch, &ScratchFloat::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchFloat>("printtee", floatscratch, &ScratchFloat::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCFlt32S<ScratchFloat>("val", floatscratch, &ScratchFloat::Get, &ScratchFloat::Set), cont, srv);
  Add(new HCFlt32<ScratchFloat>("valro", floatscratch, &ScratchFloat::Get, 0), cont, srv);
  Add(new HCFlt32<ScratchFloat>("valwo", floatscratch, 0, &ScratchFloat::Set), cont, srv);
  Add(new HCFlt32Table<ScratchFloat>("table", floatscratch, &ScratchFloat::TableGet, &ScratchFloat::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt32Table<ScratchFloat>("tablero", floatscratch, &ScratchFloat::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCFlt32Table<ScratchFloat>("tablewo", floatscratch, 0, &ScratchFloat::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt32TableS<ScratchFloat>("tableee", floatscratch, &ScratchFloat::TableGet, &ScratchFloat::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchDouble>("print", doublescratch, &ScratchDouble::Print), cont, srv);
  Add(new HCCallTable<ScratchDouble>("printt", doublescratch, &ScratchDouble::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchDouble>("printtee", doublescratch, &ScratchDouble::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCFlt64S<ScratchDouble>("val", doublescratch, &ScratchDouble::Get, &ScratchDouble::Set), cont, srv);
  Add(new HCFlt64<ScratchDouble>("valro", doublescratch, &ScratchDouble::Get, 0), cont, srv);
  Add(new HCFlt64<ScratchDouble>("valwo", doublescratch, 0, &ScratchDouble::Set), cont, srv);
  Add(new HCFlt64Table<ScratchDouble>("table", doublescratch, &ScratchDouble::TableGet, &ScratchDouble::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt64Table<ScratchDouble>("tablero", doublescratch, &ScratchDouble::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCFlt64Table<ScratchDouble>("tablewo", doublescratch, 0, &ScratchDouble::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCFlt64TableS<ScratchDouble>("tableee", doublescratch, &ScratchDouble::TableGet, &ScratchDouble::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("str");
  Add(cont, topcont);
  Add(new HCCall<ScratchString>("print", stringscratch, &ScratchString::Print), cont, srv);
  Add(new HCCallTable<ScratchString>("printt", stringscratch, &ScratchString::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchString>("printtee", stringscratch, &ScratchString::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCStrS<ScratchString>("val", stringscratch, &ScratchString::Get, &ScratchString::Set), cont, srv);
  Add(new HCStr<ScratchString>("valro", stringscratch, &ScratchString::Get, 0), cont, srv);
  Add(new HCStr<ScratchString>("valwo", stringscratch, 0, &ScratchString::Set), cont, srv);
  Add(new HCStrTable<ScratchString>("table", stringscratch, &ScratchString::TableGet, &ScratchString::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCStrTable<ScratchString>("tablero", stringscratch, &ScratchString::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCStrTable<ScratchString>("tablewo", stringscratch, 0, &ScratchString::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCStrTableS<ScratchString>("tableee", stringscratch, &ScratchString::TableGet, &ScratchString::TableSet, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCStrListS<ScratchString>("list", stringscratch, &ScratchString::ListGet, &ScratchString::ListAdd, &ScratchString::ListSub, LIST_MAX_SIZE), cont, srv);
  Add(new HCStrList<ScratchString>("listro", stringscratch, &ScratchString::ListGet, 0, 0, LIST_MAX_SIZE), cont, srv);
  Add(new HCStrList<ScratchString>("listwo", stringscratch, 0, &ScratchString::ListAdd, &ScratchString::ListSub, LIST_MAX_SIZE), cont, srv);

  cont = new HCContainer("v2f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchV2F32>("print", v2f32scratch, &ScratchV2F32::Print), cont, srv);
  Add(new HCCallTable<ScratchV2F32>("printt", v2f32scratch, &ScratchV2F32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV2F32>("printtee", v2f32scratch, &ScratchV2F32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV2F32S<ScratchV2F32>("val", v2f32scratch, &ScratchV2F32::Get, &ScratchV2F32::Set), cont, srv);
  Add(new HCV2F32<ScratchV2F32>("valro", v2f32scratch, &ScratchV2F32::Get, 0), cont, srv);
  Add(new HCV2F32<ScratchV2F32>("valwo", v2f32scratch, 0, &ScratchV2F32::Set), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("table", v2f32scratch, &ScratchV2F32::TableGet, &ScratchV2F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("tablero", v2f32scratch, &ScratchV2F32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV2F32Table<ScratchV2F32>("tablewo", v2f32scratch, 0, &ScratchV2F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F32TableS<ScratchV2F32>("tableee", v2f32scratch, &ScratchV2F32::TableGet, &ScratchV2F32::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v2f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchV2F64>("print", v2f64scratch, &ScratchV2F64::Print), cont, srv);
  Add(new HCCallTable<ScratchV2F64>("printt", v2f64scratch, &ScratchV2F64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV2F64>("printtee", v2f64scratch, &ScratchV2F64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV2F64S<ScratchV2F64>("val", v2f64scratch, &ScratchV2F64::Get, &ScratchV2F64::Set), cont, srv);
  Add(new HCV2F64<ScratchV2F64>("valro", v2f64scratch, &ScratchV2F64::Get, 0), cont, srv);
  Add(new HCV2F64<ScratchV2F64>("valwo", v2f64scratch, 0, &ScratchV2F64::Set), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("table", v2f64scratch, &ScratchV2F64::TableGet, &ScratchV2F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("tablero", v2f64scratch, &ScratchV2F64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV2F64Table<ScratchV2F64>("tablewo", v2f64scratch, 0, &ScratchV2F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV2F64TableS<ScratchV2F64>("tableee", v2f64scratch, &ScratchV2F64::TableGet, &ScratchV2F64::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v3f32");
  Add(cont, topcont);
  Add(new HCCall<ScratchV3F32>("print", v3f32scratch, &ScratchV3F32::Print), cont, srv);
  Add(new HCCallTable<ScratchV3F32>("printt", v3f32scratch, &ScratchV3F32::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV3F32>("printtee", v3f32scratch, &ScratchV3F32::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV3F32S<ScratchV3F32>("val", v3f32scratch, &ScratchV3F32::Get, &ScratchV3F32::Set), cont, srv);
  Add(new HCV3F32<ScratchV3F32>("valro", v3f32scratch, &ScratchV3F32::Get, 0), cont, srv);
  Add(new HCV3F32<ScratchV3F32>("valwo", v3f32scratch, 0, &ScratchV3F32::Set), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("table", v3f32scratch, &ScratchV3F32::TableGet, &ScratchV3F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("tablero", v3f32scratch, &ScratchV3F32::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV3F32Table<ScratchV3F32>("tablewo", v3f32scratch, 0, &ScratchV3F32::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F32TableS<ScratchV3F32>("tableee", v3f32scratch, &ScratchV3F32::TableGet, &ScratchV3F32::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("v3f64");
  Add(cont, topcont);
  Add(new HCCall<ScratchV3F64>("print", v3f64scratch, &ScratchV3F64::Print), cont, srv);
  Add(new HCCallTable<ScratchV3F64>("printt", v3f64scratch, &ScratchV3F64::TablePrint, TABLE_SIZE), cont, srv);
  Add(new HCCallTable<ScratchV3F64>("printtee", v3f64scratch, &ScratchV3F64::TablePrint, TABLE_SIZE, Eidenums), cont, srv);
  Add(new HCV3F64S<ScratchV3F64>("val", v3f64scratch, &ScratchV3F64::Get, &ScratchV3F64::Set), cont, srv);
  Add(new HCV3F64<ScratchV3F64>("valro", v3f64scratch, &ScratchV3F64::Get, 0), cont, srv);
  Add(new HCV3F64<ScratchV3F64>("valwo", v3f64scratch, 0, &ScratchV3F64::Set), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("table", v3f64scratch, &ScratchV3F64::TableGet, &ScratchV3F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("tablero", v3f64scratch, &ScratchV3F64::TableGet, 0, TABLE_SIZE), cont, srv);
  Add(new HCV3F64Table<ScratchV3F64>("tablewo", v3f64scratch, 0, &ScratchV3F64::TableSet, TABLE_SIZE), cont, srv);
  Add(new HCV3F64TableS<ScratchV3F64>("tableee", v3f64scratch, &ScratchV3F64::TableGet, &ScratchV3F64::TableSet, TABLE_SIZE, Eidenums), cont, srv);

  cont = new HCContainer("file");
  Add(cont, topcont);
  Add(new HCFile<ScratchFile>("scratchfile", filescratch, &ScratchFile::Read, &ScratchFile::Write), cont, srv);

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
  delete boolscratch;
  delete u8scratch;
  delete u16scratch;
  delete u32scratch;
  delete u64scratch;
  delete s8scratch;
  delete s16scratch;
  delete s32scratch;
  delete s64scratch;
  delete floatscratch;
  delete doublescratch;
  delete stringscratch;
  delete v2f32scratch;
  delete v2f64scratch;
  delete v3f32scratch;
  delete v3f64scratch;
  delete filescratch;

  //Cleanup TLS
  EVP_cleanup();

  //Return success
  return 0;
}
