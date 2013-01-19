//==========================================================================
//
//      src/sys/netinet/raw_ip.c
//
//==========================================================================
// ####BSDCOPYRIGHTBEGIN####                                    
// -------------------------------------------                  
// This file is part of eCos, the Embedded Configurable Operating System.
//
// Portions of this software may have been derived from FreeBSD 
// or other sources, and if so are covered by the appropriate copyright
// and license included herein.                                 
//
// Portions created by the Free Software Foundation are         
// Copyright (C) 2002 Free Software Foundation, Inc.            
// -------------------------------------------                  
// ####BSDCOPYRIGHTEND####                                      
//==========================================================================

/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)raw_ip.c	8.7 (Berkeley) 5/15/95
 * $FreeBSD: src/sys/netinet/raw_ip.c,v 1.64.2.8 2001/07/29 19:32:40 ume Exp $
 */

#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/route.h>

#define _IP_VHL
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_pcb.h>
#include <netinet/in_var.h>
#include <netinet/ip_var.h>
#include <netinet/ip_mroute.h>

#include <netinet/ip_fw.h>

#ifdef IPSEC
#include <netinet6/ipsec.h>
#endif /*IPSEC*/

#ifdef DUMMYNET
#include <netinet/ip_dummynet.h>
#endif

struct	inpcbhead ripcb;
struct	inpcbinfo ripcbinfo;

/*
 * Nominal space allocated to a raw ip socket.
 */
#define	RIPSNDQ		8192
#define	RIPRCVQ		8192

/*
 * Raw interface to IP protocol.
 */

/*
 * Initialize raw connection block q.
 */
void
rip_init()
{
	LIST_INIT(&ripcb);
	ripcbinfo.listhead = &ripcb;
	/*
	 * XXX We don't use the hash list for raw IP, but it's easier
	 * to allocate a one entry hash list than it is to check all
	 * over the place for hashbase == NULL.
	 */
	ripcbinfo.hashbase = hashinit(1, M_PCB, &ripcbinfo.hashmask);
	ripcbinfo.porthashbase = hashinit(1, M_PCB, &ripcbinfo.porthashmask);
	ripcbinfo.ipi_zone = zinit("ripcb", sizeof(struct inpcb),
				   maxsockets, ZONE_INTERRUPT, 0);
}

static struct	sockaddr_in ripsrc = { sizeof(ripsrc), AF_INET };
/*
 * Setup generic address and protocol structures
 * for raw_input routine, then pass them along with
 * mbuf chain.
 */
void
rip_input(m, off)
	struct mbuf *m;
	int off;
{
	register struct ip *ip = mtod(m, struct ip *);
	register struct inpcb *inp;
	struct inpcb *last = 0;
	struct mbuf *opts = 0;
	int proto = ip->ip_p;

	ripsrc.sin_addr = ip->ip_src;
	LIST_FOREACH(inp, &ripcb, inp_list) {
#ifdef INET6
		if ((inp->inp_vflag & INP_IPV4) == 0)
			continue;
#endif
		if (inp->inp_ip_p && inp->inp_ip_p != proto)
			continue;
		if (inp->inp_laddr.s_addr &&
                  inp->inp_laddr.s_addr != ip->ip_dst.s_addr)
			continue;
		if (inp->inp_faddr.s_addr &&
                  inp->inp_faddr.s_addr != ip->ip_src.s_addr)
			continue;
		if (last) {
			struct mbuf *n = m_copy(m, 0, (int)M_COPYALL);

#ifdef IPSEC
			/* check AH/ESP integrity. */
			if (n && ipsec4_in_reject_so(n, last->inp_socket)) {
				m_freem(n);
				ipsecstat.in_polvio++;
				/* do not inject data to pcb */
			} else
#endif /*IPSEC*/
			if (n) {
				if (last->inp_flags & INP_CONTROLOPTS ||
				    last->inp_socket->so_options & SO_TIMESTAMP)
				    ip_savecontrol(last, &opts, ip, n);
				if (sbappendaddr(&last->inp_socket->so_rcv,
				    (struct sockaddr *)&ripsrc, n,
				    opts) == 0) {
					/* should notify about lost packet */
					m_freem(n);
					if (opts)
					    m_freem(opts);
				} else
					sorwakeup(last->inp_socket);
				opts = 0;
			}
		}
		last = inp;
	}
#ifdef IPSEC
	/* check AH/ESP integrity. */
	if (last && ipsec4_in_reject_so(m, last->inp_socket)) {
		m_freem(m);
		ipsecstat.in_polvio++;
		ipstat.ips_delivered--;
		/* do not inject data to pcb */
	} else
#endif /*IPSEC*/
	if (last) {
		if (last->inp_flags & INP_CONTROLOPTS ||
		    last->inp_socket->so_options & SO_TIMESTAMP)
			ip_savecontrol(last, &opts, ip, m);
		if (sbappendaddr(&last->inp_socket->so_rcv,
		    (struct sockaddr *)&ripsrc, m, opts) == 0) {
			m_freem(m);
			if (opts)
			    m_freem(opts);
		} else
			sorwakeup(last->inp_socket);
	} else {
		m_freem(m);
		ipstat.ips_noproto++;
		ipstat.ips_delivered--;
	}
}

/*
 * Generate IP header and pass packet to ip_output.
 * Tack on options user may have setup with control call.
 */
int
rip_output(m, so, dst)
	struct mbuf *m;
	struct socket *so;
	u_long dst;
{
	register struct ip *ip;
	register struct inpcb *inp = sotoinpcb(so);
	int flags = (so->so_options & SO_DONTROUTE) | IP_ALLOWBROADCAST;

	/*
	 * If the user handed us a complete IP packet, use it.
	 * Otherwise, allocate an mbuf for a header and fill it in.
	 */
	if ((inp->inp_flags & INP_HDRINCL) == 0) {
		if (m->m_pkthdr.len + sizeof(struct ip) > IP_MAXPACKET) {
			m_freem(m);
			return(EMSGSIZE);
		}
		M_PREPEND(m, sizeof(struct ip), M_WAIT);
		ip = mtod(m, struct ip *);
		ip->ip_tos = inp->inp_ip_tos;
		ip->ip_off = 0;
		ip->ip_p = inp->inp_ip_p;
		ip->ip_len = m->m_pkthdr.len;
		ip->ip_src = inp->inp_laddr;
		ip->ip_dst.s_addr = dst;
		ip->ip_ttl = inp->inp_ip_ttl;
	} else {
		if (m->m_pkthdr.len > IP_MAXPACKET) {
			m_freem(m);
			return(EMSGSIZE);
		}
		ip = mtod(m, struct ip *);
		/* don't allow both user specified and setsockopt options,
		   and don't allow packet length sizes that will crash */
		if (((IP_VHL_HL(ip->ip_vhl) != (sizeof (*ip) >> 2))
		     && inp->inp_options)
		    || (ip->ip_len > m->m_pkthdr.len)
		    || (ip->ip_len < (IP_VHL_HL(ip->ip_vhl) << 2))) {
			m_freem(m);
			return EINVAL;
		}
		if (ip->ip_id == 0)
#ifdef RANDOM_IP_ID
			ip->ip_id = ip_randomid();
#else
			ip->ip_id = htons(ip_id++);
#endif
		/* XXX prevent ip_output from overwriting header fields */
		flags |= IP_RAWOUTPUT;
		ipstat.ips_rawout++;
	}

#ifdef IPSEC
	if (ipsec_setsocket(m, so) != 0) {
		m_freem(m);
		return ENOBUFS;
	}
#endif /*IPSEC*/

	return (ip_output(m, inp->inp_options, &inp->inp_route, flags,
			  inp->inp_moptions));
}

/*
 * Raw IP socket option processing.
 */
int
rip_ctloutput(so, sopt)
	struct socket *so;
	struct sockopt *sopt;
{
	struct	inpcb *inp = sotoinpcb(so);
	int	error, optval;

	if (sopt->sopt_level != IPPROTO_IP)
		return (EINVAL);

	error = 0;

	switch (sopt->sopt_dir) {
	case SOPT_GET:
		switch (sopt->sopt_name) {
		case IP_HDRINCL:
			optval = inp->inp_flags & INP_HDRINCL;
			error = sooptcopyout(sopt, &optval, sizeof optval);
			break;

		case IP_FW_ADD:
		case IP_FW_GET:
			if (ip_fw_ctl_ptr == 0)
				error = ENOPROTOOPT;
			else
				error = ip_fw_ctl_ptr(sopt);
			break;

#ifdef DUMMYNET
		case IP_DUMMYNET_GET:
			if (ip_dn_ctl_ptr == NULL)
				error = ENOPROTOOPT ;
			else
				error = ip_dn_ctl_ptr(sopt);
			break ;
#endif /* DUMMYNET */

		case MRT_INIT:
		case MRT_DONE:
		case MRT_ADD_VIF:
		case MRT_DEL_VIF:
		case MRT_ADD_MFC:
		case MRT_DEL_MFC:
		case MRT_VERSION:
		case MRT_ASSERT:
			error = ip_mrouter_get(so, sopt);
			break;

		default:
			error = ip_ctloutput(so, sopt);
			break;
		}
		break;

	case SOPT_SET:
		switch (sopt->sopt_name) {
		case IP_HDRINCL:
			error = sooptcopyin(sopt, &optval, sizeof optval,
					    sizeof optval);
			if (error)
				break;
			if (optval)
				inp->inp_flags |= INP_HDRINCL;
			else
				inp->inp_flags &= ~INP_HDRINCL;
			break;

		case IP_FW_ADD:
		case IP_FW_DEL:
		case IP_FW_FLUSH:
		case IP_FW_ZERO:
		case IP_FW_RESETLOG:
			if (ip_fw_ctl_ptr == 0)
				error = ENOPROTOOPT;
			else
				error = ip_fw_ctl_ptr(sopt);
			break;

#ifdef DUMMYNET
		case IP_DUMMYNET_CONFIGURE:
		case IP_DUMMYNET_DEL:
		case IP_DUMMYNET_FLUSH:
			if (ip_dn_ctl_ptr == NULL)
				error = ENOPROTOOPT ;
			else
				error = ip_dn_ctl_ptr(sopt);
			break ;
#endif

		case IP_RSVP_ON:
			error = ip_rsvp_init(so);
			break;

		case IP_RSVP_OFF:
			error = ip_rsvp_done();
			break;

			/* XXX - should be combined */
		case IP_RSVP_VIF_ON:
			error = ip_rsvp_vif_init(so, sopt);
			break;
			
		case IP_RSVP_VIF_OFF:
			error = ip_rsvp_vif_done(so, sopt);
			break;

		case MRT_INIT:
		case MRT_DONE:
		case MRT_ADD_VIF:
		case MRT_DEL_VIF:
		case MRT_ADD_MFC:
		case MRT_DEL_MFC:
		case MRT_VERSION:
		case MRT_ASSERT:
			error = ip_mrouter_set(so, sopt);
			break;

		default:
			error = ip_ctloutput(so, sopt);
			break;
		}
		break;
	}

	return (error);
}

/*
 * This function exists solely to receive the PRC_IFDOWN messages which
 * are sent by if_down().  It looks for an ifaddr whose ifa_addr is sa,
 * and calls in_ifadown() to remove all routes corresponding to that address.
 * It also receives the PRC_IFUP messages from if_up() and reinstalls the
 * interface routes.
 */
void
rip_ctlinput(cmd, sa, vip)
	int cmd;
	struct sockaddr *sa;
	void *vip;
{
	struct in_ifaddr *ia;
	struct ifnet *ifp;
	int err;
	int flags;

	switch (cmd) {
	case PRC_IFDOWN:
		for (ia = in_ifaddrhead.tqh_first; ia;
		     ia = ia->ia_link.tqe_next) {
			if (ia->ia_ifa.ifa_addr == sa
			    && (ia->ia_flags & IFA_ROUTE)) {
				/*
				 * in_ifscrub kills the interface route.
				 */
				in_ifscrub(ia->ia_ifp, ia);
				/*
				 * in_ifadown gets rid of all the rest of
				 * the routes.  This is not quite the right
				 * thing to do, but at least if we are running
				 * a routing process they will come back.
				 */
				in_ifadown(&ia->ia_ifa, 0);
				break;
			}
		}
		break;

	case PRC_IFUP:
		for (ia = in_ifaddrhead.tqh_first; ia;
		     ia = ia->ia_link.tqe_next) {
			if (ia->ia_ifa.ifa_addr == sa)
				break;
		}
		if (ia == 0 || (ia->ia_flags & IFA_ROUTE))
			return;
		flags = RTF_UP;
		ifp = ia->ia_ifa.ifa_ifp;

		if ((ifp->if_flags & IFF_LOOPBACK)
		    || (ifp->if_flags & IFF_POINTOPOINT))
			flags |= RTF_HOST;

		err = rtinit(&ia->ia_ifa, RTM_ADD, flags);
		if (err == 0)
			ia->ia_flags |= IFA_ROUTE;
		break;
	}
}

u_long	rip_sendspace = RIPSNDQ;
u_long	rip_recvspace = RIPRCVQ;

SYSCTL_INT(_net_inet_raw, OID_AUTO, maxdgram, CTLFLAG_RW,
    &rip_sendspace, 0, "Maximum outgoing raw IP datagram size");
SYSCTL_INT(_net_inet_raw, OID_AUTO, recvspace, CTLFLAG_RW,
    &rip_recvspace, 0, "Maximum incoming raw IP datagram size");

static int
rip_attach(struct socket *so, int proto, struct proc *p)
{
	struct inpcb *inp;
	int error, s;

	inp = sotoinpcb(so);
	if (inp)
		panic("rip_attach");

	error = soreserve(so, rip_sendspace, rip_recvspace);
	if (error)
		return error;
	s = splnet();
	error = in_pcballoc(so, &ripcbinfo, p);
	splx(s);
	if (error)
		return error;
	inp = (struct inpcb *)so->so_pcb;
	inp->inp_vflag |= INP_IPV4;
	inp->inp_ip_p = proto;
	inp->inp_ip_ttl = ip_defttl;
	return 0;
}

static int
rip_detach(struct socket *so)
{
	struct inpcb *inp;

	inp = sotoinpcb(so);
	if (inp == 0)
		panic("rip_detach");
	if (so == ip_mrouter)
		ip_mrouter_done();
	ip_rsvp_force_done(so);
	if (so == ip_rsvpd)
		ip_rsvp_done();
	in_pcbdetach(inp);
	return 0;
}

static int
rip_abort(struct socket *so)
{
	soisdisconnected(so);
	return rip_detach(so);
}

static int
rip_disconnect(struct socket *so)
{
	if ((so->so_state & SS_ISCONNECTED) == 0)
		return ENOTCONN;
	return rip_abort(so);
}

static int
rip_bind(struct socket *so, struct sockaddr *nam, struct proc *p)
{
	struct inpcb *inp = sotoinpcb(so);
	struct sockaddr_in *addr = (struct sockaddr_in *)nam;

	if (nam->sa_len != sizeof(*addr))
		return EINVAL;

	if (TAILQ_EMPTY(&ifnet) || ((addr->sin_family != AF_INET) &&
				    (addr->sin_family != AF_IMPLINK)) ||
	    (addr->sin_addr.s_addr &&
	     ifa_ifwithaddr((struct sockaddr *)addr) == 0))
		return EADDRNOTAVAIL;
	inp->inp_laddr = addr->sin_addr;
	return 0;
}

static int
rip_connect(struct socket *so, struct sockaddr *nam, struct proc *p)
{
	struct inpcb *inp = sotoinpcb(so);
	struct sockaddr_in *addr = (struct sockaddr_in *)nam;

	if (nam->sa_len != sizeof(*addr))
		return EINVAL;
	if (TAILQ_EMPTY(&ifnet))
		return EADDRNOTAVAIL;
	if ((addr->sin_family != AF_INET) &&
	    (addr->sin_family != AF_IMPLINK))
		return EAFNOSUPPORT;
	inp->inp_faddr = addr->sin_addr;
	soisconnected(so);
	return 0;
}

static int
rip_shutdown(struct socket *so)
{
	socantsendmore(so);
	return 0;
}

static int
rip_send(struct socket *so, int flags, struct mbuf *m, struct sockaddr *nam,
	 struct mbuf *control, struct proc *p)
{
	struct inpcb *inp = sotoinpcb(so);
	register u_long dst;

	if (so->so_state & SS_ISCONNECTED) {
		if (nam) {
			m_freem(m);
			return EISCONN;
		}
		dst = inp->inp_faddr.s_addr;
	} else {
		if (nam == NULL) {
			m_freem(m);
			return ENOTCONN;
		}
		dst = ((struct sockaddr_in *)nam)->sin_addr.s_addr;
	}
	return rip_output(m, so, dst);
}

#ifdef CYGPKG_NET_FREEBSD_SYSCTL
static int
rip_pcblist(SYSCTL_HANDLER_ARGS)
{
	int error, i, n, s;
	struct inpcb *inp, **inp_list;
	inp_gen_t gencnt;
	struct xinpgen xig;

	/*
	 * The process of preparing the TCB list is too time-consuming and
	 * resource-intensive to repeat twice on every request.
	 */
	if (req->oldptr == 0) {
		n = ripcbinfo.ipi_count;
		req->oldidx = 2 * (sizeof xig)
			+ (n + n/8) * sizeof(struct xinpcb);
		return 0;
	}

	if (req->newptr != 0)
		return EPERM;

	/*
	 * OK, now we're committed to doing something.
	 */
	s = splnet();
	gencnt = ripcbinfo.ipi_gencnt;
	n = ripcbinfo.ipi_count;
	splx(s);

	xig.xig_len = sizeof xig;
	xig.xig_count = n;
	xig.xig_gen = gencnt;
	xig.xig_sogen = so_gencnt;
	error = SYSCTL_OUT(req, &xig, sizeof xig);
	if (error)
		return error;

	inp_list = malloc(n * sizeof *inp_list, M_TEMP, M_WAITOK);
	if (inp_list == 0)
		return ENOMEM;
	
	s = splnet();
	for (inp = ripcbinfo.listhead->lh_first, i = 0; inp && i < n;
	     inp = inp->inp_list.le_next) {
		if (inp->inp_gencnt <= gencnt)
			inp_list[i++] = inp;
	}
	splx(s);
	n = i;

	error = 0;
	for (i = 0; i < n; i++) {
		inp = inp_list[i];
		if (inp->inp_gencnt <= gencnt) {
			struct xinpcb xi;
			xi.xi_len = sizeof xi;
			/* XXX should avoid extra copy */
			bcopy(inp, &xi.xi_inp, sizeof *inp);
			if (inp->inp_socket)
				sotoxsocket(inp->inp_socket, &xi.xi_socket);
			error = SYSCTL_OUT(req, &xi, sizeof xi);
		}
	}
	if (!error) {
		/*
		 * Give the user an updated idea of our state.
		 * If the generation differs from what we told
		 * her before, she knows that something happened
		 * while we were processing this request, and it
		 * might be necessary to retry.
		 */
		s = splnet();
		xig.xig_gen = ripcbinfo.ipi_gencnt;
		xig.xig_sogen = so_gencnt;
		xig.xig_count = ripcbinfo.ipi_count;
		splx(s);
		error = SYSCTL_OUT(req, &xig, sizeof xig);
	}
	free(inp_list, M_TEMP);
	return error;
}
#endif

SYSCTL_PROC(_net_inet_raw, OID_AUTO/*XXX*/, pcblist, CTLFLAG_RD, 0, 0,
	    rip_pcblist, "S,xinpcb", "List of active raw IP sockets");

struct pr_usrreqs rip_usrreqs = {
	rip_abort, pru_accept_notsupp, rip_attach, rip_bind, rip_connect,
	pru_connect2_notsupp, in_control, rip_detach, rip_disconnect,
	pru_listen_notsupp, in_setpeeraddr, pru_rcvd_notsupp,
	pru_rcvoob_notsupp, rip_send, pru_sense_null, rip_shutdown,
	in_setsockaddr, sosend, soreceive, sopoll
};
