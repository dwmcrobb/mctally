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
//!  \file DwmMcTallyServer.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::Server class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <thread>

#include "DwmSysLogger.hh"
#include "DwmMcTallyUtils.hh"
#include "DwmMcTallyServer.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::Stop()
    {
      for (auto & a : _acceptors) {
        a.cancel();
      }
      Syslog(LOG_INFO, "Server stopped");
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::AcceptLoop(boost::asio::ip::tcp::acceptor & a)
    {
      using boost::asio::ip::tcp;
      
      a.async_accept(a.get_executor(),
                     [this,&a](error_code ec, tcp::socket && s)
                     {
                       CleanupResponders();
                       if (! ec) {
                         if (ClientAllowed(s.remote_endpoint().address())) {
                           _responders.push_back(std::make_shared<Responder>(std::move(s), *this));
                         }
                         else {
                           s.close();
                         }
                         AcceptLoop(a);
                       }
                       else {
                         Syslog(LOG_ERR, "Exiting AcceptLoop()");
                       }
                     });
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Server::ClientAllowed(const boost::asio::ip::address & epAddr) const
    {
      bool  rc = _allowedClients.empty();
      if (! rc) {
        IpAddress clientAddr(epAddr.to_string());
        auto  it = std::find_if(_allowedClients.begin(),
                                _allowedClients.end(),
                                [&] (const auto & ac)
                                { return ac.Contains(clientAddr); });
        rc = (it != _allowedClients.end());
      }
      return rc;
    }
                                  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::CleanupResponders()
    {
      for (auto ri = _responders.begin(); ri != _responders.end(); ) {
        if ((*ri)->Join()) {
          ri = _responders.erase(ri++);
        }
        else {
          ++ri;
        }
      }
      return;
    }
    
  }  // namespace McTally

}  // namespace Dwm
