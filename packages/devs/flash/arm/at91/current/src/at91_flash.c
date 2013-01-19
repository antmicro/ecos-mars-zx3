//==========================================================================
//
//      at91_flash.c
//
//      Flash programming for the at91 devices which have the 
//      Embedded Flash Controller.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006, 2009 Free Software Foundation, Inc.
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
// Contributors: gthomas, dmoseley, Andrew Lunn, Oliver Munz, Kasim Malla
// Date:         2009-06-03
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/devs_flash_at91.h>

#include <cyg/hal/hal_io.h>             
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_ass.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <string.h>

/* timeout depends on the CPU clock speed */
#define FLASH_TIMEOUT       100000
#define AT91_FLASH_FMCN_VALUE \
  (CYGNUM_HAL_ARM_AT91_CLOCK_SPEED * 15 / 10000000 + 1)

#ifdef CYGBLD_DEV_FLASH_AT91_LOCKING
static cyg_uint32 sector_size;
#endif

// Disable the flash controller from erasing the page before
// programming it

#ifdef AT91_MC_FMR1
    #define PAGE_AT_WHICH_WE_USE_THE_EFC1 1024
    #define SECTOR_AT_WHICH_WE_USE_THE_EFC1 16
#endif


static void 
flash_erase_before_write_disable (void)
{
  cyg_uint32 fmr;
  
  HAL_READ_UINT32(AT91_MC+AT91_MC_FMR, fmr);
  fmr = fmr | AT91_MC_FMR_NEBP;
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR, fmr);
#ifdef AT91_MC_FMR1  /* or in other words at91sam7x512 */
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR1, fmr);
#endif
}

// Enable the flash controller to erase the page before programming
// it
static void 
flash_erase_before_write_enable (void) 
{
  
  cyg_uint32 fmr;

  HAL_READ_UINT32(AT91_MC+AT91_MC_FMR, fmr);
  fmr = fmr & ~((cyg_uint32) AT91_MC_FMR_NEBP);
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR, fmr);
#ifdef AT91_MC_FMR1  /* or in other words at91sam7x512 */
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR1, fmr);
#endif
}

// Is the flash controller ready to accept the next command?
static __inline__ cyg_bool 
flash_controller_is_ready(cyg_uint32 page)
CYGBLD_ATTRIB_SECTION(".2ram.flash_run_command");

static __inline__ cyg_bool 
flash_controller_is_ready(cyg_uint32 page)
{
  cyg_uint32 fsr;
#ifdef AT91_MC_FMR1
  if (page >= PAGE_AT_WHICH_WE_USE_THE_EFC1){
    HAL_READ_UINT32(AT91_MC+AT91_MC_FSR1, fsr);
  } else 
#endif
  HAL_READ_UINT32(AT91_MC+AT91_MC_FSR, fsr);
  
  return (fsr & AT91_MC_FSR_FRDY ? true : false);
}

// Busy loop waiting for the controller to finish the command.
// Wait a maximum of timeout loops and then return an error.
static __inline__ int 
flash_wait_for_controller (cyg_uint32 page, cyg_uint32 timeout)
CYGBLD_ATTRIB_SECTION(".2ram.flash_run_command");

static __inline__ int 
flash_wait_for_controller (cyg_uint32 page, cyg_uint32 timeout)
{
  while (!flash_controller_is_ready(page)){
    timeout--;
    if (!timeout) {
      return FLASH_ERR_DRV_TIMEOUT;
    }
  }
  return FLASH_ERR_OK;
}

// Execute one command on the flash controller. This code should
// probably not be in flash

static int 
flash_run_command(cyg_uint32 address, 
                  cyg_uint32 command, 
                  cyg_uint32 timeout) 
CYGBLD_ATTRIB_SECTION(".2ram.flash_run_command");

static int 
flash_run_command(cyg_uint32 address, 
                  cyg_uint32 command, 
                  cyg_uint32 timeout) 
{
  cyg_uint32 retcode;
  cyg_uint32 fsr;
  cyg_uint32 mask;
  cyg_uint32 page;
  cyg_uint32 page_in_flash;
  cyg_uint32 fcr_addr = AT91_MC+AT91_MC_FCR;
  cyg_uint32 fsr_addr= AT91_MC+AT91_MC_FSR;
  
  page = ((cyg_uint32) address - (cyg_uint32) flash_info.start) / 
    flash_info.block_size;
  page_in_flash = page;
  
#ifdef AT91_MC_FMR1
  if (page >= PAGE_AT_WHICH_WE_USE_THE_EFC1){
    fcr_addr = AT91_MC+AT91_MC_FCR1;
    fsr_addr = AT91_MC+AT91_MC_FSR1;
    page_in_flash = page - PAGE_AT_WHICH_WE_USE_THE_EFC1;
  }
#endif
  
  // Wait for the last command to finish
  retcode = flash_wait_for_controller(page, timeout);
  if (retcode != FLASH_ERR_OK){
    return retcode;
  }
  
  HAL_DISABLE_INTERRUPTS(mask);
  
  HAL_WRITE_UINT32(fcr_addr,
                   command |
                   ((page_in_flash & AT91_MC_FCR_PAGE_MASK) 
                    << AT91_MC_FCR_PAGE_SHIFT) |
                   AT91_MC_FCR_KEY);

  retcode = flash_wait_for_controller(page, timeout);

  HAL_RESTORE_INTERRUPTS(mask);

  if (retcode != FLASH_ERR_OK){
    return retcode;
  }

  // Check for an error
  HAL_READ_UINT32(fsr_addr, fsr);

  if ((fsr & AT91_MC_FSR_LOCKE) == AT91_MC_FSR_LOCKE)
    return FLASH_ERR_PROTECT;
  if ((fsr & AT91_MC_FSR_PROGE) == AT91_MC_FSR_PROGE)
    return FLASH_ERR_PROGRAM;

  return FLASH_ERR_OK;
}

// The flash is embedded in the CPU package. So return the chip
// ID. This allows us to determine if the chip is one we support and
// the size of the flash
int flash_query(void *data) 
{
  cyg_uint32 chipID1r;
  
  HAL_READ_UINT32(AT91_DBG+AT91_DBG_C1R, chipID1r);
  
  memcpy(data, &chipID1r, sizeof(chipID1r));
  return FLASH_ERR_OK;
}

// Initialize the hardware. Make sure we have a flash device we know
// how to program and determine its size, the size of the blocks, and
// the number of blocks. The query function returns the chip ID 1
// register which tells us about the CPU we are running on, the flash
// size etc. Use this information to determine we have a valid setup.
int 
flash_hwr_init(void){

  cyg_uint32 chipID1r;
  cyg_uint32 flash_mode;
  cyg_uint32 lock_bits;
  
  flash_query (&chipID1r);

  if ((chipID1r & AT91_DBG_C1R_CPU_MASK) != AT91_DBG_C1R_ARM7TDMI)
    goto out;

  if (((chipID1r & AT91_DBG_C1R_ARCH_MASK) != AT91_DBG_C1R_ARCH_AT91SAM7Sxx) &&
      ((chipID1r & AT91_DBG_C1R_ARCH_MASK) != AT91_DBG_C1R_ARCH_AT91SAM7Xxx) &&
      ((chipID1r & AT91_DBG_C1R_ARCH_MASK) != AT91_DBG_C1R_ARCH_AT91SAM7XC) &&
      ((chipID1r & AT91_DBG_C1R_ARCH_MASK) != AT91_DBG_C1R_ARCH_AT91SAM7SExx))
    goto out;
  
  if ((chipID1r & AT91_DBG_C1R_FLASH_MASK) == AT91_DBG_C1R_FLASH_0K)
    goto out;
  

  if ((chipID1r & AT91_DBG_C1R_NVPTYP_MASK) != AT91_DBG_C1R_NVPTYP_ROMFLASH) {
  switch (chipID1r & AT91_DBG_C1R_FLASH_MASK) {
    case AT91_DBG_C1R_FLASH_32K:
      flash_info.block_size = 128;
      flash_info.blocks = 256;
      lock_bits = 8;
      break;
    case AT91_DBG_C1R_FLASH_64K:
      flash_info.block_size = 128;
      flash_info.blocks = 512;
      lock_bits = 16;
      break;
    case AT91_DBG_C1R_FLASH_128K:
      flash_info.block_size = 256;
      flash_info.blocks = 512;
      lock_bits = 8;
      break;
    case AT91_DBG_C1R_FLASH_256K:
      flash_info.block_size = 256;
      flash_info.blocks = 1024;
      lock_bits = 16;
      break;
		case AT91_DBG_C1R_FLASH_512K:
		  flash_info.block_size = 256;
#ifdef AT91_MC_FMR1
        flash_info.blocks = 2048;
        lock_bits = 32;
#else
		  flash_info.blocks = 1024;
		  lock_bits = 16;
        (*flash_info.pf)
          ("at91_flash: Only EFC0 is supported for writes and locks");
#endif
        break;
		default:
		  goto out;
	  }
  } else {
    // if there is both flash & ROM then:
    // ROM=AT91_DBG_C1R_FLASH, flash=AT91_DBG_C1R_FLASH2
	  switch (chipID1r & AT91_DBG_C1R_FLASH2_MASK) {
		case AT91_DBG_C1R_FLASH2_32K:
		  flash_info.block_size = 128;
		  flash_info.blocks = 256;
		  lock_bits = 8;
		  break;
		case AT91_DBG_C1R_FLASH2_64K:
		  flash_info.block_size = 128;
		  flash_info.blocks = 512;
		  lock_bits = 16;
		  break;
		case AT91_DBG_C1R_FLASH2_128K:
		  flash_info.block_size = 256;
		  flash_info.blocks = 512;
		  lock_bits = 8;
		  break;
		case AT91_DBG_C1R_FLASH2_256K:
		  flash_info.block_size = 256;
		  flash_info.blocks = 1024;
		  lock_bits = 16;
		  break;
		case AT91_DBG_C1R_FLASH2_512K:
		  flash_info.block_size = 256;
#ifdef AT91_MC_FMR1
        flash_info.blocks = 2048;
        lock_bits = 32;
#else
		  flash_info.blocks = 1024;
		  lock_bits = 16;
        (*flash_info.pf)
          ("at91_flash: Only EFC0 is supported for writes and locks");
#endif
        break;
    default:
      goto out;
  }
  }
  flash_info.buffer_size = 0;
  flash_info.start = (void *) 0x00100000;
  flash_info.end = (void *)(((cyg_uint32) flash_info.start) + 
                            flash_info.block_size * flash_info.blocks);
#ifdef CYGBLD_DEV_FLASH_AT91_LOCKING
  sector_size = flash_info.block_size * flash_info.blocks / lock_bits;
#endif
  // Set the FLASH clock to 1.5 microseconds based on the MCLK.  This
  // assumes the CPU is still running from the PLL clock as defined in
  // the HAL CDL and the HAL startup code. 
  HAL_READ_UINT32(AT91_MC+AT91_MC_FMR, flash_mode);
  flash_mode = flash_mode & ~AT91_MC_FMR_FMCN_MASK;
  flash_mode = flash_mode | (AT91_FLASH_FMCN_VALUE << AT91_MC_FMR_FMCN_SHIFT);
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR, flash_mode);
#ifdef AT91_MC_FMR1
  HAL_WRITE_UINT32(AT91_MC+AT91_MC_FMR1, flash_mode);
#endif
  
  return FLASH_ERR_OK;
  
 out:
  (*flash_info.pf)("Can't identify FLASH, sorry, ChipID1 %x\n",
                   chipID1r );
  return FLASH_ERR_HWR;
}

// Erase a block. The flash controller does not have a command to
// erase a block. So instead we setup the controller to do a program
// writing all 0xff with an erase operation first.
int 
flash_erase_block (volatile unsigned long block) 
{
  cyg_uint32 retcode;
  cyg_uint32 *buffer;
  cyg_uint32 *end;
  
  buffer = (cyg_uint32 *) block;
  end = (cyg_uint32 *) (block + flash_info.block_size);
  
  while (buffer < end){ 
    *buffer = (cyg_uint32) 0xffffffff;
    buffer++;
  }
  
  flash_erase_before_write_enable();
  retcode = flash_run_command(block, 
                              AT91_MC_FCR_START_PROG, 
                              FLASH_TIMEOUT);
  
  return retcode;
}

// Write into the flash. The datasheet says that performing 8 or 16bit
// accesses results in unpredictable corruption. So the current code
// checks that these conditions are upheld. It would be possible to
// perform extra reads and masking operation to support writing to
// none word assigned addresses or not multiple or a word length.
int 
flash_program_buf (volatile unsigned long addr, unsigned long *data, int len)
{
  cyg_uint32 retcode;
  volatile unsigned long *target;
  
  CYG_ASSERT(len % 4 == 0, "Only word writes allowed by current code");
  CYG_ASSERT(addr % 4 == 0, "Address must be word aligned for current code");
  
  target = (volatile unsigned long *)addr;
  
  while (len > 0) {
    *target = *data;
    data++;
    target++;
    len = len - sizeof(unsigned long);
  }
  
  flash_erase_before_write_disable();
  retcode = flash_run_command(addr, 
                              AT91_MC_FCR_START_PROG, 
                              FLASH_TIMEOUT);
  
  return retcode;
}

#ifdef CYGBLD_DEV_FLASH_AT91_LOCKING
// Unlock a block. This is not strictly possible, we can only lock and
// unlock sectors. This will unlock the sector which contains the
// block.
int
flash_unlock_block(volatile unsigned long block, int block_size, int blocks)
{
  cyg_uint32 sector;
  cyg_uint32 retcode;
  cyg_uint32 status;
  cyg_uint32 fsr_addr = AT91_MC + AT91_MC_FSR;
  
  sector = (((cyg_uint32) block) - (cyg_uint32) flash_info.start) / 
    sector_size;
 
#ifdef AT91_MC_FMR1  /* or in other words at91sam7x512 */
  if (sector >= SECTOR_AT_WHICH_WE_USE_THE_EFC1){
    fsr_addr = AT91_MC + AT91_MC_FSR1;
    sector -= SECTOR_AT_WHICH_WE_USE_THE_EFC1;
  }
#endif
  HAL_READ_UINT32(fsr_addr, status);
  
  if (status & (1 << (sector + 16))){
      retcode = flash_run_command(block, 
                                  AT91_MC_FCR_UNLOCK, 
                                  FLASH_TIMEOUT);
      return retcode;
  } else {
    return FLASH_ERR_OK;
  }
}

// Lock a block. This is not strictly possible, we can only lock and
// unlock sectors. This will lock the sector which contains the
// block.
int
flash_lock_block(volatile unsigned long block, int block_size, int blocks)
{
  cyg_uint32 sector;
  cyg_uint32 retcode;
  cyg_uint32 status;
  cyg_uint32 fsr_addr = AT91_MC + AT91_MC_FSR;
  
  sector = (((cyg_uint32) block) - (cyg_uint32) flash_info.start) / 
    sector_size;

#ifdef AT91_MC_FMR1  /* or in other words at91sam7x512 */
  if (sector >= SECTOR_AT_WHICH_WE_USE_THE_EFC1){
    fsr_addr = AT91_MC + AT91_MC_FSR1;
    sector -= SECTOR_AT_WHICH_WE_USE_THE_EFC1;
  }
#endif
  HAL_READ_UINT32(fsr_addr, status);
  
  if (!(status & (1 << (sector + 16)))){
      retcode = flash_run_command(block, 
                                  AT91_MC_FCR_LOCK, 
                                  FLASH_TIMEOUT);
      
      return retcode;
  } else {
    return FLASH_ERR_OK;
  }
}
#endif /*CYGBLD_DEV_FLASH_AT91_LOCKING */
  
// Map a hardware status to a package error. NOP since the errors are
// already mapped.
int flash_hwr_map_error(int err){

  return err;
}

// See if a range of FLASH addresses overlaps currently running code
bool flash_code_overlaps(void *start, void *end){

    extern char _stext[], _etext[];

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}
