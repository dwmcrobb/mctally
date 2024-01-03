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
//!  \file DwmMcTallyRequest.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Request class implementation
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcTallyRequest.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    std::istream & Request::Read(std::istream & is)
    {
      _reqEnum = e_none;
      _selector.clear();
      if (StreamIO::Read(is, (uint8_t &)_reqEnum)) {
        if (e_installedPackages == _reqEnum) {
          StreamIO::Read(is, _selector);
        }
      }
      return is;
    }

    //------------------------------------------------------------------------
    std::ostream & Request::Write(std::ostream & os) const
    {
      if (StreamIO::Write(os, (const uint8_t &)_reqEnum)) {
        if (e_installedPackages == _reqEnum) {
          StreamIO::Write(os, _selector);
        }
      }
      return os;
    }

    //------------------------------------------------------------------------
    nlohmann::json Request::ToJson() const
    {
      nlohmann::json  j;
      j["reqEnum"] = _reqEnum;
      if (e_installedPackages == _reqEnum) {
        j["selector"] = _selector;
      }
      return j;
    }

    //------------------------------------------------------------------------
    bool Request::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      _reqEnum = e_none;
      _selector.clear();
      
      if (j.is_object()) {
        auto  eit = j.find("reqEnum");
        if ((j.end() != eit) && eit->is_number_integer()) {
          _reqEnum = (RequestEnum)eit->get<uint8_t>();
          if (e_installedPackages == _reqEnum) {
            auto  sit = j.find("selector");
            if ((j.end() != sit) && sit->is_string()) {
              _selector = sit->get<std::string>();
              rc = true;
            }
          }
          else {
            rc = true;
          }
        }
      }
      return rc;
    }
    
    
  }  // namespace McTally

}  // namespace Dwm
