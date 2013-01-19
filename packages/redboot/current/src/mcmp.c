//==========================================================================
//
//      mcmp.c
//
//      RedBoot memory compare (mcmp) routine
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2002-08-06
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>

RedBoot_cmd("mcmp", 
            "Compare two blocks of memory",
            "-s <location> -d <location> -l <length> [-1|-2|-4]",
            do_mcmp
    );

void
do_mcmp(int argc, char *argv[])
{
    // Fill a region of memory with a pattern
    struct option_info opts[6];
    unsigned long src_base, dst_base;
    long len;
    bool src_base_set, dst_base_set, len_set;
    bool set_32bit, set_16bit, set_8bit;

    init_opts(&opts[0], 's', true, OPTION_ARG_TYPE_NUM, 
              (void *)&src_base, (bool *)&src_base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&len, (bool *)&len_set, "length");
    init_opts(&opts[2], 'd', true, OPTION_ARG_TYPE_NUM, 
              (void *)&dst_base, (bool *)&dst_base_set, "base address");
    init_opts(&opts[3], '4', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_32bit, (bool *)0, "fill 32 bit units");
    init_opts(&opts[4], '2', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_16bit, (bool *)0, "fill 16 bit units");
    init_opts(&opts[5], '1', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_8bit, (bool *)0, "fill 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 6, 0, 0, "")) {
        return;
    }
    if (!src_base_set || !dst_base_set || !len_set) {
        diag_printf("usage: mcmp -s <addr> -d <addr> -l <length> [-1|-2|-4]\n");
        return;
    }

    

    if (set_8bit) {
        cyg_uint8 *s = (cyg_uint8 *)src_base;
        cyg_uint8 *d = (cyg_uint8 *)dst_base;
        while ((len -= sizeof(cyg_uint8)) >= 0) {
            if (*s++ != *d++) {
                s--;
                d--;
                diag_printf("Buffers don't match - %p=0x%02x, %p=0x%02x\n",
                            s, *s, d, *d);
                return;
            }

        }
    } else if (set_16bit) {
	cyg_uint16 *s = (cyg_uint16 *)src_base;
	cyg_uint16 *d = (cyg_uint16 *)dst_base;
	while ((len -= sizeof(cyg_uint16)) >= 0) {
            if (*s++ != *d++) { 
                s--;
                d--;
                diag_printf("Buffers don't match - %p=0x%04x, %p=0x%04x\n",
                            s, *s, d, *d);
                return;
            }

	}
    } else {
      cyg_uint32 *s = (cyg_uint32 *)src_base;
      cyg_uint32 *d = (cyg_uint32 *)dst_base;
      while ((len -= sizeof(cyg_uint32)) >= 0) {
	if (*s++ != *d++) {
	  s--;
	  d--;
	  diag_printf("Buffers don't match - %p=0x%08x, %p=0x%08x\n",
		      s, *s, d, *d);
	  return;
	}
      }
    }
}
