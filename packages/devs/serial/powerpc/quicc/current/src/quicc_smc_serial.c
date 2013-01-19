//==========================================================================
//
//      io/serial/powerpc/quicc_smc_serial.c
//
//      PowerPC QUICC (SMC/SCC) Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-06-20
// Purpose:      QUICC SMC Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/quicc/ppc8xx.h>
#include CYGBLD_HAL_PLATFORM_H

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC

#include "quicc_smc_serial.h"

typedef struct quicc_sxx_serial_info {
    CYG_ADDRWORD          channel;                   // Which channel SMCx/SCCx
    short                 int_num;                   // Interrupt number
    short                 type;                      // Channel type - SCC or SMC
    unsigned long         *brg;                      // Which baud rate generator
    void                  *pram;                     // Parameter RAM pointer
    void                  *ctl;                      // SMC/SCC control registers
    volatile struct cp_bufdesc     *txbd, *rxbd;     // Next Tx,Rx descriptor to use
    struct cp_bufdesc     *tbase, *rbase;            // First Tx,Rx descriptor
    int                   txsize, rxsize;            // Length of individual buffers
    cyg_interrupt         serial_interrupt;
    cyg_handle_t          serial_interrupt_handle;
} quicc_sxx_serial_info;

static bool quicc_sxx_serial_init(struct cyg_devtab_entry *tab);
static bool quicc_sxx_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo quicc_sxx_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char quicc_sxx_serial_getc(serial_channel *chan);
static Cyg_ErrNo quicc_sxx_serial_set_config(serial_channel *chan,
                                             cyg_uint32 key, const void *xbuf,
                                             cyg_uint32 *len);
static void quicc_sxx_serial_start_xmit(serial_channel *chan);
static void quicc_sxx_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 quicc_sxx_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       quicc_sxx_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(quicc_sxx_serial_funs, 
                   quicc_sxx_serial_putc, 
                   quicc_sxx_serial_getc,
                   quicc_sxx_serial_set_config,
                   quicc_sxx_serial_start_xmit,
                   quicc_sxx_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC1
static quicc_sxx_serial_info quicc_sxx_serial_info_smc1 = {
    QUICC_CPM_SMC1,                 // Channel indicator
    CYGNUM_HAL_INTERRUPT_CPM_SMC1,  // interrupt
    _SMC_CHAN
};
#if CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_BUFSIZE > 0
static unsigned char quicc_smc_serial_out_buf_smc1[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_BUFSIZE];
static unsigned char quicc_smc_serial_in_buf_smc1[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(quicc_sxx_serial_channel_smc1,
                                       quicc_sxx_serial_funs, 
                                       quicc_sxx_serial_info_smc1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &quicc_smc_serial_out_buf_smc1[0], sizeof(quicc_smc_serial_out_buf_smc1),
                                       &quicc_smc_serial_in_buf_smc1[0], sizeof(quicc_smc_serial_in_buf_smc1)
    );
#else
static SERIAL_CHANNEL(quicc_sxx_serial_channel_smc1,
                      quicc_sxx_serial_funs, 
                      quicc_sxx_serial_info_smc1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

static unsigned char quicc_smc1_txbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_TxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_TxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));
static unsigned char quicc_smc1_rxbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_RxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_RxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));

DEVTAB_ENTRY(quicc_smc_serial_io_smc1, 
             CYGDAT_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             quicc_sxx_serial_init, 
             quicc_sxx_serial_lookup,     // Serial driver may need initializing
             &quicc_sxx_serial_channel_smc1
    );
#endif //  CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC1

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC2
static quicc_sxx_serial_info quicc_sxx_serial_info_smc2 = {
    QUICC_CPM_SMC2,                     // Channel indicator
    CYGNUM_HAL_INTERRUPT_CPM_SMC2_PIP,  // interrupt
    _SMC_CHAN
};
#if CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_BUFSIZE > 0
static unsigned char quicc_smc_serial_out_buf_smc2[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_BUFSIZE];
static unsigned char quicc_smc_serial_in_buf_smc2[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(quicc_sxx_serial_channel_smc2,
                                       quicc_sxx_serial_funs, 
                                       quicc_sxx_serial_info_smc2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &quicc_smc_serial_out_buf_smc2[0], sizeof(quicc_smc_serial_out_buf_smc2),
                                       &quicc_smc_serial_in_buf_smc2[0], sizeof(quicc_smc_serial_in_buf_smc2)
    );
#else
static SERIAL_CHANNEL(quicc_sxx_serial_channel_smc2,
                      quicc_sxx_serial_funs, 
                      quicc_sxx_serial_info_smc2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
static unsigned char quicc_smc2_txbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_TxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_TxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));
static unsigned char quicc_smc2_rxbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_RxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_RxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));

DEVTAB_ENTRY(quicc_smc_serial_io_smc2, 
             CYGDAT_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             quicc_sxx_serial_init, 
             quicc_sxx_serial_lookup,     // Serial driver may need initializing
             &quicc_sxx_serial_channel_smc2
    );
#endif //  CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC2

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC1
static quicc_sxx_serial_info quicc_sxx_serial_info_scc1 = {
    QUICC_CPM_SCC1,                     // Channel indicator
    CYGNUM_HAL_INTERRUPT_CPM_SCC1,      // interrupt
    _SCC_CHAN
};
#if CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_BUFSIZE > 0
static unsigned char quicc_smc_serial_out_buf_scc1[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_BUFSIZE];
static unsigned char quicc_smc_serial_in_buf_scc1[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(quicc_sxx_serial_channel_scc1,
                                       quicc_sxx_serial_funs, 
                                       quicc_sxx_serial_info_scc1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &quicc_smc_serial_out_buf_scc1[0], sizeof(quicc_smc_serial_out_buf_scc1),
                                       &quicc_smc_serial_in_buf_scc1[0], sizeof(quicc_smc_serial_in_buf_scc1)
    );
#else
static SERIAL_CHANNEL(quicc_sxx_serial_channel_scc1,
                      quicc_sxx_serial_funs, 
                      quicc_sxx_serial_info_scc1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
static unsigned char quicc_scc1_txbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_TxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_TxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));
static unsigned char quicc_scc1_rxbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_RxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_RxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));

DEVTAB_ENTRY(quicc_smc_serial_io_scc1, 
             CYGDAT_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             quicc_sxx_serial_init, 
             quicc_sxx_serial_lookup,     // Serial driver may need initializing
             &quicc_sxx_serial_channel_scc1
    );
#endif //  CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC1

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC2
static quicc_sxx_serial_info quicc_sxx_serial_info_scc2 = {
    QUICC_CPM_SCC2,                     // Channel indicator
    CYGNUM_HAL_INTERRUPT_CPM_SCC2,      // interrupt
    _SCC_CHAN
};
#if CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_BUFSIZE > 0
static unsigned char quicc_smc_serial_out_buf_scc2[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_BUFSIZE];
static unsigned char quicc_smc_serial_in_buf_scc2[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(quicc_sxx_serial_channel_scc2,
                                       quicc_sxx_serial_funs, 
                                       quicc_sxx_serial_info_scc2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &quicc_smc_serial_out_buf_scc2[0], sizeof(quicc_smc_serial_out_buf_scc2),
                                       &quicc_smc_serial_in_buf_scc2[0], sizeof(quicc_smc_serial_in_buf_scc2)
    );
#else
static SERIAL_CHANNEL(quicc_sxx_serial_channel_scc2,
                      quicc_sxx_serial_funs, 
                      quicc_sxx_serial_info_scc2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
static unsigned char quicc_scc2_txbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_TxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_TxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));
static unsigned char quicc_scc2_rxbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_RxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_RxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));

DEVTAB_ENTRY(quicc_smc_serial_io_scc2, 
             CYGDAT_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             quicc_sxx_serial_init, 
             quicc_sxx_serial_lookup,     // Serial driver may need initializing
             &quicc_sxx_serial_channel_scc2
    );
#endif //  CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC2

#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC3
static quicc_sxx_serial_info quicc_sxx_serial_info_scc3 = {
    QUICC_CPM_SCC3,                     // Channel indicator
    CYGNUM_HAL_INTERRUPT_CPM_SCC3,      // interrupt
    _SCC_CHAN
};
#if CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_BUFSIZE > 0
static unsigned char quicc_smc_serial_out_buf_scc3[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_BUFSIZE];
static unsigned char quicc_smc_serial_in_buf_scc3[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(quicc_sxx_serial_channel_scc3,
                                       quicc_sxx_serial_funs, 
                                       quicc_sxx_serial_info_scc3,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &quicc_smc_serial_out_buf_scc3[0], sizeof(quicc_smc_serial_out_buf_scc3),
                                       &quicc_smc_serial_in_buf_scc3[0], sizeof(quicc_smc_serial_in_buf_scc3)
    );
#else
static SERIAL_CHANNEL(quicc_sxx_serial_channel_scc3,
                      quicc_sxx_serial_funs, 
                      quicc_sxx_serial_info_scc3,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
static unsigned char quicc_scc3_txbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_TxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_TxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));
static unsigned char quicc_scc3_rxbuf[CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_RxNUM*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_RxSIZE] __attribute__((aligned(HAL_DCACHE_LINE_SIZE)));

DEVTAB_ENTRY(quicc_smc_serial_io_scc3, 
             CYGDAT_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             quicc_sxx_serial_init, 
             quicc_sxx_serial_lookup,     // Serial driver may need initializing
             &quicc_sxx_serial_channel_scc3
    );
#endif //  CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC3

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
quicc_smc_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    unsigned int baud_divisor = select_baud[new_config->baud];
    cyg_uint32 _lcr;
    EPPC *eppc = eppc_base();
    volatile struct smc_regs *ctl = (volatile struct smc_regs *)smc_chan->ctl;

    if (baud_divisor == 0) return false;
    // Stop transmitter while changing baud rate
    eppc->cp_cr = smc_chan->channel | QUICC_SMC_CMD_Go | QUICC_SMC_CMD_StopTx;
    while (eppc->cp_cr & QUICC_SMC_CMD_Go )
        continue;
    HAL_IO_BARRIER();  // Inforce I/O ordering
    // Disable channel during setup
    ctl->smc_smcmr = QUICC_SMCMR_UART;  // Disabled, UART mode
    HAL_IO_BARRIER();  // Inforce I/O ordering
    // Disable port interrupts while changing hardware
    _lcr = QUICC_SMCMR_CLEN(new_config->word_length + ((new_config->parity == CYGNUM_SERIAL_PARITY_NONE)? 0: 1) + ((new_config->stop == CYGNUM_SERIAL_STOP_2)? 2: 1)) |
        smc_select_stop_bits[new_config->stop] |
        smc_select_parity[new_config->parity];
    HAL_IO_BARRIER();  // Inforce I/O ordering
    // Set baud rate generator
    *smc_chan->brg = 0x10000 | (UART_BITRATE(baud_divisor)<<1);

    // Enable channel with new configuration
    ctl->smc_smcmr = QUICC_SMCMR_UART|QUICC_SMCMR_TEN|QUICC_SMCMR_REN|_lcr;
    HAL_IO_BARRIER();  // Inforce I/O ordering
    eppc->cp_cr = smc_chan->channel | QUICC_SMC_CMD_Go | QUICC_SMC_CMD_RestartTx;
    while (eppc->cp_cr & QUICC_SMC_CMD_Go )
        continue;
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to set up internal tables for device.
static void
quicc_smc_serial_init_info(quicc_sxx_serial_info *smc_chan,
                           volatile struct smc_uart_pram *uart_pram,
                           volatile struct smc_regs *ctl,
                           int TxBD, int TxNUM, int TxSIZE,
                           cyg_uint8 *TxBUF,
                           int RxBD, int RxNUM, int RxSIZE,
                           cyg_uint8 *RxBUF,
                           int portBmask,
                           int port)
{
    EPPC *eppc = eppc_base();
    struct cp_bufdesc *txbd, *rxbd;
    int i;
    
    smc_chan->pram = (void *)uart_pram;
    smc_chan->ctl = (void *)ctl;

    // Set up baud rate generator
    smc_chan->brg = _mpc8xx_allocate_brg(port);

    // Disable channel during setup
    ctl->smc_smcmr = QUICC_SMCMR_UART;  // Disabled, UART mode

    /*
     *  Set up the PortB pins for UART operation.
     *  Set PAR and DIR to allow SMCTXDx and SMRXDx
     *  (Table 16-39)
     */
    eppc->pip_pbpar |= portBmask;
    eppc->pip_pbdir &= ~portBmask;
    /*
     *  SDMA & LCD bus request level 5
     *  (Section 16.10.2.1)
     */
    eppc->dma_sdcr = 1;
    /*
     *  Set Rx and Tx function code
     *  (Section 16.15.4.2)
     */
    uart_pram->rfcr = 0x18;
    uart_pram->tfcr = 0x18;
    /*
     *  Set pointers to buffer descriptors.
     *  (Sections 16.15.4.1, 16.15.7.12, and 16.15.7.13)
     */
    uart_pram->rbase = RxBD;
    uart_pram->tbase = TxBD;
    /* tx and rx buffer descriptors */
    txbd = (struct cp_bufdesc *)((char *)eppc + TxBD);
    rxbd = (struct cp_bufdesc *)((char *)eppc + RxBD);
    smc_chan->txbd = txbd;
    smc_chan->tbase = txbd;
    smc_chan->txsize = TxSIZE;
    smc_chan->rxbd = rxbd;
    smc_chan->rbase = rxbd;
    smc_chan->rxsize = RxSIZE;
    /* max receive buffer length */
    uart_pram->mrblr = RxSIZE;
    /* set max_idle feature - generate interrupt after 4 chars idle period */
    uart_pram->max_idl = 4;
    /* no last brk char received */
    uart_pram->brkln = 0;
    /* no break condition occurred */
    uart_pram->brkec = 0;
    /* 1 break char sent on top XMIT */
    uart_pram->brkcr = 1;
    /* setup RX buffer descriptors */
    for (i = 0;  i < RxNUM;  i++) {
        rxbd->length = 0;
        rxbd->buffer = RxBUF;
        rxbd->ctrl   = QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
        if (i == (RxNUM-1)) rxbd->ctrl |= QUICC_BD_CTL_Wrap;  // Last buffer
        RxBUF += RxSIZE;
        rxbd++;
    }
    /* setup TX buffer descriptors */
    for (i = 0;  i < TxNUM;  i++) {
        txbd->length = 0;
        txbd->buffer = TxBUF;
        txbd->ctrl   = 0;
        if (i == (TxNUM-1)) txbd->ctrl |= QUICC_BD_CTL_Wrap;  // Last buffer
        TxBUF += TxSIZE;
        txbd++;
    }
    /*
     *  Reset Rx & Tx params
     */
    HAL_IO_BARRIER();  // Inforce I/O ordering
    eppc->cp_cr = smc_chan->channel | QUICC_SMC_CMD_Go | QUICC_SMC_CMD_InitTxRx;
    while (eppc->cp_cr & QUICC_SMC_CMD_Go )
        continue;
    HAL_IO_BARRIER();  // Inforce I/O ordering
    /*
     *  Clear any previous events. Enable interrupts.
     *  (Section 16.15.7.14 and 16.15.7.15)
     */
    ctl->smc_smce = 0xFF;
    ctl->smc_smcm = QUICC_SMCE_BSY|QUICC_SMCE_TX|QUICC_SMCE_RX;
}

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
quicc_scc_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    quicc_sxx_serial_info *scc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    unsigned int baud_divisor = select_baud[new_config->baud];
    EPPC *eppc = eppc_base();
    volatile struct scc_regs *regs = (volatile struct scc_regs *)scc_chan->ctl;

    if (baud_divisor == 0) return false;
    // Set baud rate generator
    *scc_chan->brg = 0x10000 | (UART_BITRATE(baud_divisor)<<1);
    // Disable channel during setup
    HAL_IO_BARRIER();  // Inforce I/O ordering
    regs->scc_gsmr_l = 0;
    regs->scc_psmr = QUICC_SCC_PSMR_ASYNC | 
        scc_select_word_length[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5] | 
        scc_select_stop_bits[new_config->stop] |
        scc_select_parity[new_config->parity];

    // Enable channel with new configuration
    regs->scc_gsmr_h = 0x20;          // 8bit FIFO
    regs->scc_gsmr_l = 0x00028004;    // 16x TxCLK, 16x RxCLK, UART

    /*
     *  Init Rx & Tx params for SCCX
     */
    HAL_IO_BARRIER();  // Inforce I/O ordering
    eppc->cp_cr = QUICC_CPM_CR_INIT_TXRX | scc_chan->channel | QUICC_CPM_CR_BUSY;
    while (eppc->cp_cr & QUICC_CPM_CR_BUSY )
        continue;
    HAL_IO_BARRIER();  // Inforce I/O ordering
    regs->scc_gsmr_l |= (QUICC_SCC_GSMR_L_Tx | QUICC_SCC_GSMR_L_Rx);  // Enable Rx, Tx
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to set up internal tables for device.
static void
quicc_scc_serial_init_info(quicc_sxx_serial_info *scc_chan,
                           volatile struct uart_pram *uart_pram,
                           volatile struct scc_regs *ctl,
                           int TxBD, int TxNUM, int TxSIZE,
                           cyg_uint8 *TxBUF,
                           int RxBD, int RxNUM, int RxSIZE,
                           cyg_uint8 *RxBUF,
                           int portAmask, int portBmask, int portCmask,
                           int port)
{
    EPPC *eppc = eppc_base();
    struct cp_bufdesc *txbd, *rxbd;
    int i;

    // Disable channel during setup
    ctl->scc_gsmr_l = 0;
    scc_chan->pram = (void *)uart_pram;
    scc_chan->ctl = (void *)ctl;

    // Set up baud rate generator
    scc_chan->brg = _mpc8xx_allocate_brg(port);

    /*
     *  Set up the PortA/B/C pins for UART operation.
     */
    eppc->pio_papar |= portAmask;
    eppc->pio_padir &= ~portAmask;
    eppc->pio_paodr &= ~portAmask;

    eppc->pio_pcdir &= portCmask;
    eppc->pio_pcpar &= portCmask;
    eppc->pio_pcso  |= portCmask;

    eppc->pip_pbpar |= portBmask;
    eppc->pip_pbdir |= portBmask;

    /*
     *  SDMA & LCD bus request level 5
     *  (Section 16.10.2.1)
     */
    eppc->dma_sdcr = 1;
    /*
     *  Set Rx and Tx function code
     *  (Section 16.15.4.2)
     */
    uart_pram->rfcr = 0x18;
    uart_pram->tfcr = 0x18;
    /*
     *  Set pointers to buffer descriptors.
     *  (Sections 16.15.4.1, 16.15.7.12, and 16.15.7.13)
     */
    uart_pram->rbase = RxBD;
    uart_pram->tbase = TxBD;
    /* tx and rx buffer descriptors */
    txbd = (struct cp_bufdesc *)((char *)eppc + TxBD);
    rxbd = (struct cp_bufdesc *)((char *)eppc + RxBD);
    scc_chan->txbd = txbd;
    scc_chan->tbase = txbd;
    scc_chan->txsize = TxSIZE;
    scc_chan->rxbd = rxbd;
    scc_chan->rbase = rxbd;
    scc_chan->rxsize = RxSIZE;
    /* max receive buffer length */
    uart_pram->mrblr = RxSIZE;
    /* set max_idle feature - generate interrupt after 4 chars idle period */
    uart_pram->max_idl = 4;
    /* no last brk char received */
    uart_pram->brkln = 0;
    /* no break condition occurred */
    uart_pram->brkec = 0;
    /* 1 break char sent on top XMIT */
    uart_pram->brkcr = 1;
    /* character mask */
    uart_pram->rccm  = 0xC0FF;
    /* control characters */
    for (i = 0;  i < 8;  i++) {
        uart_pram->cc[i] = 0x8000;  // Mark unused
    }
    /* setup RX buffer descriptors */
    for (i = 0;  i < RxNUM;  i++) {
        rxbd->length = 0;
        rxbd->buffer = RxBUF;
        rxbd->ctrl   = QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
        if (i == (RxNUM-1)) rxbd->ctrl |= QUICC_BD_CTL_Wrap;  // Last buffer
        RxBUF += RxSIZE;
        rxbd++;
    }
    /* setup TX buffer descriptors */
    for (i = 0;  i < TxNUM;  i++) {
        txbd->length = 0;
        txbd->buffer = TxBUF;
        txbd->ctrl   = 0;
        if (i == (TxNUM-1)) txbd->ctrl |= QUICC_BD_CTL_Wrap;  // Last buffer
        TxBUF += TxSIZE;
        txbd++;
    }
    /*
     *  Reset Rx & Tx params
     */
    HAL_IO_BARRIER();  // Inforce I/O ordering
    eppc->cp_cr = scc_chan->channel | QUICC_SMC_CMD_Go | QUICC_SMC_CMD_InitTxRx;
    while (eppc->cp_cr & QUICC_SMC_CMD_Go )
        continue;
    /*
     *  Clear any previous events. Enable interrupts.
     *  (Section 16.15.7.14 and 16.15.7.15)
     */
    HAL_IO_BARRIER();  // Inforce I/O ordering
    ctl->scc_scce = 0xFFFF;
    ctl->scc_sccm = (QUICC_SCCE_BSY | QUICC_SCCE_TX | QUICC_SCCE_RX);
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
quicc_sxx_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    volatile EPPC *eppc = (volatile EPPC *)eppc_base();
    int TxBD, RxBD;
    int cache_state;

    HAL_DCACHE_IS_ENABLED(cache_state);
    HAL_DCACHE_SYNC();
    HAL_DCACHE_DISABLE();
#ifdef CYGDBG_IO_INIT
    diag_printf("QUICC_SMC SERIAL init - dev: %x.%d = %s\n", smc_chan->channel, smc_chan->int_num, tab->name);
#endif
#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC1
    if (chan == &quicc_sxx_serial_channel_smc1) {
        TxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_TxNUM);
        RxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_RxNUM);
        quicc_smc_serial_init_info(&quicc_sxx_serial_info_smc1,
                                   &eppc->pram[2].scc.pothers.smc_modem.psmc.u, // PRAM
                                   &eppc->smc_regs[0], // Control registers
                                   TxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_TxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_TxSIZE,
                                   &quicc_smc1_txbuf[0],
                                   RxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_RxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC1_RxSIZE,
                                   &quicc_smc1_rxbuf[0],
                                   0xC0, // PortB mask
                                   QUICC_CPM_SMC1
            );
    }
#endif
#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SMC2
    if (chan == &quicc_sxx_serial_channel_smc2) {
        TxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_TxNUM);
        RxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_RxNUM);
        quicc_smc_serial_init_info(&quicc_sxx_serial_info_smc2,
                                   &eppc->pram[3].scc.pothers.smc_modem.psmc.u, // PRAM
                                   &eppc->smc_regs[1], // Control registers
                                   TxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_TxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_TxSIZE,
                                   &quicc_smc2_txbuf[0],
                                   RxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_RxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SMC2_RxSIZE,
                                   &quicc_smc2_rxbuf[0],
                                   0xC00, // PortB mask
                                   QUICC_CPM_SMC2
            );
    }
#endif
#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC1
    if (chan == &quicc_sxx_serial_channel_scc1) {
        TxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_TxNUM);
        RxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_RxNUM);
        quicc_scc_serial_init_info(&quicc_sxx_serial_info_scc1,
                                   &eppc->pram[0].scc.pscc.u, // PRAM
                                   &eppc->scc_regs[0],        // Control registersn
                                   TxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_TxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_TxSIZE,
                                   &quicc_scc1_txbuf[0],
                                   RxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_RxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC1_RxSIZE,
                                   &quicc_scc1_rxbuf[0],
                                   0x0003, // PortA mask
                                   0x1000, // PortB mask
                                   0x0800, // PortC mask
                                   QUICC_CPM_SCC1
            );
    }
#endif
#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC2
    if (chan == &quicc_sxx_serial_channel_scc2) {
        TxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_TxNUM);
        RxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_RxNUM);
        quicc_scc_serial_init_info(&quicc_sxx_serial_info_scc2,
                                   &eppc->pram[1].scc.pscc.u, // PRAM
                                   &eppc->scc_regs[1],        // Control registersn
                                   TxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_TxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_TxSIZE,
                                   &quicc_scc2_txbuf[0],
                                   RxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_RxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC2_RxSIZE,
                                   &quicc_scc2_rxbuf[0],
                                   0x000C, // PortA mask
                                   0x2000, // PortB mask
                                   0x0C00, // PortC mask
                                   QUICC_CPM_SCC2
            );
    }
#endif
#ifdef CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC_SCC3
    if (chan == &quicc_sxx_serial_channel_scc3) {
        TxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_TxNUM);
        RxBD = _mpc8xx_allocBd(sizeof(struct cp_bufdesc)*CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_RxNUM);
        quicc_scc_serial_init_info(&quicc_sxx_serial_info_scc3,
                                   &eppc->pram[2].scc.pscc.u, // PRAM
                                   &eppc->scc_regs[2],        // Control registersn
                                   TxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_TxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_TxSIZE,
                                   &quicc_scc3_txbuf[0],
                                   RxBD, 
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_RxNUM,
                                   CYGNUM_IO_SERIAL_POWERPC_QUICC_SMC_SCC3_RxSIZE,
                                   &quicc_scc3_rxbuf[0],
#if defined(CYGHWR_HAL_POWERPC_MPC8XX_850)
                                   0x0000, // PortA mask
                                   0x00C0, // PortB mask
                                   0x0000, // PortC mask
#elif defined(CYGHWR_HAL_POWERPC_MPC8XX_852T)
                                   0x0030, // PortA mask
                                   0x0000, // PortB mask
                                   0x0000, // PortC mask
#elif defined(CYGHWR_HAL_POWERPC_MPC8XX_823)
                                   0x0000, // PortA mask
                                   0x00C0, // PortB mask
                                   0x0000, // PortC mask
#else
#error "Cannot route SCC3"
#endif
                                   QUICC_CPM_SCC3
            );
    }
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(smc_chan->int_num,
                                 CYGARC_SIU_PRIORITY_HIGH, // Priority - unused (but asserted)
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 quicc_sxx_serial_ISR,
                                 quicc_sxx_serial_DSR,
                                 &smc_chan->serial_interrupt_handle,
                                 &smc_chan->serial_interrupt);
        cyg_drv_interrupt_attach(smc_chan->serial_interrupt_handle);
        cyg_drv_interrupt_unmask(smc_chan->int_num);
    }
    if (smc_chan->type == _SMC_CHAN) {
        quicc_smc_serial_config_port(chan, &chan->config, true);
    } else {
        quicc_scc_serial_config_port(chan, &chan->config, true);
    }
    if (cache_state)
        HAL_DCACHE_ENABLE();
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
quicc_sxx_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}

// Force the current transmit buffer to be sent
static void
quicc_sxx_serial_flush(quicc_sxx_serial_info *smc_chan)
{
    volatile struct cp_bufdesc *txbd = smc_chan->txbd;
    int cache_state;
                                       
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
      HAL_DCACHE_FLUSH(txbd->buffer, smc_chan->txsize);
    }
    if ((txbd->length > 0) && 
        ((txbd->ctrl & (QUICC_BD_CTL_Ready|QUICC_BD_CTL_Int)) == 0)) {
        txbd->ctrl |= QUICC_BD_CTL_Ready|QUICC_BD_CTL_Int;  // Signal buffer ready
        if (txbd->ctrl & QUICC_BD_CTL_Wrap) {
            txbd = smc_chan->tbase;
        } else {
            txbd++;
        }
        smc_chan->txbd = txbd;
    }
}

// Send a character to the device output buffer.
// Return 'true' if character is sent to device
static bool
quicc_sxx_serial_putc(serial_channel *chan, unsigned char c)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    volatile struct cp_bufdesc *txbd, *txfirst;
    volatile struct smc_uart_pram *pram = (volatile struct smc_uart_pram *)smc_chan->pram;
    EPPC *eppc = eppc_base();
    bool res;

    cyg_drv_dsr_lock();  // Avoid race condition testing pointers
    txbd = (struct cp_bufdesc *)((char *)eppc + pram->tbptr);
    txfirst = txbd;
    // Scan for a non-busy buffer
    while (txbd->ctrl & QUICC_BD_CTL_Ready) {
        // This buffer is busy, move to next one
        if (txbd->ctrl & QUICC_BD_CTL_Wrap) {
            txbd = smc_chan->tbase;
        } else {
            txbd++;
        }
        if (txbd == txfirst) break;  // Went all the way around
    }
    smc_chan->txbd = txbd;
    if ((txbd->ctrl & (QUICC_BD_CTL_Ready|QUICC_BD_CTL_Int)) == 0) {
        // Transmit buffer is not full/busy
        txbd->buffer[txbd->length++] = c;
        if (txbd->length == smc_chan->txsize) {
            // This buffer is now full, tell SMC to start processing it
            quicc_sxx_serial_flush(smc_chan);
        }
        res = true;
    } else {
        // No space
        res = false;
    }
    cyg_drv_dsr_unlock();
    return res;
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
quicc_sxx_serial_getc(serial_channel *chan)
{
    unsigned char c;
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    volatile struct cp_bufdesc *rxbd = smc_chan->rxbd;

    while ((rxbd->ctrl & QUICC_BD_CTL_Ready) != 0) ;
    c = rxbd->buffer[0];
    rxbd->length = smc_chan->rxsize;
    rxbd->ctrl |= QUICC_BD_CTL_Ready;
    if (rxbd->ctrl & QUICC_BD_CTL_Wrap) {
        rxbd = smc_chan->rbase;
    } else {
        rxbd++;
    }
    smc_chan->rxbd = (struct cp_bufdesc *)rxbd;
    return c;
}

// Set up the device characteristics; baud rate, etc.
static Cyg_ErrNo
quicc_sxx_serial_set_config(serial_channel *chan, cyg_uint32 key,
                            const void *xbuf, cyg_uint32 *len)
{
    int res;

    switch (key) {
    case CYG_IO_SET_CONFIG_SERIAL_INFO:
    {
        // FIXME - The documentation says that you can't change the baud rate
        // again until at least two BRG input clocks have occurred.
        cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
        quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
        if ( *len < sizeof(cyg_serial_info_t) ) {
            return -EINVAL;
        }
        *len = sizeof(cyg_serial_info_t);
        if (smc_chan->type == _SMC_CHAN) {
            res = quicc_smc_serial_config_port(chan, config, true);
        } else {
            res = quicc_scc_serial_config_port(chan, config, true);
        }
        if ( true != res )
            return -EINVAL;
    }
    break;
    default:
        return -EINVAL;
    }
    return ENOERR;
}

// Enable the transmitter (interrupt) on the device
static void
quicc_sxx_serial_start_xmit(serial_channel *chan)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    cyg_drv_dsr_lock();
    if (smc_chan->txbd->length == 0) {
        // See if there is anything to put in this buffer, just to get it going
        (chan->callbacks->xmt_char)(chan);
    }
    if (smc_chan->txbd->length != 0) {
        // Make sure it gets started
        quicc_sxx_serial_flush(smc_chan);
    }
    cyg_drv_dsr_unlock();
}

// Disable the transmitter on the device
static void 
quicc_sxx_serial_stop_xmit(serial_channel *chan)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    // If anything is in the last buffer, need to get it started
    if (smc_chan->txbd->length != 0) {
        quicc_sxx_serial_flush(smc_chan);
    }
}

// Serial I/O - low level interrupt handler (ISR)
static cyg_uint32 
quicc_sxx_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(smc_chan->int_num);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Cause DSR to be run
}

// Serial I/O - high level interrupt handler (DSR)
static void
quicc_smc_serial_DSR(serial_channel *chan)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    volatile struct smc_regs *ctl = (volatile struct smc_regs *)smc_chan->ctl;
    volatile struct cp_bufdesc *txbd;
    volatile struct cp_bufdesc *rxbd = smc_chan->rxbd;
    volatile struct smc_uart_pram *pram = (volatile struct smc_uart_pram *)smc_chan->pram;
    struct cp_bufdesc *rxlast;
    int i, cache_state;

    if (ctl->smc_smce & QUICC_SMCE_TX) {
        // Transmit interrupt
        ctl->smc_smce = QUICC_SMCE_TX;  // Reset interrupt state;
        txbd = smc_chan->tbase;  // First buffer
        while (true) {
            if ((txbd->ctrl & (QUICC_BD_CTL_Ready|QUICC_BD_CTL_Int)) == QUICC_BD_CTL_Int) {
                txbd->length = 0;
                txbd->ctrl &= ~QUICC_BD_CTL_Int;  // Reset interrupt bit
            }
            if (txbd->ctrl & QUICC_BD_CTL_Wrap) {
                txbd = smc_chan->tbase;
                break;
            } else {
                txbd++;
            }
        }
        (chan->callbacks->xmt_char)(chan);
    }
    while (ctl->smc_smce & QUICC_SMCE_RX) {
        // Receive interrupt
        ctl->smc_smce = QUICC_SMCE_RX;  // Reset interrupt state;
        rxlast = (struct cp_bufdesc *) ((char *)eppc_base() + pram->rbptr);
        while (rxbd != rxlast) {
            if ((rxbd->ctrl & QUICC_BD_CTL_Ready) == 0) {
                if((rxbd->ctrl & (QUICC_BD_CTL_Frame | QUICC_BD_CTL_Parity)) == 0) {
                    for (i = 0;  i < rxbd->length;  i++) {
                        (chan->callbacks->rcv_char)(chan, rxbd->buffer[i]);
                    }
                } else {
                    // is this necessary?
                    rxbd->ctrl &= QUICC_BD_CTL_MASK;
                    // should we report the error?
                }
                // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
                HAL_DCACHE_IS_ENABLED(cache_state);
                if (cache_state) {
                    HAL_DCACHE_INVALIDATE(rxbd->buffer, smc_chan->rxsize);  // Make sure no stale data
                }
                rxbd->length = 0;
                rxbd->ctrl |= QUICC_BD_CTL_Ready;
            }
            if (rxbd->ctrl & QUICC_BD_CTL_Wrap) {
                rxbd = smc_chan->rbase;
            } else {
                rxbd++;
            }
        }
        smc_chan->rxbd = (struct cp_bufdesc *)rxbd;
    }
    if (ctl->smc_smce & QUICC_SMCE_BSY) {
        ctl->smc_smce = QUICC_SMCE_BSY;  // Reset interrupt state;
    }
    cyg_drv_interrupt_acknowledge(smc_chan->int_num);
    cyg_drv_interrupt_unmask(smc_chan->int_num);
}

static void
quicc_scc_serial_DSR(serial_channel *chan)
{
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;
    volatile struct scc_regs *ctl = (volatile struct scc_regs *)smc_chan->ctl;
    volatile struct cp_bufdesc *txbd;
    volatile struct cp_bufdesc *rxbd = smc_chan->rxbd;
    volatile struct uart_pram *pram = (volatile struct uart_pram *)smc_chan->pram;
    struct cp_bufdesc *rxlast;
    int i, cache_state;

    if (ctl->scc_scce & QUICC_SCCE_TX) {
        // Transmit interrupt
        ctl->scc_scce = QUICC_SCCE_TX;  // Reset interrupt state;
        txbd = smc_chan->tbase;  // First buffer
        while (true) {
            if ((txbd->ctrl & (QUICC_BD_CTL_Ready|QUICC_BD_CTL_Int)) == QUICC_BD_CTL_Int) {
                txbd->length = 0;
                txbd->ctrl &= ~QUICC_BD_CTL_Int;  // Reset interrupt bit
            }
            if (txbd->ctrl & QUICC_BD_CTL_Wrap) {
                txbd = smc_chan->tbase;
                break;
            } else {
                txbd++;
            }
        }
        (chan->callbacks->xmt_char)(chan);
    }
    while (ctl->scc_scce & QUICC_SCCE_RX) {
        // Receive interrupt
        ctl->scc_scce = QUICC_SCCE_RX;  // Reset interrupt state;
        rxlast = (struct cp_bufdesc *) ((char *)eppc_base() + pram->rbptr);
        while (rxbd != rxlast) {
            if ((rxbd->ctrl & QUICC_BD_CTL_Ready) == 0) {
                if((rxbd->ctrl & (QUICC_BD_CTL_Frame | QUICC_BD_CTL_Parity)) == 0) {
                    for (i = 0;  i < rxbd->length;  i++) {
                        (chan->callbacks->rcv_char)(chan, rxbd->buffer[i]);
                    }
                } else {
                    // is this necessary?
                    rxbd->ctrl &= QUICC_BD_CTL_MASK;
                    // should we report the error?
                }
                // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
                HAL_DCACHE_IS_ENABLED(cache_state);
                if (cache_state) {
                    HAL_DCACHE_INVALIDATE(rxbd->buffer, smc_chan->rxsize);  // Make sure no stale data
                }
                rxbd->length = 0;
                rxbd->ctrl |= QUICC_BD_CTL_Ready;
            }
            if (rxbd->ctrl & QUICC_BD_CTL_Wrap) {
                rxbd = smc_chan->rbase;
            } else {
                rxbd++;
            }
        }
        smc_chan->rxbd = (struct cp_bufdesc *)rxbd;
    }
    if (ctl->scc_scce & QUICC_SCCE_BSY) {
        ctl->scc_scce = QUICC_SCCE_BSY;  // Reset interrupt state;
    }
    cyg_drv_interrupt_acknowledge(smc_chan->int_num);
    cyg_drv_interrupt_unmask(smc_chan->int_num);
}

static void       
quicc_sxx_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    quicc_sxx_serial_info *smc_chan = (quicc_sxx_serial_info *)chan->dev_priv;

    if (smc_chan->type == _SMC_CHAN) {
        quicc_smc_serial_DSR(chan);
    } else {
        quicc_scc_serial_DSR(chan);
    }
}

void
show_rxbd(int dump_all)
{
#ifdef CYGDBG_DIAG_BUF
    EPPC *eppc = eppc_base();
    struct smc_uart_pram *pram = &eppc->pram[2].scc.pothers.smc_modem.psmc.u;
    struct cp_bufdesc *rxbd = (struct cp_bufdesc *)((char *)eppc+pram->rbase);
    int _enable = enable_diag_uart;
    enable_diag_uart = 0;
#if 1
    diag_printf("SMC Mask: %x, Events: %x, Rbase: %x, Rbptr: %x\n", 
                eppc->smc_regs[0].smc_smcm, eppc->smc_regs[0].smc_smce,
                pram->rbase, pram->rbptr);
    while (true) {
        diag_printf("Rx BD: %x, ctl: %x, length: %d\n", rxbd, rxbd->ctrl, rxbd->length);
        if (rxbd->ctrl & QUICC_BD_CTL_Wrap) break;
        rxbd++;
    }
#endif
    enable_diag_uart = _enable;
    if (dump_all) dump_diag_buf();
#endif // CYGDBG_DIAG_BUF
}
#endif // CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC

// ------------------------------------------------------------------------
// EOF powerpc/quicc_smc_serial.c
