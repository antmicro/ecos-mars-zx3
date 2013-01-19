/*==========================================================================
//
//      lpc_misc.c
//
//      HAL misc board support code for Keil MCB2100 
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
// Author(s):    jani 
// Contributors: 
// Date:         2004-09-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/
#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>             // IO macros

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/var_io.h>

extern void cyg_hal_plf_serial_init(void);
//Display the low byte of mask on the 8 leds of the MCB21000 board
void hal_lpc2xxx_set_leds(int mask)
{
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_IO_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_IO1CLR, 0x00FF0000);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_IO_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_IO1SET, (mask & 0xFF) << 16);
}

void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;
	
    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init();
}
//--------------------------------------------------------------------------
// EOF mcb2100_misc.c
