//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2023
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
//!  \file TestTallyUname.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Uname unit tests
//---------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcTallyUname.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  struct utsname   u;
  memset(&u, 0, sizeof(u));
  if (UnitAssert(uname(&u) == 0)) {
    const McTally::Uname  un1(u);
    ostringstream  os;
    if (UnitAssert(un1.Write(os))) {
      istringstream  is(os.str());
      McTally::Uname  un2;
      if (UnitAssert(un2.Read(is))) {
        UnitAssert(un1 == un2);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLocalInit()
{
  struct utsname   u;
  memset(&u, 0, sizeof(u));
  if (UnitAssert(uname(&u) == 0)) {
    const McTally::Uname  un1(u);
    UnitAssert(! un1.SysName().empty());
    UnitAssert(! un1.NodeName().empty());
    UnitAssert(! un1.Release().empty());
    UnitAssert(! un1.Version().empty());
    UnitAssert(! un1.Machine().empty());
    UnitAssert(! un1.PrettyName().empty());
    //    cerr << "PrettyName: " << un1.PrettyName() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestJson()
{
  nlohmann::json  j;
  j["osName"]     = "FreeBSD";
  j["nodeName"]   = "foo.bar.com";
  j["release"]    = "13.2-STABLE";
  j["version"]    = "FreeBSD 13.2-STABLE stable/13-n256964-42b80d160b4d kiva";
  j["machine"]    = "amd64";
  j["prettyName"] = "FreeBSD 13.2-STABLE";

  McTally::Uname  un1;
  if (UnitAssert(un1.FromJson(j))) {
    McTally::Uname  un2;
    if (UnitAssert(un2.FromJson(un1.ToJson()))) {
      UnitAssert(un1 == un2);
    }
  }

  nlohmann::json  j2;
  //  Intentionally don't populate osName, FromJson(j2) should hence fail.
  j2["nodeName"]   = "foo.bar.com";
  j2["release"]    = "13.2-STABLE";
  j2["version"]    = "FreeBSD 13.2-STABLE stable/13-n256964-42b80d160b4d kiva";
  j2["machine"]    = "amd64";
  j2["prettyName"] = "FreeBSD 13.2-STABLE";
  UnitAssert(! un1.FromJson(j2));
  
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
