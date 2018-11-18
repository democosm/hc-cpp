// HC console
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

#ifndef _HCCONSOLE_HH_
#define _HCCONSOLE_HH_

#include "device.hh"
#include "hccell.hh"
#include "hccontainer.hh"
#include "hcmessage.hh"
#include <inttypes.h>
#include <termios.h>
#include <string>
#include <vector>

//Command
class HCCommand
{
public:
  HCCommand();
  ~HCCommand();
  void operator =(const HCCommand &cmd);
  bool operator ==(const HCCommand &cmd);
  const std::string GetStr();
  void GetEnd(std::string &str);
  uint32_t GetLen();
  uint32_t GetIndex();
  void Reset();
  void Trim();
  bool Left();
  bool Right();
  void Insert(char ch);
  void Delete();
  char EndChar();
  void Tokenize(std::vector<std::string> &toks);

private:
  std::string _str;
  uint32_t _index;
};

//History
class HCHistory
{
public:
  static const uint32_t DEPTH = 50;

public:
  HCHistory();
  ~HCHistory(void);
  bool AtOldest(void);
  bool AtNewest(void);
  void Back(void);
  void Forward(void);
  void GetCurrentCmd(HCCommand &cmd);
  void NewCmd(HCCommand &cmd);
  void Show(void);

private:
  uint32_t NextIndex(uint32_t curindex);
  uint32_t PrevIndex(uint32_t curindex);

private:
  HCCommand _cmd[DEPTH];
  uint32_t _oldest;
  uint32_t _newest;
  uint32_t _index;
};

//Console
class HCConsole
{
public:
  //Key codes
  static const int KEY_UP = -1;
  static const int KEY_DOWN = -2;
  static const int KEY_LEFT = -3;
  static const int KEY_RIGHT = -4;
  static const int KEY_HOME = -5;
  static const int KEY_END = -6;
  static const int KEY_BACKSPACE = -7;
  static const int KEY_TAB = -8;
  static const int KEY_ENTER = -9;
  static const int KEY_DELETE = -10;

public:
  HCConsole(HCContainer *top);
  ~HCConsole();
  void Run(void);

private:
  int WaitKey(void);
  void UpProc(void);
  void DownProc(void);
  void LeftProc(void);
  void RightProc(void);
  void HomeProc(void);
  void EndProc(void);
  void BackspaceProc(void);
  void TabProc(uint32_t recurlevel);
  void EnterProc(void);
  void DeleteProc(void);
  void DefaultProc(char ch);
  void HelpCmdProc(uint32_t tokcnt);
  void HistCmdProc(uint32_t tokcnt);
  void ChdirCmdProc(uint32_t tokcnt);
  void ListCmdProc(uint32_t tokcnt);
  void InfoCmdProc(uint32_t tokcnt);
  void FindCmdProc(uint32_t tokcnt);
  void ExitCmdProc(uint32_t tokcnt);
  void CallCmdProc(uint32_t tokcnt);
  void ParamCmdProc(uint32_t tokcnt);
  void Prompt(void);
  bool GetNextCommonChar(const std::string &name, HCContainer *startcont, char &ch, size_t index=0);
  void ShowListing(const std::string &name, HCContainer *startcont, size_t index=0);
  void ShowInfo(const std::string &name, HCContainer *startcont, size_t index=0);
  void ShowNames(const std::string &name, HCContainer *startcont, size_t index=0);
  void ShowFinds(const std::string &name, HCContainer *cont);

private:
  //Maximum counts
  static const unsigned long TOK_CNT_MAX = 12;
  static const unsigned long TOK_SIZ_MAX = 80;

private:
  struct termios _oldopts;
  HCContainer *_top;
  HCContainer *_workcont;
  HCHistory *_hist;
  HCCommand _cmd;
  HCCommand _cmdcopy;
  std::vector<std::string> _tokens;
  bool _exit;
};

#endif //_HCCONSOLE_HH_
