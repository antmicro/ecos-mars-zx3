//==========================================================================
//
//      iq80310_flash.c
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
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include "flash.h"

#define _si(p) ((p[1]<<8)|p[0])

extern int strncmp(const char *s1, const char *s2, size_t len);
int
flash_hwr_init(void)
{
    struct FLASH_query data, *qp;
    int num_regions, region_size;

    flash_dev_query((void*)&data);
    
    qp = &data;
    if (/*(qp->manuf_code == FLASH_Intel_code) && */
        (strncmp(qp->id, "QRY", 3) == 0)) {
        num_regions = _si(qp->num_regions)+1;
        region_size = _si(qp->region_size)*256;

        flash_info.block_size = region_size;
        flash_info.blocks = num_regions;
        flash_info.start = (void *)0x00000000;
        flash_info.end = (void *)(0x00000000+(num_regions*region_size));
        return FLASH_ERR_OK;
    } else {
	(*flash_info.pf)("Can't identify FLASH sorry\n");
        diag_dump_buf((void*)&data, sizeof(data));
        return FLASH_ERR_HWR;
    }
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    if (err & 0x7E) {
        (*flash_info.pf)("Err = %x\n", err);
        if (err & 0x10) {
            return FLASH_ERR_PROGRAM;
        } else 
        if (err & 0x20) {
            return FLASH_ERR_ERASE;
        } else 
        return FLASH_ERR_HWR;  // FIXME
    } else {
        return FLASH_ERR_OK;
    }
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern char _stext[], _etext[];

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}
