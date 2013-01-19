//==========================================================================
//
//      arm_aim711_flash.c
//
//      Flash programming for ARM Industrial Module AIM 711
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
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
// Author(s):    jskov, rcassebohm
// Contributors: jskov, rcassebohm, andrew lunn
// Date:         2001-02-21
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_arm_aim711.h>

#if (CYGNUM_DEVS_FLASH_ARM_AIM711_SIZE == 1)

#define CYGPKG_DEVS_FLASH_SST_39VF080
#define CYGNUM_FLASH_INTERLEAVE	(1)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_BASE 	(0x2000000u)

#elif (CYGNUM_DEVS_FLASH_ARM_AIM711_SIZE == 2)

#define CYGPKG_DEVS_FLASH_SST_39VF016
#define CYGNUM_FLASH_INTERLEAVE	(1)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_BASE 	(0x2000000u)

#endif

#include "cyg/io/flash_sst_39vfxxx.inl"

static const cyg_flash_block_info_t cyg_flash_sst_block_info[1] = {
    { FLASH_BLOCK_SIZE, FLASH_NUM_REGIONS * CYGNUM_FLASH_SERIES }
};
CYG_FLASH_DRIVER(cyg_flash_sst_flashdev,
                 &cyg_sst_funs,
                 0,                     // Flags
                 CYGNUM_FLASH_BASE,     // Start
                 CYGNUM_FLASH_BASE + (FLASH_BLOCK_SIZE * FLASH_NUM_REGIONS * CYGNUM_FLASH_SERIES) - 1,    // End
                 1,                     // Number of block infos
                 cyg_flash_sst_block_info,
                 NULL                   // priv
    );

// ------------------------------------------------------------------------
// EOF arm_aim711_flash.c
