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
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:       LPC-H2294 platform specific support routines
// Description:
// Usage:         #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/hal/var_io.h>

        // There is one LED on the board.
        // |P0.30 470 Om     LED
        // |------\/\/\/-----|<|-----> 3.3 V
        // |
        .macro  _led_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
        ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0DIR]
        orr r1,r1,#(1<<30)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0DIR]

        .endm // _led_init

        .macro _led x
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
        ldr r1,=(1<<30)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0SET]
        ldr r1,=((\x & 1)<<30)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0CLR]

        .endm // _led

        .macro _pll_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE

        mov r2,#0xAA
        mov r3,#0x55

        // enable PLL
        mov r1,#1
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]

        mov r1,#(0x20 | (CYGNUM_HAL_ARM_LPC2XXX_PLL_MUL - 1))
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCFG]

        // update PLL registers
        str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]
        str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

        // wait for it to lock
1:
        ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLSTAT]
        ands r1,r1,#(1<<10)
        beq 1b

        // connect PLL
        mov r1,#3
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]
        // update PLL registers
        str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]
        str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

        .endm // _pll_init

        .macro _mem_init
        // copy first 64 bytes from ROM to on-chip RAM
        mov r0,#0
        mov r1,#0x40000000
        mov r2,#0x40
1:
        ldr r3,[r0,#4]!
        str r3,[r1,#4]!
        cmp r0,r2
        bne 1b

        ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE
        mov r1,#2 // interrupt vector table is mapped to RAM
        str r1, [r0,#CYGARC_HAL_LPC2XXX_REG_MEMMAP]

        // flash timings
        mov r1,#4
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMTIM]
        mov r1,#2 // 2, full MAM
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMCR]

        // External memory interface depends on the bank width (32, 16 or 8 bit
        // selected via MW bits in corresponding BCFG register).  Furthermore,
        // choice of the memory chip(s) will require an adequate setup of RBLE
        // bit in BCFG register, too. RBLE = 0 in case of 8-bit based external
        // memories, while memory chips capable of accepting 16 or 32 bit wide
        // data will work with RBLE = 1.
        //
        // BANK0: 4M FLASH
        // TE28F320C3BD70 (1024Kx32 x 1, 70nS)
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG0
        ldr r1,=  (0x3 << 0)    /* IDCY=3, idle timing  */\
                | (0x4 << 5)    /* WST1=4, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x6 << 11)   /* WST2=6, write timing */\
                | (0x1 << 28)   /* MW=1,   16-bits      */
        str r1,[r0]

        // BANK1: 1M RAM
        // IDT71V416L (512Kx16 x 2, 12nS)
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG1
        // Warning: changed these timings, you can fall dramatically the eCos
        // kernel performance. Check it then using the eCos 'tm_basic' test.
        ldr r1,=  (0x0 << 0)    /* IDCY=0, idle cycles  */\
                | (0x0 << 5)    /* WST1=0, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x0 << 11)   /* WST2=0, write timing */\
                | (0x2 << 28)   /* MW=2,   32-bits      */
        str r1,[r0]

        .endm // _mem_init

        .macro _gpio_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE

        // Configure P0.15:0 as PIO, but UART0
        ldr r1,=  (0x1 << 0)    /* P0.0 as TxD0         */\
                | (0x1 << 2)    /* P0.1 as RxD0         */
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL0]

        // Configure P0.30:16 as PIO
        ldr r1,=0
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL1]

        // GPIO P1.25:16, P3.0, P3.25:24 are used as PIO
        ldr r1,=  (0x1 << 2)    /* P1.31:26 Debug port  */\
                | (0x2 << 4)    /* D31:0,CS0,OE,BLS0-3  */\
                | (0x1 << 8)    /* WE enabled           */\
                | (0x1 << 11)   /* CS1 enabled          */\
                | (0x1 << 24)   /* A1 enabled           */\
                | (0x7 << 25)   /* A23:2 enabled        */
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL2]

        .endm // _gpio_init

        .macro _relocate
        // Relocate ROM/FLASH to RAM
        ldr r0,= -8192
        and r0,r0,pc
        ldr r1,=(CYGMEM_REGION_ram)
        ldr r2,=(CYGMEM_REGION_ram+0x3E000)
10:     ldr r3,[r0],#4
        str r3,[r1],#4
        cmp r1,r2
        bne 10b
        ldr r0,=20f
        mov pc,r0
20:
        .endm // relocate

#define CYGHWR_LED_MACRO _led \x

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

        .macro  _setup

        _pll_init

        _mem_init

        _gpio_init

#if defined(CYG_HAL_STARTUP_ROMRAM)
        _relocate
#endif
        _led_init

        .endm


#define CYGSEM_HAL_ROM_RESET_USES_JUMP

#else

        .macro  _setup

        .endm

#endif // CYG_HAL_STARTUP_ROM

#define PLATFORM_SETUP1     _setup

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
