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
// Copyright (C) 2011 Free Software Foundation, Inc.                        
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
// Author(s):   ilijak
// Date:        2011-02-05
// Purpose:     TWR-K40X256 platform specific registers
// Description:
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm_kinetis_twr_k40x256.h>


// UART PINs

#ifndef CYGHWR_HAL_FREESCALE_UART3_PIN_RX
# define CYGHWR_HAL_FREESCALE_UART3_PIN_RX CYGHWR_HAL_KINETIS_PIN(C, 16, 3, 0)
# define CYGHWR_HAL_FREESCALE_UART3_PIN_TX CYGHWR_HAL_KINETIS_PIN(C, 17, 3, 0)
# define CYGHWR_HAL_FREESCALE_UART3_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_HAL_FREESCALE_UART3_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE

# define CYGHWR_IO_FREESCALE_UART3_PIN_RX CYGHWR_HAL_FREESCALE_UART3_PIN_RX
# define CYGHWR_IO_FREESCALE_UART3_PIN_TX CYGHWR_HAL_FREESCALE_UART3_PIN_TX
# define CYGHWR_IO_FREESCALE_UART3_PIN_RTS CYGHWR_HAL_FREESCALE_UART3_PIN_RTS
# define CYGHWR_IO_FREESCALE_UART3_PIN_CTS CYGHWR_HAL_FREESCALE_UART3_PIN_CTS
#endif


// DSPI
// DSPI Pins

#define CYGHWR_IO_FREESCALE_SPI1_PIN_SIN  CYGHWR_HAL_KINETIS_PIN(E, 3, 2, KINETIS_PIN_SPI1_IN_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_SOUT CYGHWR_HAL_KINETIS_PIN(E, 1, 2, KINETIS_PIN_SPI1_OUT_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_SCK  CYGHWR_HAL_KINETIS_PIN(E, 2, 2, KINETIS_PIN_SPI1_OUT_OPT)

#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS0  CYGHWR_HAL_KINETIS_PIN(E, 4, 2, KINETIS_PIN_SPI1_CS_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS1  CYGHWR_HAL_KINETIS_PIN(E, 0, 2, KINETIS_PIN_SPI1_CS_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS2  CYGHWR_HAL_KINETIS_PIN(E, 5, 2, KINETIS_PIN_SPI1_CS_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS3  CYGHWR_HAL_KINETIS_PIN(E, 6, 2, KINETIS_PIN_SPI1_CS_OPT)
#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS4  CYGHWR_HAL_KINETIS_PIN_NONE
#define CYGHWR_IO_FREESCALE_SPI1_PIN_CS5  CYGHWR_HAL_KINETIS_PIN_NONE

// I2C
// I2C Pins

# define CYGHWR_IO_I2C_FREESCALE_I2C0_PIN_SDA CYGHWR_HAL_KINETIS_PIN(B, 2, 2, 0)
# define CYGHWR_IO_I2C_FREESCALE_I2C0_PIN_SCL CYGHWR_HAL_KINETIS_PIN(B, 3, 2, 0)
# define CYGHWR_IO_I2C_FREESCALE_I2C1_PIN_SDA CYGHWR_HAL_KINETIS_PIN(C, 11, 2, 0)
# define CYGHWR_IO_I2C_FREESCALE_I2C1_PIN_SCL CYGHWR_HAL_KINETIS_PIN(C, 10, 2, 0)

//=============================================================================
// Memory access checks.
//
// Accesses to areas not backed by real devices or memory can cause
// the CPU to hang. These macros allow the GDB stubs to avoid making
// accidental accesses to these areas.

__externC int cyg_hal_stub_permit_data_access( CYG_ADDRESS addr, cyg_uint32 count );

#define CYG_HAL_STUB_PERMIT_DATA_READ(_addr_, _count_) cyg_hal_stub_permit_data_access( _addr_, _count_ )

#define CYG_HAL_STUB_PERMIT_DATA_WRITE(_addr_, _count_ ) cyg_hal_stub_permit_data_access( _addr_, _count_ )

//=============================================================================


//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
