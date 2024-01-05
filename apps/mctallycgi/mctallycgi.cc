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
#include <thread>

#include "DwmCgi.hh"
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
    if (installedPkgs.Pkgs().empty()) {
      cout << "  <tr><th colspan=2>No installed packages matching '"
           << resp.Req().Selector() << "'</th></tr>\n";
    }
    else {
      cout << "  <tr><th colspan=2 class=tblsubhdr>"
           << "Installed packages matching '" << resp.Req().Selector() << "'"
           << "</th></tr>\n";
      for (const auto & pkg : installedPkgs.Pkgs()) {
        cout << "  <tr><td><tt>" << pkg.first << "</tt></td>"
             << "<td><tt>" << pkg.second << "</tt></td></tr>\n";
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
    cout << "  <tr><th colspan=2 class=tblsubhdr><b>uname</b></th></tr>\n"
         << "  <tr><td align=left><tt>sysname</tt></td>"
         << "<td align=left><tt>" << un.SysName() << "</tt></td></tr>\n"
         << "  <tr><td align=left><tt>nodename</tt></td>"
         << "<td align=left><tt>" << un.NodeName() << "</tt></td></tr>\n"
         << "  <tr><td align=left><tt>release</tt></td>"
         << "<td align=left><tt>" << un.Release() << "</tt></td></tr>\n"
         << "  <tr><td align=left><tt>version</tt></td>"
         << "<td align=left><tt>" << un.Version() << "</tt></td></tr>\n"
         << "  <tr><td align=left><tt>machine</tt></td>"
         << "<td align-left><tt>" << un.Machine() << "</tt></td></tr>\n"
         << "  <tr><td align=left><tt>prettyName</tt></td>"
         << "<td align=left><tt>" << un.PrettyName() << "</tt></td></tr>\n";
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
        << "  <tr><td colspan=2><b><pre>"
        << setw(10) << "USER" << ' ' << setw(8) << "TTY" << ' '
        << setw(16) << "LOGIN_TIME" << ' ' << setw(16) << "IDLE_TIME" << ' '
        << "FROM</pre></b></td></tr>\n";
    for (const auto & entry : logins.Entries()) {
      oss << "  <tr><td colspan=2><pre>"
          << setw(10) << entry.User() << ' '
          << setw(8) << entry.Tty() << ' '
          << setw(16) << entry.LoginTimeString() << ' '
          << setw(16) << entry.IdleTimeString() << ' '
          << entry.FromHost() << "</pre></td></tr>\n";
    }
    cout << " <tr><th colspan=2 class=tblsubhdr>active logins</th></tr>\n"
         << oss.str();
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
    ostringstream  oss;
    oss << "  <tr><th colspan=2 class=tblsubhdr>load averages</th></tr>\n"
        << "  <tr><td><tt>1 minute</tt></td><td><tt>"
        << setprecision(4)
        << setw(6) << fixed << loadAvgs.Avg1Min() << "</tt></td></tr>\n"
        << "  <tr><td><tt>5 minutes<tt></td><td><tt>"
        << setw(6) << fixed << loadAvgs.Avg5Min() << "</tt></td></tr>\n"
        << "  <tr><td><tt>15 minutes</td><td><tt>"
        << setw(6) << fixed << loadAvgs.Avg15Min() << "</tt></td></tr>\n";
    cout << oss.str();
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
  cout << "<table width=100% class=responsetbl>\n"
       << "  <tr><th colspan=2 class=tblhosthdr>" << host
       << "</th></tr>\n";
  for (const auto & resp : responses) {
    PrintResponse(host, resp);
  }
  cout << "</table>\n"
       << "<hr>\n";

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
static void PeerThread(string host, const vector<McTally::Request> & requests)
{
  Credence::Peer  peer;
  if (GetPeer(host, peer)) {
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
int main(int argc, char *argv[])
{
  extern int                optind;
  int                       optChar;
  vector<string>            hosts = {
    "ria",    "www",   "kiva", "grover", "lassie", "nomnom", "pi4e",
    "snoopy", "spark", "thrip"
  };
  vector<McTally::Request>  requests = {
    McTally::Request(McTally::e_uname),
    McTally::Request(McTally::e_loadAverages),
    McTally::Request(McTally::e_logins),
    McTally::Request("net.mcplex.*|mcblock|mcloc|mcrover|mctally"
                     "|DwmDns|libDwm.*")
  };
  
  Dwm::SysLogger::Open("mctallycgi", LOG_PERROR, LOG_USER);
  Dwm::SysLogger::MinimumPriority(LOG_ERR);
  Dwm::SysLogger::ShowFileLocation(true);

  cout << "<html>\n"
       << "<head>\n"
       << "  <style>\n"
       << "    th, td {\n"
       << "      padding-right: 20px;\n"
       << "    }\n"
       << "    .tblsubhdr {\n"
       << "      background-color: #FFFFA0;\n"
       << "    }\n"
       << "    .tblhosthdr {\n"
       << "      font-weight: bold;\n"
       << "      font-size: x-large;\n"
       << "      background-color: #B0D0FF;\n"
       << "    }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n";
  
  vector<std::thread>  peerThreads;
  for (const auto & host : hosts) {
    peerThreads.emplace_back(thread(PeerThread, host, std::ref(requests)));
  }
  for (auto & peerThread : peerThreads) {
    peerThread.join();
  }

  cout << "</body>\n" << "</html>\n";
  
  return 0;
}

