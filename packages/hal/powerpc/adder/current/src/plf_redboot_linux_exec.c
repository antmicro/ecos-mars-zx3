//==========================================================================
//
//      plf_redboot_linux_boot.c
//
//      Platform support for RedBoot command to boot Linux
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
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
// Contributors: 
// Date:         2003-08-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/redboot_linux_exec.h>

//
// Export system configuration - used when booting a Linux kernel
// Note: this function is expected to set up the fields which are
// platform/variant dependent.  It may also override any of the
// architecture common fields (like memory layout, etc) as appropriate
//
externC void 
plf_redboot_linux_exec(bd_t *board_info)
{
    board_info->bi_intfreq	= CYGHWR_HAL_POWERPC_BOARD_SPEED*1000000;
    board_info->bi_busfreq	= 66*1000000;
    board_info->bi_flashbase    = 0xFE000000;
    board_info->bi_flashsize    = 0x00800000;
    board_info->bi_flashwidth   = 16;
}

//=========================================================================
// EOF plf_redboot_linux_exec.c
