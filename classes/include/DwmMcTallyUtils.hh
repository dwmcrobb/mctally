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
//!  \file DwmMcTallyUtils.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYUTILS_HH_
#define _DWMMCTALLYUTILS_HH_

#include <map>
#include <string>
#include <vector>

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Utils 
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      static std::string GetInstalledVersion(std::string pkgname);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      static bool
      GetInstalledVersions(const std::string & regExp,
                           std::map<std::string,std::string> & pkgs);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      static bool
      GetInstalledVersions(const std::vector<std::string> & regExp,
                           std::map<std::string,std::string> & pkgs);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      static uint64_t GetUptime();

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      static std::string TimeIntervalString(uint64_t secs);
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYUTILS_HH_
