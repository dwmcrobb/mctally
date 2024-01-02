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
//!  \file TestTallyInstalledPackages.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::InstalledPackages unit tests
//---------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcTallyInstalledPackages.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  McTally::InstalledPackages  pkgs1;
  pkgs1.Pkgs()["libDwm"] = "0.0.1";
  pkgs1.Pkgs()["libDwmCredence"] = "0.0.2";
  pkgs1.Pkgs()["mcloc"] = "0.0.3";
  pkgs1.Pkgs()["mctally"] = "0.0.4";
  pkgs1.Pkgs()["mcrover"] = "0.1.0";
  pkgs1.Pkgs()["DwmDns"] = "0.2.0";
  pkgs1.Pkgs()["mcweather"] = "0.3.0";
  
  ostringstream  os;
  if (UnitAssert(pkgs1.Write(os))) {
    istringstream  is(os.str());
    McTally::InstalledPackages  pkgs2;
    if (UnitAssert(pkgs2.Read(is))) {
      UnitAssert(pkgs1 == pkgs2);
      UnitAssert(pkgs2.Pkgs().size() == 7);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestJson()
{
  nlohmann::json  j;
  j[0]["pkg"]     = "libDwm";          j[0]["ver"] = "0.0.1";
  j[1]["pkg"]     = "libDwmCredence";  j[1]["ver"] = "0.0.2";
  j[2]["pkg"]     = "mcloc";           j[2]["ver"] = "0.0.3";
  j[3]["pkg"]     = "mctally";         j[3]["ver"] = "0.0.4";
  j[4]["pkg"]     = "mcrover";         j[4]["ver"] = "0.1.0";
  j[5]["pkg"]     = "DwmDns";          j[5]["ver"] = "0.2.0";
  j[6]["pkg"]     = "mcweather";       j[6]["ver"] = "0.3.0";
  
  McTally::InstalledPackages  pkgs1;
  if (UnitAssert(pkgs1.FromJson(j))) {
    McTally::InstalledPackages  pkgs2;
    if (UnitAssert(pkgs2.FromJson(pkgs1.ToJson()))) {
      UnitAssert(pkgs1 == pkgs2);
    }
  }

  return;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int  rc = 1;

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
