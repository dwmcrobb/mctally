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
//!  \brief client program to show installed applications, uptime, uname,
//!    load averages or active logins
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <iomanip>
#include <iostream>
#include <regex>
#include <thread>

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
static bool GetPeer(const string & host, uint16_t port, Credence::Peer & peer)
{
  bool  rc = false;
  if (peer.Connect(host, port)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      rc = true;
    }
    else {
      peer.Disconnect();
      cerr << "Failed to authenticate with " << host << ':' << port << '\n';
    }
  }
  else {
    cerr << "Failed to connect to " << host << ':' << port << '\n';
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
    cout << host;
    if (installedPkgs.Pkgs().empty()) {
      cout << ": no";
    }
    cout << " installed packages matching '" << resp.Req().Selector() << "'";
    if (installedPkgs.Pkgs().empty()) {
      cout << '\n';
    }
    else {
      cout << ":\n";
      for (const auto & pkg : installedPkgs.Pkgs()) {
        cout << "  " << pkg.first << ' ' << pkg.second << '\n';
      }
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
    cout << host << " uname:\n"
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
  if (! logins.Entries().empty()) {
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
    cout << host << " active logins:\n" << oss.str();
  }
  else {
    cout << host << ": no active logins\n";
  }
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
         << setw(40) << host + " load averages: "
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
static void PrintUptime(const string & host, const McTally::Response & resp)
{
  if (std::holds_alternative<uint64_t>(resp.Data())) {
    auto  uptime = std::get<uint64_t>(resp.Data());
    cout << host << " uptime: "
         << McTally::Utils::TimeIntervalString(uptime) << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintResponse(const string & host, const McTally::Response & resp)
{
  switch (resp.Req().ReqEnum()) {
    case McTally::e_installedPackages:
      PrintInstalledPackages(host, resp);
      break;
    case McTally::e_uname:
      PrintUname(host, resp);
      break;
    case McTally::e_loadAverages:
      PrintLoadAverages(host, resp);
      break;
    case McTally::e_logins:
      PrintLogins(host, resp);
      break;
    case McTally::e_uptime:
      PrintUptime(host, resp);
      break;
    default:
      break;
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintResponses(const string & host,
                           const vector<McTally::Response> & responses)
{
  static mutex  mtx;
  lock_guard<mutex>  lock(mtx);
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
      else {
        cerr << "Failed to receive response from " << peer.EndPointString()
             << '\n';
      }
    }
    else {
      cerr << "Failed to send request to " << peer.EndPointString() << '\n';
    }
  }
          
  return (requests.size() == responses.size());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PeerThread(string host, uint16_t port,
                       const vector<McTally::Request> & requests)
{
  Credence::Peer  peer;
  if (GetPeer(host, port, peer)) {
    vector<McTally::Response>  responses;
    if (SendReceive(peer, requests, responses)) {
      PrintResponses(host, responses);
    }
    peer.Disconnect();
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "usage: " << argv0
       << " [-h host(s)] [-p port] [-a] [-l] [-t] [-u] [v] [packages_regex]\n"
       << "  -a: show load averages\n"
       << "  -l: show active logins\n"
       << "  -p port: connect to the given port (default 2125)\n"
       << "  -t: show uptime\n"
       << "  -u: show uname\n"
       << "  -v: show version\n"
       << "  -h hosts: specify hosts to query (comma-separated list)\n"
       << "  packages_regex: regular expression to match installed packages\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  extern int                optind;
  int                       optChar;
  vector<string>            hosts;
  uint16_t                  port = 2125;
  vector<McTally::Request>  requests;
  
  Dwm::SysLogger::Open("mctally", LOG_PERROR, LOG_USER);
  Dwm::SysLogger::MinimumPriority(LOG_ERR);
  Dwm::SysLogger::ShowFileLocation(true);

  while ((optChar = getopt(argc, argv, "h:p:ulatv")) != -1) {
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
      case 'p':
        port = std::stoul(optarg);
        break;
      case 'l':
        requests.push_back(McTally::Request(McTally::e_logins));
        break;
      case 't':
        requests.push_back(McTally::Request(McTally::e_uptime));
        break;
      case 'u':
        requests.push_back(McTally::Request(McTally::e_uname));
        break;
      case 'v':
        cout << Dwm::McTally::Version.Version() << '\n';
        return 0;
        break;
      default:
        Usage(argv[0]);
        return 1;
        break;
    }
  }

  if (hosts.empty()) {
    hosts.push_back("localhost");
  }

  vector<string>  regExps;
  for (int n = optind; n < argc; ++n) {
    try {
      regex  rgx(argv[n]);
      requests.push_back(McTally::Request(McTally::PackageSelector(argv[n])));
    }
    catch (...) {
      cerr << "Bad regular expression '" << argv[n] << "'\n";
      return 1;
    }
  }

  if (requests.empty()) {
    Usage(argv[0]);
    return 1;
  }
  
  vector<std::thread>  peerThreads;
  for (const auto & host : hosts) {
    peerThreads.emplace_back(thread(PeerThread, host, port,
                                    std::ref(requests)));
  }
  for (auto & peerThread : peerThreads) {
    peerThread.join();
  }
  
  return 0;
}

