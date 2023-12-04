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
//!  \file mctally.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <iostream>

#include "DwmCredencePeer.hh"
#include "DwmMcTallyRequest.hh"
#include "DwmMcTallyUname.hh"
#include "DwmMcTallyUtils.hh"
#include "DwmMcTallyVersion.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowRemoteVersions(const string & host,
                               const vector<string> & regExps)
{
  bool            rc = false;
  Credence::Peer  peer;
  if (peer.Connect(host, 2125)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      uint8_t  req = McTally::e_installedPackages;
      if (peer.Send(req)) {
        if (peer.Send(regExps)) {
          map<string,string>  pkgs;
          if (peer.Receive(pkgs)) {
            rc = true;
            for (const auto & pkg : pkgs) {
              cout << pkg.first << ' ' << pkg.second << '\n';
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
static bool ShowUname(string host)
{
  bool            rc = false;
  if (host.empty()) {
    struct utsname  u;
    memset(&u, 0, sizeof(u));
    if (0 == uname(&u)) {
      McTally::Uname  un(u);
      cout << "sysname:  " << un.SysName() << '\n'
           << "nodename: " << un.NodeName() << '\n'
           << "release:  " << un.Release() << '\n'
           << "version:  " << un.Version() << '\n'
           << "machine:  " << un.Machine() << '\n';
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
        uint8_t  req = McTally::e_uname;
        if (peer.Send(req)) {
          McTally::Uname  un;
          if (peer.Receive(un)) {
            cout << "sysname:  " << un.SysName() << '\n'
                 << "nodename: " << un.NodeName() << '\n'
                 << "release:  " << un.Release() << '\n'
                 << "version:  " << un.Version() << '\n'
                 << "machine:  " << un.Machine() << '\n';
            rc = true;
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
  string       host;
  bool         getUname = false;
  extern int   optind;
  int          optChar;

  while ((optChar = getopt(argc, argv, "h:u")) != -1) {
    switch (optChar) {
      case 'h':
        host = optarg;
        break;
      case 'u':
        getUname = true;
        break;
      default:
        break;
    }
  }

  if (getUname) {
    if (ShowUname(host)) {
      return 0;
    }
    else {
      return 1;
    }
  }
  
  vector<string>  regExps;
  for (int n = optind; n < argc; ++n) {
    regExps.push_back(argv[n]);
  }

  if (! host.empty()) {
    if (ShowRemoteVersions(host, regExps)) {
      return 0;
    }
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

