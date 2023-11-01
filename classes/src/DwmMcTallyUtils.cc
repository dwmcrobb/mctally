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
//!  \file DwmMcTallyUtils.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <cctype>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <regex>
    
#include "DwmMcTallyUtils.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static string GetInstalledVersionFreeBSD(string pkgname)
    {
      string  rc;
      string  cmd("pkg info -I " + pkgname + " 2>/dev/null");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        size_t  bytesRead = fread(buf, 1, 1024, p);
        if (bytesRead > 0) {
          string  bufstr(buf);
          auto  idx = bufstr.find_first_of(' ');
          if ((string::npos != idx)
              && ((idx + 1) < bufstr.size())) {
            bufstr = bufstr.substr(0, idx + 1);
          }
          idx = bufstr.find_last_of('-');
          if ((string::npos != idx)
              && ((idx + 1) < bufstr.size())) {
            rc = bufstr.substr(idx + 1, bufstr.size() - idx);
          }
        }
        pclose(p);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static string GetInstalledVersionMacOS(string pkgname)
    {
      string  rc;
      string  cmd("pkgutil --pkg-info " + pkgname);
      char    buf[1024] = { 0 };
      regex   rgx("^version\\: ([^ ]+)\n",
                  regex::ECMAScript | regex::optimize);
      smatch  sm;
      FILE   *p = popen(cmd.c_str(), "r");
      if (p) {
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          if (regex_match(bufstr, sm, rgx)) {
            if (sm.size() == 2) {
              rc = sm[1].str();
              break;
            }
          }
          memset(buf, 0, sizeof(buf));
        }
        pclose(p);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static string GetInstalledVersionDebian(string pkgname)
    {
      transform(pkgname.begin(), pkgname.end(), pkgname.begin(), 
                [](unsigned char c){ return tolower(c); });
      string  rc;
      string  cmd("dpkg-query -W -f '${Package} ${Version}\n' ");
      cmd += pkgname + " 2>/dev/null";
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        size_t  bytesRead = fread(buf, 1, 1024, p);
        if (bytesRead > 0) {
          string  bufstr(buf);
          while ('\n' == *(bufstr.rbegin())) {
            bufstr.resize(bufstr.size() - 1);
          }
          regex   rgx("([^ ]+) ([^ ]+)",
                      regex::ECMAScript | regex::optimize);
          smatch  sm;
          if (regex_match(bufstr, sm, rgx)) {
            if (sm.size() == 3) {
              rc = sm[2].str();
            }
          }
        }
        pclose(p);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    string Utils::GetInstalledVersion(string pkgname)
    {
      string  rc;
#if defined(__APPLE__)
      rc = GetInstalledVersionMacOS(pkgname);
#elif defined(__FreeBSD__)
      rc = GetInstalledVersionFreeBSD(pkgname);
#elif defined(__linux__)
      rc = GetInstalledVersionDebian(pkgname);
#endif
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetInstalledVersionsMacOS(const string & regExp,
                                          map<string,string> & pkgs)
    {
      string  cmd("pkgutil --pkgs='" + regExp + "'");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          while ('\n' == *(bufstr.rbegin())) {
            bufstr.resize(bufstr.size() - 1);
          }
          string  vers(GetInstalledVersionMacOS(bufstr));
          if (! vers.empty()) {
            pkgs[bufstr] = vers;
          }
        }
        pclose(p);
      }
      return (! pkgs.empty());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetInstalledVersionsMacOS(const vector<string> & regExps,
                                          map<string,string> & pkgs)
    {
      string  cmd("pkgutil --pkgs");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char    buf[1024] = { 0 };
        vector<regex>  rgxvec;
        for (const auto & regExp : regExps) {
          rgxvec.emplace(rgxvec.end(),
                         regex(regExp, regex::ECMAScript | regex::optimize));
        }
        smatch  sm;
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          while ('\n' == *(bufstr.rbegin())) {
            bufstr.resize(bufstr.size() - 1);
          }
          for (const auto & rgx : rgxvec) {
            if (regex_match(bufstr, sm, rgx)) {
              string  vers(GetInstalledVersionMacOS(bufstr));
              if (! vers.empty()) {
                pkgs[bufstr] = vers;
                break;
              }
            }
          }
        }
        pclose(p);
      }
      return (! pkgs.empty());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetInstalledVersionsFreeBSD(const string & regExp,
                                            map<string,string> & pkgs)
    {
      string  cmd("pkg info -Ia 2>/dev/null");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          auto  idx = bufstr.find_first_of(' ');
          if ((string::npos != idx)
              && ((idx + 1) < bufstr.size())) {
            bufstr = bufstr.substr(0, idx);
          }
          try {
            idx = bufstr.find_last_of('-');
            if ((string::npos != idx)
                && ((idx + 1) < bufstr.size())) {
              string  pkgname(bufstr.substr(0,idx));
              string  pkgvers(bufstr.substr(idx+1,bufstr.size() - idx));
              regex   rgx(regExp, regex::ECMAScript | regex::optimize);
              smatch  sm;
              if (regex_match(pkgname, sm, rgx)) {
                pkgs[pkgname] = pkgvers;
              }
            }
          }
          catch (...) {
          }
          memset(buf, 0, sizeof(buf));
        }
        pclose(p);
      }
      return (! pkgs.empty());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetInstalledVersionsDebian(const string & regExp,
                                           map<string,string> & pkgs)
    {
      string  cmd("dpkg-query -W -f '${Package} ${Version}\n' 2>/dev/null");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          auto  idx = bufstr.find_first_of(' ');
          if ((string::npos != idx)
              && ((idx + 1) < bufstr.size())) {
            while ('\n' == *(bufstr.rbegin())) {
              bufstr.resize(bufstr.size() - 1);
            }
            try {
              regex   rgx(regExp,
                          regex::ECMAScript | regex::optimize | regex::icase);
              smatch  sm;
              string  pkgname(bufstr.substr(0, idx));
              if (regex_match(pkgname, sm, rgx)) {
                pkgs[pkgname] = bufstr.substr(idx + 1, bufstr.size() - idx);
              }
            }
            catch (...) {
              break;
            }
          }
          memset(buf, 0, sizeof(buf));
        }
        pclose(p);
      }
      return (! pkgs.empty());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetInstalledVersionsDebian(const vector<string> & regExps,
                                           map<string,string> & pkgs)
    {
      string  cmd("dpkg-query -W -f '${Package} ${Version}\n' 2>/dev/null");
      FILE  *p = popen(cmd.c_str(), "r");
      if (p) {
        char  buf[1024] = { 0 };
        while (fgets(buf, sizeof(buf), p)) {
          string  bufstr(buf);
          auto  idx = bufstr.find_first_of(' ');
          if ((string::npos != idx)
              && ((idx + 1) < bufstr.size())) {
            while ('\n' == *(bufstr.rbegin())) {
              bufstr.resize(bufstr.size() - 1);
            }
            string  pkgname(bufstr.substr(0, idx));
            string  pkgvers(bufstr.substr(idx + 1, bufstr.size() - idx));
            for (const auto & rgxstr : regExps) {
              try {
                regex  rgx(rgxstr,
                           regex::ECMAScript
                           | regex::optimize
                           | regex::icase);
                smatch  sm;
                if (regex_match(pkgname, sm, rgx)) {
                  pkgs[pkgname] = pkgvers;
                }
              }
              catch (...) {
              }
            }
          }
          memset(buf, 0, sizeof(buf));
        }
        pclose(p);
      }
      return (! pkgs.empty());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Utils::GetInstalledVersions(const string & regExp,
                                     map<string,string> & pkgs)
    {
#if defined(__APPLE__)
      return GetInstalledVersionsMacOS(regExp, pkgs);
#elif defined(__FreeBSD__)
      return GetInstalledVersionsFreeBSD(regExp, pkgs);
#elif defined(__linux__)
      return GetInstalledVersionsDebian(regExp, pkgs);
#endif
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Utils::GetInstalledVersions(const vector<string> & regExps,
                                     map<string,string> & pkgs)
    {
#if defined(__APPLE__)
      return GetInstalledVersionsMacOS(regExps, pkgs);
#elif defined(__FreeBSD__)
      return GetInstalledVersionsFreeBSD(regExps, pkgs);
#elif defined(__linux__)
      return GetInstalledVersionsDebian(regExps, pkgs);
#endif
    }
    
    
  }  // namespace McTally

}  // namespace Dwm
