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

#include "error.hh"
#include "hcconsole.hh"
#include "hcutility.hh"
#include "str.hh"
#include <ctype.h>
#include <cassert>
#include <iostream>
#include <string>
#include <unistd.h>

//Command
HCCommand::HCCommand()
{
  //Initialize member variables
  _str = "";
  _index = 0;
}

HCCommand::~HCCommand()
{
}

void HCCommand::operator =(const HCCommand &cmd)
{
  //Copy from string and set length and index
  _str = cmd._str;
  _index = _str.length();
}

bool HCCommand::operator ==(const HCCommand &cmd)
{
  //Compare strings
  return _str == cmd._str;
}

const string HCCommand::GetStr()
{
  return _str;
}

void HCCommand::GetEnd(string &str)
{
  str = _str.substr(_index, _str.length()-_index);
}

uint32_t HCCommand::GetLen()
{
  return _str.length();
}

uint32_t HCCommand::GetIndex()
{
  return _index;
}

void HCCommand::Reset()
{
  //Reset variables
  _str.clear();
  _index = 0;
}

void HCCommand::Trim()
{
  size_t nwspos;

  //Find index of first character that is not white space and check for none found
  if((nwspos = _str.find_first_not_of(" \t")) == string::npos)
  {
    _str.clear();
    return;
  }

  //Trim beginning white space
  _str = _str.substr(nwspos);

  //Find index of last character that is not white space and check for none found
  if((nwspos = _str.find_last_not_of(" \t")) == string::npos)
  {
    _str.clear();
    return;
  }

  //Trim ending spaces
  _str = _str.substr(0, nwspos+1);
}

bool HCCommand::Left()
{
  //Check for index at min already
  if(_index <= 0)
    return false;

  //Decrement index
  _index--;

  return true;
}

bool HCCommand::Right()
{
  //Check for index at max already
  if(_index >= _str.length())
    return false;

  //Increment index
  _index++;

  return true;
}

void HCCommand::Insert(char ch)
{
  //Insert character at index
  _str.insert(_index++, 1, ch);
}

void HCCommand::Delete()
{
  //Remove character at index
  _str.erase(_index, 1);
}

char HCCommand::EndChar()
{
  size_t len;

  //Check for no characters in command
  if((len = _str.length()) == 0)
    return '\0';

  //Return last character in command
  return _str.at(len-1);
}

void HCCommand::Tokenize(vector<string> &toks)
{
  bool literalmode;
  string token;
  size_t len;
  size_t strindex;
  char ch;

  //Clear tokens
  toks.clear();

  //Reset high level variables
  literalmode = false;

  //Clear token
  token.clear();

  //Get string length
  len = _str.length();

  //Go until all tokens are consumed or error
  for(strindex=0; strindex<len; strindex++)
  {
    //Get next character in string
    ch = _str.at(strindex);

    //Check for literal mode
    if(literalmode)
    {
      //Check for end of literal mode
      if(ch == '"')
      {
        //Set to non literal mode
        literalmode = false;

        //Add token to list if not zero length
        if(token.length() != 0)
          toks.push_back(token);

        //Add special character to list
        token = ch;
        toks.push_back(token);

        //Clear token
        token.clear();
      }
      else
      {
        //Append character to token
        token += ch;
      }
    }
    else
    {
      //Check for special characters
      switch(ch)
      {
      case '"':
        //Set to literal mode
        literalmode = true;
      case '=':
      case '(':
      case ')':
      case '[':
      case ']':
      case '<':
      case '>':
      case '@':
      case '#':
        //Add token to list if not zero length
        if(token.length() != 0)
          toks.push_back(token);

        //Add special character to list
        token = ch;
        toks.push_back(token);

        //Clear token
        token.clear();

        break;
      default:
        //Check for non white space
        if(isspace(ch) == 0)
        {
          //Append character to token
          token += ch;
        }
        else
        {
          //Check for not leading white space
          if(token.length() != 0)
          {
            //Add token to list
            toks.push_back(token);

            //Clear token
            token.clear();
          }
        }

        break;
      }
    }
  }

  //Add token to list if not zero length
  if(token.length() != 0)
    toks.push_back(token);
}

//History
HCHistory::HCHistory()
{
  //Initialize indices
  _oldest = 0;
  _newest = 0;
  _index = 0;
}

HCHistory::~HCHistory()
{
}

bool HCHistory::AtOldest(void)
{
  //Return true if index is at oldest, otherwise false
  return _index == _oldest ? true : false;
}

bool HCHistory::AtNewest(void)
{
  //Return true if index is at newest, otherwise false
  return _index == _newest ? true : false;
}

void HCHistory::Back(void)
{
  //Check for index already at oldest command
  if(_index == _oldest)
    return;

  //Move index to previous
  _index = PrevIndex(_index);
}

void HCHistory::Forward(void)
{
  //Check for index already at newest command
  if(_index == _newest)
    return;

  //Move index to next
  _index = NextIndex(_index);
}

void HCHistory::GetCurrentCmd(HCCommand &cmd)
{
  //Copy command at current index
  cmd = _cmd[_index];
}

void HCHistory::NewCmd(HCCommand &cmd)
{
  uint32_t prev;

  //Set index to newest index should equal newest whenever this method exits
  _index = _newest;

  //Don't advance if zero length command
  if(cmd.GetLen() == 0)
    return;

  //Get previous command index
  prev = PrevIndex(_newest);

  //Check for same as previous command
  if(cmd == _cmd[prev])
    return;

  //Copy command to newest
  _cmd[_newest] = cmd;

  //Advance newest index
  _newest = NextIndex(_newest);

  //Set index to updated newest index
  _index = _newest;

  //If newest overtakes oldest, then increment oldest with wrap
  if(_newest == _oldest)
    _oldest = NextIndex(_oldest);

  //Reset newest command
  _cmd[_newest].Reset();
}

void HCHistory::Show(void)
{
  uint32_t i;
  uint32_t j;

  //Print each command except newest
  for(i=_oldest, j=0; i != _newest; i=NextIndex(i), j++)
    cout << j << " - " << _cmd[i].GetStr() << endl;
}

uint32_t HCHistory::NextIndex(uint32_t curindex)
{
  //Check for current at maximum
  if(curindex == (DEPTH - 1))
    return 0;

  return curindex + 1;
}

uint32_t HCHistory::PrevIndex(uint32_t curindex)
{
  //Check for current at minimum
  if(curindex == 0)
    return DEPTH - 1;

  return curindex - 1;
}

//Console
HCConsole::HCConsole(HCContainer *top)
{
  struct termios opts;

  //Assert valid arguments
  assert(top != 0);

  //Remember options for stdin
  tcgetattr(STDIN_FILENO, &_oldopts);

  //Set stdin to raw input mode
  opts = _oldopts;
  opts.c_cc[VMIN] = 1;
  opts.c_cc[VTIME] = 0;
  opts.c_lflag &= ~ICANON;
  opts.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &opts);

  //Initialize member variables
  _top = top;
  _workcont = top;
  _hist = new HCHistory();
  _exit = false;
}

HCConsole::~HCConsole()
{
  //Cleanup member variables
  delete _hist;

  //Restore original options for stdin
  tcsetattr(STDIN_FILENO, TCSANOW, &_oldopts);
}

void HCConsole::Run(void)
{
  int key;

  //Display command prompt
  cout << endl;
  cout << "Control Console" << endl;
  cout << "Type h, help, or ? for help" << endl;
  cout << endl;
  Prompt();

  //Main processing loop
  while(!_exit)
  {
    //Read a character from lower device and check for error
    key = WaitKey();

    //Process character
    switch(key)
    {
    case KEY_UP:
      UpProc();
      break;
    case KEY_DOWN:
      DownProc();
      break;
    case KEY_LEFT:
      LeftProc();
      break;
    case KEY_RIGHT:
      RightProc();
      break;
    case KEY_HOME:
      HomeProc();
      break;
    case KEY_END:
      EndProc();
      break;
    case KEY_BACKSPACE:
      BackspaceProc();
      break;
    case KEY_TAB:
      TabProc();
      break;
    case KEY_ENTER:
      EnterProc();
      break;
    case KEY_DELETE:
      DeleteProc();
      break;
    default:
      DefaultProc((char)key);
      break;
    }
  }

  //Indicate exit
  cout << "Exiting" << endl;
}

int HCConsole::WaitKey(void)
{
  int state;
  int key;
  int lastkey;

  //Initialize state and return value
  state = 0;

  //Loop until character input is translated
  while(true)
  {
    //Read character from standard input
    key = getchar();

    //Translate characters if necessary
    switch(state)
    {
    case 0:
      //Check keys where no translation is necessary
      if((key >= (int)' ') && (key <= (int)'~'))
        return key;

      //Check for simple translation keys
      if(key == 1)
        return KEY_HOME;
      else if(key == 5)
        return KEY_END;
      else if(key == 8)
        return KEY_BACKSPACE;
      else if(key == 9)
        return KEY_TAB;
      else if(key == 10)
        return KEY_ENTER;
      else if(key == 127)
        return KEY_BACKSPACE;

      //Check for more than one byte key sequence
      if(key == 27)
      {
        //Go to next state
        state++;
        break;
      }

      //Reset state machine
      state = 0;
      break;
    case 1:
      //Check for more than two byte in key sequence
      if(key == 91)
      {
        //Go to next state
        state++;
        break;
      }

      //Reset state machine
      state = 0;
      break;
    case 2:
      //Check for more than three characters in key sequence
      if((key == 49) || (key == 51) || (key == 52) || (key == 53) || (key == 54))
      {
        //Go to next state
        state++;
        break;
      }

      //Check for simple translation keys
      if(key == 65)
        return KEY_UP;
      else if(key == 66)
        return KEY_DOWN;
      else if(key == 67)
        return KEY_RIGHT;
      else if(key == 68)
        return KEY_LEFT;
      else if(key == 70)
        return KEY_END;
      else if(key == 72)
        return KEY_HOME;

      //Reset state machine
      state = 0;
      break;
    case 3:
      //Check for four characters in key sequence
      if(key == 126)
      {
        //Check for simple translation keys
        if(lastkey == 49)
          return KEY_HOME;
        else if(lastkey == 51)
          return KEY_DELETE;
        else if(lastkey == 52)
          return KEY_END;
        else if(lastkey == 53)
          return KEY_UP;
        else if(lastkey == 54)
          return KEY_DOWN;

        //Reset state machine
        state = 0;
        break;
      }

      //Reset state machine
      state = 0;
      break;
    }

    //Remember last key
    lastkey = key;
  }
}

void HCConsole::UpProc(void)
{
  //Check for history at oldest
  if(_hist->AtOldest())
  {
    //Beep
    cout << '\a';
    return;
  }

  //Check for first time moving up in history
  if(_hist->AtNewest())
    _cmdcopy = _cmd;

  //Erase old command
  Prompt();
  cout << string(_cmd.GetLen(), ' ');

  //Move backward in command history
  _hist->Back();

  //Get new command from history
  _hist->GetCurrentCmd(_cmd);

  //Show new command
  Prompt();
  cout << _cmd.GetStr();
}

void HCConsole::DownProc(void)
{
  //Check for history at newest
  if(_hist->AtNewest())
  {
    //Beep
    cout << '\a';
    return;
  }

  //Erase old command
  Prompt();
  cout << string(_cmd.GetLen(), ' ');

  //Move forward in command history
  _hist->Forward();

  //If just reached newest command, then get from local copy, else get from history
  if(_hist->AtNewest())
    _cmd = _cmdcopy;
  else
    _hist->GetCurrentCmd(_cmd);

  //Show new command
  Prompt();
  cout << _cmd.GetStr();
}

void HCConsole::LeftProc(void)
{
  //Move left in command buffer and check for limit
  if(!_cmd.Left())
  {
    //Beep
    cout << '\a';
    return;
  }

  //Move cursor left
  cout << "\033[D";
}

void HCConsole::RightProc(void)
{
  //Move right in command buffer and check for limit
  if(!_cmd.Right())
  {
    //Beep
    cout << '\a';
    return;
  }

  //Move cursor right
  cout << "\033[C";
}

void HCConsole::HomeProc(void)
{
  //Move left in command buffer until at beginning
  while(_cmd.Left())
  {
    //Move cursor left
    cout << "\033[D";
  }
}

void HCConsole::EndProc(void)
{
  //Move right in command buffer until at end
  while(_cmd.Right())
  {
    //Move cursor right
    cout << "\033[C";
  }
}

void HCConsole::BackspaceProc(void)
{
  uint32_t count;
  uint32_t i;
  string temp;

  //Move left in command buffer and check for error
  if(!_cmd.Left())
  {
    //Beep
    cout << '\a';
    return;
  }

  //Move cursor left
  cout << "\033[D";

  //Delete character at this location and check for error
  _cmd.Delete();

  //Print remainder of command and space at end to delete last char
  _cmd.GetEnd(temp);
  cout << temp << ' ';

  //Move cursor back
  count = temp.length() + 1;
  for(i=0; i<count; i++)
    cout << "\033[D";
}

void HCConsole::TabProc(void)
{
  HCContainer *startcont;
  char nextchar;

  //Don't do anything if command ends in whitespace
  if(isspace((int)_cmd.EndChar()) != 0)
  {
    //Beep
    cout << '\a';

    return;
  }

  //Tokenize command
  _cmd.Tokenize(_tokens);

  //Check for at least one token in command
  if(_tokens.size() > 0)
  {
    //If name begins with '/' start from top, else start from current working container
    if(_tokens[_tokens.size() - 1].at(0) == '/')
      startcont = _top;
    else
      startcont = _workcont;

    //get next common character and check for not found
    if(!GetNextCommonChar(_tokens[_tokens.size() - 1], startcont, nextchar))
    {
      cout << '\a';
    }
    else
    {
      //Act like operator hit this key
      DefaultProc(nextchar);

      //Recurse if exact container or parameter not found
      if((nextchar != '/') && (nextchar != ' '))
        TabProc();
    }
  }
}

void HCConsole::EnterProc(void)
{
  //Move to a clean line
  cout << endl;

  //Trim whitespace from command
  _cmd.Trim();

  //Advance history
  _hist->NewCmd(_cmd);

  //Tokenize command
  _cmd.Tokenize(_tokens);

  //Check for any tokens
  if(_tokens.size() > 0)
  {
    //Process first token
    if((_tokens[0] == "help") || (_tokens[0] == "h") || (_tokens[0] == "?"))
    {
      HelpCmdProc(_tokens.size());
    }
    else if((_tokens[0] == "hist"))
    {
      HistCmdProc(_tokens.size());
    }
    else if(_tokens[0] == "cd")
    {
      ChdirCmdProc(_tokens.size());
    }
    else if(_tokens[0] == "ls")
    {
      ListCmdProc(_tokens.size());
    }
    else if((_tokens[0] == "info") || (_tokens[0] == "i"))
    {
      InfoCmdProc(_tokens.size());
    }
    else if((_tokens[0] == "find"))
    {
      FindCmdProc(_tokens.size());
    }
    else if((_tokens[0] == "exit") || (_tokens[0] == "x"))
    {
      ExitCmdProc(_tokens.size());
    }
    else if((_tokens[0] == "call"))
    {
      CallCmdProc(_tokens.size());
    }
    else
    {
      ParamCmdProc(_tokens.size());
    }
  }

  //Reset command
  _cmd.Reset();

  //Display prompt
  Prompt();
}

void HCConsole::DeleteProc(void)
{
  uint32_t count;
  uint32_t i;
  string temp;

  //Delete character from command and check for error
  _cmd.Delete();

  //Print remainder of command and space at end to delete last char
  _cmd.GetEnd(temp);
  cout << temp << ' ';

  //Move cursor back
  count = temp.length() + 1;
  for(i=0; i<count; i++)
    cout << "\033[D";
}

void HCConsole::DefaultProc(char ch)
{
  uint32_t count;
  uint32_t i;
  string temp;

  //Insert character into command and check for error
  _cmd.Insert(ch);

  //Print character to console
  cout << ch;

  //Print remainder of command
  _cmd.GetEnd(temp);
  cout << temp;

  //Move cursor back
  count = temp.length();
  for(i=0; i<count; i++)
    cout << "\033[D";
}

void HCConsole::HelpCmdProc(uint32_t tokcnt)
{
  //Check for invalid argument count
  if(tokcnt != 1)
  {
    cout << "Syntax: " << _tokens[0] << endl;
    return;
  }

  //Show help information
  cout << "help|h|?        = Show this help information" << endl;
  cout << "hist            = Show command history" << endl;
  cout << "chdir|cd C      = Change working container to container C" << endl;
  cout << "list|ls E       = Show values of parameters or containers matching expression E" << endl;
  cout << "info|i E        = Show information for parameters or containers matching expression E" << endl;
  cout << "find E          = Show all parameters and containers matching expression E" << endl;
  cout << "exit|x          = Exit application" << endl;
  cout << "call P          = Call parameter P" << endl;
  cout << "P()             = Call parameter P" << endl;
  cout << "P(I)            = ICall parameter P with EID of I" << endl;
  cout << "P(\"E\")          = ICall parameter P with EID of enum E" << endl;
  cout << "P=N             = Set parameter P to value N" << endl;
  cout << "P=\"S\"           = Set parameter P to string or enum S" << endl;
  cout << "P[I]=N          = ISet parameter P with EID of I to value N" << endl;
  cout << "P[I]=\"S\"        = ISet parameter P with EID of I to string or enum S" << endl;
  cout << "P[\"E\"]=N        = ISet parameter P with EID of enum E to value N" << endl;
  cout << "P[\"E\"]=\"S\"      = ISet parameter P with EID of E to string or enum S" << endl;
  cout << "P<N             = Add value N to parameter P list" << endl;
  cout << "P>N             = Subtract value N from parameter P list" << endl;
  cout << "P<\"S\"           = Add string or enum S to parameter P list" << endl;
  cout << "P>\"S\"           = Subtract string or enum S from parameter P list" << endl;
  cout << "P@\"S\"           = Upload parameter P from local file S" << endl;
  cout << "P#\"S\"           = Download parameter P to local file S" << endl;
}

void HCConsole::HistCmdProc(uint32_t tokcnt)
{
  //Check for invalid argument count
  if(tokcnt != 1)
  {
    cout << "Syntax: " << _tokens[0] << endl;
    return;
  }

  //Show command history
  _hist->Show();
}

void HCConsole::ChdirCmdProc(uint32_t tokcnt)
{
  HCContainer *startcont;
  HCContainer *cont;

  //Check for invalid argument count
  if(tokcnt != 2)
  {
    cout << "Syntax: " << _tokens[0] << " <CONTAINER NAME>" << endl;
    return;
  }

  //If name begins with '/' start from top, else start from current working container
  if(_tokens[tokcnt - 1].at(0) == '/')
    startcont = _top;
  else
    startcont = _workcont;

  //Get container with specified name and check for found
  if((cont = HCUtility::GetCont(_tokens[1], startcont)) != 0)
  {
    //Update working container
    _workcont = cont;
  }
  else
  {
    //See if there is a parameter with that name
    if(HCUtility::GetParam(_tokens[1], startcont) != 0)
      cout << _tokens[1] << ": Not a container" << endl;
    else
      cout << _tokens[1] << ": No such container" << endl;
  }
}

void HCConsole::ListCmdProc(uint32_t tokcnt)
{
  HCContainer *startcont;
  uint32_t i;

  //Check for no arguments
  if(tokcnt == 1)
  {
    //Show listing of everything in working container
    ShowListing("*", _workcont);
  }
  else
  {
    //Loop through all arguments
    for(i=1; i<tokcnt; i++)
    {
      //If name begins with '/' start from top, else start from current working container
      if(_tokens[i].at(0) == '/')
        startcont = _top;
      else
        startcont = _workcont;

      //Show listing of matching parameters and containers
      ShowListing(_tokens[i], startcont);
    }
  }
}

void HCConsole::InfoCmdProc(uint32_t tokcnt)
{
  HCContainer *startcont;
  uint32_t i;

  //Check for no arguments
  if(tokcnt == 1)
  {
    //Show information of everything in working container
    ShowInfo("*", _workcont);
  }
  else
  {
    //Loop through all arguments
    for(i=1; i<tokcnt; i++)
    {
      //If name begins with '/' start from top, else start from current working container
      if(_tokens[i].at(0) == '/')
        startcont = _top;
      else
        startcont = _workcont;

      //Show information of matching parameters and containers
      ShowInfo(_tokens[i], startcont);
    }
  }
}

void HCConsole::FindCmdProc(uint32_t tokcnt)
{
  uint32_t i;

  //Check for invalid argument count
  if(tokcnt < 2)
  {
    cout << "Syntax: " << _tokens[0] << " <EXPRESSION(s)>" << endl;
    return;
  }

  //Loop through all specified arguments
  for(i=1; i<tokcnt; i++)
  {
    //Show finds of matching parameters and containers
    ShowFinds(_tokens[i], _top);
  }
}

void HCConsole::ExitCmdProc(uint32_t tokcnt)
{
  //Check for invalid argument count
  if(tokcnt != 1)
  {
    cout << "Syntax: " << _tokens[0] << endl;
    return;
  }

  //Set exit flag
  _exit = true;
}

void HCConsole::CallCmdProc(uint32_t tokcnt)
{
  HCContainer *startcont;
  uint32_t i;
  HCParameter *param;
  int ierr;

  //Check invalid number of arguments
  if(tokcnt < 2)
  {
    cout << "Syntax: " << _tokens[0] << " <CALL PARAM LIST>" << endl;
    return;
  }

  //Loop through all specified arguments
  for(i=1; i<tokcnt; i++)
  {
    //If name begins with '/' start from top, else start from current working container
    if(_tokens[i].at(0) == '/')
      startcont = _top;
    else
      startcont = _workcont;

    //See if there is a parameter with that name
    if((param = HCUtility::GetParam(_tokens[i], startcont)) != 0)
    {
      //Call parameter and check for error
      if((ierr = param->Call()) != ERR_NONE)
        cout << "Error (" << ErrToString(ierr) << ')' << endl;
    }
    else
    {
      cout << _tokens[i] << ": No such parameter" << endl;
    }
  }
}

void HCConsole::ParamCmdProc(uint32_t tokcnt)
{
  HCContainer *startcont;
  HCParameter *param;
  uint32_t eid;
  int ierr;

  //If name begins with '/' start from top, else start from current working container
  if(_tokens[0].at(0) == '/')
    startcont = _top;
  else
    startcont = _workcont;

  //Get parameter with specified name and check for not found
  if((param = HCUtility::GetParam(_tokens[0], startcont)) == 0)
  {
    cout << _tokens[0] << ": No such parameter" << endl;
    return;
  }

  //Check for P=N pattern
  if((tokcnt == 3) && (_tokens[1] == "="))
  {
    //Set parameter value using string and check for error
    if((ierr = param->SetStr(_tokens[2])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P<N pattern
  if((tokcnt == 3) && (_tokens[1] == "<"))
  {
    //Add value to parameter using string and check for error
    if((ierr = param->AddStr(_tokens[2])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P>N pattern
  if((tokcnt == 3) && (_tokens[1] == ">"))
  {
    //Subtract native value string and check for error
    if((ierr = param->SubStr(_tokens[2])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P() pattern
  if((tokcnt == 3) && (_tokens[1] == "(") && (_tokens[2] == ")"))
  {
    //Call parameter and check for error
    if((ierr = param->Call()) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P="" pattern
  if((tokcnt == 4) && (_tokens[1] == "=") && (_tokens[2] == "\"") && (_tokens[3] == "\""))
  {
    //Set parameter value to empty string and check for error
    if((ierr = param->SetStrLit("")) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P<"" pattern
  if((tokcnt == 4) && (_tokens[1] == "<") && (_tokens[2] == "\"") && (_tokens[3] == "\""))
  {
    //Add empty string and check for error
    if((ierr = param->AddStrLit("")) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P>"" pattern
  if((tokcnt == 4) && (_tokens[1] == ">") && (_tokens[2] == "\"") && (_tokens[3] == "\""))
  {
    //Subtract empty string and check for error
    if((ierr = param->SubStrLit("")) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P(I) pattern
  if((tokcnt == 4) && (_tokens[1] == "(") && (_tokens[3] == ")"))
  {
    //Convert table id to integer and check for error
    if(!StringConvert(_tokens[2], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Call parameter and check for error
    if((ierr = param->CallTbl(eid)) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P="S" pattern
  if((tokcnt == 5) && (_tokens[1] == "=") && (_tokens[2] == "\"") && (_tokens[4] == "\""))
  {
    //Set parameter value to literal string and check for error
    if((ierr = param->SetStrLit(_tokens[3])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P<"S" pattern
  if((tokcnt == 5) && (_tokens[1] == "<") && (_tokens[2] == "\"") && (_tokens[4] == "\""))
  {
    //Add literal string and check for error
    if((ierr = param->AddStrLit(_tokens[3])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P>"S" pattern
  if((tokcnt == 5) && (_tokens[1] == ">") && (_tokens[2] == "\"") && (_tokens[4] == "\""))
  {
    //Subtract literal string and check for error
    if((ierr = param->SubStrLit(_tokens[3])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P@"S" pattern
  if((tokcnt == 5) && (_tokens[1] == "@") && (_tokens[2] == "\"") && (_tokens[4] == "\""))
  {
    //Execute file upload
    if((ierr = param->Upload(_tokens[3])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P#"S" pattern
  if((tokcnt == 5) && (_tokens[1] == "#") && (_tokens[2] == "\"") && (_tokens[4] == "\""))
  {
    //Execute file download
    if((ierr = param->Download(_tokens[3])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P[I]=N pattern
  if((tokcnt == 6) && (_tokens[1] == "[") && (_tokens[3] == "]") && (_tokens[4] == "="))
  {
    //Convert table id to integer and check for error
    if(!StringConvert(_tokens[2], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to native value string and check for error
    if((ierr = param->SetStrTbl(eid, _tokens[5])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P("E") pattern
  if((tokcnt == 6) && (_tokens[1] == "(") && (_tokens[2] == "\"") && (_tokens[4] == "\"") && (_tokens[5] == ")"))
  {
    //Convert table id string to number
    if(!param->EIDStrToNum(_tokens[3], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Call parameter and check for error
    if((ierr = param->CallTbl(eid)) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P[I]="" pattern
  if((tokcnt == 7) && (_tokens[1] == "[") && (_tokens[3] == "]") && (_tokens[4] == "=") && (_tokens[5] == "\"") && (_tokens[6] == "\""))
  {
    //Convert table id to integer and check for error
    if(!StringConvert(_tokens[2], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to empty string and check for error
    if((ierr = param->SetStrLitTbl(eid, "")) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P[I]="S" pattern
  if((tokcnt == 8) && (_tokens[1] == "[") && (_tokens[3] == "]") && (_tokens[4] == "=") && (_tokens[5] == "\"") && (_tokens[7] == "\""))
  {
    //Convert table id to integer and check for error
    if(!StringConvert(_tokens[2], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to literal string and check for error
    if((ierr = param->SetStrLitTbl(eid, _tokens[6])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P["E"]=N pattern
  if((tokcnt == 8) && (_tokens[1] == "[") && (_tokens[2] == "\"") && (_tokens[4] == "\"") && (_tokens[5] == "]") && (_tokens[6] == "="))
  {
    //Convert table id string to number
    if(!param->EIDStrToNum(_tokens[3], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to native value string and check for error
    if((ierr = param->SetStrTbl(eid, _tokens[7])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P["E"]="" pattern
  if((tokcnt == 9) && (_tokens[1] == "[") && (_tokens[2] == "\"") && (_tokens[4] == "\"") && (_tokens[5] == "]") && (_tokens[6] == "=") && (_tokens[7] == "\"") && (_tokens[8] == "\""))
  {
    //Convert table id string to number
    if(!param->EIDStrToNum(_tokens[3], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to empty string and check for error
    if((ierr = param->SetStrTbl(eid, "")) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //Check for P["E"]="S" pattern
  if((tokcnt == 10) && (_tokens[1] == "[") && (_tokens[2] == "\"") && (_tokens[4] == "\"") && (_tokens[5] == "]") && (_tokens[6] == "=") && (_tokens[7] == "\"") && (_tokens[9] == "\""))
  {
    //Convert table id string to number
    if(!param->EIDStrToNum(_tokens[3], eid))
    {
      cout << "Error (" << ErrToString(ERR_EID) << ')' << endl;
      return;
    }

    //Set parameter value to literal string and check for error
    if((ierr = param->SetStrLitTbl(eid, _tokens[8])) != ERR_NONE)
      cout << "Error (" << ErrToString(ierr) << ')' << endl;

    return;
  }

  //No pattern recognized
  cout << "Unrecognized command" << endl;
}

void HCConsole::Prompt(void)
{
  //Show prompt
  cout << '\r' << _workcont->GetName() << '>';
}

bool HCConsole::GetNextCommonChar(const string &name, HCContainer *startcont, char &ch, size_t index)
{
  string nodename;
  size_t nextindex;
  HCContainer *cont;
  HCParameter *param;
  bool foundone;
  char nextchar;

  //Assert valid arguments
  assert(startcont != 0);

  //Loop through directory names in path
  while((nextindex = name.find('/', index)) != string::npos)
  {
    //Extract node name
    nodename = name.substr(index, nextindex-index);

    //Update index
    index = nextindex+1;

    //Check for special strings first
    if((nodename == "") || (nodename == "."))
    {
      //Continue to next iteration
      continue;
    }
    else if(nodename == "..")
    {
      //If no parent, then continue to next iteration
      if(startcont->GetParent() == 0)
      {
        //Continue to next iteration
        continue;
      }

      //Recurse
      return GetNextCommonChar(name, startcont->GetParent(), ch, index);
    }
    else
    {
      //Loop through all containers
      for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
        if(cont->IsNamed(nodename))
          return GetNextCommonChar(name, cont, ch, index);
    }

    //Container not found
    ch = '\0';
    return false;
  }

  //Extract ending node name
  nodename = name.substr(index, name.length()-index);

  //Check for special strings first
  if(nodename == "..")
  {
    ch = '/';
    return true;
  }

  //Inidcate no common character found yet
  foundone = false;

  //Loop through all parameters
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
  {
    //Check for name starting with node name and get next character
    if(param->GetNextCharInName(nodename, nextchar))
    {
      //Check for first time finding a character or no match
      if(!foundone)
        ch = nextchar;
      else if(nextchar != ch)
        return false;

      //Indicate match found
      foundone = true;
    }
  }

  //Loop through all containers
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
  {
    //Check for name starting with node name and get next character
    if(cont->GetNextCharInName(nodename, nextchar))
    {
      //Check for first time finding a character or no match
      if(!foundone)
        ch = nextchar;
      else if(nextchar != ch)
        return false;

      //Indicate match found
      foundone = true;
    }
  }

  //Return found indication
  return foundone;
}

void HCConsole::ShowListing(const string &name, HCContainer *startcont, size_t index)
{
  string nodename;
  size_t nextindex;
  HCContainer *cont;
  HCParameter *param;

  //Assert valid arguments
  assert(startcont != 0);

  //Loop through directory names in path
  while((nextindex = name.find('/', index)) != string::npos)
  {
    //Extract node name
    nodename = name.substr(index, nextindex-index);

    //Update index
    index = nextindex+1;

    //Check for special strings first
    if((nodename == "") || (nodename == "."))
    {
      //Continue to next iteration
      continue;
    }
    else if(nodename == "..")
    {
      //If no parent, then continue to next iteration
      if(startcont->GetParent() == 0)
      {
        //Continue to next iteration
        continue;
      }

      //Recurse
      ShowListing(name, startcont->GetParent(), index);
      return;
    }
    else
    {
      //Loop through all containers
      for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
        if(cont->NameMatchesExpression(nodename))
          ShowListing(name, cont, index);
    }

    //Done
    return;
  }

  //Extract node name
  nodename = name.substr(index, name.length()-index);

  //List parameters with matching name (expressions allowed)
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
    if(param->NameMatchesExpression(nodename))
      param->PrintVal();

  //List containers with matching name (expressions allowed)
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
    if(cont->NameMatchesExpression(nodename))
      cout << cont->GetName() << endl;
}

void HCConsole::ShowInfo(const string &name, HCContainer *startcont, size_t index)
{
  string nodename;
  size_t nextindex;
  HCContainer *cont;
  HCParameter *param;

  //Assert valid arguments
  assert(startcont != 0);

  //Loop through directory names in path
  while((nextindex = name.find('/', index)) != string::npos)
  {
    //Extract node name
    nodename = name.substr(index, nextindex-index);

    //Update index
    index = nextindex+1;

    //Check for special strings first
    if((nodename == "") || (nodename == "."))
    {
      //Continue to next iteration
      continue;
    }
    else if(nodename == "..")
    {
      //If no parent, then continue to next iteration
      if(startcont->GetParent() == 0)
      {
        //Continue to next iteration
        continue;
      }

      //Recurse
      ShowInfo(name, startcont->GetParent(), index);
      return;
    }
    else
    {
      //Loop through all containers
      for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
        if(cont->NameMatchesExpression(nodename))
          ShowInfo(name, cont, index);
    }

    //Done
    return;
  }

  //Extract node name
  nodename = name.substr(index, name.length()-index);

  //Print information about parameters with matching name (expressions allowed)
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
  {
    if(param->NameMatchesExpression(nodename))
    {
      param->PrintInfo();
      cout << "\n";
    }
  }

  //Print information about containers with matching name (expressions allowed)
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
  {
    if(cont->NameMatchesExpression(nodename))
    {
      cont->PrintInfo();
      cout << "\n";
    }
  }
}

void HCConsole::ShowFinds(const string &name, HCContainer *startcont)
{
  HCParameter *param;
  HCContainer *cont;

  //Assert valid arguments
  assert(startcont != 0);

  //List parameters with matching name (expressions allowed)
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
  {
    if(param->NameMatchesExpression(name))
    {
      startcont->PrintPath();
      cout << param->GetName() << endl;
    }
  }

  //List containers with matching name (expressions allowed)
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
  {
    if(cont->NameMatchesExpression(name))
    {
      startcont->PrintPath();
      cout << cont->GetName() << '/' << endl;
    }

    //Recurse
    ShowFinds(name, cont);
  }
}