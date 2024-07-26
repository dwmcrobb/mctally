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
//!  \file DwmMcTallyResponse.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <variant>

#include <nlohmann/json.hpp>

#include "DwmStreamIOCapable.hh"
#include "DwmMcTallyRequest.hh"
#include "DwmMcTallyInstalledPackages.hh"
#include "DwmMcTallyLoadAvg.hh"
#include "DwmMcTallyLogins.hh"
#include "DwmMcTallyUname.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  Should encapsulate a response to a request.
    //------------------------------------------------------------------------
    class Response
    {
    public:
      using VariantType =
        std::variant<std::monostate,
                     LoadAvg,
                     Uname,
                     InstalledPackages,
                     Logins,
                     uint64_t>;
        
      Response();
      Response(const Response & response) = default;
      Response(Response && response) = default;
      Response & operator = (const Response &) = default;
      Response & operator = (Response &&) = default;
      ~Response() = default;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Response(const LoadAvg & avg);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Response(const Uname & uname);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Response(const InstalledPackages & pkgs);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Response(const Logins & logins);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Response(uint64_t uptime);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Request Req() const  { return _request; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const VariantType & Data() const  { return _data; }
        
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      VariantType & Data()  { return _data; }
        
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      nlohmann::json ToJson() const;

      bool FromJson(const nlohmann::json & j);
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline bool operator == (const Response & response) const
      {
        return ((_request == response._request)
                && (_data == response._data));
      }
      
    private:
      Request      _request;
      VariantType  _data;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      template <typename DT>
      bool DataFromJson(const nlohmann::json & j, const Request & req)
      {
        bool  rc = false;

        auto  dit = j.find("data");
        if (j.end() != dit) {
          DT  data;
          if (data.FromJson(*dit)) {
            _request = req;
            _data = data;
            rc = true;
          }
        }
        return rc;
      }
      
    };
    
  }  // namespace McTally

}  // namespace Dwm
