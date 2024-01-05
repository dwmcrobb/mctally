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
#include <regex>

#include "DwmSysLogger.hh"
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
static bool GetPeer(const string & host, Credence::Peer & peer)
{
  bool  rc = false;
  if (peer.Connect(host, 2125)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      rc = true;
    }
    else {
      peer.Disconnect();
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintInstalledPackages(const string & host,
                                   const McTally::Response & resp)
{
  if (std::holds_alternative<McTally::InstalledPackages>(resp.Data())) {
    const auto & installedPkgs =
      std::get<McTally::InstalledPackages>(resp.Data());
    cout << host << ":\n";
    for (const auto & pkg : installedPkgs.Pkgs()) {
      cout << "  " << pkg.first << ' ' << pkg.second << '\n';
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintUname(const string & host, const McTally::Response & resp)
{
  if (std::holds_alternative<McTally::Uname>(resp.Data())) {
    const auto & un = std::get<McTally::Uname>(resp.Data());
    cout << host << ":\n"
         << "  sysname:    " << un.SysName() << '\n'
         << "  nodename:   " << un.NodeName() << '\n'
         << "  release:    " << un.Release() << '\n'
         << "  version:    " << un.Version() << '\n'
         << "  machine:    " << un.Machine() << '\n'
         << "  prettyname: " << un.PrettyName() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintLoginEntries(const string & host,
                              const McTally::Logins & logins)
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
  cout << host << ":\n" << oss.str();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintLogins(const string & host, const McTally::Response & resp)
{
  if (std::holds_alternative<McTally::Logins>(resp.Data())) {
    const auto & logins = std::get<McTally::Logins>(resp.Data());
    PrintLoginEntries(host, logins);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintLoadAverages(const string & host,
                              const McTally::Response & resp)
{
  if (std::holds_alternative<McTally::LoadAvg>(resp.Data())) {
    const auto & loadAvgs = std::get<McTally::LoadAvg>(resp.Data());
    cout << setiosflags(ios::left)
         << setw(31) << host << ' '
         << setprecision(4)
         << setw(6) << fixed << loadAvgs.Avg1Min() << ' '
         << setw(6) << fixed << loadAvgs.Avg5Min() << ' '
         << setw(6) << fixed << loadAvgs.Avg15Min() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintResponse(const string & host, const McTally::Response & resp)
{
  if (resp.Req().ReqEnum() == McTally::e_installedPackages) {
    PrintInstalledPackages(host, resp);
  }
  else if (resp.Req().ReqEnum() == McTally::e_uname) {
    PrintUname(host, resp);
  }
  else if (resp.Req().ReqEnum() == McTally::e_logins) {
    PrintLogins(host, resp);
  }
  else if (resp.Req().ReqEnum() == McTally::e_loadAverages) {
    PrintLoadAverages(host, resp);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintResponses(const string & host,
                           const vector<McTally::Response> & responses)
{
  for (const auto & resp : responses) {
    PrintResponse(host, resp);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool SendReceive(Credence::Peer & peer,
                        const vector<McTally::Request> & requests,
                        vector<McTally::Response> & responses)
{
  bool  rc = false;
  responses.clear();
  for (const auto & req : requests) {
    if (peer.Send(req)) {
      McTally::Response  resp;
      if (peer.Receive(resp)) {
        if (resp.Req().ReqEnum() == req.ReqEnum()) {
          responses.push_back(resp);
        }
      }
    }
  }
          
  return (requests.size() == responses.size());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  extern int                optind;
  int                       optChar;
  vector<string>            hosts;
  vector<McTally::Request>  requests;
  
  Dwm::SysLogger::Open("mctally", LOG_PERROR, LOG_USER);
  Dwm::SysLogger::MinimumPriority(LOG_ERR);
  Dwm::SysLogger::ShowFileLocation(true);

  while ((optChar = getopt(argc, argv, "h:ula")) != -1) {
    switch (optChar) {
      case 'a':
        requests.push_back(McTally::Request(McTally::e_loadAverages));
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
        requests.push_back(McTally::Request(McTally::e_logins));
        break;
      case 'u':
        requests.push_back(McTally::Request(McTally::e_uname));
        break;
      default:
        break;
    }
  }

  vector<string>  regExps;
  for (int n = optind; n < argc; ++n) {
    try {
      regex  rgx(argv[n]);
      requests.push_back(McTally::Request(McTally::PackageSelector(argv[n])));
    }
    catch (...) {
      cerr << "Bad regular expression '" << argv[n] << "'\n";
    }
  }
  
  if (hosts.empty()) {
    hosts.push_back("localhost");
  }
  for (const auto & host : hosts) {
    Credence::Peer  peer;
    if (GetPeer(host, peer)) {
      vector<McTally::Response>  responses;
      if (SendReceive(peer, requests, responses)) {
        PrintResponses(host, responses);
      }
      peer.Disconnect();
    }
  }
  
  return 0;
}

