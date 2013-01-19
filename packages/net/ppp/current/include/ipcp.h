//==========================================================================
//
//      include/ipcp.h
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003 Free Software Foundation, Inc.                        
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
// ####BSDALTCOPYRIGHTBEGIN####                                             
// -------------------------------------------                              
// Portions of this software may have been derived from FreeBSD, OpenBSD,   
// or other sources, and if so are covered by the appropriate copyright     
// and license included herein.                                             
// -------------------------------------------                              
// ####BSDALTCOPYRIGHTEND####                                               
//==========================================================================

/*
 * ipcp.h - IP Control Protocol definitions.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $FreeBSD: src/usr.sbin/pppd/ipcp.h,v 1.10 1999/08/28 01:19:03 peter Exp $
 */

/*
 * Options.
 */
#define CI_ADDRS	1	/* IP Addresses */
#define CI_COMPRESSTYPE	2	/* Compression Type */
#define	CI_ADDR		3

#define CI_MS_DNS1	129	/* Primary DNS value */
#define CI_MS_WINS1	130	/* Primary WINS value */
#define CI_MS_DNS2	131	/* Secondary DNS value */
#define CI_MS_WINS2	132	/* Secondary WINS value */

#define MAX_STATES 16		/* from slcompress.h */

#define IPCP_VJMODE_OLD 1	/* "old" mode (option # = 0x0037) */
#define IPCP_VJMODE_RFC1172 2	/* "old-rfc"mode (option # = 0x002d) */
#define IPCP_VJMODE_RFC1332 3	/* "new-rfc"mode (option # = 0x002d, */
                                /*  maxslot and slot number compression) */

#define IPCP_VJ_COMP 0x002d	/* current value for VJ compression option*/
#define IPCP_VJ_COMP_OLD 0x0037	/* "old" (i.e, broken) value for VJ */
				/* compression option*/ 

typedef struct ipcp_options {
    int neg_addr : 1;		/* Negotiate IP Address? */
    int old_addrs : 1;		/* Use old (IP-Addresses) option? */
    int req_addr : 1;		/* Ask peer to send IP address? */
    int default_route : 1;	/* Assign default route through interface? */
    int proxy_arp : 1;		/* Make proxy ARP entry for peer? */
    int neg_vj : 1;		/* Van Jacobson Compression? */
    int old_vj : 1;		/* use old (short) form of VJ option? */
    int accept_local : 1;	/* accept peer's value for ouraddr */
    int accept_remote : 1;	/* accept peer's value for hisaddr */
#ifdef CYGOPT_PPP_NS_NEGOTIATE
    int neg_dns1 : 1;           /* Negotiate primary domain name server? */
    int neg_wins1 : 1;          /* Negotiate primary WINS? */
    int neg_dns2 : 1;           /* Negotiate secondary domain name server? */
    int neg_wins2 : 1;          /* Negotiate secondary WINS? */
#endif
    u_short vj_protocol;	/* protocol value to use in VJ option */
    u_char maxslotindex, cflag;	/* values for RFC1332 VJ compression neg. */
    u_int32_t ouraddr, hisaddr;	/* Addresses in NETWORK BYTE ORDER */
    u_int32_t dnsaddr[2];	/* Primary and secondary MS DNS entries */
    u_int32_t winsaddr[2];	/* Primary and secondary MS WINS entries */
} ipcp_options;

extern fsm ipcp_fsm[];
extern ipcp_options ipcp_wantoptions[];
extern ipcp_options ipcp_gotoptions[];
extern ipcp_options ipcp_allowoptions[];
extern ipcp_options ipcp_hisoptions[];

char *ip_ntoa __P((u_int32_t));

extern struct protent ipcp_protent;
