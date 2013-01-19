//==========================================================================
//
//      arm_e7t_flash.c
//
//      Flash programming for SST Flash device on ARM Evaluator-7T board
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
// Author(s):    Chris Garry <cgarry@sweeneydesign.co.uk>
// Contributors: andrew.lunn@ascom.ch
// Date:         2003-04-21
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

//--------------------------------------------------------------------------
// Device properties
#include <pkgconf/devs_flash_arm_e7t.h>


// The Evaluator-7T has either one SST 39VF400A part or one Am29LV400B part
#define CYGNUM_FLASH_INTERLEAVE (1)
#define CYGNUM_FLASH_SERIES     (1)
#define CYGNUM_FLASH_WIDTH      (16)
#define CYGNUM_FLASH_BASE       (0x01800000)

//--------------------------------------------------------------------------
// Platform specific extras
#define CYGPKG_DEVS_FLASH_SST_39VF400

//--------------------------------------------------------------------------
// Now include the driver code.
#ifdef CYGINT_DEVS_FLASH_SST_39VFXXX_REQUIRED
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
#endif

#ifdef CYGINT_DEVS_FLASH_AMD_AM29XXXXX_REQUIRED
#include "cyg/io/flash_am29xxxxx.inl"
#endif

// ------------------------------------------------------------------------
// EOF arm_e7t_flash.c
