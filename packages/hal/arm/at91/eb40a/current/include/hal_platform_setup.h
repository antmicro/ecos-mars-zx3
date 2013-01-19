#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2009 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:gthomas, tdrury, nickg
// Date:        2001-07-12
// Purpose:     AT91/EB40A platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/var_io.h>

        .macro  _led_init
        ldr     r0,=AT91_PIO
        ldr     r1,=0x000F0000
        str     r1,[r0,#AT91_PIO_PER]
        str     r1,[r0,#AT91_PIO_OER]
        _led    0
        _led    15
        .endm
        
        .macro _led y
        ldr     r0,=AT91_PIO
        ldr     r1,=0x000F0000
        str     r1,[r0,#AT91_PIO_SODR]
        ldr     r1,=(\y<<16)
        str     r1,[r0,#AT91_PIO_CODR]
#if 0                        
        mov     r0,#0x00100000        
1:      sub     r0,r0,#1
        cmp     r0,#0
        bne     1b
#endif
        .endm

#define CYGHWR_LED_MACRO _led \x
        
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

        
        .macro  _setup
        _led_init
        ldr     r10,=_InitMemory        // Initialize memory controller
        movs    r0,pc,lsr #20           // If ROM startup, PC < 0x100000
        moveq   r10,r10,lsl #12         //   mask address to low 20 bits
        moveq   r10,r10,lsr #12
        ldmia   r10!,{r0-r9,r11-r12}    // Table of initialization constants
#if defined(CYG_HAL_STARTUP_ROMRAM)
        ldr     r10,=0x0000FFFF
        and     r12,r12,r10
        ldr     r10,=0x01000000
        orr     r12,r12,r10
#endif
        stmia   r11!,{r0-r9}            // Write to controller
        mov     pc,r12                  // Change address space, break pipeline
_InitMemory:
        .long   0x01002535  // 0x01000000, 16MB,  2 cycles after transfer, 16-bit, 6 wait states
        .long   0x02002121  // 0x02000000, 16MB,  0 cycles after transfer, 16-bit, 1 wait state
        .long   0x20003E3E  // 0x20000000, 1MB,   7 cycles after transfer, 8-bit, 8 wait states
        .long   0x30000000  // unused
        .long   0x40000000  // unused
        .long   0x50000000  // unused
        .long   0x60000000  // unused
        .long   0x70000000  // unused
        .long   0x00000001  // REMAP commande
        .long   0x00000006  // 7 memory regions, standard read
        .long   AT91_EBI    // External Bus Interface address
        .long   10f         // address where to jump
10:
#if defined(CYG_HAL_STARTUP_ROMRAM)
        ldr     r0,=0x01000000          // Relocate FLASH/ROM to on-chip RAM
        ldr     r1,=0x02000000          // RAM base & length
        ldr     r2,=0x02010000
20:     ldr     r3,[r0],#4
        str     r3,[r1],#4
        cmp     r1,r2
        bne     20b
        ldr     r0,=30f
        mov     pc,r0
30:
#endif
        ldr     r0,=AT91_PS             // Power saving interface
        ldr     r1,=0xFFFFFFFF          // Enable all peripheral [clocks]
        str     r1,[r0,#AT91_PS_PCER]
        ldr     r0,=AT91_PIO            // Disable PIO (so peripherals can use bits)
        ldr     r1,=0x0070FE49          // UART, FIQ, EINT, Timer clocks
        str     r1,[r0,#AT91_PIO_PDR]
        ldr     r1,=0x000001B6          // LEDs
        str     r1,[r0,#AT91_PIO_OER]
        str     r1,[r0,#AT91_PIO_SODR]
        .endm

#define CYGSEM_HAL_ROM_RESET_USES_JUMP
#define PLATFORM_SETUP1     _setup
#else
#define PLATFORM_SETUP1     _led_init
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
