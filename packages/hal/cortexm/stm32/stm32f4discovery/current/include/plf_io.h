#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Platform specific registers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2013 Free Software Foundation, Inc.
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
// Author(s):   jld
// Date:        2013-06-06
// Purpose:     STM32F4-Discovery platform specific registers
// Description: 
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm_stm32_stm32f4discovery.h>

// User LEDs and button

#define CYGHWR_HAL_STM32F4DISCOVERY_LED1 CYGHWR_HAL_STM32_PIN_OUT( D, 12, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32F4DISCOVERY_LED2 CYGHWR_HAL_STM32_PIN_OUT( D, 13, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32F4DISCOVERY_LED3 CYGHWR_HAL_STM32_PIN_OUT( D, 14, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32F4DISCOVERY_LED4 CYGHWR_HAL_STM32_PIN_OUT( D, 15, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32F4DISCOVERY_BTN1 CYGHWR_HAL_STM32_PIN_IN( A, 0, NONE )

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
