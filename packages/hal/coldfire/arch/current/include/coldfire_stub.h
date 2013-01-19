#ifndef CYGONCE_HAL_COLDFIRE_STUB_H
#define CYGONCE_HAL_COLDFIRE_STUB_H

//========================================================================
//
//      coldfire_stub.h
//
//      ColdFire-specific definitions for generic stub
//
//========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006 Free Software Foundation, Inc.
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
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       
// Description:   ColdFire-specific definitions for generic stub.
//
//####DESCRIPTIONEND####
//========================================================================

#ifdef __cplusplus
extern "C" {
#endif

#define NUMREGS 18

// ColdFire stub has special needs for register handling because flating point
// registers are bigger than the rest. Special put_register and get_register
// are provided.
#define CYGARC_STUB_REGISTER_ACCESS_DEFINED 1

#define REGSIZE( _x_ ) (4)

typedef unsigned long target_register_t;

enum regnames {
    D0, D1, D2, D3, D4, D5, D6, D7,
    A0, A1, A2, A3, A4, A5, A6, A7,
    SR, PC,
};

#define SP              A7

typedef enum regnames regnames_t;

// Given a trap value TRAP, return the corresponding signal
extern int __computeSignal(unsigned int trap_number);

// Return the ColdFire trap number corresponding to the last-taken trap
extern int __get_trap_number(void);

// Return the currently-saved value corresponding to register REG
extern target_register_t get_register(regnames_t reg);

// Store VALUE in the register corresponding to WHICH
extern void put_register(regnames_t which, target_register_t value);

// Read the contents of register WHICH into VALUE as raw bytes
extern int get_register_as_bytes(regnames_t which, char *value);

// Write the contents of register WHICH into VALUE as raw bytes
extern int put_register_as_bytes(regnames_t which, char *value);

// Set the currently-saved pc register value to PC. This also updates NPC
// as needed.
extern void set_pc(target_register_t pc);

// Set things up so that the next user resume will execute one instruction.
// This may be done by setting breakpoints or setting a single step flag
// in the saved user registers, for example.
extern void __single_step(void);

// Clear the single-step state
extern void __clear_single_step(void);

// If the breakpoint we hit is in the breakpoint() instruction, return a
// non-zero value
extern int __is_breakpoint_function(void);

// Skip the current instruction
extern void __skipinst(void);

extern void __install_breakpoints(void);

extern void __clear_breakpoints(void);

// We have to rewind the PC in case of a breakpoint.
#define HAL_STUB_PLATFORM_STUBS_FIXUP()                         \
CYG_MACRO_START                                                 \
    if (CYGNUM_HAL_VECTOR_DEBUGTRAP == __get_trap_number())     \
    {                                                           \
        CYG_ADDRESS pc = get_register(PC) - HAL_BREAKINST_SIZE; \
        put_register(PC, pc);                                   \
    }                                                           \
CYG_MACRO_END

#ifdef __cplusplus
}      // extern "C"
#endif

// ---------------------------------------------------------------------------
// End of coldfire_stub.h
#endif // ifndef CYGONCE_HAL_COLDFIRE_STUB_H
