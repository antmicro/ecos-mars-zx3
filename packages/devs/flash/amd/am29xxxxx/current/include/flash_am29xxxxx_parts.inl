#ifndef CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_PARTS_INL
#define CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_PARTS_INL
//==========================================================================
//
//      am29xxxxx_parts.inl
//
//      AMD AM29xxxxx part descriptors
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2009 Free Software Foundation, Inc.
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
// Author(s):    jskov
// Contributors: jskov, Koichi Nagashima
// Date:         2001-06-08
// Purpose:
// Description:  AMD AM29xxxxx part descriptors
// Usage:        Should be included from the flash_am29xxxxx.inl file only.
//
// FIXME:        Add configury for selecting bottom/top bootblocks
//####DESCRIPTIONEND####
//
//==========================================================================

//
// Note: 'bootblocks' are a set of blocks which are treated by
// the driver as a single larger block.  This simplifies the driver
// so as to only have to deal with single size blocks (even though
// the physical device may differ).  The data structure is laid out as:
//    <address of start of boot block area 1>
//    <size of sub-block 1>
//    <size of sub-block 2>
//    ...
//    <size of sub-block n>
//    <address of start of boot block area 2>
//    <size of sub-block 1>
//    <size of sub-block 2>
//    ...
//    <size of sub-block n>
//    _LAST_BOOTBLOCK
//
// Finally, when specifying a device with bootblocks, the total number
// of blocks should reflect this collapse, i.e. if the device has 15
// full size blocks and 8 blocks which are 1/8 each, then the total
// should be 16 blocks.
//
#define _LAST_BOOTBLOCK (-1)

#if CYGNUM_FLASH_WIDTH == 8
#ifdef CYGHWR_DEVS_FLASH_AMD_MX29LV128
    {   // MX29LV128M-B (Bottom-boot)
        long_device_id: true,
        device_id  : FLASHWORD(0x7e),
        device_id2 : FLASHWORD(0x11),
        device_id3 : FLASHWORD(0x00),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 32
    },
    {   // MX29LV128M-T (Top-boot)
        long_device_id: true,
        device_id  : FLASHWORD(0x7e),
        device_id2 : FLASHWORD(0x11),
        device_id3 : FLASHWORD(0x01),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xff0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 32
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29F002T
    {   // AM29F002T
        device_id  : FLASHWORD(0xb0),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x030000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29F010
    {   // AM29F010
        device_id  : FLASHWORD(0x20),
        block_size : 0x4000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 8,
        device_size: 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x20000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
	    banked     : false
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29F040B
    {   // AM29F040B
        device_id  : FLASHWORD(0xa4),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 8,
        device_size: 0x80000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x80000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV160
    {   // MBM29LV160-T | AM29LV160-T
        device_id  : FLASHWORD(0xc4),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x1f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // MBM29LV160-B | AM29LV160-B
        device_id  : FLASHWORD(0x49),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV200
    {   // AM29LV200-T
        device_id  : FLASHWORD(0x3b),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x030000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV200-B
        device_id  : FLASHWORD(0xbf),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV320D
    {   // AM29LV320DT
        device_id  : FLASHWORD(0xF6),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV320D
        device_id  : FLASHWORD(0xF9),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL322D
    {   // AM29DL322D-T
        device_id  : FLASHWORD(0x55),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x380000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL322D-B
        device_id  : FLASHWORD(0x56),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x80000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL323D
    {   // AM29DL323D-T
        device_id  : FLASHWORD(0x50),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x300000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL323D-B
        device_id  : FLASHWORD(0x53),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x100000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL324D
    {   // AM29DL324D-T
        device_id  : FLASHWORD(0x5c),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL324D-B
        device_id  : FLASHWORD(0x5f),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL640D
{   // AM29DL640D
        long_device_id: true,
        device_id  : FLASHWORD(0x7e),
        device_id2 : FLASHWORD(0x02),
        device_id3 : FLASHWORD(0x01),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x0800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x8000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x7F0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x0700000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0400000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0100000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29F800
    {   // AM29F800-T
        device_id  : FLASHWORD(0xd6),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xf0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x08000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x04000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29F800-B
        device_id  : FLASHWORD(0x58),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV800
    {   // AM29LV800-T
        device_id  : FLASHWORD(0xda),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xf0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x08000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x04000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV800-B
        device_id  : FLASHWORD(0x5b),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_TC58FVB800
    {   // Toshiba TC58FVB800 (compatible with AM29LV800-B except for IDs.)
        device_id  : FLASHWORD(0xCE),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV081B
    {   // AM29LV081B
        device_id  : FLASHWORD(0x38),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 1
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV017D
    {   // AM29LV017D
        device_id  : FLASHWORD(0xC8),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 1
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV033C
    {   // AM29LV033C
        device_id  : FLASHWORD(0xA3),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        // Although this device is not a true banked device, we
        // treat the device as having two banks to get the 
        // Sector Protect Verify to work for the upper half of
        // the device.  Reference Note 9 for Table 9 in the
        // AMD data sheet.
        banked     : true,
        banks      : { 0x200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV065D
    {   // AM29LV065D
        device_id  : FLASHWORD(0x93),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x800000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 1
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL128N
    {   // AMD/SPANSION S29GL128N
        long_device_id: true,
        device_id  : FLASHWORD(0x7e),
        device_id2 : FLASHWORD(0x21),
        device_id3 : FLASHWORD(0x01),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 32,
    },
#endif

#else // 16 bit devices

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV128
    {   // AM29LV128
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2212),
        device_id3 : FLASHWORD(0x2200),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_MX29LV128
    {   // MX29LV128M-B
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2211),
        device_id3 : FLASHWORD(0x2200),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 16
    },
    {   // MX29LV128M-T
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2211),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xff0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 16
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV160
    {   // MBM29LV160-T | AM29LV160-T
        device_id  : FLASHWORD(0x22c4),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x1f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // MBM29LV160-B | AM29LV160-B
        device_id  : FLASHWORD(0x2249),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29PL160
    {   // AM29PL160-T
	    device_id  : FLASHWORD(0x2227),
		block_size : 0x00040000 * CYGNUM_FLASH_INTERLEAVE,
		block_count: 8,
        device_size: 0x00200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x00200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
		bootblocks : { 0x1c0000 * CYGNUM_FLASH_INTERLEAVE,
					   0x038000 * CYGNUM_FLASH_INTERLEAVE,
					   0x002000 * CYGNUM_FLASH_INTERLEAVE,
					   0x002000 * CYGNUM_FLASH_INTERLEAVE,
					   0x004000 * CYGNUM_FLASH_INTERLEAVE,
					   _LAST_BOOTBLOCK
					 },
        banked     : false,
        bufsiz     : 1
	},
    {   // AM29PL160-B
        device_id  : FLASHWORD(0x2245),
        block_size : 0x00040000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 8,
        device_size: 0x00200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x00200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x038000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV200
    {   // AM29LV200-T
        device_id  : FLASHWORD(0x223b),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x030000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV200-B
        device_id  : FLASHWORD(0x22bf),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_ST_M29W200B
    {   // ST M29W200BT
        device_id  : FLASHWORD(0x0051),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x030000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // ST M29W200BB
        device_id  : FLASHWORD(0x0057),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 4,
        device_size: 0x40000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x40000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_ST_M29W320D
    {   // M29W320DT
        device_id  : FLASHWORD(0x22ca),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE, // offset
                       0x008000 * CYGNUM_FLASH_INTERLEAVE, // size 1
                       0x002000 * CYGNUM_FLASH_INTERLEAVE, // size 2
                       0x002000 * CYGNUM_FLASH_INTERLEAVE, // size 3
                       0x004000 * CYGNUM_FLASH_INTERLEAVE, // size 4
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // M29W320DB
        device_id  : FLASHWORD(0x22cb),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE, // offset 
                       0x004000 * CYGNUM_FLASH_INTERLEAVE, // size 1
                       0x002000 * CYGNUM_FLASH_INTERLEAVE, // size 2
                       0x002000 * CYGNUM_FLASH_INTERLEAVE, // size 3
                       0x008000 * CYGNUM_FLASH_INTERLEAVE, // size 4
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV320D
    {   // AM29LV320DT
        device_id  : FLASHWORD(0x22F6),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV320D
        device_id  : FLASHWORD(0x22F9),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL322D
    {   // AM29DL322D-T
        device_id  : FLASHWORD(0x2255),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x380000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL322D-B
        device_id  : FLASHWORD(0x2256),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x80000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL323D
    {   // AM29DL323D-T
        device_id  : FLASHWORD(0x2250),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x300000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL323D-B
        device_id  : FLASHWORD(0x2253),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,  
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x100000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL324D
    {   // AM29DL324D-T
        device_id  : FLASHWORD(0x225c),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x3f0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
    {   // AM29DL324D-B
        device_id  : FLASHWORD(0x225f),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,  
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29DL640D
{   // AM29DL640D
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2202),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x800000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x7F0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x700000 * CYGNUM_FLASH_INTERLEAVE,
                       0x400000 * CYGNUM_FLASH_INTERLEAVE,
                       0x100000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV400
    {   // AM29LV400-T
        device_id  : FLASHWORD(0x22b9),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 8,
        device_size: 0x80000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x80000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xf0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x08000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x04000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV400-B
        device_id  : FLASHWORD(0x22ba),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 8,
        device_size: 0x80000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x80000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29F800
    {   // AM29F800-T
        device_id  : FLASHWORD(0x22d6),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xf0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x08000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x04000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29F800-B
        device_id  : FLASHWORD(0x2258),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV800
    {   // AM29LV800-T
        device_id  : FLASHWORD(0x22da),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0xf0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x08000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x02000 * CYGNUM_FLASH_INTERLEAVE,
                       0x04000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
    {   // AM29LV800-B
        device_id  : FLASHWORD(0x225b),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV640
    {   // MBM29LV640xx
        device_id  : FLASHWORD(0x22d7),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x800000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,  
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV640M
    {   // MBM29LV640xx
        device_id  : FLASHWORD(0x227e),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x800000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_TC58FVB800
    {   // Toshiba TC58FVB800 (compatible with AM29LV800-B except for IDs.)
        device_id  : FLASHWORD(0xCE),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 16,
        device_size: 0x100000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x100000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x004000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x002000 * CYGNUM_FLASH_INTERLEAVE,
                       0x008000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : false,
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29LV256
    {   // AMD AM29LV256
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2212),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 512,
        device_size: 0x2000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x2000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29PL032J
    {   // S29PL032J
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x220a),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x0400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x4000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x3F0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x0380000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0200000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0080000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29PL064J
    {   // S29PL064J
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2202),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x0800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x8000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x7F0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x0700000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0400000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0100000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29PL127J
    {   // S29PL127J
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2220),
        device_id3 : FLASHWORD(0x2200),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0xFF0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                     },
        banked     : true,
        banks      : { 0x0e00000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0800000 * CYGNUM_FLASH_INTERLEAVE,
                       0x0200000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL064M
    {   // AMD/SPANSION S29GL064M
	long_device_id: true,
	device_id  : FLASHWORD(0x227e),
	device_id2 : FLASHWORD(0x2210),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x0800000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x8000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : true,
        bootblocks : { 0x00000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x7F0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       0x2000 * CYGNUM_FLASH_INTERLEAVE,
                       _LAST_BOOTBLOCK
                      },
         banked     : true,
         banks      : { 0x0700000 * CYGNUM_FLASH_INTERLEAVE,
                        0x0400000 * CYGNUM_FLASH_INTERLEAVE,
                        0x0100000 * CYGNUM_FLASH_INTERLEAVE,
                        0
    		      },
        bufsiz     : 1
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL128N
    {   // AMD/SPANSION S29GL128N
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2221),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 128,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL256N
    {   // AMD/SPANSION S29GL256N
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2222),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x2000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x2000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL512N
    {   // AMD/SPANSION S29GL512N
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2223),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 512,
        device_size: 0x4000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x4000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL128M
    {   // AMD/SPANSION S29GL128M
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2212),
        device_id3 : FLASHWORD(0x2200),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 256,
        device_size: 0x1000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x1000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 16,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL512P
    {   // AMD/SPANSION S29GL512P
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2223),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 512,
        device_size: 0x4000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x4000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 32,
    },
#endif
#ifdef CYGHWR_DEVS_FLASH_AMD_S29GL01GP
    {   // AMD/SPANSION S29GL01GP
        long_device_id: true,
        device_id  : FLASHWORD(0x227e),
        device_id2 : FLASHWORD(0x2228),
        device_id3 : FLASHWORD(0x2201),
        block_size : 0x20000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 1024,
        device_size: 0x8000000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x8000000 * CYGNUM_FLASH_INTERLEAVE - 1),
        bootblock  : false,
        banked     : false,
        bufsiz     : 32,
    },
#endif

#endif // 16 bit devices

#endif // CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_PARTS_INL
