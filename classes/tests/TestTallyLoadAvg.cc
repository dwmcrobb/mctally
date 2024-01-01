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
//!  \file TestTallyUname.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Uname unit tests
//---------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcTallyLoadAvg.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  std::array<double,3>  avgs;
  if (UnitAssert(getloadavg(avgs.data(), avgs.size()) == avgs.size())) {
    const McTally::LoadAvg  loadAvg1(avgs);
    ostringstream  os;
    if (UnitAssert(loadAvg1.Write(os))) {
      istringstream  is(os.str());
      McTally::LoadAvg  loadAvg2;
      if (UnitAssert(loadAvg2.Read(is))) {
        UnitAssert(loadAvg1 == loadAvg2);
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
  std::array<double,3>  avgs;
  if (UnitAssert(getloadavg(avgs.data(), avgs.size()) == avgs.size())) {
    const McTally::LoadAvg  loadAvg(avgs);
    UnitAssert(loadAvg.Avg1Min() == avgs[0]);
    UnitAssert(loadAvg.Avg5Min() == avgs[1]);
    UnitAssert(loadAvg.Avg15Min() == avgs[2]);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestJson()
{
  nlohmann::json  j;
  j["load1"]     = 1.1;
  j["load5"]     = 5.5;
  j["load15"]    = 15.15;

  McTally::LoadAvg  loadAvg1;
  if (UnitAssert(loadAvg1.FromJson(j))) {
    McTally::LoadAvg  loadAvg2;
    if (UnitAssert(loadAvg2.FromJson(loadAvg1.ToJson()))) {
      UnitAssert(loadAvg1 == loadAvg2);
    }
  }

  nlohmann::json  j2;
  //  Intentionally don't populate load1, FromJson(j2) should hence fail.
  j2["load5"]   = 4.4;
  j2["load15"]  = 2.2;
  UnitAssert(! loadAvg1.FromJson(j2));
  
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
