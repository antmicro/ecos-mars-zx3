//=============================================================================
//
//      helpers.c
//
//      Helper functions to access the interface information
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2009 Free Software Foundation, Inc.                        
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
// Author(s):   andrew.lunn
// Contributors:
// Date:        2003-02-22
// Description: Provides helper functions to access the network interface
//              information.
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/system.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>
#include <net/if_types.h>

/* How many interfaces are there? */

#ifdef CYGPKG_NET_FREEBSD_STACK
extern struct ifaddr **ifnet_addrs;

long cyg_snmp_num_interfaces(void) {
  int i,n=0;

  for (i=0; i<if_index; ++i)
    if (ifnet_addrs[i])
      ++n;

  return n;
}

struct ifnet *cyg_snmp_get_if(int if_num) {
  int i,n=0;

  for (i=0; i<if_index; ++i)
    if (ifnet_addrs[i])
      if (++n == if_num)
        return ifnet_addrs[i]->ifa_ifp;

  return NULL;
}
#endif

#ifdef CYGPKG_NET_OPENBSD_STACK
long cyg_snmp_num_interfaces(void) {
  register struct ifnet *ifp;
  long long_ret = 0;

  for (ifp = ifnet.tqh_first; ifp != 0; ifp = ifp->if_list.tqe_next)
    long_ret++;
  
  return long_ret;
}

struct ifnet *cyg_snmp_get_if(int if_num) {
  struct ifnet *ifp;
  
  for ( ifp = ifnet.tqh_first;
	if_num > 1 && ifp != 0;
          if_num-- )
        ifp = ifp->if_list.tqe_next;
  
  return ifp;
}
#endif


