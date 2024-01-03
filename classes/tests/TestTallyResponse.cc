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
//!  \file TestTallyResponse.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Response unit tests
//---------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcTallyResponse.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
void IOTestTemplate(const T & value)
{
  McTally::Response  response1(value);
  ostringstream  os;
  if (UnitAssert(response1.Write(os))) {
    istringstream  is(os.str());
    McTally::Response  response2;
    if (UnitAssert(response2.Read(is))) {
      UnitAssert(response1 == response2);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLoginsIO()
{
  McTally::Logins  logins1;
  logins1.SetFromUtmp();
  UnitAssert(! logins1.Entries().empty());
  IOTestTemplate(logins1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestUnameIO()
{
  struct utsname   u;
  memset(&u, 0, sizeof(u));
  if (UnitAssert(uname(&u) == 0)) {
    const McTally::Uname  un1(u);
    IOTestTemplate(un1);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLoadAvgsIO()
{
  std::array<double,3>  avgs;
  if (UnitAssert(getloadavg(avgs.data(), avgs.size()) == avgs.size())) {
    const McTally::LoadAvg  loadAvg1(avgs);
    IOTestTemplate(loadAvg1);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestInstalledPackagesIO()
{
  McTally::InstalledPackages  pkgs1;
  pkgs1.Selector("libDwm.*|mc.*|DwmDns");
  pkgs1.Pkgs()["libDwm"] = "0.0.1";
  pkgs1.Pkgs()["libDwmCredence"] = "0.0.2";
  pkgs1.Pkgs()["mcloc"] = "0.0.3";
  pkgs1.Pkgs()["mctally"] = "0.0.4";
  pkgs1.Pkgs()["mcrover"] = "0.1.0";
  pkgs1.Pkgs()["DwmDns"] = "0.2.0";
  pkgs1.Pkgs()["mcweather"] = "0.3.0";

  IOTestTemplate(pkgs1);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  TestLoginsIO();
  TestUnameIO();
  TestLoadAvgsIO();
  TestInstalledPackagesIO();
  
  return;
}

#if 0
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLocalInit()
{
  time_t  now = time((time_t *)0);
  McTally::Logins  logins1;
  logins1.SetFromUtmp();
  if (UnitAssert(! logins1.Entries().empty())) {
    for (const auto & login : logins1.Entries()) {
      UnitAssert(! login.User().empty());
      UnitAssert(! login.Tty().empty());
      // UnitAssert(! login.FromHost().empty());  // empty on local host
      UnitAssert(login.LoginTime() > 0);
      UnitAssert(login.LoginTime() <= (now + 5));
    }
  }
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

  //  Should fail on empty JSON
  nlohmann::json  j2;
  UnitAssert(! logins1.FromJson(j2));
  
  return;
}

#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int  rc = 1;

  //  TestLocalInit();
  TestIO();
  //  TestJson();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  return rc;

}
