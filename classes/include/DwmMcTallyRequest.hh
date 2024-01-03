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
//!  \file DwmMcTallyRequest.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Request declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYREQUEST_HH_
#define _DWMMCTALLYREQUEST_HH_

#include <cstdint>

#include <nlohmann/json.hpp>

#include "DwmStreamIOCapable.hh"
#include "DwmMcTallyPackageSelector.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  Requests that can be set to @c mctallyd.
    //!
    //!  @c e_installedPackages requests a @c map<string,string> of installed
    //!  software packages whose name matches a regular expression that's
    //!  sent immediately after the McTallyRequest.
    //!
    //!  @c e_uname requests a Uname containing uname information.
    //------------------------------------------------------------------------
    enum RequestEnum : std::uint8_t {
      e_none                = 0,
      e_installedPackages   = 1,
      e_uname               = 2,
      e_loadAverages        = 3,
      e_logins              = 4,
      e_buhBye              = 255
    };

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Request
      : public StreamIOCapable
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Request()
          : _reqEnum(e_none), _selector()
      {}
      
      Request(const Request &) = default;
      Request(Request &&) = default;
      Request & operator = (const Request &) = default;
      Request & operator = (Request &&) = default;
      ~Request() = default;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Request(RequestEnum reqEnum)
          : _reqEnum(reqEnum), _selector()
      {}
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Request(const PackageSelector & selector)
          : _reqEnum(e_installedPackages), _selector(selector)
      {}
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      RequestEnum ReqEnum() const  { return _reqEnum; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      PackageSelector Selector() const { return _selector; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is) override;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const override;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const Request & req) const
      {
        return ((_reqEnum == req._reqEnum)
                && (_selector == req._selector));
      }
        
    private:
      RequestEnum      _reqEnum;
      PackageSelector  _selector;
    };
      
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYREQUEST_HH_
