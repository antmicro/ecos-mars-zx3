#ifndef CYGONCE_HAL_VAR_REGS_H
#define CYGONCE_HAL_VAR_REGS_H
//=============================================================================
//
//      var_regs.h
//
//      SH4 CPU definitions
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
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
// Author(s):   jskov
// Contributors:Ryozaburo Suzuki, nickg
// Date:        2000-04-18
// Purpose:     Define CPU memory mapped registers etc.
// Usage:       Included via the acrhitecture register header:
//              #include <cyg/hal/sh_regs.h>
// Notes:
//   This file describes registers for on-core modules found in all
//   the SH4 CPUs supported by the HAL. For each CPU is defined a
//   module specification file (mod_<CPU model number>.h) which lists
//   modules (and their version if applicable) included in that
//   particular CPU model.  Note that the versioning is ad hoc;
//   it doesn't reflect Hitachi internal versioning in any way.
//              
//####DESCRIPTIONEND####
//

// Find out which modules are supported by the chosen CPU
#include <pkgconf/system.h>
#include CYGBLD_HAL_CPU_MODULES_H

//==========================================================================
//                             CPU Definitions
//==========================================================================

//--------------------------------------------------------------------------
// Status register
#define CYGARC_REG_SR_MD                0x40000000
#define CYGARC_REG_SR_RB                0x20000000
#define CYGARC_REG_SR_BL                0x10000000
#define CYGARC_REG_SR_M                 0x00000200
#define CYGARC_REG_SR_Q                 0x00000100
#define CYGARC_REG_SR_IMASK             0x000000f0
#define CYGARC_REG_SR_I3                0x00000080
#define CYGARC_REG_SR_I2                0x00000040
#define CYGARC_REG_SR_I1                0x00000020
#define CYGARC_REG_SR_I0                0x00000010
#define CYGARC_REG_SR_S                 0x00000002
#define CYGARC_REG_SR_T                 0x00000001
#define CYGARC_REG_SR_FD                0x00008000

//--------------------------------------------------------------------------
// FPSCR register initial value

#define CYG_FPSCR 0x000C0000

//==========================================================================
//                             Module Definitions
//==========================================================================

#include <cyg/hal/mod_regs_bsc.h>
#include <cyg/hal/mod_regs_cac.h>
#include <cyg/hal/mod_regs_cpg.h>
#include <cyg/hal/mod_regs_intc.h>
#include <cyg/hal/mod_regs_mmu.h>
#include <cyg/hal/mod_regs_rtc.h>
#include <cyg/hal/mod_regs_ser.h>
#include <cyg/hal/mod_regs_tmu.h>
#include <cyg/hal/mod_regs_ubc.h>

#ifdef CYGPKG_HAL_SH_202
#include <cyg/hal/mod_regs_femi.h>
#include <cyg/hal/mod_regs_emi.h>
#endif

#ifdef CYGARC_SH_MOD_PCIC
#include <cyg/hal/mod_regs_pcic.h>
#endif

#endif // CYGONCE_HAL_VAR_REGS_H
