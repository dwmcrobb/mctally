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
//!  \file DwmMcTallyLogins.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Logins class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/stat.h>
  #include <utmpx.h>
}

#include <cassert>
#include <mutex>

#include "DwmStreamIO.hh"
#include "DwmMcTallyLogins.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static const string  sg_devPath("/dev/");

    //------------------------------------------------------------------------
    LoginEntry::LoginEntry(const struct utmpx *u)
    {
      assert(nullptr != u);
      if (nullptr != u) {
        User(u->ut_user);
        Tty(u->ut_line);
        FromHost(u->ut_host);
        time_t  now = time((time_t *)0);
        LoginTime(u->ut_tv.tv_sec);
        IdleTime(GetIdleTime());
      }
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::string LoginEntry::IdleTimeString() const
    {
      std::string  rc;
      uint64_t     secsRemaining = IdleTime();
      uint64_t     days = secsRemaining / (24 * 3600);
      if (days) {
        rc += std::to_string(days) + 'd';
        secsRemaining -= days * (24 * 3600);
      }
      uint64_t  hours = secsRemaining / 3600;
      if (hours) {
        rc += std::to_string(hours) + 'h';
        secsRemaining -= hours * 3600;
      }
      uint64_t  minutes = secsRemaining / 60;
      if (minutes) {
        rc += std::to_string(minutes) + 'm';
        secsRemaining -= minutes * 60;
      }
      rc += std::to_string(secsRemaining) + 's';
      return rc;
    }

    //------------------------------------------------------------------------
    std::string LoginEntry::LoginTimeString() const
    {
      std::string  rc;
      time_t       now = time((time_t *)0);
      time_t       loginTime = LoginTime();
      if (now >= loginTime) {
        struct tm  tm;
        localtime_r(&loginTime, &tm);
        if ((now - loginTime) < (24 * 3600)) {
          char  buf[8];
          if (strftime(buf, sizeof(buf), "%H:%M", &tm)) {
            rc = buf;
          }
        }
        else if ((now - loginTime) < (7 * 24 * 3600)) {
          char  buf[16];
          if (strftime(buf, sizeof(buf), "%a %H:%M", &tm)) {
            rc = buf;
          }
        }
        else {
          char  buf[32];
          if (strftime(buf, sizeof(buf), "%b %d %H:%M", &tm)) {
            rc = buf;
          }
        }
      }
      else {
        rc = "?";
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    std::istream & LoginEntry::Read(std::istream & is)
    { return StreamIO::Read(is, _data); }
    
    //------------------------------------------------------------------------
    std::ostream & LoginEntry::Write(std::ostream & os) const
    { return StreamIO::Write(os, _data); }
      
    //------------------------------------------------------------------------
    nlohmann::json LoginEntry::ToJson() const
    {
      nlohmann::json  j;
      j["user"] = User();
      j["tty"] = Tty();
      j["fromHost"] = FromHost();
      j["loginTime"] = LoginTime();
      j["idleTime"] = IdleTime();
      return j;
    }
    
    //------------------------------------------------------------------------
    bool LoginEntry::FromJson(const nlohmann::json & j)
    { 
      bool  rc = false;
      if (j.is_object()) {
        auto  uit = j.find("user");
        if ((j.end() != uit) && uit->is_string()) {
          auto  yit = j.find("tty");
          if ((j.end() != yit) && yit->is_string()) {
            auto  fit = j.find("fromHost");
            if ((j.end() != fit) && fit->is_string()) {
              auto  lit = j.find("loginTime");
              if ((j.end() != lit) && lit->is_number_integer()) {
                auto  iit = j.find("idleTime");
                if ((j.end() != iit) && iit->is_number_integer()) {
                  User(uit->get<string>());
                  Tty(yit->get<string>());
                  FromHost(fit->get<string>());
                  LoginTime(lit->get<uint64_t>());
                  IdleTime(iit->get<uint64_t>());
                  rc = true;
                }
              }
            }
          }
        }
      }
      return rc;
    }
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint64_t LoginEntry::GetIdleTime() const
    {
      uint64_t     rc = 0;
      string       ttypath(sg_devPath + Tty());
      struct stat  sb;
      if (stat(ttypath.c_str(), &sb) == 0 && S_ISCHR(sb.st_mode)) {
        time_t  now = time((time_t *)0);
        if (sb.st_atime > LoginTime()) {
          rc = now - sb.st_atime;
        }
        else if (now >= LoginTime()) {
          rc = now - LoginTime();
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    void Logins::SetFromUtmp()
    {
      static std::mutex            mtx;
      bool                         rc = false;
      struct utmpx                *u = nullptr;
      _entries.clear();
      std::lock_guard<std::mutex>  lock(mtx);
      while (nullptr != (u = getutxent())) {
        if (USER_PROCESS == u->ut_type) {
          _entries.emplace_back(LoginEntry(u));
        }
      }
      endutxent();
      if (! _entries.empty()) {
        std::sort(_entries.begin(), _entries.end(),
                  [] (const auto & a, const auto & b) 
                  { return a.IdleTime() < b.IdleTime(); });
      }
      return;
    }
    
    //------------------------------------------------------------------------
    istream & Logins::Read(istream & is)
    {
      return StreamIO::Read(is, _entries);
    }
    
    //------------------------------------------------------------------------
    ostream & Logins::Write(ostream & os) const
    {
      return StreamIO::Write(os, _entries);
    }
    
    //------------------------------------------------------------------------
    nlohmann::json Logins::ToJson() const
    {
      nlohmann::json  j = nlohmann::json::array();
      for (size_t i = 0; i < _entries.size(); ++i) {
        j[i] = _entries[i].ToJson();
      }
      return j;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Logins::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      _entries.clear();
      if (j.is_array()) {
        size_t  i = 0;
        for ( ; i < j.size(); ++i) {
          LoginEntry  loginEntry;
          if (loginEntry.FromJson(j[i])) {
            _entries.push_back(loginEntry);
          }
          else {
            cerr << "Failed to create LoginEntry\n";
            break;
          }
        }
        rc = (j.size() == i);
      }
      return rc;
    }
    
              
    
  }  // namespace McTally

}  // namespace Dwm
