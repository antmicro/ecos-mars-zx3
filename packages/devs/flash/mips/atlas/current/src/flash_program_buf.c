//==========================================================================
//
//      flash_program_buf.c
//
//      Flash programming
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):    gthomas
// Contributors: gthomas, msalter
// Date:         2000-12-07
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "flash.h"

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>


int
flash_program_buf(volatile unsigned long *addr, unsigned long *data, int len)
 __attribute__ ((section (".2ram.flash_program_buf")));
int
flash_program_buf(volatile unsigned long *addr, unsigned long *data, int len)
{
    volatile unsigned long *ROM;
    unsigned long stat = 0;
    int timeout = 50000;

    FLASH_WRITE_ENABLE();

    addr = (volatile unsigned long *)CYGARC_UNCACHED_ADDRESS((unsigned long)addr);
    ROM = (volatile unsigned long *)((unsigned long)addr & FLASH_BASE_MASK);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    while (len > 0) {
        ROM[0] = FLASH_Program;
        *addr = *data;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                goto bad;
            }
        }
        if (stat & 0x007E007E) {
            break;
        }
        ROM[0] = FLASH_Reset;            
        if (*addr++ != *data++) {
            stat = 0x99109910;
            break;
        }
        len -= 4;
    }

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = FLASH_Reset;            

    FLASH_WRITE_DISABLE();

    return stat;
}
