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
        : _request(e_none), _data()
    {}

    //------------------------------------------------------------------------
    Response::Response(const LoadAvg & avg)
        : _request(e_loadAverages), _data(avg)
    {}

    //------------------------------------------------------------------------
    Response::Response(const Uname & uname)
        : _request(e_uname), _data(uname)
    {}

    //------------------------------------------------------------------------
    Response::Response(const InstalledPackages & pkgs)
        : _request(pkgs.Selector()), _data(pkgs)
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Response::Response(const Logins & logins)
        : _request(e_logins), _data(logins)
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Response::Response(uint64_t uptime)
        : _request(e_uptime), _data(uptime)
    {}

    //------------------------------------------------------------------------
    std::istream & Response::Read(std::istream & is)
    {
      if (_request.Read(is)) {
        StreamIO::Read(is, _data);
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    std::ostream & Response::Write(std::ostream & os) const
    {
      if (_request.Write(os)) {
        StreamIO::Write(os, _data);
      }
      return os;
    }

    //------------------------------------------------------------------------
    nlohmann::json Response::ToJson() const
    {
      nlohmann::json  j;
      j["req"] = _request.ToJson();
      switch (_request.ReqEnum()) {
        case e_installedPackages:
          j["data"] = std::get<InstalledPackages>(_data).ToJson();
          break;
        case e_uname:
          j["data"] = std::get<Uname>(_data).ToJson();
          break;
        case e_loadAverages:
          j["data"] = std::get<LoadAvg>(_data).ToJson();
          break;
        case e_logins:
          j["data"] = std::get<Logins>(_data).ToJson();
          break;
        case e_uptime:
          j["data"] = std::get<uint64_t>(_data);
          break;
        default:
          break;
      }
      return j;
    }
    
    //------------------------------------------------------------------------
    bool Response::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      if (j.is_object()) {
        auto  rit = j.find("req");
        if ((j.end() != rit)) {
          Request  req;
          if (req.FromJson(*rit)) {
            switch (req.ReqEnum()) {
              case e_installedPackages:
                rc = DataFromJson<InstalledPackages>(j, req);
                break;
              case e_uname:
                rc = DataFromJson<Uname>(j, req);
                break;
              case e_loadAverages:
                rc = DataFromJson<LoadAvg>(j, req);
                break;
              case e_logins:
                rc = DataFromJson<Logins>(j, req);
                break;
              case e_uptime:
               {
                 auto  it = j.find("data");
                 if ((j.end() != it) && it->is_number_integer()) {
                   _request = req;
                   _data = it->get<uint64_t>();
                   rc = true;
                 }
               }
               break;
              default:
                break;
            }
          }
          else {
            std::cerr << "Failed to load request from JSON\n";
          }
        }
      }
      return rc;
    }
    
    
  }  // namespace McTally
  
}  // namespace Dwm
