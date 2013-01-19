//==========================================================================
//
//      usbs_d12_data.cxx
//
//      Static data for the D12 USB device driver
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    fmp
// Contributors: fmp
// Date:         2004-05-27
//
// This file contains various objects that should go into extras.o
// rather than libtarget.a, e.g. devtab entries that would normally
// be eliminated by the selective linking.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/io/devtab.h>
#include <cyg/io/usb/usbs_d12.h>
#include <pkgconf/devs_usb_d12.h>

// ----------------------------------------------------------------------------
// Initialization. The goal here is to call usbs_d12_init()
// early on during system startup, to take care of things like
// registering interrupt handlers etc. which are best done
// during system init.
//
// If the endpoint 0 devtab entry is available then its init()
// function can be used to take care of this. However the devtab
// entries are optional so an alternative mechanism must be
// provided. Unfortunately although it is possible to give
// a C function the constructor attribute, it cannot be given
// an initpri attribute. Instead it is necessary to define a
// dummy C++ class.

extern "C" void usbs_d12_init(void);

#ifndef CYGVAR_DEVS_USB_D12_EP0_DEVTAB_ENTRY
class usbs_d12_initialization {
public:
  usbs_d12_initialization() {
    usbs_d12_init();
  }
};

static usbs_d12_initialization usbs_d12_init_object 
       CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);
#endif

// ----------------------------------------------------------------------------
// The devtab entries. Each of these is optional, many applications
// will want to use the lower-level API rather than go via
// open/read/write/ioctl.

#ifdef CYGVAR_DEVS_USB_D12_EP0_DEVTAB_ENTRY

// For endpoint 0 the only legal operations are get_config() and
// set_config(), and these are provided by the common package.

static bool 
usbs_d12_devtab_ep0_init(struct cyg_devtab_entry* tab)
{
  CYG_UNUSED_PARAM(struct cyg_devtab_entry*, tab);
  usbs_d12_init();
  return true;
}

static 
CHAR_DEVIO_TABLE(usbs_d12_ep0_devtab_functions,
		 &cyg_devio_cwrite,
		 &cyg_devio_cread,
		 &cyg_devio_select,
		 &usbs_devtab_get_config,
		 &usbs_devtab_set_config);

static CHAR_DEVTAB_ENTRY(usbs_d12_ep0_devtab_entry,
			 CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "0c",
			 0,
			 &usbs_d12_ep0_devtab_functions,
			 &usbs_d12_devtab_ep0_init,
			 0,
			 (void*) &usbs_d12_ep0);
#endif

// ----------------------------------------------------------------------------
// Common routines for ep1 and ep2.

#if defined(CYGVAR_DEVS_USB_D12_TX_EP1_DEVTAB_ENTRY) ||	    \
  defined(CYGVAR_DEVS_USB_D12_RX_EP1_DEVTAB_ENTRY) ||	    \
  defined(CYGVAR_DEVS_USB_D12_TX_EP2_DEVTAB_ENTRY) ||	    \
  defined(CYGVAR_DEVS_USB_D12_RX_EP2_DEVTAB_ENTRY)

static bool 
usbs_d12_devtab_dummy_init(struct cyg_devtab_entry* tab)
{
  CYG_UNUSED_PARAM(struct cyg_devtab_entry*, tab);
  return true;
}
#endif

// ----------------------------------------------------------------------------
// tx (in) ep1 devtab entry. This can only be used for slave->host,
// so only the cwrite() function makes sense.

#ifdef CYGVAR_DEVS_USB_D12_TX_EP1_DEVTAB_ENTRY

static 
CHAR_DEVIO_TABLE(usbs_d12_tx_ep1_devtab_functions,
		 &usbs_devtab_cwrite,
		 &cyg_devio_cread,
		 &cyg_devio_select,
		 &usbs_devtab_get_config,
		 &usbs_devtab_set_config);

static 
CHAR_DEVTAB_ENTRY(usbs_d12_tx_ep1_devtab_entry,
		  CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "1w",
		  0,
		  &usbs_d12_tx_ep1_devtab_functions,
		  &usbs_d12_devtab_dummy_init,
		  0,
		  (void*) &usbs_d12_tx_ep1);
#endif

// ----------------------------------------------------------------------------
// rx (out) ep1 devtab entry. This can only be used for host->slave, 
// so only the cread() function makes sense.

#ifdef CYGVAR_DEVS_USB_D12_RX_EP1_DEVTAB_ENTRY

static 
CHAR_DEVIO_TABLE(usbs_d12_rx_ep1_devtab_functions,
		 &cyg_devio_cwrite,
		 &usbs_devtab_cread,
		 &cyg_devio_select,
		 &usbs_devtab_get_config,
		 &usbs_devtab_set_config);

static 
CHAR_DEVTAB_ENTRY(usbs_d12_rx_ep1_devtab_entry,
		  CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "1r",
		  0,
		  &usbs_d12_rx_ep1_devtab_functions,
		  &usbs_d12_devtab_dummy_init,
		  0,
		  (void*) &usbs_d12_rx_ep1);
#endif


// ----------------------------------------------------------------------------
// tx (in) ep2 devtab entry. This can only be used for slave->host, so only the
// cwrite() function makes sense.

#ifdef CYGVAR_DEVS_USB_D12_TX_EP2_DEVTAB_ENTRY

static 
CHAR_DEVIO_TABLE(usbs_d12_tx_ep2_devtab_functions,
		 &usbs_devtab_cwrite,
		 &cyg_devio_cread,
		 &cyg_devio_select,
		 &usbs_devtab_get_config,
		 &usbs_devtab_set_config);

static 
CHAR_DEVTAB_ENTRY(usbs_d12_tx_ep2_devtab_entry,
		  CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "2w",
		  0,
		  &usbs_d12_tx_ep2_devtab_functions,
		  &usbs_d12_devtab_dummy_init,
		  0,
		  (void*) &usbs_d12_tx_ep2);
#endif

// ----------------------------------------------------------------------------
// rx (out) ep2 devtab entry. This can only be used for host->slave, 
// so only the cread() function makes sense.

#ifdef CYGVAR_DEVS_USB_D12_RX_EP2_DEVTAB_ENTRY

static 
CHAR_DEVIO_TABLE(usbs_d12_rx_ep2_devtab_functions,
		 &cyg_devio_cwrite,
		 &usbs_devtab_cread,
		 &cyg_devio_select,
		 &usbs_devtab_get_config,
		 &usbs_devtab_set_config);

static 
CHAR_DEVTAB_ENTRY(usbs_d12_rx_ep2_devtab_entry,
		  CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "2r",
		  0,
		  &usbs_d12_rx_ep2_devtab_functions,
		  &usbs_d12_devtab_dummy_init,
		  0,
		  (void*) &usbs_d12_rx_ep2);
#endif
