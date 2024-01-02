//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file TestTallyLogins.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Logins unit tests
//---------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcTallyLogins.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  McTally::Logins  logins1;
  logins1.SetFromUtmp();
  UnitAssert(! logins1.Entries().empty());
  ostringstream  os;
  if (UnitAssert(logins1.Write(os))) {
    istringstream  is(os.str());
    McTally::Logins  logins2;
    if (UnitAssert(logins2.Read(is))) {
      UnitAssert(logins1 == logins2);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLocalInit()
{
  McTally::Logins  logins1;
  logins1.SetFromUtmp();
  UnitAssert(! logins1.Entries().empty());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestJson()
{
  time_t  now = time((time_t *)0);
  nlohmann::json  j = nlohmann::json::array();
  j[0]["user"] = "dwm";
  j[0]["tty"]  = "tty00";
  j[0]["fromHost"] = "localhost";
  j[0]["loginTime"] = now;
  j[0]["idleTime"] = 0;
  
  j[1]["user"] = "root";
  j[1]["tty"]  = "ttys01";
  j[1]["fromHost"] = "";
  j[1]["loginTime"] = now;
  j[1]["idleTime"] = 60;

  McTally::Logins  logins1;
  if (UnitAssert(logins1.FromJson(j))) {
    if (UnitAssert(logins1.Entries().size() == 2)) {
      UnitAssert(logins1.Entries()[0].User() == "dwm");
      UnitAssert(logins1.Entries()[0].Tty() == "tty00");
      UnitAssert(logins1.Entries()[0].FromHost() == "localhost");
      UnitAssert(logins1.Entries()[0].LoginTime() == now);
      UnitAssert(logins1.Entries()[0].IdleTime() == 0);
      UnitAssert(logins1.Entries()[1].User() == "root");
      UnitAssert(logins1.Entries()[1].Tty() == "ttys01");
      UnitAssert(logins1.Entries()[1].FromHost() == "");
      UnitAssert(logins1.Entries()[1].LoginTime() == now);
      UnitAssert(logins1.Entries()[1].IdleTime() == 60);
    
      McTally::Logins  logins2;
      if (UnitAssert(logins2.FromJson(logins1.ToJson()))) {
        UnitAssert(logins1 == logins2);
      }
    }
  }

  nlohmann::json  j2;
  UnitAssert(! logins1.FromJson(j2));
  
  return;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int  rc = 1;

  TestLocalInit();
  TestIO();
  TestJson();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  return rc;

}
