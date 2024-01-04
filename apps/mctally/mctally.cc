//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2023, 2024
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
//!  \file mctally.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <iomanip>
#include <iostream>

#include "DwmCredencePeer.hh"
#include "DwmMcTallyResponse.hh"
#include "DwmMcTallyUname.hh"
#include "DwmMcTallyUtils.hh"
#include "DwmMcTallyVersion.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static vector<string> SplitArg(const string & arg)
{
  vector<string>  rc;
  size_t   startIndex = 0;
  size_t   foundIndex;
  while ((foundIndex = arg.find_first_of(',',startIndex)) != string::npos) {
    string  argpiece = arg.substr(startIndex, foundIndex - startIndex);
    if (! argpiece.empty()) {
      rc.push_back(arg.substr(startIndex, foundIndex - startIndex));
    }
    startIndex = foundIndex + 1;
  }
  if (startIndex < arg.size()) {
    string  argpiece = arg.substr(startIndex);
    if (! argpiece.empty()) {
      rc.push_back(arg.substr(startIndex));
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowRemoteVersions(const string & host,
                               const string & regExp)
{
  bool            rc = false;
  Credence::Peer  peer;
  if (peer.Connect(host, 2125)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      McTally::Request  req(regExp);
      if (peer.Send(req)) {
        McTally::Response  response;
        if (peer.Receive(response)) {
          if (response.Req().ReqEnum() == McTally::e_installedPackages) {
            const auto & installedPkgs =
              std::get<McTally::InstalledPackages>(response.Data());
            cout << host << ':' << '\n';
            for (const auto & pkg : installedPkgs.Pkgs()) {
              cout << "  " << pkg.first << ' ' << pkg.second << '\n';
            }
            rc = true;
          }
        }
        else {
          cerr << "Failed to received pkg list from " << host << '\n';
        }
      }
      else {
          cerr << "Failed to send request to " << host << '\n';
      }
    }
    else {
      cerr << "Failed to authenticate with " << host << '\n';
    }
  }
  else {
    cerr << "Failed to connect to " << host << '\n';
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ShowLoginEntries(const McTally::Logins & logins)
{
  ostringstream  oss;
  oss << setiosflags(ios::left)
      << "  " << setw(10) << "USER" << ' ' << setw(8) << "TTY" << ' '
      << setw(16) << "LOGIN_TIME" << ' ' << setw(16) << "IDLE_TIME" << ' '
      << "FROM\n";
  for (const auto & entry : logins.Entries()) {
    oss << "  " << setw(10) << entry.User() << ' '
        << setw(8) << entry.Tty() << ' '
        << setw(16) << entry.LoginTimeString() << ' '
        << setw(16) << entry.IdleTimeString() << ' '
        << entry.FromHost() << '\n';
  }
  cout << oss.str();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowLogins(string host)
{
  bool  rc = false;
  if (host.empty()) {
    McTally::Logins  logins;
    logins.SetFromUtmp();
    cout << "localhost:";
    if (! logins.Entries().empty()) {
      cout << '\n';
      ShowLoginEntries(logins);
    }
    else {
      cout << " no active logins\n";
    }
    rc = true;
  }
  else {
    Credence::Peer  peer;
    if (peer.Connect(host, 2125)) {
      Credence::KeyStash   keyStash;
      Credence::KnownKeys  knownKeys;
      if (peer.Authenticate(keyStash, knownKeys)) {
        McTally::Request  req(McTally::e_logins);
        if (peer.Send(req)) {
          McTally::Response  response;
          if (peer.Receive(response)) {
            if (response.Req().ReqEnum() == McTally::e_logins) {
              auto const & logins =
                std::get<McTally::Logins>(response.Data());
              cout << host << ":";
              if (! logins.Entries().empty()) {
                cout << '\n';
                ShowLoginEntries(logins);
              }
              else {
                cout << " no active logins\n";
              }
              rc = true;
            }
          }
          else {
            cerr << "Failed to receive logins from " << host << '\n';
          }
        }
        else {
          cerr << "Failed to send uname request to " << host << '\n';
        }
      }
      else {
        cerr << "Authentication with " << host << " failed\n";
      }
    }
    else {
      cerr << "Failed to connect to " << host << '\n';
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowLoadAverages(string host)
{
  bool  rc = false;
  if (host.empty()) {
    std::array<double,3>  avgs;
    if (getloadavg(avgs.data(), avgs.size()) == avgs.size()) {
      cout << setiosflags(ios::left)
           << "localhost  "
           << setprecision(5)
           << setw(6) << fixed << avgs.at(0) << ' '
           << setw(6) << fixed << avgs.at(1) << ' '
           << setw(6) << fixed << avgs.at(2) << '\n';
    }
  }
  else {
    Credence::Peer  peer;
    if (peer.Connect(host, 2125)) {
      Credence::KeyStash   keyStash;
      Credence::KnownKeys  knownKeys;
      if (peer.Authenticate(keyStash, knownKeys)) {
        McTally::Request  req(McTally::e_loadAverages);
        if (peer.Send(req)) {
          McTally::Response  response;
          if (peer.Receive(response)) {
            if (response.Req().ReqEnum() == McTally::e_loadAverages) {
              auto const & loadAvgs =
                std::get<McTally::LoadAvg>(response.Data());
              cout << setiosflags(ios::left)
                   << setw(31) << host << ' '
                   << setprecision(4)
                   << setw(6) << fixed << loadAvgs.Avg1Min() << ' '
                   << setw(6) << fixed << loadAvgs.Avg5Min() << ' '
                   << setw(6) << fixed << loadAvgs.Avg15Min() << '\n';
              rc = true;
            }
          }
          else {
            cerr << "Failed to receive load averages from " << host << '\n';
          }
        }
        else {
          cerr << "Failed to send load averages request to " << host << '\n';
        }
      }
      else {
        cerr << "Authentication with " << host << " failed\n";
      }
    }
    else {
      cerr << "Failed to connect to " << host << '\n';
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowUname(string host)
{
  bool            rc = false;
  if (host.empty()) {
    struct utsname  u;
    memset(&u, 0, sizeof(u));
    if (0 == uname(&u)) {
      McTally::Uname  un(u);
      cout << "sysname:    " << un.SysName() << '\n'
           << "nodename:   " << un.NodeName() << '\n'
           << "release:    " << un.Release() << '\n'
           << "version:    " << un.Version() << '\n'
           << "machine:    " << un.Machine() << '\n'
           << "prettyname: " << un.PrettyName() << '\n';
      rc = true;
    }
    else {
      cerr << "uname() failed\n";
    }
  }
  else {
    Credence::Peer  peer;
    if (peer.Connect(host, 2125)) {
      Credence::KeyStash   keyStash;
      Credence::KnownKeys  knownKeys;
      if (peer.Authenticate(keyStash, knownKeys)) {
        McTally::Request  req(McTally::e_uname);
        if (peer.Send(req)) {
          McTally::Response  response;
          if (peer.Receive(response)) {
            if (response.Req().ReqEnum() == McTally::e_uname) {
              auto const & un =
                std::get<McTally::Uname>(response.Data());
              cout << "sysname:    " << un.SysName() << '\n'
                   << "nodename:   " << un.NodeName() << '\n'
                   << "release:    " << un.Release() << '\n'
                   << "version:    " << un.Version() << '\n'
                   << "machine:    " << un.Machine() << '\n'
                   << "prettyname: " << un.PrettyName() << '\n';
              rc = true;
            }
          }
          else {
            cerr << "Failed to receive uname from " << host << '\n';
          }
        }
        else {
          cerr << "Failed to send uname request to " << host << '\n';
        }
      }
      else {
        cerr << "Authentication with " << host << " failed\n";
      }
    }
    else {
      cerr << "Failed to connect to " << host << '\n';
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  vector<string>  hosts;
  bool            getUname = false, getLogins = false, getLoadAverages = false;
  extern int      optind;
  int             optChar;

  while ((optChar = getopt(argc, argv, "h:ula")) != -1) {
    switch (optChar) {
      case 'a':
        getLoadAverages = true;
        break;
      case 'h':
        {
          vector<string>  newhosts = SplitArg(optarg);
          for (const auto & newhost : newhosts) {
            hosts.push_back(newhost);
          }
        }
        break;
      case 'l':
        getLogins = true;
        break;
      case 'u':
        getUname = true;
        break;
      default:
        break;
    }
  }

  if (getUname) {
    if (hosts.empty()) {
      if (! ShowUname("")) {
        return 1;
      }
      cout << '\n';
    }
    else {
      for (auto host : hosts) {
        if (! ShowUname(host)) {
          return 1;
        }
        cout << '\n';
      }
    }
    return 0;
  }

  if (getLogins) {
    if (hosts.empty()) {
      if (! ShowLogins("")) {
        return 1;
      }
    }
    else {
      for (auto host : hosts) {
        if (! ShowLogins(host)) {
          return 1;
        }
      }
    }
    return 0;
  }

  if (getLoadAverages) {
    if (hosts.empty()) {
      if (! ShowLoadAverages("")) {
        return 1;
      }
    }
    else {
      for (auto host : hosts) {
        if (! ShowLoadAverages(host)) {
          return 1;
        }
      }
    }
    return 0;
  }
  
  vector<string>  regExps;
  for (int n = optind; n < argc; ++n) {
    regExps.push_back(argv[n]);
  }

  if (! hosts.empty()) {
    for (auto host : hosts) {
      if (! ShowRemoteVersions(host, regExps.front())) {
        return 1;
      }
    }
    return 0;
  }
  else {
    map<string,string>  pkgs;
    McTally::Utils::GetInstalledVersions(regExps, pkgs);
    if (! pkgs.empty()) {
      for (const auto & pkg : pkgs) {
        cout << pkg.first << ' ' << pkg.second << '\n';
      }
      return 0;
    }
  }
  
  return 1;
}

