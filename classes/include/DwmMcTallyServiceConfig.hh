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
//!  \file DwmMcTallyServiceConfig.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::ServiceConfig class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYSERVICECONFIG_HH_
#define _DWMMCTALLYSERVICECONFIG_HH_

#include <set>
#include <boost/asio.hpp>

#include "DwmIpPrefix.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  Encapsulates the mctallyd network service configuration.
    //------------------------------------------------------------------------
    class ServiceConfig
    {
    public:
      //----------------------------------------------------------------------
      //!  Returns a reference to the endpoints on which we'll listen for
      //!  client connections.
      //----------------------------------------------------------------------
      const std::set<boost::asio::ip::tcp::endpoint> & Addresses() const;
      
      //----------------------------------------------------------------------
      //!  Sets and returns the endpoints on which we'll listen for client
      //!  connections.
      //----------------------------------------------------------------------
      const std::set<boost::asio::ip::tcp::endpoint> &
      Addresses(const std::set<boost::asio::ip::tcp::endpoint> & addrs);

      //----------------------------------------------------------------------
      //!  Adds an endpoint to the set of endpoints on which we'll listen
      //!  for client connections.
      //----------------------------------------------------------------------
      void AddAddress(const boost::asio::ip::tcp::endpoint & addr);
      
      //----------------------------------------------------------------------
      //!  Returns the directory where our private key, public key and known
      //!  client keys are stored.
      //----------------------------------------------------------------------
      const std::string & KeyDirectory() const;
      
      //----------------------------------------------------------------------
      //!  Sets and returns the directory where our private key, public key
      //!  and known client keys are stored.
      //----------------------------------------------------------------------
      const std::string & KeyDirectory(const std::string & keyDir);
      
      //----------------------------------------------------------------------
      //!  Returns a reference to const of the set of prefixes from which
      //!  we'll allow clients to connect.
      //----------------------------------------------------------------------
      const std::set<IpPrefix> & AllowedClients() const;

      //----------------------------------------------------------------------
      //!  Returns a reference to the set of prefixes from which we'll allow
      //!  clients to connect.
      //----------------------------------------------------------------------
      std::set<IpPrefix> & AllowedClients();

      //----------------------------------------------------------------------
      //!  Clears the service configuration.
      //----------------------------------------------------------------------
      void Clear();

      //----------------------------------------------------------------------
      //!  Prints the given service configuration @c cfg to the given
      //!  ostream @c os.
      //----------------------------------------------------------------------
      friend std::ostream &
      operator << (std::ostream & os, const ServiceConfig & cfg);
      
    private:
      std::set<boost::asio::ip::tcp::endpoint>  _serviceAddresses;
      std::string                               _keyDirectory;
      std::set<IpPrefix>                        _allowedClients;
    };
    
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYSERVICECONFIG_HH_
