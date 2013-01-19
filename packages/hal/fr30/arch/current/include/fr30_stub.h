//========================================================================
//
//      fr30_stub.h
//
//      FR30-specific definitions for remote debugging via gdb
//
//========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   larsi
// Contributors:
// Date:        2007-07-09
// Purpose:     Fujitsu FR30-specific definitions for gdb stubs support
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#ifndef CYGONCE_HAL_FR30_STUB_H
#define CYGONCE_HAL_FR30_STUB_H

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGPKG_IO_SERIAL
#include <pkgconf/io_serial.h>
#endif

#include <cyg/hal/hal_diag.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM, externC

#define HAL_STUB_PLATFORM_INIT_SERIAL()       HAL_DIAG_INIT()

#define HAL_STUB_PLATFORM_GET_CHAR()                                        \
((cyg_int8)({                                                               \
    cyg_int8 _ch_;                                                          \
    HAL_DIAG_READ_CHAR(_ch_);                                               \
    _ch_;                                                                   \
}))

#define HAL_STUB_PLATFORM_PUT_CHAR(c)         HAL_DIAG_WRITE_CHAR((c))

#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int,(baud))

#define HAL_STUB_PLATFORM_RESET()             HAL_DIAG_INIT()

#define HAL_STUB_PLATFORM_INIT()              HAL_DIAG_INIT()

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#define FR30_GENREGS        16
#define FR30_DEDICATEDREGS  8
#define FR30_REGSIZE        4 /* bytes */

// only NUMREGS and REGSIZE are the really needed macros

#define NUMREGS (FR30_GENREGS + FR30_DEDICATEDREGS)
#define REGSIZE( _x_ ) FR30_REGSIZE


typedef unsigned long target_register_t;

enum regnames {
     R0,   R1,  R2,   R3,   R4,   R5,   R6,   R7,
     R8,   R9,  R10,  R11,  R12,  R13,  R14,  SP,
     PC,   PS,  TBR,  RP,   SSP,  USP,  MDH,  MDL
};

typedef enum regnames regnames_t;

// Override generic stubs get_register() and use arch-specific version
#define CYGARC_STUB_REGISTER_ACCESS_DEFINED

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
#define HAL_GET_PROFILE_INFO( _thepc_, _thesp_ )                \
    CYG_MACRO_START                                             \
    extern HAL_SavedRegisters *hal_saved_interrupt_state;       \
    if ( hal_saved_interrupt_state ) {                          \
        (_thepc_) = (char *)(hal_saved_interrupt_state->pc);    \
        (_thesp_) = (char *)(hal_saved_interrupt_state->sp);    \
    }                                                           \
    CYG_MACRO_END
#endif

/* Given a trap value TRAP, return the corresponding signal. */
externC int __computeSignal (unsigned int trap_number);

/* Return the trap number corresponding to the last-taken trap. */
externC int __get_trap_number (void);

/* Return the currently-saved value corresponding to register REG. */
externC target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
externC void put_register (regnames_t which, target_register_t value);

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */
externC void set_pc (target_register_t pc);

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
externC void __single_step (void);

/* Clear the single-step state. */
externC void __clear_single_step (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
externC int __is_breakpoint_function (void);

/* Skip the current instruction. */
externC void __skipinst (void);

externC void __install_breakpoints (void);

externC void __clear_breakpoints (void);

#endif // ifndef CYGONCE_HAL_FR30_STUB_H

// EOF fr30_stub.h
