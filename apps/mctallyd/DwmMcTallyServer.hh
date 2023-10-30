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
//!  \file DwmMcTallyServer.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Server class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYSERVER_HH_
#define _DWMMCTALLYSERVER_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
}

#include <atomic>
#include <deque>
#include <memory>
#include <thread>
#include <boost/asio.hpp>

#include "DwmCredenceKeyStash.hh"
#include "DwmCredenceKnownKeys.hh"
#include "DwmMcTallyConfig.hh"
#include "DwmMcTallyResponder.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  Encapsulates a server to listen for TCP connections from those
    //!  interested in retrieving software inventory information.  Spawns a
    //!  Responder for each connection, which is responsible for
    //!  authentication and responding to client requests.
    //------------------------------------------------------------------------
    class Server
    {
    public:
      //----------------------------------------------------------------------
      //!  Construct from a mctallyd configuration @c config.
      //----------------------------------------------------------------------
      template <typename Ex>
      Server(Ex executor, const Config & config)
          : _config(config), _keyStash(config.Service().KeyDirectory()),
            _knownKeys(config.Service().KeyDirectory()),
            _allowedClients(config.Service().AllowedClients()),
            _acceptors(), _responders()
      {
        namespace ip = boost::asio::ip;
        using boost::asio::ip::tcp;
        
        for (auto & ep : config.Service().Addresses()) {
          ip::tcp::acceptor  acc(executor);
          if (ep.address().is_v6()) {
            acc.open(ip::tcp::v6());
            acc.set_option(ip::v6_only(true));
          }
          else {
            acc.open(ip::tcp::v4());
          }
          acc.bind(ep);
          auto & a = _acceptors.emplace_back(std::move(acc));
          a.listen();
          AcceptLoop(a);
        }
      }

      const Credence::KeyStash & GetKeyStash() const  { return _keyStash; }
      const Credence::KnownKeys & GetKnownKeys() const  { return _knownKeys; }
      const Config GetConfig() const  { return _config; }
      
      //----------------------------------------------------------------------
      //!  Stops the server.
      //----------------------------------------------------------------------
      void Stop();
      
    private:
      Config                                      _config;
      Credence::KeyStash                          _keyStash;
      Credence::KnownKeys                         _knownKeys;
      std::set<IpPrefix>                          _allowedClients;
      std::deque<boost::asio::ip::tcp::acceptor>  _acceptors;
      std::atomic<bool>                           _run;
      std::vector<std::shared_ptr<Responder>>     _responders;

      void AcceptLoop(boost::asio::ip::tcp::acceptor & a);
      
      bool ClientAllowed(const boost::asio::ip::address & epAddr) const;
      void CleanupResponders();
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYSERVER_HH_
