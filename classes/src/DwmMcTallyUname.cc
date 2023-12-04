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
//!  \file DwmMcTallyUname.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Uname class implementation
//---------------------------------------------------------------------------

#include <cstdio>
#include <map>
#include <regex>

#include "DwmStreamIO.hh"
#include "DwmMcTallyUname.hh"

namespace Dwm {

  namespace McTally {

#if defined(__APPLE__)

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::string GetMacOSReleaseName(const std::string & vers)
    {
      static const std::regex  rgx("([0-9]+)\\..*",
                                   std::regex::optimize
                                   |std::regex::ECMAScript);
      static const std::map<std::string,std::string>  releaseNames = {
        { "14", "Sonoma" },
        { "13", "Ventura" },
        { "12", "Monterey" },
        { "11", "Big Sur" }
      };
      std::string  rc;
      std::smatch  sm;
      if (regex_match(vers, sm, rgx)) {
        if (sm.size() == 2) {
          auto  it = releaseNames.find(sm[1].str());
          if (it != releaseNames.end()) {
            rc = it->second;
          }
        }
      }
      return rc;
    }
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::string GetPrettyName()
    {
      static const std::regex  namergx("ProductName\\:\\t\\t([a-zA-Z]+)\\n",
                                       std::regex::optimize
                                       |std::regex::ECMAScript);
      static const std::regex  versrgx("ProductVersion\\:\\t\\t([0-9]+(\\.[0-9]+)*)\\n",
                                       std::regex::optimize
                                       |std::regex::ECMAScript);
      std::string  rc;
      FILE  *p = popen("/usr/bin/sw_vers", "r");
      if (p) {
        std::string  name, vers, macReleaseName;
        char        *line = nullptr;
        size_t       linecap = 0;
        ssize_t      linelen;
        while ((linelen = getline(&line, &linecap, p)) > 0) {
          if (line) {
            std::string  linestr(line);
            std::smatch  sm;
            if (std::regex_match(linestr, sm, versrgx)) {
              if (sm.size() >= 2) {
                vers = sm[1].str();
                macReleaseName = GetMacOSReleaseName(vers);
              }
            }
            else if (std::regex_match(linestr, sm, namergx)) {
              if (sm.size() == 2) {
                name = sm[1].str();
              }
            }
            free(line);
            line = nullptr;
          }
          linecap = 0;
        }
        pclose(p);

        if ((! name.empty())
            && (! vers.empty())
            && (! macReleaseName.empty())) {
          rc = name + ' ' + vers + " (" + macReleaseName + ")";
        }
      }
      return rc;
    }
    
#else
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::string Uname::GetPrettyName()
    {
      std::string  rc;
      
      return rc;
    }
    
#endif
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Uname::Uname(const struct utsname & u)
        : _utsName({u.sysname,u.nodename,u.release,u.version,u.machine})
    {
      PrettyName(GetPrettyName());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::istream & Uname::Read(std::istream & is)
    {
      for (auto & s : _utsName) {
        s.clear();
      }
      return StreamIO::Read(is, _utsName);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & Uname::Write(std::ostream & os) const
    {
      return StreamIO::Write(os, _utsName);
    }

    
  }  // namespace McTally

}  // namespace Dwm
