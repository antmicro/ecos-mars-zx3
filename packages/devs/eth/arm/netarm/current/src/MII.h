#ifndef CYGONCE_DEVS_ETH_ARM_NETARM_MII_H
#define CYGONCE_DEVS_ETH_ARM_NETARM_MII_H
// ====================================================================
//
//      MII.h
//
//      PHY chip configuration
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:        	01.08.2004
// Purpose:     	Functions for PHY control
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

void     cyg_netarm_mii_reset(void);
void     cyg_netarm_mii_set_speed (cyg_uint32 speed, cyg_bool duplex);
cyg_uint32 cyg_netarm_mii_check_speed(void);
cyg_bool cyg_netarm_mii_check_duplex(void);
cyg_bool cyg_netarm_mii_negotiate(void);

#endif // CYGONCE_DEVS_ETH_ARM_NETARM_MII_H
