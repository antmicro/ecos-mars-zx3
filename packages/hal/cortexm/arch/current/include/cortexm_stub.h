#ifndef CYGONCE_HAL_CORTEXM_STUB_H
#define CYGONCE_HAL_CORTEXM_STUB_H
/*==========================================================================
//
//      cortexm_stub.h
//
//      Cortex-M GDB stub support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2012 Free Software Foundation, Inc.                        
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
// Author(s):      nickg
// Contributor(s): ilijak
// Date:         2008-07-30
//
//####DESCRIPTIONEND####
//
//========================================================================
*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGHWR_HAL_CORTEXM_FPU
#include <cyg/hal/cortexm_fpu.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifndef CYGHWR_HAL_CORTEXM_FPU
// The ARM has float (and possibly other coprocessor) registers that are
// larger than it can hold in a target_register_t.
# define TARGET_HAS_LARGE_REGISTERS

// ARM stub has special needs for register handling (not all regs are the
// the same size), so special put_register and get_register are provided.
# define CYGARC_STUB_REGISTER_ACCESS_DEFINED 1

# define NUMREGS    (16+8+2)  // 16 GPR, 8 FPR (unused), 2 PS

# define REGSIZE( _x_ ) (((_x_) < F0 || (_x_) >= FPS) ? 4 : 12)

# ifndef TARGET_REGISTER_T_DEFINED
#  define TARGET_REGISTER_T_DEFINED
typedef unsigned long target_register_t;
# endif

enum regnames {
    R0, R1, R2, R3, R4, R5, R6, R7,
    R8, R9, R10, FP, IP, SP, LR, PC,
    F0, F1, F2, F3, F4, F5, F6, F7,
    FPS, PS
};

#endif // CYGHWR_HAL_CORTEXM_FPU

# define HAL_STUB_REGISTERS_SIZE \
   ((sizeof(HAL_CORTEXM_GDB_Registers) + sizeof(target_register_t) - 1) / sizeof(target_register_t))

# define PS_N 0x80000000
# define PS_Z 0x40000000
# define PS_C 0x20000000
# define PS_V 0x10000000

typedef enum regnames regnames_t;

//------------------------------------------------------------------------

/* Given a trap value TRAP, return the corresponding signal. */
extern int __computeSignal (unsigned int trap_number);

/* Return the trap number corresponding to the last-taken trap. */
extern int __get_trap_number (void);

/* Return the currently-saved value corresponding to register REG. */
extern target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
extern void put_register (regnames_t which, target_register_t value);

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */
extern void set_pc (target_register_t pc);

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
void __single_step (void);

/* Clear the single-step state. */
void __clear_single_step (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
extern int __is_breakpoint_function (void);

/* Skip the current instruction. */
extern void __skipinst (void);

extern void __install_breakpoints (void);

extern void __clear_breakpoints (void);

extern int __is_bsp_syscall(void);

//------------------------------------------------------------------------
// Special definition of CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// we can only do this at all if break support is enabled:

// If this macro is used from Thumb code, we need to pass this information
// along to the place_break function so it can do the right thing.
#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )                         \
do {                                                                          \
    HAL_DISABLE_INTERRUPTS(_old_);                                            \
    cyg_hal_gdb_place_break((target_register_t)((unsigned long)&&cyg_hal_gdb_break_place));\
} while ( 0 )

#endif // CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT


#ifdef __cplusplus
} /* extern "C" */
#endif

//==========================================================================
#endif // ifndef CYGONCE_HAL_CORTEXM_STUB_H
