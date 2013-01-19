//==========================================================================
//
//      dev/KS8721.c
//
//      Ethernet transceiver (PHY) support 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2005 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler <uwe_kindler@web.de>
// Contributors: Markus Schade <marks@peppercon.de>
// Date:         2007-04-04
// Purpose:      
// Description:  Support for ethernet Micrel KS8721 PHY
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_phy.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>

#include <cyg/io/eth_phy.h>
#include <cyg/io/eth_phy_dev.h>


static bool ks8721_stat(eth_phy_access_t *f, int *state)
{
    unsigned short phy_state;
    unsigned short phy_anadv_reg;
    int tries;
    int ms;

    // Read negotiated state
    if (_eth_phy_read(f, PHY_BMSR, f->phy_addr, &phy_state)) 
    {
        if ((phy_state & PHY_BMSR_AUTO_NEG) == 0) 
        {
            eth_phy_printf("... waiting for auto-negotiation");
            for (tries = 0;  tries < CYGINT_DEVS_ETH_PHY_AUTO_NEGOTIATION_TIME;  tries++) 
            {
                if (_eth_phy_read(f, PHY_BMSR, f->phy_addr, &phy_state)) 
                {
                    if ((phy_state & PHY_BMSR_AUTO_NEG) != 0) 
                    {
                        break;
                    }
                }
                
                //
                // Wait for 1 second
                //
                for (ms = 0; ms < 1000; ++ms)
                {
                    CYGACC_CALL_IF_DELAY_US(1000);   // 1 ms
                }
                eth_phy_printf(".");
            }
            eth_phy_printf("\n");
        }
        if ((phy_state & PHY_BMSR_AUTO_NEG) != 0) 
        {
            _eth_phy_read(f, PHY_AN_ADV, f->phy_addr, &phy_anadv_reg);
            eth_phy_printf("\nAuto negotiation advertisement: %x\n", phy_anadv_reg);
            *state = 0;
            if ((phy_state & PHY_BMSR_LINK) != 0) *state |= ETH_PHY_STAT_LINK;
            if ((phy_anadv_reg & PHY_AN_ADV_100FDX) != 0) *state |= ETH_PHY_STAT_100MB | ETH_PHY_STAT_FDX;
            return true;
        }
    }
    return false;
}

_eth_phy_dev("Micrel KS8721", 0x00221619, ks8721_stat)
