#ifndef CYGONCE_DEVS_FLASH_SST_39VFXXX_INL
#define CYGONCE_DEVS_FLASH_SST_39VFXXX_INL
//==========================================================================
//
//      flash_sst_39vfxxx.inl
//
//      SST 39VFxxx series flash driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2006 Free Software Foundation, Inc.
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
// Contributors: gthomas, jskov, rcassebohm, Andrew Lunn
// Date:         2001-02-21
// Purpose:      
// Description:  
//              
// Notes:        FLASH_P2V is not properly used.
//               Needs locking.
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/infra/cyg_ass.h>
#include CYGHWR_MEMORY_LAYOUT_H

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

//----------------------------------------------------------------------------
// Common device details.
#define FLASH_Read_ID                   FLASHWORD( 0x90 )
#define FLASH_Read_ID_Exit              FLASHWORD( 0xF0 )
#define FLASH_Reset                     FLASHWORD( 0xFF )
#define FLASH_Program                   FLASHWORD( 0xA0 )
#define FLASH_Block_Erase               FLASHWORD( 0x30 )

#define FLASH_Data                      FLASHWORD( 0x80 ) // Data complement
#define FLASH_Busy                      FLASHWORD( 0x40 ) // "Toggle" bit
#define FLASH_Err                       FLASHWORD( 0x20 )
#define FLASH_Sector_Erase_Timer        FLASHWORD( 0x08 )

#define FLASH_Setup_Addr1               (0x5555)
#define FLASH_Setup_Addr2               (0x2AAA)
#define FLASH_Setup_Code1               FLASHWORD( 0xAA )
#define FLASH_Setup_Code2               FLASHWORD( 0x55 )
#define FLASH_Setup_Erase               FLASHWORD( 0x80 )

// Platform code must define the below
// #define CYGNUM_FLASH_INTERLEAVE      : Number of interleaved devices (in parallel)
// #define CYGNUM_FLASH_SERIES          : Number of devices in series
// And select one of the below device variants

#ifdef CYGPKG_DEVS_FLASH_SST_39VF080
# define FLASH_BLOCK_SIZE               (0x1000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (256)
# define CYGNUM_FLASH_BASE_MASK         (0xFFF00000u) // 1024kB devices
# define CYGNUM_FLASH_WIDTH             (8)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0xBF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xD8)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF016
# define FLASH_BLOCK_SIZE               (0x1000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (512)
# define CYGNUM_FLASH_BASE_MASK         (0xFFE00000u) // 2048kB devices
# define CYGNUM_FLASH_WIDTH             (8)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0xBF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xD9)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF400
# define FLASH_BLOCK_SIZE               (0x1000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (0x80000/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFF80000u) // 512kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x2780)
#endif

#ifdef CYGPKG_DEVS_FLASH_SST_39VF160
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((2*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFE00000u) // 2048kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x2782)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF1601
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((2*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFE00000u) // 2048kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x234B)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF1602
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((2*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFE00000u) // 2048kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x234A)
#endif

#ifdef CYGPKG_DEVS_FLASH_SST_39VF320
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((4*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFC00000u) // 4096kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x2784)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF3201
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((4*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFC00000u) // 4096kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x235B)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF3202
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((4*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFFC00000u) // 4096kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x235A)
#endif

#ifdef CYGPKG_DEVS_FLASH_SST_39VF6401
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((8*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFF800000u) // 8184kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x236B)
#endif
#ifdef CYGPKG_DEVS_FLASH_SST_39VF6402
# define FLASH_BLOCK_SIZE               ((4*1024)*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              ((8*1024*1024)/FLASH_BLOCK_SIZE)
# define CYGNUM_FLASH_BASE_MASK         (0xFF800000u) // 8184kB devices
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x00BF)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x236A)
#endif

#define FLASH_DEVICE_SIZE               (FLASH_BLOCK_SIZE*FLASH_NUM_REGIONS)
#define CYGNUM_FLASH_DEVICES            (CYGNUM_FLASH_INTERLEAVE*CYGNUM_FLASH_SERIES)

//----------------------------------------------------------------------------
// Now that device properties are defined, include magic for defining
// accessor type and constants.
#include <cyg/io/flash_dev.h>

//----------------------------------------------------------------------------
// Functions that put the flash device into non-read mode must reside
// in RAM.
static size_t sst_query(struct cyg_flash_dev *dev, void* data, size_t len)
  __attribute__ ((section (".2ram.flash_query")));
static int sst_erase_block(struct cyg_flash_dev *dev, cyg_flashaddr_t block_base)
  __attribute__ ((section (".2ram.flash_erase_block")));
static int sst_program(struct cyg_flash_dev *dev, cyg_flashaddr_t base, 
            const void* data, size_t length)
  __attribute__ ((section (".2ram.flash_program_buf")));


//----------------------------------------------------------------------------
// Initialize driver details
static int
sst_init(struct cyg_flash_dev *dev)
{
  flash_data_t id[2];

  dev->funs->flash_query(dev,id,sizeof(id));

  // Check that flash_id data is matching the one the driver was
  // configured for.
  if (id[0] != CYGNUM_FLASH_ID_MANUFACTURER
      || id[1] != CYGNUM_FLASH_ID_DEVICE)
    return CYG_FLASH_ERR_DRV_WRONG_PART;
  return CYG_FLASH_ERR_OK;
}

//----------------------------------------------------------------------------
// Flash Query
//
// Only reads the manufacturer and part number codes for the first
// device(s) in series. It is assumed that any devices in series
// will be of the same type.

static size_t
sst_query(struct cyg_flash_dev *dev, void* data, size_t len)
{
    volatile flash_data_t *ROM;
    flash_data_t* id = (flash_data_t*) data;
    int i;
    
    CYG_ASSERT(len == 2, "Invalid length");

    ROM = (volatile flash_data_t*) dev->start;

    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Read_ID;

    // FIXME: 10ms delay
    for (i = 10000; i > 0; i--);

    // Manufacturers' code
    id[0] = ROM[0];
    // Part number
    id[1] = ROM[1];

    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Read_ID_Exit;

    // FIXME: 10ms delay
    for (i = 10000; i > 0; i--);
    
    return len;
}

//----------------------------------------------------------------------------
// Erase Block
static int
sst_erase_block(struct cyg_flash_dev *dev, cyg_flashaddr_t block_base)
{
    volatile flash_data_t* ROM;
    volatile flash_data_t* addr_ptr = (volatile flash_data_t*) block_base;
    int len = dev->block_info[0].block_size;
    int res = CYG_FLASH_ERR_OK;

    while ((CYG_FLASH_ERR_OK == res) && (len > 0)) {
	int timeout;
        flash_data_t state, prev_state;

        // Base address of device(s) being programmed. 
        ROM = (volatile flash_data_t*)((unsigned long)block_base & ~(FLASH_DEVICE_SIZE-1));

        // Program data [byte] - 6 step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Erase;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        *addr_ptr = FLASH_Block_Erase;

        // Wait for completion (bit 6 stops toggling)
        timeout = 5000000;
        prev_state = *addr_ptr & FLASH_Busy;
        while (true) {
            state = *addr_ptr & FLASH_Busy;
            if (prev_state == state) {
                break;
            }
            if (--timeout == 0) {
                res = CYG_FLASH_ERR_DRV_TIMEOUT;
                break;
            }
            prev_state = state;
        }
        // Verify loaded data bytes
        while (len > 0) {
            if (*addr_ptr != FLASH_BlankValue) {
                // Only update return value if erase operation was OK
                if (CYG_FLASH_ERR_OK == res) res = CYG_FLASH_ERR_DRV_VERIFY;
                break;
            }
            addr_ptr++;
            len -= sizeof(*addr_ptr);
        }
    }
    return CYG_FLASH_ERR_OK;
}

//----------------------------------------------------------------------------
// Program Buffer
static int
sst_program(struct cyg_flash_dev *dev, cyg_flashaddr_t base, 
            const void* data, size_t length)
{
    volatile flash_data_t* ROM;
    volatile flash_data_t* addr_ptr = (volatile flash_data_t*) base;
    volatile flash_data_t* data_ptr = (volatile flash_data_t*) data;
    size_t len = length;
    int res = CYG_FLASH_ERR_OK;

#if 0
    CYG_ASSERT((unsigned long)data_ptr & (sizeof(flash_data_t)-1) == 0, 
               "Data not properly aligned");
    CYG_ASSERT((unsigned long)addr_ptr & (CYGNUM_FLASH_INTERLEAVE*sizeof(flash_data_t)-1) == 0, 
               "Addr not properly aligned (to first interleaved device)");
#endif
    while ((CYG_FLASH_ERR_OK == res) && (len > 0)) {
	int timeout;
        flash_data_t state, prev_state;

        // Base address of device(s) being programmed. 
        ROM = (volatile flash_data_t*)((unsigned long)base & ~(FLASH_DEVICE_SIZE-1));

        // Program data [byte] - 4 step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Program;
        *addr_ptr = *data_ptr;

        // Wait for completion (bit 6 stops toggling)
        timeout = 5000000;
        prev_state = *addr_ptr & FLASH_Busy;
        while (true) {
            state = *addr_ptr & FLASH_Busy;
            if (prev_state == state) {
                break;
            }
            if (--timeout == 0) {
                res = CYG_FLASH_ERR_DRV_TIMEOUT;
                break;
            }
            prev_state = state;
        }

        // Verify loaded data bytes
        if (*addr_ptr != *data_ptr) {
            // Only update return value if program operation was OK
            if (CYG_FLASH_ERR_OK == res) res = CYG_FLASH_ERR_DRV_VERIFY;
            break;
        }
        addr_ptr++;
        data_ptr++;
        len -= sizeof(*data_ptr);
    }

    // Ideally, we'd want to return not only the failure code, but also
    // the address/device that reported the error.
    return res;
}

static const CYG_FLASH_FUNS(cyg_sst_funs,
	               sst_init,
	               sst_query,
	               sst_erase_block,
	               sst_program,
	               NULL,              // read
	               cyg_flash_devfn_lock_nop,
	               cyg_flash_devfn_unlock_nop);

#endif // CYGONCE_DEVS_FLASH_SST_39VFXXX_INL
