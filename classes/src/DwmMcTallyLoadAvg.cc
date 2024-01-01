//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2023, 2024
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
//!  \file DwmMcTallyLoadAvg.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::LoadAvg class implementation
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcTallyLoadAvg.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    LoadAvg::LoadAvg()
    {
      for (auto & avg : _loadAvgs)  { avg = -1; }
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    istream & LoadAvg::Read(istream & is)
    {
      return StreamIO::Read(is, _loadAvgs);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ostream & LoadAvg::Write(ostream & os) const
    {
      return StreamIO::Write(os, _loadAvgs);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    nlohmann::json LoadAvg::ToJson() const
    {
      nlohmann::json  j;
      j["load1"]  = _loadAvgs.at(0);
      j["load5"]  = _loadAvgs.at(1);
      j["load15"] = _loadAvgs.at(2);
      return j;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool LoadAvg::FromJson(const nlohmann::json & j)
    {
      static const array<string,3>  fieldNames = {
        "load1", "load5", "load15"
      };
      assert(fieldNames.size() == _loadAvgs.size());

      bool  rc = false;
      for (auto & avg : _loadAvgs) { avg = -1; }
      if (j.is_object()) {
        size_t  i = 0;
        for ( ; i < fieldNames.size(); ++i) {
          auto  it = j.find(fieldNames.at(i));
          if ((it != j.end()) && it->is_number_float()) {
            _loadAvgs.at(i) = it->get<double>();
          }
          else {
            break;
          }
        }
        rc = (fieldNames.size() == i);
      }
      return rc;
    }
    
  }  // namespace McTally

}  // namespace Dwm
