//=================================================================
//
//        flash1.c
//
//        Simple tests for FLASH drivers
//
//=================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2005 Free Software Foundation, Inc.
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  
// Date:          2004-04-24
// Description:   Simple test of FLASH I/O.
//
// Options:
//####DESCRIPTIONEND####

// #define DEBUG_PRINTFS

#include <pkgconf/hal.h>
#include <pkgconf/io_flash.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <cyg/io/flash.h>
#include <cyg/hal/hal_if.h>

#ifndef CYGSEM_IO_FLASH_LEGACY_API
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Only usable with legacy API");
}
#else
externC void
cyg_start( void )
{
    int stat;
    void *err_addr;
    CYG_ADDRWORD flash_start, flash_end;
    void **flash_start_addr = (void *)&flash_start;
    void **flash_end_addr = (void *)&flash_end;
    void *flash_test_start, *flash_addr;
    cyg_int32 flash_block_size, flash_num_blocks;
    CYG_ADDRWORD test_buf1, test_buf2;
    cyg_uint32 *lp1, *lp2;
    int i, len;
    cyg_bool passed, ok;

    CYG_TEST_INIT();

#if 0
    int j;
    
    diag_printf("Testing udelay: ");
    for (i = 0;  i < 30;  i++) {
        for (j = 0;  j < 1000;  j++) {
            CYGACC_CALL_IF_DELAY_US(1000);  // Should be 1 second
        }
        diag_printf(".");
    }
    diag_printf("\n");
#endif

    passed = true;

    if ((stat = flash_init(diag_printf)) != 0) {
        diag_printf("FLASH: driver init failed: %s\n", flash_errmsg(stat));
        CYG_TEST_FAIL_FINISH("FLASH driver init failed");
    }
    flash_get_limits((void *)0, flash_start_addr, flash_end_addr);
    // Keep 'end' address as last valid location, to avoid wrap around problems
    flash_end = flash_end - 1;
    flash_get_block_info(&flash_block_size, &flash_num_blocks);

    diag_printf("FLASH: 0x%x - 0x%x, %d blocks of 0x%x bytes each.\n", 
                flash_start, flash_end + 1, flash_num_blocks, 
                flash_block_size);

    // Verify that the testing limits are within the bounds of the
    // physical device.  Also verify that the size matches with
    // the erase block size on the device
    if ((CYGNUM_IO_FLASH_TEST_OFFSET > (flash_end - flash_start)) ||
        ((CYGNUM_IO_FLASH_TEST_OFFSET + CYGNUM_IO_FLASH_TEST_LENGTH) > (flash_end - flash_start))) {
        CYG_TEST_FAIL_FINISH("FLASH test region outside physical limits");
    }
    if ((CYGNUM_IO_FLASH_TEST_LENGTH % flash_block_size) != 0) {
        CYG_TEST_FAIL_FINISH("FLASH test region must be integral multiple of erase block size");
    }

    // Allocate two buffers large enough for the test
    test_buf1 = (CYG_ADDRWORD)CYGMEM_SECTION_heap1;
    test_buf2 = test_buf1 + CYGNUM_IO_FLASH_TEST_LENGTH;
    if (CYGMEM_SECTION_heap1_SIZE < (CYGNUM_IO_FLASH_TEST_LENGTH * 2)) {
        CYG_TEST_FAIL_FINISH("FLASH not enough heap space - reduce size of test region");
    }
    diag_printf("... Using test buffers at %p and %p\n", (void *)test_buf1, (void *)test_buf2);
    flash_test_start = (void *)(flash_start + CYGNUM_IO_FLASH_TEST_OFFSET);

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING    
    // Unlock test
    diag_printf("... Unlock test\n");
    ok = true;
    if ((stat = flash_unlock(flash_test_start, 
                             CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: unlock failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
#endif
    
    // Erase test
    diag_printf("... Erase test\n");
    ok = true;
    if ((stat = flash_erase(flash_test_start, 
                            CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: erase failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
    if (ok && (stat = flash_read(flash_test_start, (void *)test_buf1,
                           CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: read/verify after erase failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
    lp1 = (cyg_uint32 *)test_buf1;
    for (i = 0;  i < CYGNUM_IO_FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        if (*lp1++ != 0xFFFFFFFF) {
            diag_printf("FLASH: non-erased data found at offset %x\n", (CYG_ADDRWORD)(lp1-1) - test_buf1);
            diag_dump_buf((void *)(lp1-1), 32);
            ok = false;
            break;
        }
    }
    // Try reading in little pieces
    len = CYGNUM_IO_FLASH_TEST_LENGTH;
    flash_addr = flash_test_start;
    while (len > 0) {
        if ((stat = flash_read(flash_addr, (void *)test_buf1, 0x200, &err_addr)) != CYG_FLASH_ERR_OK) {
            diag_printf("FLASH: read[short]/verify after erase failed: %s\n", flash_errmsg(stat));
            ok = false;
            break;
        }    
        flash_addr = (cyg_uint8 *)flash_addr + 0x200;
        len -= 0x200;
        lp1 = (cyg_uint32 *)test_buf1;
        for (i = 0;  i < 0x200;  i += sizeof(cyg_uint32)) {
            if (*lp1++ != 0xFFFFFFFF) {
                diag_printf("FLASH: non-erased data found at offset %p\n", 
                            (cyg_uint8 *)flash_addr + (CYG_ADDRWORD)((lp1-1) - test_buf1));
                diag_dump_buf((void *)(lp1-1), 32);
                ok = false;
                len = 0;
                break;
            }
        }
    }
    
    if (!ok) {
        CYG_TEST_INFO("FLASH erase failed");
        passed = false;
    }

    // Simple write/verify test
    diag_printf("... Write/verify test\n");
    lp1 = (cyg_uint32 *)test_buf1;
    for (i = 0;  i < CYGNUM_IO_FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        *lp1 = (cyg_uint32)lp1;
        lp1++;
    }
    ok = true;
    if (ok && (stat = flash_program(flash_test_start, (void *)test_buf1,
                                    CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: write failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
    if (ok && (stat = flash_read(flash_test_start, (void *)test_buf2,
                                    CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: read/verify after write failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
    lp1 = (cyg_uint32 *)test_buf1;
    lp2 = (cyg_uint32 *)test_buf2;
    for (i = 0;  i < CYGNUM_IO_FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        if (*lp2++ != *lp1++) {
            diag_printf("FLASH: incorrect data found at offset %x\n", (CYG_ADDRWORD)(lp2-1) - test_buf2);
            diag_dump_buf((void *)(lp2-1), 32);
            ok = false;
            break;
        }
    }
    // Try reading in little pieces
    len = CYGNUM_IO_FLASH_TEST_LENGTH;
    flash_addr = flash_test_start;
    lp1 = (cyg_uint32 *)test_buf1;
    lp2 = (cyg_uint32 *)test_buf2;
    while (len > 0) {
        if ((stat = flash_read(flash_addr, lp2, 0x200, &err_addr)) != CYG_FLASH_ERR_OK) {
            diag_printf("FLASH: read[short]/verify after erase failed: %s\n", flash_errmsg(stat));
            ok = false;
            break;
        }    
        flash_addr = (cyg_uint8 *)flash_addr + 0x200;
        len -= 0x200;
        for (i = 0;  i < 0x200;  i += sizeof(cyg_uint32)) {
            if (*lp2++ != *lp1++) {
                diag_printf("FLASH: incorrect data found at offset %p\n", 
                            (cyg_uint8 *)flash_addr + (CYG_ADDRWORD)((lp2-1) - test_buf2));
                diag_dump_buf((void *)(lp2-1), 32);
                ok = false;
                len = 0;
                break;
            }
        }
    }
    
    if (!ok) {
        CYG_TEST_INFO("FLASH write/verify failed");
    }

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
    // Lock test
    diag_printf("... Lock test\n");
    ok = true;
    if ((stat = flash_lock(flash_test_start, 
                           CYGNUM_IO_FLASH_TEST_LENGTH, &err_addr)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: unlock failed: %s\n", flash_errmsg(stat));
        ok = false;
    }    
#endif
    
    if (passed) {
        CYG_TEST_PASS_FINISH("FLASH test1");
    } else {
        CYG_TEST_FAIL_FINISH("FLASH test1");
    }
}
#endif

// EOF flash1.c
