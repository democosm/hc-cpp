// HC client application
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

#include "str.hh"
#include "hcaggregator.hh"
#include "hcconsole.hh"
#include <getopt.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
  HCAggregator* agg;
  HCConsole* hccons;

  //Check for wrong number of arguments
  if(argc != 2)
  {
    cout << "Usage: " << argv[0] << " <AGGREGATOR FILE NAME>" << "\n";
    return -1;
  }

  //Initialize SSL
  SSL_load_error_strings();	
  OpenSSL_add_ssl_algorithms();

  //Create aggregator
  agg = new HCAggregator(argv[1]);

  //Create HC console
  hccons = new HCConsole(agg->GetTopCont());

  //Run console in context of this thread
  hccons->Run();

  //Cleanup
  delete hccons;
  delete agg;

  //Cleanup SSL
  EVP_cleanup();

  return 0;
}
