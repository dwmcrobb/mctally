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
//!  \file DwmMcTallyServiceConfig.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McTally::ServiceConfig class implementation
//---------------------------------------------------------------------------

#include "DwmMcTallyServiceConfig.hh"

namespace Dwm {

  namespace McTally {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const set<boost::asio::ip::tcp::endpoint> &
    ServiceConfig::Addresses() const
    {
      return _serviceAddresses;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const set<boost::asio::ip::tcp::endpoint> &
    ServiceConfig::Addresses(const set<boost::asio::ip::tcp::endpoint> & addrs)
    {
      _serviceAddresses = addrs;
      return _serviceAddresses;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void ServiceConfig::AddAddress(const boost::asio::ip::tcp::endpoint & addr)
    {
      _serviceAddresses.insert(addr);
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & ServiceConfig::KeyDirectory() const
    {
      return _keyDirectory;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & ServiceConfig::KeyDirectory(const string & keyDirectory)
    {
      _keyDirectory = keyDirectory;
      return _keyDirectory;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const set<IpPrefix> & ServiceConfig::AllowedClients() const
    {
      return _allowedClients;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    set<IpPrefix> & ServiceConfig::AllowedClients()
    {
      return _allowedClients;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void ServiceConfig::Clear()
    {
      _serviceAddresses.clear();
      _keyDirectory = "/usr/local/etc/mctallyd";
      _allowedClients.clear();
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & operator << (std::ostream & os, const ServiceConfig & cfg)
    {
      if (os) {
        os << "#============================================================================\n"
           << "#  Network service to allow authorized clients to fetch alerts.\n"
           << "#============================================================================\n"
           << "service {\n";
        if (! cfg._serviceAddresses.empty()) {
          os << "    addresses = [\n";
          auto  iter = cfg._serviceAddresses.begin();
          os << "        { address = \"" << iter->address()
             << "\"; port = " << iter->port() << "; }";
          ++iter;
          for ( ; iter != cfg._serviceAddresses.end(); ++iter) {
            os << ",\n        { address = \"" << iter->address()
               << "\"; port = " << iter->port() << "; }";
          }
          os << "\n    ];\n\n";
        }
        if (! cfg._keyDirectory.empty()) {
          os << "    keyDirectory = \"" << cfg._keyDirectory << "\";\n\n";
        }
        
        if (! cfg._allowedClients.empty()) {
          os << "    allowedClients = [\n";
          auto  iter = cfg._allowedClients.begin();
          os << "        \"" << *iter << "\"";
          ++iter;
          for ( ; iter != cfg._allowedClients.end(); ++iter) {
            os << ",\n        \"" << *iter << "\"";
          }
          os << "\n    ];\n";
        }
        os << "};\n";
      }
      return os;
    }
    
  }  // namespace McTally

}  // namespace Dwm
