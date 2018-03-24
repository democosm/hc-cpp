// Error
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

#ifndef _ERROR_HH_
#define _ERROR_HH_

#include <string>

using namespace std;

//Definitions
#define ERR_NONE      0
#define ERR_UNSPEC    -1
#define ERR_TIMEOUT   -2
#define ERR_OWNER     -3
#define ERR_RESET     -4
#define ERR_DESTROYED -5
#define ERR_OVERFLOW  -6
#define ERR_TYPE      -7
#define ERR_PATTERN   -8
#define ERR_ACCESS    -9
#define ERR_RANGE     -10
#define ERR_STEP      -11
#define ERR_INVALID   -12
#define ERR_ALIGNMENT -13
#define ERR_DESER     -14
#define ERR_OPCODE    -15
#define ERR_PID       -16
#define ERR_EID       -17
#define ERR_NOTFOUND  -18
#define ERR_UNKNOWN   -19 //Don't use. Must be last

//Functions
const string ErrToString(int err);

#endif //_ERROR_HH_
