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
// Author(s):    Andrew.Lunn@ascom.ch
// Contributors: andrew.lunn
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "synth.h"

#include <pkgconf/devs_flash_synth.h>
#include <cyg/hal/hal_io.h>
#include <cyg/io/flash.h>

#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif

int
flash_program_buf(volatile flash_t *addr, flash_t *data, int len,
                  unsigned long block_mask, int buffer_size)
{
    unsigned int offset = (unsigned int) addr;
    cyg_uint8 *buf = (cyg_uint8 *) data;

    // This helps speed up the programming
    static cyg_uint8 tmp[4096];

    offset -= (unsigned int) cyg_dev_flash_synth_base;

    while (len > 0) {
        int i;
        int write_size = MIN(len, sizeof(tmp));
        // Writing to NOR flash only sets bits from 1 to 0, not vice-versa
        cyg_hal_sys_lseek(cyg_dev_flash_synth_flashfd, offset,
                          CYG_HAL_SYS_SEEK_SET);
        cyg_hal_sys_read(cyg_dev_flash_synth_flashfd, tmp, write_size);
        for (i = 0; i < write_size; i++)
            tmp[i] = tmp[i] & buf[i];
        cyg_hal_sys_lseek(cyg_dev_flash_synth_flashfd, offset,
                          CYG_HAL_SYS_SEEK_SET);
        cyg_hal_sys_write(cyg_dev_flash_synth_flashfd, tmp, write_size);
        // Process next chunk
        buf += write_size;
        offset += write_size;
        len -= write_size;        
    }
    
    return FLASH_ERR_OK;
}
