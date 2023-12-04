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
//!  \file DwmMcTallyUname.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Uname class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYUNAME_HH_
#define _DWMMCTALLYUNAME_HH_

extern "C" {
  #include <sys/utsname.h>
}

#include <array>
#include <string>

#include "DwmStreamIOCapable.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  Encapsulate uname information.  See uname(2|3) and
    //!  IEEE Std 1003.1-1988 ("POSIX.1").
    //------------------------------------------------------------------------
    class Uname
      : public StreamIOCapable
    {
    public:
      Uname() = default;
      Uname(const Uname &) = default;
      Uname(Uname &&) = default;
      Uname & operator = (const Uname &) = default;
      Uname & operator = (Uname &&) = default;
      ~Uname() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Uname(const struct utsname & u);
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & SysName() const
      { return _utsName[0]; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & SysName(const std::string & sysName)
      { return _utsName[0] = sysName; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & NodeName() const     { return _utsName[1]; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & NodeName(const std::string & nodeName)
      { return _utsName[1] = nodeName; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Release() const     { return _utsName[2]; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Release(const std::string & release)
      { return _utsName[2] = release; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Version() const     { return _utsName[3]; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Version(const std::string & version)
      { return _utsName[3] = version; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Machine() const     { return _utsName[4]; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Machine(const std::string & machine)
      { return _utsName[4] = machine; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const Uname & u) const
      { return _utsName == u._utsName; }
      
    private:
      //  [ OSName, NodeName, Release, Version, Machine ]
      std::array<std::string,5>  _utsName;
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYUNAME_HH_
