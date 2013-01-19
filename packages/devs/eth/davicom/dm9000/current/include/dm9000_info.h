#ifndef CYGONCE_DEVS_ETH_DAVICOM_DM9000_INFO_H
#define CYGONCE_DEVS_ETH_DAVICOM_DM9000_INFO_H
/*==========================================================================
//
//        dm9000_info.h
//
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
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
// Author(s):     msalter
// Contributors:  
// Date:          2004-03-18
// Description:
//
//####DESCRIPTIONEND####
*/

#include <pkgconf/devs_eth_davicom_dm9000.h>

struct dm9000 {
    struct eth_drv_sc *sc;
    cyg_uint8  active;           // interface is active
    cyg_uint8  reset_pending;
    cyg_uint8  txbusy;
    cyg_uint8  mac_address[6];
    cyg_uint16 rxlen;
    unsigned long txkey;
    volatile unsigned char *io_addr;  // addr register
    volatile unsigned char *io_data;  // data register
    cyg_vector_t interrupt;           // Interrupt vector used by controller
    int (*read_data)(struct dm9000 *p, cyg_uint8 *dest);
    int (*write_data)(struct dm9000 *p, cyg_uint8 *src);
    int buswidth;
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_handle_t  interrupt_handle;
    cyg_interrupt interrupt_object;
#endif
};

#define CYGDAT_DEVS_ETH_DESCRIPTION "Davicom DM9000 Ethernet"

#endif

/* EOF dm9000_info.h */
