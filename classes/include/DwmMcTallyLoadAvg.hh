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
//!  \file DwmMcTallyLoadAvg.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCTALLYLOADAVG_HH_
#define _DWMMCTALLYLOADAVG_HH_

#include <array>

#include <nlohmann/json.hpp>

#include "DwmStreamIOCapable.hh"

namespace Dwm {

  namespace McTally {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class LoadAvg
      : public StreamIOCapable
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      LoadAvg();
      
      LoadAvg(const LoadAvg &) = default;
      LoadAvg(LoadAvg &&) = default;
      LoadAvg & operator = (const LoadAvg &) = default;
      LoadAvg & operator = (LoadAvg &&) = default;
      ~LoadAvg() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline LoadAvg(const std::array<double,3> & loadAvgs)
          : _loadAvgs(loadAvgs)
      { }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg1Min() const  { return std::get<0>(_loadAvgs); }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg1Min(double a)  { return std::get<0>(_loadAvgs) = a; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg5Min() const  { return std::get<1>(_loadAvgs); }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg5Min(double a)  { return std::get<1>(_loadAvgs) = a; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg15Min() const  { return std::get<2>(_loadAvgs); }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline double Avg15Min(double a)  { return std::get<2>(_loadAvgs) = a; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is) override;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const override;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const LoadAvg & avg) const
      { return (_loadAvgs == avg._loadAvgs); }
        
    private:
      std::array<double,3>  _loadAvgs;
    };
    
  }  // namespace McTally

}  // namespace Dwm

#endif  // _DWMMCTALLYLOADAVG_HH_
