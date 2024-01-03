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
//!  \file DwmMcTallyLogins.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYLOGINS_HH_
#define _DWMMCTALLYLOGINS_HH_

#include <string>
#include <tuple>
#include <vector>

#include <nlohmann/json.hpp>

#include "DwmStreamIOCapable.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class LoginEntry
      : public StreamIOCapable
    {
    public:
      LoginEntry() = default;
      LoginEntry(const LoginEntry &) = default;
      LoginEntry(LoginEntry &&) = default;
      LoginEntry & operator = (const LoginEntry &) = default;
      LoginEntry & operator = (LoginEntry &&) = default;
      ~LoginEntry() = default;

      LoginEntry(const struct utmpx *u);
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & User() const  { return std::get<0>(_data); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & User(const std::string & user)
      { return (std::get<0>(_data) = user); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Tty() const  { return std::get<1>(_data); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Tty(const std::string & tty)
      { return (std::get<1>(_data) = tty); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & FromHost() const  { return std::get<2>(_data); }
 
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & FromHost(const std::string & fromHost)
      { return (std::get<2>(_data) = fromHost); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint64_t LoginTime() const  { return std::get<3>(_data); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint64_t LoginTime(uint64_t loginTime)
      { return (std::get<3>(_data) = loginTime); }

      std::string LoginTimeString() const;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint64_t IdleTime() const  { return std::get<4>(_data); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint64_t IdleTime(uint64_t idleTime)
      { return (std::get<4>(_data) = idleTime); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::string IdleTimeString() const;
      
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
      inline bool operator == (const LoginEntry & loginEntry) const
      { return (_data == loginEntry._data); }
        
    private:
      //  user, tty, host, loginTime, idleTime
      std::tuple<std::string,std::string,std::string,uint64_t,uint64_t>  _data;

      uint64_t GetIdleTime() const;
    };
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Logins
      : public StreamIOCapable
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<LoginEntry> & Entries() const
      { return _entries; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::vector<LoginEntry> & Entries()
      { return _entries; }

      void SetFromUtmp();
      
      std::istream & Read(std::istream & is) override;
      std::ostream & Write(std::ostream & os) const override;
      nlohmann::json ToJson() const;
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline bool operator == (const Logins & logins) const
      { return (_entries == logins._entries); }
      
    private:
      std::vector<LoginEntry>  _entries;
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYLOGINS_HH_
