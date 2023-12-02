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
//!  \file mctallyd.cc
//!  \author Daniel W. McRobb
//!  \brief McTally server daemon
//---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>

#include "DwmDaemonUtils.hh"
#include "DwmSignal.hh"
#include "DwmSysLogger.hh"
#include "DwmMcTallyServer.hh"

using namespace std;
using namespace Dwm;

static string            g_pidFile;
static atomic<bool>      g_gotSigTerm = false;
boost::asio::io_context  g_serverContext;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void SavePID(const string & pidFile)
{
#ifndef O_EXLOCK
  #define O_EXLOCK 0
#endif
  g_pidFile = pidFile;
  pid_t  pid = getpid();
  string  &&pidstr = to_string(pid) + "\n";
  int    fd = open(g_pidFile.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_EXLOCK, 0644);
  if (fd >= 0) {
    write(fd, pidstr.c_str(), pidstr.size());
    close(fd);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void RemovePID()
{
  if (! g_pidFile.empty()) {
    std::remove(g_pidFile.c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void SigTermHandler(int sigNum)
{
  if (SIGTERM == sigNum) {
    g_gotSigTerm = true;
    g_serverContext.stop();
    Syslog(LOG_INFO, "SigTermHandler got SIGTERM");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void BoostSignalHandler(const boost::system::error_code & ec,
                               int sigNum)
{
  if (SIGTERM == sigNum) {
    g_gotSigTerm = true;
    g_serverContext.stop();
    Syslog(LOG_INFO, "BoostSignalHandler got SIGTERM");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0 << " [-c configFile] [-p pidFile]\n"
       << "  Default configFile: /usr/local/etc/mctallyd.cfg\n"
       << "  Default pidFile: /var/run/mctallyd.pid\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  string  configFile("/usr/local/etc/mctallyd.cfg");
  string  pidFile("/var/run/mctallyd.pid");
  bool    daemonize = true;
  
  int  optChar;
  while ((optChar = getopt(argc, argv, "c:dp:")) != -1) {
    switch (optChar) {
      case 'c':
        configFile = optarg;
        break;
      case 'd':
        daemonize = false;
        break;
      case 'p':
        pidFile = optarg;
        break;
      default:
        Usage(argv[0]);
        return 1;
        break;
    }
  }

  McTally::Config  config;
  if (config.Parse(configFile)) {
    if (daemonize) {
      DaemonUtils::Daemonize();
    }
    Dwm::SysLogger::Open("mctallyd", LOG_PID, config.SyslogFacility());
    Dwm::SysLogger::MinimumPriority(config.SyslogLevel());
    Dwm::SysLogger::ShowFileLocation(config.SyslogLocations());

    Signal sigHup(SIGTERM);
    sigHup.PushHandler(SigTermHandler);
    SavePID(pidFile);
    boost::asio::signal_set boostSignals(g_serverContext, SIGTERM);
    boostSignals.async_wait(BoostSignalHandler);
    McTally::Server  server(g_serverContext.get_executor(), config);
    g_serverContext.run();
    for (;;) {
      if (g_gotSigTerm) {
        server.Stop();
        RemovePID();
        break;
      }
      else {
        this_thread::sleep_for(chrono::milliseconds(500));
      }
    }
  }
  else {
    cerr << "failed to parse configuration in '" << configFile << "'\n";
    return 1;
  }
  
  return 0;
}
