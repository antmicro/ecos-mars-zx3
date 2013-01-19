//==========================================================================
//
//      spi_at91.c
//
//      Atmel AT91 (ARM) SPI driver 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2009 Free Software Foundation, Inc.
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
// Author(s):     Savin Zlobec <savin@elatec.si> 
// Date:          2004-08-25
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>
#include <pkgconf/devs_spi_arm_at91.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>
#include <cyg/io/spi_at91.h>
#include <cyg/error/codes.h>

// -------------------------------------------------------------------------
static void spi_at91_init_bus(cyg_spi_at91_bus_t * bus);

static cyg_uint32 spi_at91_ISR(cyg_vector_t vector, cyg_addrword_t data);

static void spi_at91_DSR(cyg_vector_t   vector, 
                         cyg_ucount32   count, 
                         cyg_addrword_t data);

static void spi_at91_transaction_begin(cyg_spi_device *dev);

static void spi_at91_transaction_transfer(cyg_spi_device  *dev,
                                          cyg_bool         polled,
                                          cyg_uint32       count,
                                          const cyg_uint8 *tx_data,
                                          cyg_uint8       *rx_data,
                                          cyg_bool         drop_cs);

static void spi_at91_transaction_tick(cyg_spi_device *dev,
                                      cyg_bool        polled,
                                      cyg_uint32      count);

static void spi_at91_transaction_end(cyg_spi_device* dev);

static int spi_at91_get_config(cyg_spi_device *dev, 
                               cyg_uint32      key, 
                               void           *buf,
                               cyg_uint32     *len);

static int spi_at91_set_config(cyg_spi_device *dev, 
                               cyg_uint32      key, 
                               const void     *buf, 
                               cyg_uint32     *len);

// -------------------------------------------------------------------------
// AT91 SPI BUS

#ifdef CYGHWR_DEVS_SPI_ARM_AT91_BUS0
cyg_spi_at91_bus_t cyg_spi_at91_bus0 = {
    .spi_bus.spi_transaction_begin    = spi_at91_transaction_begin,
    .spi_bus.spi_transaction_transfer = spi_at91_transaction_transfer,
    .spi_bus.spi_transaction_tick     = spi_at91_transaction_tick,
    .spi_bus.spi_transaction_end      = spi_at91_transaction_end,
    .spi_bus.spi_get_config           = spi_at91_get_config,
    .spi_bus.spi_set_config           = spi_at91_set_config,
    .interrupt_number                 = CYGNUM_HAL_INTERRUPT_SPI,
    .base                             = AT91_SPI,
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS0_NONE
    .cs_en[0]                         = true,
    .cs_gpio[0]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS0,
#else
    .cs_en[0]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS1_NONE
    .cs_en[1]                         = true,
    .cs_gpio[1]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS1,
#else
    .cs_en[1]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS2_NONE
    .cs_en[2]                         = true,
    .cs_gpio[2]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS2,
#else
    .cs_en[2]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS3_NONE
    .cs_en[3]                         = true,
    .cs_gpio[3]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS0_NPCS3,
#else
    .cs_en[3]                         = false,
#endif
};

CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_at91_device_t, 0);
#endif
#ifdef CYGHWR_DEVS_SPI_ARM_AT91_BUS1
cyg_spi_at91_bus_t cyg_spi_at91_bus1 = {
    .spi_bus.spi_transaction_begin    = spi_at91_transaction_begin,
    .spi_bus.spi_transaction_transfer = spi_at91_transaction_transfer,
    .spi_bus.spi_transaction_tick     = spi_at91_transaction_tick,
    .spi_bus.spi_transaction_end      = spi_at91_transaction_end,
    .spi_bus.spi_get_config           = spi_at91_get_config,
    .spi_bus.spi_set_config           = spi_at91_set_config,
    .interrupt_number                 = CYGNUM_HAL_INTERRUPT_SPI1,
    .base                             = AT91_SPI1,
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS0_NONE
    .cs_en[0]                         = true,
    .cs_gpio[0]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS0,
#else
    .cs_en[0]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS1_NONE
    .cs_en[1]                         = true,
    .cs_gpio[1]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS1,
#else
    .cs_en[1]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS2_NONE
    .cs_en[2]                         = true,
    .cs_gpio[2]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS2,
#else
    .cs_en[2]                         = false,
#endif
#ifndef CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS3_NONE
    .cs_en[3]                         = true,
    .cs_gpio[3]                       = CYGDAT_DEVS_SPI_ARM_AT91_BUS1_NPCS3,
#else
    .cs_en[3]                         = false,
#endif
};

CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_at91_device_t, 1);
#endif
// -------------------------------------------------------------------------

// If C constructor with init priority functionality is not in compiler,
// rely on spi_at91_init.cxx to init us.
#ifndef CYGBLD_ATTRIB_C_INIT_PRI
# define CYGBLD_ATTRIB_C_INIT_PRI(x)
#endif

void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
cyg_spi_at91_bus_init(void)
{

#ifdef CYGHWR_DEVS_SPI_ARM_AT91_BUS0
   // NOTE: here we let the SPI controller control 
   //       the data in, out and clock signals, but 
   //       we need to handle the chip selects manually 
   //       in order to achieve better chip select control 
   //       in between transactions.

   // Put SPI MISO, MOSI and SPCK pins into peripheral mode
   HAL_ARM_AT91_PIO_CFG(AT91_SPI_SPCK);
   HAL_ARM_AT91_PIO_CFG(AT91_SPI_MISO);
   HAL_ARM_AT91_PIO_CFG(AT91_SPI_MOSI);
   spi_at91_init_bus(&cyg_spi_at91_bus0);
#endif
#ifdef CYGHWR_DEVS_SPI_ARM_AT91_BUS1
   // NOTE: here we let the SPI controller control 
   //       the data in, out and clock signals, but 
   //       we need to handle the chip selects manually 
   //       in order to achieve better chip select control 
   //       in between transactions.

   // Put SPI MISO, MOSI and SPCK pins into peripheral mode
   HAL_ARM_AT91_PIO_CFG(AT91_SPI1_SPCK);
   HAL_ARM_AT91_PIO_CFG(AT91_SPI1_MISO);
   HAL_ARM_AT91_PIO_CFG(AT91_SPI1_MOSI);
   spi_at91_init_bus(&cyg_spi_at91_bus1);
#endif
}

// -------------------------------------------------------------------------

static void spi_at91_init_bus(cyg_spi_at91_bus_t * spi_bus)
{
    cyg_uint32 ctr;
    // Create and attach SPI interrupt object
    cyg_drv_interrupt_create(spi_bus->interrupt_number,
                             4,                   
                             (cyg_addrword_t)spi_bus,   
                             spi_at91_ISR,
                             spi_at91_DSR,
                             &spi_bus->spi_interrupt_handle,
                             &spi_bus->spi_interrupt);

    cyg_drv_interrupt_attach(spi_bus->spi_interrupt_handle);

    // Init transfer mutex and condition
    cyg_drv_mutex_init(&spi_bus->transfer_mx);
    cyg_drv_cond_init(&spi_bus->transfer_cond, 
                      &spi_bus->transfer_mx);
   
    // Init flags
    spi_bus->transfer_end = true;
    spi_bus->cs_up        = false;
    
    // Soft reset the SPI controller
    HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_CR, AT91_SPI_CR_SWRST);

    // Configure SPI pins
      

    // Put SPI chip select pins in IO output mode
    for(ctr = 0;ctr<4;ctr++)
    {
       if(spi_bus->cs_en[ctr])
       {
          HAL_ARM_AT91_GPIO_CFG_DIRECTION(spi_bus->cs_gpio[ctr],AT91_PIN_OUT);
          HAL_ARM_AT91_GPIO_SET(spi_bus->cs_gpio[ctr]);
       }
    }
    // Call upper layer bus init
    CYG_SPI_BUS_COMMON_INIT(&spi_bus->spi_bus);
}

static cyg_uint32 
spi_at91_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 stat;
    cyg_spi_at91_bus_t * spi_bus = (cyg_spi_at91_bus_t *)data;
    // Read the status register and disable
    // the SPI int events that have occurred
    
    HAL_READ_UINT32(spi_bus->base+AT91_SPI_SR,   stat);
    HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_IDR, stat);
    
    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);

    return CYG_ISR_CALL_DSR;
}

static void 
spi_at91_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_spi_at91_bus_t *spi_bus = (cyg_spi_at91_bus_t *) data;
    cyg_uint32 stat;
    
    // Read the status register and 
    // check for transfer completion
    
    HAL_READ_UINT32(spi_bus->base+AT91_SPI_SR, stat);

    if((stat & AT91_SPI_SR_ENDRX) && (stat & AT91_SPI_SR_ENDTX))
    {
        // Transfer ended  
        spi_bus->transfer_end = true;
        cyg_drv_cond_signal(&spi_bus->transfer_cond);
    }
    else
    {
        // Transfer still in progress - unmask the SPI 
        // int so we can get more SPI int events
        cyg_drv_interrupt_unmask(vector);
    }
}

static cyg_bool 
spi_at91_calc_scbr(cyg_spi_at91_device_t *dev)
{
    cyg_uint32 scbr;
    cyg_bool   res = true;
    
    // Calculate SCBR from baud rate
    
    scbr = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / dev->cl_brate;
    if ((2*(CYGNUM_HAL_ARM_AT91_CLOCK_SPEED % dev->cl_brate)) >= dev->cl_brate)
        scbr++;

    if (scbr < 2)
    {
        dev->cl_scbr  = 2;
        dev->cl_div32 = 0;
        res = false;
    }
    else if (scbr > 255)
    {
        dev->cl_div32 = 1;
        
        scbr = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / (32*dev->cl_brate);

        if (scbr < 2) 
        {
            dev->cl_scbr = 2;
            res = false;
        }
        else if (scbr > 255) 
        {
            dev->cl_scbr = 255;
            res = false;
        }
        else
            dev->cl_scbr = (cyg_uint8)scbr;
    }
    else
    {
        dev->cl_scbr  = (cyg_uint8)scbr;
        dev->cl_div32 = 0;
    }
    
    return res;
}

static void
spi_at91_set_npcs(cyg_spi_at91_bus_t *spi_bus,int val)
{
   cyg_uint32 ctr;
   for(ctr=0;ctr<4;ctr++)
   {
      if(spi_bus->cs_en[ctr])
      {
        HAL_ARM_AT91_GPIO_PUT(spi_bus->cs_gpio[ctr], (val & (1<<ctr)));
      }
   }
}

static void
spi_at91_start_transfer(cyg_spi_at91_device_t *dev)
{
    cyg_spi_at91_bus_t *spi_bus = (cyg_spi_at91_bus_t *)dev->spi_device.spi_bus;
  
    if (spi_bus->cs_up)
        return;
 
    // Force minimal delay between two transfers - in case two transfers
    // follow each other w/o delay, then we have to wait here in order for
    // the peripheral device to detect cs transition from inactive to active. 
    CYGACC_CALL_IF_DELAY_US(dev->tr_bt_udly);
    
    // Raise CS

#ifdef CYGHWR_DEVS_SPI_ARM_AT91_PCSDEC
    spi_at91_set_npcs(spi_bus,~dev->dev_num);
#else
    spi_at91_set_npcs(spi_bus,~(1<<dev->dev_num));
#endif
    CYGACC_CALL_IF_DELAY_US(dev->cs_up_udly);
   
    spi_bus->cs_up = true;
}

static void
spi_at91_drop_cs(cyg_spi_at91_device_t *dev)
{
    cyg_spi_at91_bus_t *spi_bus = (cyg_spi_at91_bus_t *)dev->spi_device.spi_bus;
    
    if (!spi_bus->cs_up)
       return;
           
    // Drop CS

    CYGACC_CALL_IF_DELAY_US(dev->cs_dw_udly);
    spi_at91_set_npcs(spi_bus,0x0F); 
    spi_bus->cs_up = false;
}

static void
spi_at91_transfer(cyg_spi_at91_device_t *dev,
                  cyg_uint32             count, 
                  const cyg_uint8       *tx_data,
                  cyg_uint8             *rx_data)
{
    cyg_spi_at91_bus_t *spi_bus = (cyg_spi_at91_bus_t *)dev->spi_device.spi_bus;

    // Since PDC transfer buffer counters are 16 bit long, 
    // we have to split longer transfers into chunks. 
    while (count > 0)
    {
        cyg_uint16 tr_count = count > 0xFFFF ? 0xFFFF : count;

        // Set rx buf pointer and counter 
        if (NULL != rx_data)
        {
            HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_RPR, (cyg_uint32)rx_data);
            HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_RCR, (cyg_uint32)tr_count);
        }

        // Set tx buf pointer and counter  
        HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_TPR, (cyg_uint32)tx_data);
        HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_TCR, (cyg_uint32)tr_count);
   
#ifdef AT91_SPI_PTCR
        HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_PTCR, 
                         AT91_SPI_PTCR_RXTEN | AT91_SPI_PTCR_TXTEN);
#endif
        // Enable the SPI int events we are interested in
        HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_IER, 
                         AT91_SPI_SR_ENDRX | AT91_SPI_SR_ENDTX);

        cyg_drv_mutex_lock(&spi_bus->transfer_mx);
        {
            spi_bus->transfer_end = false;
    
            // Unmask the SPI int
            cyg_drv_interrupt_unmask(spi_bus->interrupt_number);
        
            // Wait for its completion
            cyg_drv_dsr_lock();
            {
                while (!spi_bus->transfer_end)
                    cyg_drv_cond_wait(&spi_bus->transfer_cond);
            }
            cyg_drv_dsr_unlock();
        }    
        cyg_drv_mutex_unlock(&spi_bus->transfer_mx);

        if (NULL == rx_data)
        {
            cyg_uint32 val;
        
            // If rx buffer was NULL, then the PDC receiver data transfer
            // was not started and we didn't wait for ENDRX, but only for 
            // ENDTX. Meaning that right now the last byte is being serialized 
            // over the line and when finished input data will appear in 
            // rx data reg. We have to wait for this to happen here, if we
            // don't we'll get the last received byte as the first one in the
            // next transfer!
        
            // FIXME: is there any better way to do this? 
            // If not, then precalculate this value.
            val = 8000000/dev->cl_brate;
            CYGACC_CALL_IF_DELAY_US(val > 1 ? val : 1);

            // Clear the rx data reg
            HAL_READ_UINT32(spi_bus->base+AT91_SPI_RDR, val);
        }

        // Adjust running variables
        
        if (NULL != rx_data)
            rx_data += tr_count;
        tx_data += tr_count;
        count   -= tr_count;
    }
}

static void
spi_at91_transfer_polled(cyg_spi_at91_device_t *dev, 
                         cyg_uint32             count,
                         const cyg_uint8       *tx_data,
                         cyg_uint8             *rx_data)
{
    cyg_uint32 val;
    cyg_spi_at91_bus_t *spi_bus = (cyg_spi_at91_bus_t *)dev->spi_device.spi_bus;

    // Transmit and receive byte by byte
    while (count-- > 0)
    {
        // Wait for transmit data register empty
        do
        {
            HAL_READ_UINT32(spi_bus->base+AT91_SPI_SR, val);
        } while ( !(val & AT91_SPI_SR_TDRE) );
       
        // Send next byte over the wire 
        val = *tx_data++;
        HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_TDR, val);
        
        // Wait for reveive data register full 
        do
        {
            HAL_READ_UINT32(spi_bus->base+AT91_SPI_SR, val);
        } while ( !(val & AT91_SPI_SR_RDRF) );
        
        // Store received byte 
        HAL_READ_UINT32(spi_bus->base+AT91_SPI_RDR, val);
        if (NULL != rx_data)
            *rx_data++ = val; 
    }
}

// -------------------------------------------------------------------------

static void 
spi_at91_transaction_begin(cyg_spi_device *dev)
{
    cyg_spi_at91_device_t *at91_spi_dev = (cyg_spi_at91_device_t *) dev;    
    cyg_spi_at91_bus_t *spi_bus = 
      (cyg_spi_at91_bus_t *)at91_spi_dev->spi_device.spi_bus;
    cyg_uint32 val;
    
    if (!at91_spi_dev->init)
    {
        at91_spi_dev->init = true;
        spi_at91_calc_scbr(at91_spi_dev);
    }
    
    // Configure SPI channel 0 - this is the only channel we 
    // use for all devices since we drive chip selects manually
    
    val = AT91_SPI_CSR_BITS8;

    if (1 == at91_spi_dev->cl_pol)
        val |= AT91_SPI_CSR_CPOL;

    if (1 == at91_spi_dev->cl_pha)
        val |= AT91_SPI_CSR_NCPHA;

    val |= AT91_SPI_CSR_SCBR(at91_spi_dev->cl_scbr);

    HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_CSR0, val); 

    // Enable SPI clock
    HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER, 1<<spi_bus->interrupt_number);

    // Enable the SPI controller
    HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_CR, AT91_SPI_CR_SPIEN);
    
    /* As we are using this driver only in master mode with NPCS0 
       configured as GPIO instead of a peripheral pin, it is necessary 
       for the Mode Failure detection to be switched off as this will
       cause havoc with the driver */ 

    // Put SPI bus into master mode
    if (1 == at91_spi_dev->cl_div32) {
      val = AT91_SPI_MR_MSTR | AT91_SPI_MR_DIV32;
#ifdef AT91_SPI_MR_MODFDIS
      val |= AT91_SPI_MR_MODFDIS;
#endif
      HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_MR, val);
    } else {
      val = AT91_SPI_MR_MSTR;
#ifdef AT91_SPI_MR_MODFDIS
      val |= AT91_SPI_MR_MODFDIS;
#endif
      HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_MR, val);
    }
}

static void 
spi_at91_transaction_transfer(cyg_spi_device  *dev, 
                              cyg_bool         polled,  
                              cyg_uint32       count, 
                              const cyg_uint8 *tx_data, 
                              cyg_uint8       *rx_data, 
                              cyg_bool         drop_cs) 
{
    cyg_spi_at91_device_t *at91_spi_dev = (cyg_spi_at91_device_t *) dev;

    // Select the device if not already selected
    spi_at91_start_transfer(at91_spi_dev);
 
    // Perform the transfer
    if (polled)
        spi_at91_transfer_polled(at91_spi_dev, count, tx_data, rx_data);
    else
        spi_at91_transfer(at91_spi_dev, count, tx_data, rx_data);

    // Deselect the device if requested
    if (drop_cs)
        spi_at91_drop_cs(at91_spi_dev);
}

static void 
spi_at91_transaction_tick(cyg_spi_device *dev, 
                          cyg_bool        polled,  
                          cyg_uint32      count)
{
    const cyg_uint32 zeros[10] = { 0,0,0,0,0,0,0,0,0,0 };

    cyg_spi_at91_device_t *at91_spi_dev = (cyg_spi_at91_device_t *) dev;
    
    // Transfer count zeros to the device - we don't touch the
    // chip select, the device could be selected or deselected.
    // It is up to the device driver to decide in wich state the
    // device will be ticked.
    
    while (count > 0)
    {
        int tcnt = count > 40 ? 40 : count;
        
        if (polled)
            spi_at91_transfer_polled(at91_spi_dev, tcnt, 
                                     (const cyg_uint8 *) zeros, NULL);
        else
            spi_at91_transfer(at91_spi_dev, tcnt, 
                              (const cyg_uint8 *) zeros, NULL);

        count -= tcnt;
    }
}

static void                    
spi_at91_transaction_end(cyg_spi_device* dev)
{
    cyg_spi_at91_device_t * at91_spi_dev = (cyg_spi_at91_device_t *)dev; 
    cyg_spi_at91_bus_t *spi_bus = 
      (cyg_spi_at91_bus_t *)at91_spi_dev->spi_device.spi_bus;

    // Disable the SPI controller
    HAL_WRITE_UINT32(spi_bus->base+AT91_SPI_CR, AT91_SPI_CR_SPIDIS);
   
    // Disable SPI clock
    HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCDR,1<<spi_bus->interrupt_number);
   
    spi_at91_drop_cs((cyg_spi_at91_device_t *) dev);
}

static int                     
spi_at91_get_config(cyg_spi_device *dev, 
                    cyg_uint32      key, 
                    void           *buf,
                    cyg_uint32     *len)
{
    cyg_spi_at91_device_t *at91_spi_dev = (cyg_spi_at91_device_t *) dev;
    
    switch (key) 
    {
        case CYG_IO_GET_CONFIG_SPI_CLOCKRATE:
        {
            if (*len != sizeof(cyg_uint32))
                return -EINVAL;
            else
            {
                cyg_uint32 *cl_brate = (cyg_uint32 *)buf;
                *cl_brate = at91_spi_dev->cl_brate; 
            }
        }
        break;
        default:
            return -EINVAL;
    }
    return ENOERR;
}

static int                     
spi_at91_set_config(cyg_spi_device *dev, 
                    cyg_uint32      key, 
                    const void     *buf, 
                    cyg_uint32     *len)
{
    cyg_spi_at91_device_t *at91_spi_dev = (cyg_spi_at91_device_t *) dev;
   
    switch (key) 
    {
        case CYG_IO_SET_CONFIG_SPI_CLOCKRATE:
        {
            if (*len != sizeof(cyg_uint32))
                return -EINVAL;
            else
            {
                cyg_uint32 cl_brate     = *((cyg_uint32 *)buf);
                cyg_uint32 old_cl_brate = at91_spi_dev->cl_brate;
           
                at91_spi_dev->cl_brate = cl_brate;
            
                if (!spi_at91_calc_scbr(at91_spi_dev))
                {
                    at91_spi_dev->cl_brate = old_cl_brate;
                    spi_at91_calc_scbr(at91_spi_dev);
                    return -EINVAL;
                }
            }
        }
        break;
        default:
            return -EINVAL;
    }
    return ENOERR;
}

// -------------------------------------------------------------------------
// EOF spi_at91.c
