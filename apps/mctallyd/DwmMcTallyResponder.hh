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
//!  \file DwmMcTallyResponder.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYRESPONDER_HH_
#define _DWMMCTALLYRESPONDER_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
}

#include <string>
#include <thread>
#include <vector>

#include "DwmCredencePeer.hh"
#include "DwmMcTallyRequest.hh"

namespace Dwm {

  namespace McTally {

    class Server;
    
    //------------------------------------------------------------------------
    //!  Encapsulates a thread that responds to requests from a single
    //!  client.
    //------------------------------------------------------------------------
    class Responder
    {
    public:
      //--------------------------------------------------------------------
      //!  Construct from the given socket @c s, Server @c server,
      //!  AlertOrigin @c origin and end point @c endPoint.
      //--------------------------------------------------------------------
      Responder(boost::asio::ip::tcp::socket && s, Server & server);

      //----------------------------------------------------------------------
      //!  Destructor.
      //----------------------------------------------------------------------
      ~Responder();
      
      //----------------------------------------------------------------------
      //!  Join the responder's thread.  Returns true if the thread is
      //!  joinable and done, and we successfully joined it.
      //----------------------------------------------------------------------
      bool Join();
      
    private:
      Credence::Peer        _peer;
      Server               &_server;
      std::string           _agreedKey;
      std::thread           _thread;
      std::atomic<bool>     _running;

      bool HandleRequest(Request request);
      bool SendInstalledPackages(const std::string & regExp);
      bool SendLoadAverages();
      bool SendLogins();
      bool SendUname();
      bool SendUptime();
      void Run();
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYRESPONDER_HH_
