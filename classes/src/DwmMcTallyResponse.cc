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
//!  \file DwmMcTallyResponse.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Response class implementation
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcTallyResponse.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    Response::Response()
        : _request(Request::e_none), _data()
    {}

    //------------------------------------------------------------------------
    Response::Response(const LoadAvg & avg)
        : _request(Request::e_loadAverages), _data(avg)
    {}

    //------------------------------------------------------------------------
    Response::Response(const Uname & uname)
        : _request(Request::e_uname), _data(uname)
    {}

    //------------------------------------------------------------------------
    Response::Response(const InstalledPackages & pkgs)
        : _request(Request::e_installedPackages), _data(pkgs)
    {}

    Response::Response(const Logins & logins)
        : _request(Request::e_logins), _data(logins)
    {}
    
    //------------------------------------------------------------------------
    std::istream & Response::Read(std::istream & is)
    {
      if (StreamIO::Read(is, (uint8_t &)_request)) {
        StreamIO::Read(is, _data);
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    std::ostream & Response::Write(std::ostream & os) const
    {
      if (StreamIO::Write(os, _request)) {
        StreamIO::Write(os, _data);
      }
      return os;
    }

  }  // namespace McTally
  
}  // namespace Dwm
