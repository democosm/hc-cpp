// HC XML application
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

#include "tinyxml2.hh"
#include <iostream>

using namespace tinyxml2;
using namespace std;

void Usage(const char *appname)
{
  cout << "Usage: " << appname << " <XML FILE NAME>\n";
}

bool ParseElement(XMLElement *pelt)
{
  XMLElement *elt;
  XMLElement *nameelt;
  const char *name;

  //Check for null parent object
  if(pelt == 0)
    return false;

  //Check name of element and process it appropriately
  if(pelt->GetText() == 0)
  {
    //Check for special enums element
    if((strcmp(pelt->Name(), "valenums") == 0) || (strcmp(pelt->Name(), "eidenums") == 0))
    {
      //Print list item header with parameter name
      cout << "<li>" << pelt->Name() << "\n";

      //Print unordered list header
      cout << "<ul>\n";

      //Loop through all children
      for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
        ParseElement(elt);

      //Print unordered list footer
      cout << "</ul>\n";

      //Print list item footer
      cout << "</li>\n";
    }

    //Find name element and check for error
    if((nameelt = pelt->FirstChildElement("name")) == 0)
      return false;

    //Find name text and check for error
    if((name = nameelt->GetText()) == 0)
      return false;

    //Print list item header with parameter name
    cout << "<li>" << name << "\n";

    //Print unordered list header
    cout << "<ul>\n";

    //Print type (element name is actually parameter type)
    cout << "<li>type - " << pelt->Name() << "</li>\n";

    //Loop through all children
    for(elt = pelt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
      ParseElement(elt);

    //Print unordered list footer
    cout << "</ul>\n";

    //Print list item footer
    cout << "</li>\n";
  }
  else
  {
    //Don't print name since it was printed already
    if(strcmp(pelt->Name(), "name") != 0)
      cout << "<li>" << pelt->Name() << " - " << pelt->GetText() << "</li>\n";
  }

  //Return success
  return true;
}

int main(int argc, char **argv)
{
  XMLDocument doc;

  //Check for wrong number of arguments
  if(argc != 2)
  {
    Usage(argv[0]);
    return -1;
  }

  //Parse file and check for error
  if(doc.LoadFile(argv[1]) != 0)
  {
    cout << "Error parsing file (\"" << argv[1] << "\")\n";
    return -1;
  }

  //Print unordered list header
  cout << "<ul id=\"hctree\">\n";

  //Parse server from DOM
  ParseElement(doc.FirstChildElement("server"));

  //Print unordered list footer
  cout << "</ul>\n";

  return 0;
}
