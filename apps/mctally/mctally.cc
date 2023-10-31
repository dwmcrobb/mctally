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
#include "DwmMcTallyUtils.hh"
#include "DwmMcTallyVersion.hh"

using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ShowRemoteVersions(const std::string & host,
                               const std::vector<std::string> & regExps)
{
  bool            rc = false;
  Credence::Peer  peer;
  if (peer.Connect(host, 2125)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      uint8_t  req = McTally::e_installedPackages;
      for (const auto & rgx : regExps) {
        if (peer.Send(req)) {
          if (peer.Send(rgx)) {
            std::map<std::string,std::string>  pkgs;
            if (peer.Receive(pkgs)) {
              rc = true;
              for (const auto & pkg : pkgs) {
                std::cout << pkg.first << ' ' << pkg.second << '\n';
              }
            }
            else {
              break;
            }
          }
          else {
            break;
          }
        }
        else {
          break;
        }
      }
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::string  host;
  extern int   optind;
  int          optChar;

  while ((optChar = getopt(argc, argv, "h:")) != -1) {
    switch (optChar) {
      case 'h':
        host = optarg;
        break;
      default:
        break;
    }
  }
  std::vector<std::string>  regExps;
  for (int n = optind; n < argc; ++n) {
    regExps.push_back(argv[n]);
  }

  if (! host.empty()) {
    if (ShowRemoteVersions(host, regExps)) {
      return 0;
    }
  }
  else {
    std::map<std::string,std::string>  pkgs;
    McTally::Utils::GetInstalledVersions(regExps, pkgs);
    if (! pkgs.empty()) {
      for (const auto & pkg : pkgs) {
        std::cout << pkg.first << ' ' << pkg.second << '\n';
      }
      return 0;
    }
  }
  
  return 1;
}

