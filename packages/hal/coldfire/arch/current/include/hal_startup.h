#ifndef CYGONCE_HAL_STARTUP_H
#define CYGONCE_HAL_STARTUP_H

//=============================================================================
//
//      hal_startup.h
//
//      HAL startup definitions for the ColdFire architecture
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Provide ColdFire-specific startup definitions.
// Usage:         #include <cyg/hal/hal_startup.h>
//
//####DESCRIPTIONEND####
//========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_intr.h>

// Include the variant-specific startup header
#include <cyg/hal/var_startup.h>

externC void hal_reset(void) __attribute__ ((section (".boot")));

// ---------------------------------------------------------------------------
// End of hal_startup.h
#endif // CYGONCE_HAL_STARTUP_H
