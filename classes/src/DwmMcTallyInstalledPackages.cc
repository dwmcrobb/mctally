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
//!  \file DwmMcTallyInstalledPackages.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::InstalledPackages class implementation
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcTallyInstalledPackages.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;
    
    //------------------------------------------------------------------------
    istream & InstalledPackages::Read(istream & is)
    {
      if (StreamIO::Read(is, _selector)) {
        StreamIO::Read(is, _pkgs);
      }
      return is;
    }

    //------------------------------------------------------------------------
    ostream & InstalledPackages::Write(ostream & os) const
    {
      if (StreamIO::Write(os, _selector)) {
        StreamIO::Write(os, _pkgs);
      }
      return os;
    }
      
    //------------------------------------------------------------------------
    nlohmann::json InstalledPackages::ToJson() const
    {
      nlohmann::json  j;
      j["selector"] = _selector;
      size_t  i = 0;
      j["pkgs"] = nlohmann::json::array();
      for (const auto & pkg : _pkgs) {
        j["pkgs"][i]["pkg"] = pkg.first;
        j["pkgs"][i]["ver"] = pkg.second;
        ++i;
      }
      return j;
    }

    //------------------------------------------------------------------------
    bool InstalledPackages::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      _pkgs.clear();

      if (! j.is_object())                               { return false; }

      auto  ait = j.find("selector");
      if ((j.end() == ait) || (! ait->is_string()))      { return false; }
      
      _selector = ait->get<string>();
      
      ait = j.find("pkgs");
      if ((j.end() == ait) || (! ait->is_array()))       { return false; }
      
      size_t  i = 0;
      for ( ; i < ait->size(); ++i) {
        if ((*ait)[i].is_object()) {
          auto  pit = (*ait)[i].find("pkg");
          if ((pit != (*ait)[i].end()) && pit->is_string()) {
            auto  vit = (*ait)[i].find("ver");
            if ((vit != (*ait)[i].end()) && vit->is_string()) {
              _pkgs[pit->get<string>()] = vit->get<string>();
            }
            else { break; }
          }
          else { break; }
        }
        else { break; }
      }
      return (ait->size() == i);
    }
                
  }  // namespace McTally

}  // namespace Dwm
