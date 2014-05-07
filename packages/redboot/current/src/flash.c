//==========================================================================
//
//      flash.c
//
//      RedBoot - FLASH memory support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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
// Contributors: gthomas, tkoeller
// Date:         2000-07-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <cyg/io/flash.h>
#include <fis.h>
#include <sib.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros

#ifdef CYGSEM_REDBOOT_FLASH_COMBINED_FIS_AND_CONFIG
// Note horrid intertwining of functions, to save precious FLASH
extern void conf_endian_fixup(void *p);
#endif

// Round a quantity up
#define _rup(n,s) ((((n)+(s-1))/s)*s)

#ifdef CYGOPT_REDBOOT_FIS
// Image management functions
local_cmd_entry("init",
                "Initialize FLASH Image System [FIS]",
                "[-f]",
                fis_init,
                FIS_cmds
    );
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
# define FIS_LIST_OPTS "[-c] [-d]"
#else
# define FIS_LIST_OPTS "[-d]"
#endif
local_cmd_entry("list",
                "Display contents of FLASH Image System [FIS]",
                FIS_LIST_OPTS,
                fis_list,
                FIS_cmds
    );
local_cmd_entry("free",
                "Display free [available] locations within FLASH Image System [FIS]",
                "",
                fis_free,
                FIS_cmds
    );
local_cmd_entry("delete",
                "Delete an image from FLASH Image System [FIS]",
                "name",
                fis_delete,
                FIS_cmds
    );

static char fis_load_usage[] = 
#ifdef CYGPRI_REDBOOT_ZLIB_FLASH
                      "[-d] "
#endif
                      "[-b <memory_load_address>] [-c] name";

local_cmd_entry("load",
                "Load image from FLASH Image System [FIS] into RAM",
                fis_load_usage,
                fis_load,
                FIS_cmds
    );
local_cmd_entry("create",
                "Create an image",
                "[-b <mem_base>] [-l <image_length>] [-s <data_length>]\n"
                "      [-f <flash_addr>] [-e <entry_point>] [-r <ram_addr>] [-n] <name>",
                fis_create,
                FIS_cmds
    );
#endif

// Raw flash access functions
local_cmd_entry("erase",
                "Erase FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_erase,
                FIS_cmds
    );
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
local_cmd_entry("lock",
                "LOCK FLASH contents",
                "[-f <flash_addr> -l <length>] [name]",
                fis_lock,
                FIS_cmds
    );
local_cmd_entry("unlock",
                "UNLOCK FLASH contents",
                "[-f <flash_addr> -l <length>] [name]",
                fis_unlock,
                FIS_cmds
    );
#endif
local_cmd_entry("write",
                "Write raw data directly to FLASH",
                "-f <flash_addr> -b <mem_base> -l <image_length>",
                fis_write,
                FIS_cmds
    );

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __FIS_cmds_TAB__, FIS_cmds);
CYG_HAL_TABLE_END( __FIS_cmds_TAB_END__, FIS_cmds);

extern struct cmd __FIS_cmds_TAB__[], __FIS_cmds_TAB_END__;

// CLI function
static cmd_fun do_fis;
RedBoot_nested_cmd("fis", 
            "Manage FLASH images", 
            "{cmds}",
            do_fis,
            __FIS_cmds_TAB__, &__FIS_cmds_TAB_END__
    );

// Local data used by these routines
cyg_flashaddr_t flash_start, flash_end;
size_t flash_block_size;
cyg_uint32 flash_num_blocks;
#ifdef CYGOPT_REDBOOT_FIS
void *fis_work_block;
cyg_flashaddr_t fis_addr;
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
cyg_flashaddr_t redundant_fis_addr;
#endif
int fisdir_size;  // Size of FIS directory.
#endif
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
extern cyg_flashaddr_t cfg_base;   // Location in Flash of config data
extern size_t cfg_size;            // Length of config data - rounded to Flash block size
extern struct _config *config;
#endif

static void
fis_usage(char *why)
{
    diag_printf("*** invalid 'fis' command: %s\n", why);
    cmd_usage(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, "fis ");
}

static void        
_show_invalid_flash_address(CYG_ADDRESS flash_addr, int stat)
{
    cyg_uint32 i=0;
    cyg_flash_info_t info;
    int ret;
    
    diag_printf("Invalid FLASH address %p: %s\n", (void *)flash_addr, 
                cyg_flash_errmsg(stat));
    do {
      ret = cyg_flash_get_info(i, &info);
      if (ret == CYG_FLASH_ERR_OK) {
          diag_printf("   valid range is %p - %p\n", (void*)info.start, (void*)info.end);
      }
      i++;
    } while (ret != CYG_FLASH_ERR_INVALID);
}

// Avoid overwriting the current executable. This is not a complete
// implementation, there may be code outside the text region, but it
// is generally good enough. If either the start of the text region or
// the end of the text region is within the specified range then at
// least some of the code is in the area of flash about to be erased
// or programmed.
static cyg_bool
check_code_overlaps(cyg_flashaddr_t start, cyg_flashaddr_t end)
{
  extern char _stext[], _etext[];
  
  return ((((unsigned long)&_stext >= (unsigned long)start) &&
           ((unsigned long)&_stext < (unsigned long)end))
          ||
          (((unsigned long)&_etext >= (unsigned long)start) &&
           ((unsigned long)&_etext < (unsigned long)end)));
}

#ifdef CYGOPT_REDBOOT_FIS

// fis_endian_fixup() is used to swap endianess if required.
//
static inline void fis_endian_fixup(void *addr)
{
#ifdef REDBOOT_FLASH_REVERSE_BYTEORDER
    struct fis_image_desc *p = addr;
    int cnt = fisdir_size / sizeof(struct fis_image_desc);

    while (cnt-- > 0) {
        p->flash_base = CYG_SWAP32(p->flash_base);
        p->mem_base = CYG_SWAP32(p->mem_base);
        p->size = CYG_SWAP32(p->size);
        p->entry_point = CYG_SWAP32(p->entry_point);
        p->data_length = CYG_SWAP32(p->data_length);
        p->desc_cksum = CYG_SWAP32(p->desc_cksum);
        p->file_cksum = CYG_SWAP32(p->file_cksum);
        p++;
    }
#endif
}

void
fis_read_directory(void)
{
    cyg_flashaddr_t err_addr;

    cyg_flash_read(fis_addr, fis_work_block, fisdir_size, &err_addr);
    fis_endian_fixup(fis_work_block);
}

struct fis_image_desc *
fis_lookup(char *name, int *num)
{
    int i;
    struct fis_image_desc *img;

    fis_read_directory();

    img = (struct fis_image_desc *)fis_work_block;
    for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++) {
        if ((img->u.name[0] != '\xFF') &&
            (strcasecmp(name, img->u.name) == 0)) {
            if (num) *num = i;
            return img;
        }
    }
    return (struct fis_image_desc *)0;
}

int fis_start_update_directory(int autolock)
{
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
   // Ensure [quietly] that the directory is unlocked before trying to update and locked again afterwards
   int do_autolock=1;
#else
   int do_autolock=autolock;
#endif
#endif

   struct fis_image_desc* img=NULL;
   cyg_flashaddr_t err_addr;
   cyg_flashaddr_t tmp_fis_addr;
   int stat;

   /*exchange old and new valid fis tables*/
   tmp_fis_addr=fis_addr;
   fis_addr=redundant_fis_addr;
   redundant_fis_addr=tmp_fis_addr;

   //adjust the contents of the new fis table
   img=(struct fis_image_desc*)fis_work_block;

   memcpy(img->u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC, CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH);
   img->u.valid_info.valid_flag[0]=CYG_REDBOOT_RFIS_IN_PROGRESS;
   img->u.valid_info.valid_flag[1]=CYG_REDBOOT_RFIS_IN_PROGRESS;
   img->u.valid_info.version_count=img->u.valid_info.version_count+1;

   //ready to go....
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
   if (do_autolock)
      cyg_flash_unlock(fis_addr, fisdir_size, &err_addr);
#endif

   if ((stat = cyg_flash_erase(fis_addr, fisdir_size, &err_addr)) != 0) {
       diag_printf("Error erasing FIS directory at %p: %s\n",
                   (void*)err_addr, cyg_flash_errmsg(stat));
       return 1;
   }
   //now magic is 0xffffffff
   fis_endian_fixup(fis_work_block);
   if ((stat = cyg_flash_program(fis_addr, fis_work_block, flash_block_size, &err_addr)) != 0) {
       diag_printf("Error writing FIS directory at %p: %s\n",
                   (void*)err_addr, cyg_flash_errmsg(stat));
       return 1;
   }
   fis_endian_fixup(fis_work_block);
   //now magic is 0xff1234ff, valid is IN_PROGRESS, version_count is the old one +1

#else
   /* nothing to do here without redundant fis */
#endif
   return 0;

}

int
fis_update_directory(int autolock, int error)
{
   cyg_flashaddr_t err_addr;
   int stat;

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
   // Ensure [quietly] that the directory is unlocked before trying to update and locked again afterwards
   int do_autolock=1;
#else
   int do_autolock=autolock;
#endif
#endif

#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS

   struct fis_image_desc* img=(struct fis_image_desc*)fis_work_block;

   // called from invalid state
   if (img->u.valid_info.valid_flag[0]!=CYG_REDBOOT_RFIS_IN_PROGRESS)
      return -1;

   //if it failed, reset is0Valid to the state before startUpdateDirectory()
   //g_data.fisTable hasn't been changed yet, so it doesn't have to be reset now
   //then reread the contents from flash
   //setting the valid flag of the failed table to "INVALID" might also be not too bad
   //but IN_PROGRESS is also good enough I think
   if (error!=0)
   {
      cyg_flashaddr_t swap_fis_addr=fis_addr;
      fis_addr=redundant_fis_addr;
      redundant_fis_addr=swap_fis_addr;
   }
   else //success
   {
      cyg_flashaddr_t tmp_fis_addr=((CYG_ADDRESS)fis_addr+CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH);

      img->u.valid_info.valid_flag[0]=CYG_REDBOOT_RFIS_VALID;
      img->u.valid_info.valid_flag[1]=CYG_REDBOOT_RFIS_VALID;

      if ((stat = cyg_flash_program(tmp_fis_addr, img->u.valid_info.valid_flag,
                                    sizeof(img->u.valid_info.valid_flag), &err_addr)) != 0) {
          diag_printf("Error writing FIS directory at %p: %s\n", 
                      (void*)err_addr, cyg_flash_errmsg(stat));
      }
   }
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
   if (do_autolock)
      flash_lock((void *)fis_addr, fisdir_size, (void **)&err_addr);
#endif

#else // CYGOPT_REDBOOT_REDUNDANT_FIS
    int blk_size = fisdir_size;

    fis_endian_fixup(fis_work_block);
#ifdef CYGSEM_REDBOOT_FLASH_COMBINED_FIS_AND_CONFIG
    memcpy((char *)fis_work_block+fisdir_size, config, cfg_size);
    conf_endian_fixup((char *)fis_work_block+fisdir_size);
    blk_size += cfg_size;
#endif
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
    if (do_autolock)
        cyg_flash_unlock(fis_addr, blk_size, &err_addr);
#endif

    if ((stat = cyg_flash_erase(fis_addr, blk_size, &err_addr)) != 0) {
        diag_printf("Error erasing FIS directory at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
    } else {
        if ((stat = cyg_flash_program(fis_addr, fis_work_block, blk_size,
                                      &err_addr)) != 0) {
            diag_printf("Error writing FIS directory at %p: %s\n", 
                        (void*)err_addr, cyg_flash_errmsg(stat));
        }
    }
    fis_endian_fixup(fis_work_block);
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
    if (do_autolock)
        cyg_flash_lock(fis_addr, blk_size, &err_addr);
#endif

#endif // CYGOPT_REDBOOT_REDUNDANT_FIS

    return 0;
}

#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS

int
fis_get_valid_buf(struct fis_image_desc* img0, struct fis_image_desc* img1, int* update_was_interrupted)
{
   *update_was_interrupted=0;
   if (strncmp(img1->u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC, CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH)!=0)  //buf0 must be valid
   {
      if (img0->u.valid_info.version_count>0)
      {
         *update_was_interrupted=1;
      }
      return 0;
   }
   else if (strncmp(img0->u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC, CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH)!=0)  //buf1 must be valid
   {
      if (img1->u.valid_info.version_count>0)
      {
         *update_was_interrupted=1;
      }
      return 1;
   }
   //magic is ok for both, now check the valid flag
   if ((img1->u.valid_info.valid_flag[0]!=CYG_REDBOOT_RFIS_VALID)
       || (img1->u.valid_info.valid_flag[1]!=CYG_REDBOOT_RFIS_VALID)) //buf0 must be valid
   {
      *update_was_interrupted=1;
      return 0;
   }
   else if ((img0->u.valid_info.valid_flag[0]!=CYG_REDBOOT_RFIS_VALID)
            || (img0->u.valid_info.valid_flag[1]!=CYG_REDBOOT_RFIS_VALID)) //buf1 must be valid
   {
      *update_was_interrupted=1;
      return 1;
   }

   //now check the version
   if (img1->u.valid_info.version_count == (img0->u.valid_info.version_count+1)) //buf1 must be valid
      return 1;

   return 0;
}

void
fis_erase_redundant_directory(void)
{
    int stat;
    cyg_flashaddr_t err_addr;

#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is unlocked before trying
    // to update
    cyg_flash_unlock(redundant_fis_addr, fisdir_size,
                     &err_addr);
#endif
    if ((stat = cyg_flash_erase(redundant_fis_addr, fisdir_size,
                                &err_addr)) != 0) {
         diag_printf("Error erasing FIS directory at %p: %s\n",
                     (void*)err_addr, cyg_flash_errmsg(stat));
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is locked after the update
    cyg_flash_lock(redundant_fis_addr, fisdir_size, &err_addr);
#endif
}
#endif

static void
fis_init(int argc, char *argv[])
{
    int stat;
    struct fis_image_desc *img;
    cyg_flashaddr_t err_addr;
    bool full_init = false;
    struct option_info opts[1];
    CYG_ADDRESS redboot_flash_start;
    unsigned long redboot_image_size;

    init_opts(&opts[0], 'f', false, OPTION_ARG_TYPE_FLG, 
              (void *)&full_init, (bool *)0, "full initialization, erases all of flash");
    if (!scan_opts(argc, argv, 2, opts, 1, 0, 0, ""))
    {
        return;
    }

    if (!verify_action("About to initialize [format] FLASH image system")) {
        diag_printf("** Aborted\n");
        return;
    }
    diag_printf("*** Initialize FLASH Image System\n");

#define MIN_REDBOOT_IMAGE_SIZE CYGBLD_REDBOOT_MIN_IMAGE_SIZE
    redboot_image_size = flash_block_size > MIN_REDBOOT_IMAGE_SIZE ? 
                         flash_block_size : MIN_REDBOOT_IMAGE_SIZE;

    img = (struct fis_image_desc *)fis_work_block;
    memset(img, '\xFF', fisdir_size);  // Start with erased data

#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
    //create the valid flag entry
    memset(img, 0, sizeof(struct fis_image_desc));
    strcpy(img->u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC);
    img->u.valid_info.valid_flag[0]=CYG_REDBOOT_RFIS_VALID;
    img->u.valid_info.valid_flag[1]=CYG_REDBOOT_RFIS_VALID;
    img->u.valid_info.version_count=0;
    img++;
#endif

    // Create a pseudo image for RedBoot
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "(reserved)");
    img->flash_base = (CYG_ADDRESS)flash_start;
    img->mem_base = (CYG_ADDRESS)flash_start;
    img->size = CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
    img->data_length = img->size;
    img++;
#endif
    redboot_flash_start = (CYG_ADDRESS)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "RedBoot");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img->data_length = img->size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
#ifdef CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET
    // Take care to place the POST entry at the right offset:
    redboot_flash_start = (CYG_ADDRESS)flash_start + CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET;
#endif
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "RedBoot[post]");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img->data_length = img->size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
    // And a backup image
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "RedBoot[backup]");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img->data_length = img->size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && defined(CYGHWR_REDBOOT_FLASH_CONFIG_MEDIA_FLASH)
    // And a descriptor for the configuration data
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "RedBoot config");
    img->flash_base = (CYG_ADDRESS)cfg_base;
    img->mem_base = (CYG_ADDRESS)cfg_base;
    img->size = cfg_size;
    img->data_length = img->size;
    img++;
#endif
    // And a descriptor for the descriptor table itself
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "FIS directory");
    img->flash_base = (CYG_ADDRESS)fis_addr;
    img->mem_base = (CYG_ADDRESS)fis_addr;
    img->size = fisdir_size;
    img->data_length = img->size;
    img++;

    //create the entry for the redundant fis table
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
    memset(img, 0, sizeof(*img));
    strcpy(img->u.name, "Redundant FIS");
    img->flash_base = (CYG_ADDRESS)redundant_fis_addr;
    img->mem_base = (CYG_ADDRESS)redundant_fis_addr;
    img->size = fisdir_size;
    img++;
#endif

#ifdef CYGOPT_REDBOOT_FIS_DIRECTORY_ARM_SIB_ID
    // FIS gets the size of a full block - note, this should be changed
    // if support is added for multi-block FIS structures.
    img = (struct fis_image_desc *)((CYG_ADDRESS)fis_work_block + fisdir_size);
    // Add a footer so the FIS will be recognized by the ARM Boot
    // Monitor as a reserved area.
    {
        tFooter* footer_p = (tFooter*)((CYG_ADDRESS)img - sizeof(tFooter));
        cyg_uint32 check = 0;
        cyg_uint32 *check_ptr = (cyg_uint32 *)footer_p;
        cyg_int32 count = (sizeof(tFooter) - 4) >> 2;

        // Prepare footer. Try to protect all but the reserved space
        // and the first RedBoot image (which is expected to be
        // bootable), but fall back to just protecting the FIS if it's
        // not at the default position in the flash.
#if defined(CYGOPT_REDBOOT_FIS_RESERVED_BASE) && (-1 == CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK)
        footer_p->blockBase = (char*)_ADDR_REDBOOT_TO_ARM(flash_start);
        footer_p->blockBase += CYGNUM_REDBOOT_FLASH_RESERVED_BASE + redboot_image_size;
#else
        footer_p->blockBase = (char*)_ADDR_REDBOOT_TO_ARM(fis_work_block);
#endif
        footer_p->infoBase = NULL;
        footer_p->signature = FLASH_FOOTER_SIGNATURE;
        footer_p->type = TYPE_REDHAT_REDBOOT;

        // and compute its checksum
        for ( ; count > 0; count--) {
            if (*check_ptr > ~check)
                check++;
            check += *check_ptr++;
        }
        footer_p->checksum = ~check;
    }
#endif

    // Do this after creating the initialized table because that inherently
    // calculates where the high water mark of default RedBoot images is.

    if (full_init) {
        unsigned long erase_size;
        CYG_ADDRESS erase_start;
        // Erase everything except default RedBoot images, fis block, 
        // and config block.
        // First deal with the possible first part, before RedBoot images:
#if (CYGBLD_REDBOOT_FLASH_BOOT_OFFSET > CYGNUM_REDBOOT_FLASH_RESERVED_BASE)
        erase_start = (CYG_ADDRESS)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
        erase_size =  (CYG_ADDRESS)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
        if ( erase_size > erase_start ) {
            erase_size -= erase_start;
            if ((stat = cyg_flash_erase((void *)erase_start, erase_size,
                                        &err_addr)) != 0) {
                diag_printf("   initialization failed at %p: %s\n",
                            err_addr, cyg_flash_errmsg(stat));
            }
        }
#endif
        // second deal with the larger part in the main:
        erase_start = redboot_flash_start; // high water of created images
        // Now the empty bits between the end of Redboot and the cfg and dir 
        // blocks. 
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && \
    defined(CYGHWR_REDBOOT_FLASH_CONFIG_MEDIA_FLASH) && \
    !defined(CYGSEM_REDBOOT_FLASH_COMBINED_FIS_AND_CONFIG)
        if (fis_addr > cfg_base) {
          erase_size = (CYG_ADDRESS)cfg_base - erase_start; // the gap between HWM and config data
        } else {
          erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between HWM and fis data
        }
        if ((stat = cyg_flash_erase(erase_start, erase_size,&err_addr)) != 0) {
          diag_printf("   initialization failed %p: %s\n",
                      (void*)err_addr, cyg_flash_errmsg(stat));
        }
        erase_start += (erase_size + flash_block_size);
        if (fis_addr > cfg_base) {
          erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between config and fis data
        } else {
          erase_size = (CYG_ADDRESS)cfg_base - erase_start; // the gap between fis and config data
        }
        if ((stat = cyg_flash_erase(erase_start, erase_size,&err_addr)) != 0) {
          diag_printf("   initialization failed %p: %s\n",
                      (void*)err_addr, cyg_flash_errmsg(stat));
        }
        erase_start += (erase_size + flash_block_size);
#else  // !CYGSEM_REDBOOT_FLASH_CONFIG        
        erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between HWM and fis data
        if ((stat = cyg_flash_erase(erase_start, erase_size,&err_addr)) != 0) {
          diag_printf("   initialization failed %p: %s\n",
                      (void*)err_addr, cyg_flash_errmsg(stat));
        }
        erase_start += (erase_size + flash_block_size);          
#endif
        // Lastly, anything at the end
        erase_size = ((CYG_ADDRESS)flash_end - erase_start) + 1;
        if ((erase_size > 0) &&
            ((stat = cyg_flash_erase(erase_start, erase_size,
                                     &err_addr))) != 0) {
            diag_printf("   initialization failed at %p: %s\n",
                        (void*)err_addr, cyg_flash_errmsg(stat));
        }
#ifndef CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS
    // In this case, 'fis free' works by scanning for erased blocks.  Since the
    // "-f" option was not supplied, there may be areas which are not used but
    // don't appear to be free since they are not erased - thus the warning
    } else {
        diag_printf("    Warning: device contents not erased, some blocks may not be usable\n");
#endif
    }
    fis_start_update_directory(0);
    fis_update_directory(0, 0);
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
    fis_erase_redundant_directory();
#endif
}

static void
fis_list(int argc, char *argv[])
{
    struct fis_image_desc *img;
    int i, image_indx;
    bool show_cksums = false;
    bool show_datalen = false;
    struct option_info opts[2];
    unsigned long last_addr, lowest_addr;
    bool image_found;

#ifdef CYGHWR_REDBOOT_ARM_FLASH_SIB
    // FIXME: this is somewhat half-baked
    extern void arm_fis_list(void);
    arm_fis_list();
    return;
#endif

    init_opts(&opts[0], 'd', false, OPTION_ARG_TYPE_FLG, 
              (void *)&show_datalen, (bool *)0, "display data length");
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
    init_opts(&opts[1], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void *)&show_cksums, (bool *)0, "display checksums");
    i = 2;
#else
    i = 1;
#endif
    if (!scan_opts(argc, argv, 2, opts, i, 0, 0, "")) {
        return;
    }
    fis_read_directory();

    // Let diag_printf do the formatting in both cases, rather than counting
    // cols by hand....
    diag_printf("%-16s  %-10s  %-10s  %-10s  %-s\n",
                "Name","FLASH addr",
                show_cksums ? "Checksum" : "Mem addr",
                show_datalen ? "Datalen" : "Length",
                "Entry point" );
    last_addr = 0;
    image_indx = 0;
    do {
        image_found = false;
        lowest_addr = 0xFFFFFFFF;
        img = (struct fis_image_desc *) fis_work_block;
        for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++) {
            if (img->u.name[0] != '\xFF') {
                if ((img->flash_base >= last_addr) && (img->flash_base < lowest_addr)) {
                    lowest_addr = img->flash_base;
                    image_found = true;
                    image_indx = i;
                }
            }
        }
        if (image_found) {
            img = (struct fis_image_desc *) fis_work_block;
            img += image_indx;
            diag_printf("%-16s  0x%08lX  0x%08lX  0x%08lX  0x%08lX\n", img->u.name,
                        (unsigned long)img->flash_base, 
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
                        show_cksums ? img->file_cksum : img->mem_base, 
                        show_datalen ? img->data_length : img->size, 
#else
                        img->mem_base, 
                        img->size, 
#endif
                        (unsigned long)img->entry_point);
        }
        last_addr = lowest_addr + 1;
    } while (image_found == true);
}

#ifdef CYGNUM_REDBOOT_FLASH_RESERVED_DEVICES
static CYG_ADDRESS flash_reserved_devices[] = { CYGNUM_REDBOOT_FLASH_RESERVED_DEVICES, 0xFFFFFFFF };

static cyg_bool flash_reserved( CYG_ADDRESS start )
{
    int i;
    for( i = 0; flash_reserved_devices[i] != 0xFFFFFFFF; i++ )
        if( start == flash_reserved_devices[i] )
            return true;
    return false;
}
#else
#define flash_reserved(__start) false
#endif

#ifdef CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS
struct free_chunk {
    CYG_ADDRESS start, end;
};

static int
find_free(struct free_chunk *chunks)
{
    cyg_flash_info_t info;
    struct fis_image_desc *img;
    int i=0, idx;
    int num_chunks = 0;
    int ret;
    
    do {
        // get info for each flash device
        ret = cyg_flash_get_info(i, &info);

        if (ret == CYG_FLASH_ERR_OK && !flash_reserved( info.start )) {
            
#ifdef CYGNUM_REDBOOT_FLASH_BASE
            if ( CYGNUM_REDBOOT_FLASH_BASE == info.start )
#else
            if (i == 0 )
#endif
            {
                // Do not search the area reserved for pre-RedBoot systems:
                chunks[num_chunks].start = (info.start + 
                                            CYGNUM_REDBOOT_FLASH_RESERVED_BASE);
                chunks[num_chunks].end = info.end;
                num_chunks++;
            } else {   // Contiguous flash? If so collapse the chunks together.
                if (chunks[num_chunks-1].end == (info.start -1)) {
                    chunks[num_chunks-1].end = info.end;
                } else {
                    chunks[num_chunks].start = info.start;
                    chunks[num_chunks].end = info.end;
                    num_chunks++;
                }
            }
        }
        i++;
    } while (ret != CYG_FLASH_ERR_INVALID);
    
    fis_read_directory();
    img = (struct fis_image_desc *) fis_work_block;
    for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++) {
        if (img->u.name[0] != '\xFF') {
            // Figure out which chunk this is in and split it
            for (idx = 0;  idx < num_chunks;  idx++) {
                if ((img->flash_base >= chunks[idx].start) && 
                    (img->flash_base <= chunks[idx].end)) {
                    if (img->flash_base == chunks[idx].start) {
                        chunks[idx].start += img->size;
                        if (chunks[idx].start >= chunks[idx].end) {
                            // This free chunk has collapsed
                            num_chunks--;
                            while (idx < num_chunks) {
                                chunks[idx] = chunks[idx+1];
                                idx++;
                            }
                        }
                    } else if ((img->flash_base+img->size-1) >= chunks[idx].end) {
                        chunks[idx].end = img->flash_base - 1;
                    } else {
                        // Split chunk into two parts
                        int idxtmp;
                        
                        // shift chunks along one so we insert the new one
                        for (idxtmp=num_chunks; idxtmp > (idx+1); idxtmp--)
                        {
                            chunks[idxtmp] = chunks[idxtmp-1];
                        }
                        
                        chunks[idx+1].start = img->flash_base + img->size;
                        chunks[idx+1].end = chunks[idx].end;
                        chunks[idx].end = img->flash_base - 1;
                        if (++num_chunks == CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS) {
                          diag_printf("Warning: too many free chunks\n");
                          return num_chunks;
                        }
                    }
                    break;
                }
            }
        }
    }
    return num_chunks;
}
#endif // CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS

static void
fis_free(int argc, char *argv[])
{
#if !defined(CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS)
    cyg_uint32 flash_data;
    cyg_flashaddr_t area_start;
    cyg_flashaddr_t err_addr;
    cyg_uint32 flash_dev_no;
    int flash_err;
    cyg_flash_info_t flash_info;
    cyg_uint32 curr_block, curr_block_info;
    cyg_flashaddr_t curr_flash_addr, next_flash_addr;

    // For each flash device
    for (flash_dev_no=0;; flash_dev_no++)
    {
        flash_err = cyg_flash_get_info( flash_dev_no, &flash_info );
        if ( CYG_FLASH_ERR_OK != flash_err )  // assume all done
            break;

        if( flash_reserved( flash_info.start ) ) // Ignore reserved devices
            continue;
        
        // Once more, from the top...
        curr_flash_addr = area_start = flash_info.start;

        // We must not search the area reserved for pre-RedBoot systems,
        // but this is only the case for the first flash device, or
        // the one corresponding to CYGNUM_REDBOOT_FLASH_BASE.
        // FIXME: this is insufficiently generic by design - can only
        // reserve on one flash.
#ifdef CYGNUM_REDBOOT_FLASH_BASE
        if ( CYGNUM_REDBOOT_FLASH_BASE == area_start )
#else
        if ( 0 == flash_dev_no )
#endif
        {
            //cyg_flashaddr_t asold = area_start;
            area_start += CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
            //diag_printf("area_start was %08x now %08x\n", asold, area_start );
        }
        // For each region of blocks
        for ( curr_block_info = 0;
              curr_block_info < flash_info.num_block_infos;
              curr_block_info++ )
        {
            // For each individual block
            for ( curr_block = 0;
                  curr_block < flash_info.block_info[curr_block_info].blocks;
                  curr_flash_addr = next_flash_addr, curr_block++ )
            {
                cyg_ucount32 i;
                cyg_bool is_blank = true; // until proved otherwise
                size_t amount_to_check;

                // determine this now to avoid recalculating it later in this block, so we know the
                // end of this block
                next_flash_addr = curr_flash_addr + flash_info.block_info[curr_block_info].block_size;

                // If area_start got adjusted further up, skip until we reach it
                if ( curr_flash_addr < area_start )
                    continue;
                
                //diag_printf("block region %d, block %d, flashaddr %08x\n",curr_block_info,curr_block,curr_flash_addr);
                
                // check 32 bytes at most. Reading it all will take too long on many devices.
                // Perhaps this should be a config option.
                amount_to_check = 32; 
                if ( amount_to_check > flash_info.block_info[curr_block_info].block_size ) // paranoia
                    amount_to_check = flash_info.block_info[curr_block_info].block_size;

                for ( i=0; i<amount_to_check; i += sizeof(cyg_uint32) )
                {
                    flash_err = cyg_flash_read(curr_flash_addr+i, &flash_data, sizeof(cyg_uint32), &err_addr);
                    if ( (CYG_FLASH_ERR_OK != flash_err) || (flash_data != 0xffffffff) )
                    {
                        is_blank = false;
                        break; // no point continuing
                    }
                } // for
                
                if (!is_blank)
                {
                    /* If not blank, output the preceding region if any */
                    if ( curr_flash_addr != area_start )
                    {
                        diag_printf("  0x%08lX .. 0x%08lX\n",
                                    area_start,
                                    next_flash_addr-1 );
                    }
                    area_start = next_flash_addr;
                }
            } // for block
        } // for block region

        /* If the blank region extended to the very end of the device, we need to do one
         * final check at the end of the device.
         */
        if ( curr_flash_addr != area_start )
        {
            diag_printf("  0x%08lX .. 0x%08lX\n",
                        area_start,
                        next_flash_addr-1 );
        }
    } // for flash device
#else
    struct free_chunk chunks[CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS];
    int idx, num_chunks;

    num_chunks = find_free(chunks);
    for (idx = 0;  idx < num_chunks;  idx++) {
        diag_printf("  0x%08lX .. 0x%08lX\n", 
		    (unsigned long)chunks[idx].start,
		    (unsigned long)chunks[idx].end);
    }
#endif
}

// Find the first unused area of flash which is long enough
static bool
fis_find_free(CYG_ADDRESS *addr, unsigned long length)
{
#ifndef CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS
    cyg_uint32 flash_data;
    cyg_flashaddr_t area_start;
    cyg_flashaddr_t err_addr;
    cyg_uint32 flash_dev_no;
    int flash_err;
    cyg_flash_info_t flash_info;
    cyg_uint32 curr_block, curr_block_info;
    cyg_flashaddr_t curr_flash_addr, next_flash_addr;

    // For each flash device
    for (flash_dev_no=0;; flash_dev_no++)
    {
        flash_err = cyg_flash_get_info( flash_dev_no, &flash_info );
        if ( CYG_FLASH_ERR_OK != flash_err )  // assume all done
            break;

        if( flash_reserved( flash_info.start ) ) // Ignore reserved devices
            continue;
        
        // Once more, from the top...
        curr_flash_addr = area_start = flash_info.start;

        // We must not search the area reserved for pre-RedBoot systems,
        // but this is only the case for the first flash device, or
        // the one corresponding to CYGNUM_REDBOOT_FLASH_BASE.
        // FIXME: this is insufficiently generic by design - can only
        // reserve on one flash.
#ifdef CYGNUM_REDBOOT_FLASH_BASE
        if ( CYGNUM_REDBOOT_FLASH_BASE == area_start )
#else
        if ( 0 == flash_dev_no )
#endif
        {
            //cyg_flashaddr_t asold = area_start;
            area_start += CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
            //diag_printf("area_start was %08x now %08x\n", asold, area_start );
        }
        // For each region of blocks
        for ( curr_block_info = 0;
              curr_block_info < flash_info.num_block_infos;
              curr_block_info++ )
        {
            // For each individual block
            for ( curr_block = 0;
                  curr_block < flash_info.block_info[curr_block_info].blocks;
                  curr_flash_addr = next_flash_addr, curr_block++ )
            {
                cyg_ucount32 i;
                cyg_bool is_blank = true; // until proved otherwise
                size_t amount_to_check;

                // determine this now to avoid recalculating it later in this block, so we know the
                // end of this block
                next_flash_addr = curr_flash_addr + flash_info.block_info[curr_block_info].block_size;

                // If area_start got adjusted further up, skip until we reach it
                if ( curr_flash_addr < area_start )
                    continue;
                
                //diag_printf("block region %d, block %d, flashaddr %08x\n",curr_block_info,curr_block,curr_flash_addr);
                
                // check 32 bytes at most. Reading it all will take too long on many devices.
                // Perhaps this should be a config option.
                amount_to_check = 32; 
                if ( amount_to_check > flash_info.block_info[curr_block_info].block_size ) // paranoia
                    amount_to_check = flash_info.block_info[curr_block_info].block_size;

                for ( i=0; i<amount_to_check; i += sizeof(cyg_uint32) )
                {
                    flash_err = cyg_flash_read(curr_flash_addr+i, &flash_data, sizeof(cyg_uint32), &err_addr);
                    if ( (CYG_FLASH_ERR_OK != flash_err) || (flash_data != 0xffffffff) )
                    {
                        is_blank = false;
                        break; // no point continuing
                    }
                } // for
                
                if (!is_blank)
                {
                    /* If not blank, output the preceding region if any */
                    if ( curr_flash_addr != area_start )
                    {
                        if ( length <= (next_flash_addr - area_start) )
                        {
                            *addr = (CYG_ADDRESS)area_start;
                            return true;
                        }    
                    }
                    area_start = next_flash_addr;
                }
            } // for block
        } // for block region

        /* If the blank region extended to the very end of the device, we need to do one
         * final check at the end of the device.
         */
        if ( curr_flash_addr != area_start )
        {
            if ( length <= (next_flash_addr - area_start) )
            {
                *addr = (CYG_ADDRESS)area_start;
                return true;
            }    
        }
    } // for flash device
#else
    struct free_chunk chunks[CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS];
    int idx, num_chunks;

    num_chunks = find_free(chunks);
    for (idx = 0;  idx < num_chunks;  idx++) {
        if ((chunks[idx].end - chunks[idx].start + 1) >= length) {
            *addr = (CYG_ADDRESS)chunks[idx].start;
            return true;
        }
    }
#endif
    return false;
}

static void
fis_create(int argc, char *argv[])
{
    int i, stat;
    unsigned long length, img_size;
    CYG_ADDRESS mem_addr, exec_addr, flash_addr, entry_addr, flash_offset;
    char *name;
    bool mem_addr_set = false;
    bool exec_addr_set = false;
    bool entry_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    bool img_size_set = false;
    bool no_copy = false;
    cyg_flashaddr_t err_addr;
    struct fis_image_desc *img = NULL;
    bool defaults_assumed;
    struct option_info opts[7];
    bool prog_ok = true;
    size_t block_size;
    
    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void *)&mem_addr, (bool *)&mem_addr_set, "memory base address");
    init_opts(&opts[1], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void *)&exec_addr, (bool *)&exec_addr_set, "ram base address");
    init_opts(&opts[2], 'e', true, OPTION_ARG_TYPE_NUM, 
              (void *)&entry_addr, (bool *)&entry_addr_set, "entry point address");
    init_opts(&opts[3], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void *)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[4], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&length, (bool *)&length_set, "image length [in FLASH]");
    init_opts(&opts[5], 's', true, OPTION_ARG_TYPE_NUM, 
              (void *)&img_size, (bool *)&img_size_set, "image size [actual data]");
    init_opts(&opts[6], 'n', false, OPTION_ARG_TYPE_FLG, 
              (void *)&no_copy, (bool *)0, "don't copy from RAM to FLASH, just update directory");
    if (!scan_opts(argc, argv, 2, opts, 7, (void *)&name, OPTION_ARG_TYPE_STR, "file name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    fis_read_directory();
    defaults_assumed = false;
    if (name) {
        // Search existing files to acquire defaults for params not specified:
        img = fis_lookup(name, NULL);
        if (img) {
            // Found it, so get image size from there
            if (!length_set) {
                length_set = true;
                length = img->size;
                defaults_assumed = true;
            }
        }
    }
    if ((!mem_addr_set || mem_addr == load_address) && !no_copy && (load_address >= (CYG_ADDRESS)ram_start) &&
	(load_address_end) < (CYG_ADDRESS)ram_end) {
	mem_addr = load_address;
	mem_addr_set = true;
        defaults_assumed = true;
        // Get entry address from loader, unless overridden
        if (!entry_addr_set) {
            entry_addr = entry_address;
            entry_addr_set = true;
        }
	if (!length_set) {
	    length = load_address_end - load_address;
	    length_set = true;
	} else if (defaults_assumed && !img_size_set) {
	    /* We got length from the FIS table, so the size of the
	       actual loaded image becomes img_size */
	    img_size = load_address_end - load_address;
	    img_size_set = true;
	}
    }
    // Get the remaining fall-back values from the fis
    if (img) {
        if (!exec_addr_set) {
            // Preserve "normal" behaviour
            exec_addr_set = true;
            exec_addr = flash_addr_set ? flash_addr : mem_addr;
        }
        if (!flash_addr_set) {
            flash_addr_set = true;
            flash_addr = img->flash_base;
            defaults_assumed = true;
        }
    }

    if ((!no_copy && !mem_addr_set) ||
        !length_set || !name) {
        fis_usage("required parameter missing");
        return;
    }
    if (!img_size_set) {
        img_size = length;
    }
    if (length < img_size) {
        diag_printf("Invalid FLASH image size/length combination\n");
        return;
    }

    if (strlen(name) >= sizeof(img->u.name)) {
        diag_printf("Name is too long, must be less than %d chars\n", (int)sizeof(img->u.name));
        return;
    }

    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }
    if (!no_copy) {
        if ((mem_addr < (CYG_ADDRESS)ram_start) ||
            ((mem_addr+img_size) >= (CYG_ADDRESS)ram_end)) {
            diag_printf("** WARNING: RAM address: %p may be invalid\n", (void *)mem_addr);
            diag_printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
        }
    }
    if (!flash_addr_set && !fis_find_free(&flash_addr, length)) {
        diag_printf("Can't locate %lx(%ld) bytes free in FLASH\n", length, length);
        return;
    }
    flash_addr_set = true;

    block_size = cyg_flash_block_size(flash_addr + length);
    length = ((length + block_size - 1) / block_size) * block_size;
    if (length < img_size) {
        diag_printf("Invalid FLASH image size/length combination\n");
        return;
    }
    if ((stat = cyg_flash_verify_addr(flash_addr)) ||
        (stat = cyg_flash_verify_addr((flash_addr+length-1)))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }
    block_size = cyg_flash_block_size(flash_addr);
    flash_offset = (flash_addr-flash_start)/block_size;
    if( flash_start + (flash_offset * block_size) != flash_addr ) {
        diag_printf("Invalid FLASH address: %p\n", (void *)flash_addr);
        diag_printf("   must be 0x%x aligned\n", (unsigned int)flash_block_size);
        return;
    }
    
    // First, see if the image by this name has agreable properties
    if (img) {
        if (img->flash_base != flash_addr) {
            diag_printf("Image found, but flash address (%p)\n"
                        "             is incorrect (present image location %p)\n",
                        (void*)flash_addr, (void*)img->flash_base);
            
            return;
        }
        if (img->size != length) {
            diag_printf("Image found, but length (0x%lx, necessitating image size 0x%lx)\n"
                        "             is incorrect (present image size 0x%lx)\n",
                        img_size, length, img->size);
            return;
        }
        if (!verify_action("An image named '%s' exists", name)) {
            return;
        } else {                
            if (defaults_assumed) {
                if (no_copy &&
                    !verify_action("* CAUTION * about to program '%s'\n            at %p..%p from %p", 
                                   name, (void *)flash_addr, (void *)(flash_addr+img_size-1),
                                   (void *)mem_addr)) {
                    return;  // The guy gave up
                }
            }
        }
    } else {
#ifdef CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS
        // Make sure that any FLASH address specified directly is truly free
        if (!no_copy) {
            struct free_chunk chunks[CYGDAT_REDBOOT_FIS_MAX_FREE_CHUNKS];
            int idx, num_chunks;
            bool is_free = false;

            num_chunks = find_free(chunks);
            for (idx = 0;  idx < num_chunks;  idx++) {
                //diag_printf("addr %08x, length %d chunk start %08x, end %08x\n",flash_addr, length, chunks[idx].start, chunks[idx].end);
                if ((flash_addr >= chunks[idx].start) && 
                    ((flash_addr+length-1) <= chunks[idx].end)) {
                    is_free = true;
                }
            }
            if (!is_free) {
                diag_printf("Invalid FLASH address - not free!\n");
                return;
            }
        }
#endif
        // If no image by that name, try and find an empty slot
        img = (struct fis_image_desc *)fis_work_block;
        for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++) {
            if (img->u.name[0] == '\xFF') {
                break;
            }
        }
	if (i >= fisdir_size/sizeof(*img)) {
	    diag_printf("Can't find an empty slot in FIS directory!\n");
	    return;
	}
    }
    if (!no_copy) {
        // Safety check - make sure the address range is not within the code we're running
        if (check_code_overlaps(flash_addr, (flash_addr+img_size-1))) {
            diag_printf("Can't program this region - contains code in use!\n");
            return;
        }
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING            
        if (prog_ok) {
            // Unlock area to be programmed
            if ((stat = cyg_flash_unlock((cyg_flashaddr_t)flash_addr, length, &err_addr)) != 0) {
                diag_printf("Can't unlock region at %p: %s\n", (void*)err_addr, flash_errmsg(stat));
                prog_ok = false;
            }
        }
#endif
        if (prog_ok) {
            // Erase area to be programmed
            if ((stat = cyg_flash_erase(flash_addr, length, &err_addr)) != 0) {
                diag_printf("Can't erase region at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
                prog_ok = false;
            }
        }
        if (prog_ok) {
            // Now program it
            if ((stat = cyg_flash_program(flash_addr, (void *)mem_addr, img_size, 
                                          &err_addr)) != 0) {
                diag_printf("Can't program region at %p: %s\n", (void*)err_addr, 
                            cyg_flash_errmsg(stat));
                prog_ok = false;
            }
        }
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING            
        if (prog_ok) {
            // Lock area programmed
            if ((stat = cyg_flash_lock((cyg_flashaddr_t)flash_addr, length, &err_addr)) != 0) {
                diag_printf("Can't lock region at %p: %s\n", (void*)err_addr, flash_errmsg(stat));
                prog_ok = false;
            }
        }
#endif
    }
    if (prog_ok) {
        // Update directory
        memset(img, 0, sizeof(*img));
        strcpy(img->u.name, name);
        img->flash_base = flash_addr;
        img->mem_base = exec_addr_set ? exec_addr : (mem_addr_set ? mem_addr : flash_addr);
        img->entry_point = entry_addr_set ? entry_addr : (CYG_ADDRESS)entry_address;  // Hope it's been set
        img->size = length;
        img->data_length = img_size;
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
        if (!no_copy) {
            img->file_cksum = cyg_crc32((unsigned char *)mem_addr, img_size);
        } else {
            // No way to compute this, sorry
            img->file_cksum = 0;
        }
#endif
        fis_start_update_directory(0);
        fis_update_directory(0, 0);
    }
}

extern void arm_fis_delete(char *);
static void
fis_delete(int argc, char *argv[])
{
    char *name;
    int num_reserved, i, stat;
    cyg_flashaddr_t err_addr;
    struct fis_image_desc *img;

    if (!scan_opts(argc, argv, 2, 0, 0, (void *)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
#ifdef CYGHWR_REDBOOT_ARM_FLASH_SIB
    // FIXME: this is somewhat half-baked
    arm_fis_delete(name);
    return;
#endif
    img = (struct fis_image_desc *)fis_work_block;
    num_reserved = 0;
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
    num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
    num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
    num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
    num_reserved++;
#endif
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && defined(CYGHWR_REDBOOT_FLASH_CONFIG_MEDIA_FLASH)
    num_reserved++;
#endif
#if 1 // And the descriptor for the descriptor table itself
    num_reserved++;
#endif

    img = fis_lookup(name, &i);
    if (img) {
        if (i < num_reserved) {
            diag_printf("Sorry, '%s' is a reserved image and cannot be deleted\n", img->u.name);
            return;
        }
        if (!verify_action("Delete image '%s'", name)) {
            return;
        }
    } else {
        diag_printf("No image '%s' found\n", name);
        return;
    }
    // Erase Data blocks (free space)
    if ((stat = cyg_flash_erase(img->flash_base, img->size, &err_addr)) != 0) {
        diag_printf("Error erasing at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
    } else {
        img->u.name[0] = '\xFF';    
        fis_start_update_directory(0);
        fis_update_directory(0, 0);
    }
}

static void
fis_load(int argc, char *argv[])
{
    char *name;
    struct fis_image_desc *img;
    CYG_ADDRESS mem_addr;
    bool mem_addr_set = false;
    bool show_cksum = false;
    struct option_info opts[3];
#if defined(CYGSEM_REDBOOT_FIS_CRC_CHECK)
    unsigned long cksum;
#endif
    int num_options;
#if defined(CYGPRI_REDBOOT_ZLIB_FLASH) ||  defined(CYGSEM_REDBOOT_FIS_CRC_CHECK)
    bool decompress = false;
#endif
    cyg_flashaddr_t err_addr;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void *)&mem_addr, (bool *)&mem_addr_set, "memory [load] base address");
    init_opts(&opts[1], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void *)&show_cksum, (bool *)0, "display checksum");
    num_options = 2;
#ifdef CYGPRI_REDBOOT_ZLIB_FLASH
    init_opts(&opts[num_options], 'd', false, OPTION_ARG_TYPE_FLG, 
              (void *)&decompress, 0, "decompress");
    num_options++;
#endif

    CYG_ASSERT(num_options <= NUM_ELEMS(opts), "Too many options");

    if (!scan_opts(argc, argv, 2, opts, num_options, (void *)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
    if ((img = fis_lookup(name, NULL)) == (struct fis_image_desc *)0) {
        diag_printf("No image '%s' found\n", name);
        return;
    }
    if (!mem_addr_set) {
        mem_addr = img->mem_base;
    }
    // Load image from FLASH into RAM
#ifdef CYGSEM_REDBOOT_VALIDATE_USER_RAM_LOADS
    if (!valid_address((void *)mem_addr)) {
        diag_printf("Not a loadable image - try using -b ADDRESS option\n");
        return;
    }
#endif
#ifdef CYGPRI_REDBOOT_ZLIB_FLASH
    if (decompress) {
        int err;
        _pipe_t fis_load_pipe;
        _pipe_t* p = &fis_load_pipe;
        p->out_buf = (unsigned char*) mem_addr;
        p->out_max = p->out_size = -1;
        p->in_buf = (unsigned char*) img->flash_base;
        p->in_avail = img->data_length;

        err = (*_dc_init)(p);

        if (0 == err)
            err = (*_dc_inflate)(p);

        // Free used resources, do final translation of
        // error value.
        err = (*_dc_close)(p, err);

        if (0 != err && p->msg) {
            diag_printf("decompression error: %s\n", p->msg);
        } else {
            diag_printf("Image loaded from %p-%p\n", (unsigned char *)mem_addr, p->out_buf);
        }

        // Set load address/top
        load_address = mem_addr;
        load_address_end = (unsigned long)p->out_buf;

        // Reload fis directory
        fis_read_directory();
    } else // dangling block
#endif
    {
        cyg_flash_read(img->flash_base, (void *)mem_addr, img->data_length, 
                       &err_addr);

        // Set load address/top
        load_address = mem_addr;
        load_address_end = mem_addr + img->data_length;
    }
    entry_address = (unsigned long)img->entry_point;

#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
    cksum = cyg_crc32((unsigned char *)mem_addr, img->data_length);
    if (show_cksum) {
        diag_printf("Checksum: 0x%08lx\n", cksum);
    }
    // When decompressing, leave CRC checking to decompressor
    if (!decompress && img->file_cksum) {
        if (cksum != img->file_cksum) {
            diag_printf("** Warning - checksum failure.  stored: 0x%08lx, computed: 0x%08lx\n",
                        img->file_cksum, cksum);
            entry_address = (unsigned long)NO_MEMORY;
        }
    }
#endif
}
#endif // CYGOPT_REDBOOT_FIS

static void
fis_write(int argc, char *argv[])
{
    int stat;
    unsigned long length;
    CYG_ADDRESS mem_addr, flash_addr, flash_offset;
    bool mem_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    cyg_flashaddr_t err_addr;
    struct option_info opts[3];
    bool prog_ok;
    size_t block_size;
    
    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void *)&mem_addr, (bool *)&mem_addr_set, "memory base address");
    init_opts(&opts[1], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void *)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[2], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&length, (bool *)&length_set, "image length [in FLASH]");
    if (!scan_opts(argc, argv, 2, opts, 3, 0, 0, 0))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!mem_addr_set || !flash_addr_set || !length_set) {
        fis_usage("required parameter missing");
        return;
    }

    // Round up length to FLASH block size
    block_size = cyg_flash_block_size(flash_addr + length);
    length = ((length + block_size - 1) / block_size) * block_size;
    if ((stat = cyg_flash_verify_addr(flash_addr)) ||
         (stat = cyg_flash_verify_addr((flash_addr+length-1)))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }

    block_size = cyg_flash_block_size(flash_addr);
    flash_offset = (flash_addr-flash_start)/block_size;
    if( flash_start + (flash_offset * block_size) != flash_addr ) {
        diag_printf("Invalid FLASH address: %p\n", (void *)flash_addr);
        diag_printf("   must be 0x%x aligned\n", (unsigned int)block_size);
        return;
    }
    if ((mem_addr < (CYG_ADDRESS)ram_start) ||
        ((mem_addr+length) >= (CYG_ADDRESS)ram_end)) {
        diag_printf("** WARNING: RAM address: %p may be invalid\n", (void *)mem_addr);
        diag_printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
    }
    // Safety check - make sure the address range is not within the code we're running
    if (check_code_overlaps(flash_addr, (flash_addr+length-1))) {
        diag_printf("Can't program this region - contains code in use!\n");
        return;
    }
    if (!verify_action("* CAUTION * about to program FLASH\n            at %p..%p from %p", 
                       (void *)flash_addr, (void *)(flash_addr+length-1),
                       (void *)mem_addr)) {
        return;  // The guy gave up
    }
    prog_ok = true;
    if (prog_ok) {
        // Erase area to be programmed
        if ((stat = cyg_flash_erase(flash_addr, length, &err_addr)) != 0) {
            diag_printf("Can't erase region at %p: %s\n", (void*)err_addr, 
                        cyg_flash_errmsg(stat));
            prog_ok = false;
        }
    }
    if (prog_ok) {
        // Now program it
        if ((stat = cyg_flash_program(flash_addr, (void *)mem_addr, length, 
                                      &err_addr)) != 0) {
            diag_printf("Can't program region at %p: %s\n", (void*)err_addr, 
                        cyg_flash_errmsg(stat));
            prog_ok = false;
        }
    }
}

static void
fis_erase(int argc, char *argv[])
{
    int stat;
    unsigned long length;
    CYG_ADDRESS flash_addr, flash_offset;
    bool flash_addr_set = false;
    bool length_set = false;
    cyg_flashaddr_t err_addr;
    struct option_info opts[2];
    size_t block_size;
    

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void *)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = cyg_flash_verify_addr(flash_addr)) ||
         (stat = cyg_flash_verify_addr((flash_addr+length-1))))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }
    block_size = cyg_flash_block_size(flash_addr);
    flash_offset = (flash_addr-flash_start)/block_size;
    if( flash_addr_set && (flash_start + (flash_offset * block_size) != flash_addr) ) {
        diag_printf("Invalid FLASH address: %p\n", (void *)flash_addr);
        diag_printf("   must be 0x%x aligned\n", (unsigned int)flash_block_size);
        return;
    }
    // Safety check - make sure the address range is not within the code we're running
    if (check_code_overlaps(flash_addr, (flash_addr+length-1))) {
        diag_printf("Can't erase this region - contains code in use!\n");
        return;
    }
    if ((stat = cyg_flash_erase(flash_addr, length, &err_addr)) != 0) {
        diag_printf("Error erasing at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
    }
}

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING

static void
fis_lock(int argc, char *argv[])
{
    char *name;
    int stat;
    unsigned long length;
    CYG_ADDRESS flash_addr;
    bool flash_addr_set = false;
    bool length_set = false;
    cyg_flashaddr_t err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void *)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, &name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
#ifdef CYGOPT_REDBOOT_FIS
    /* Get parameters from image if specified */
    if (name) {
        struct fis_image_desc *img;
        if ((img = fis_lookup(name, NULL)) == (struct fis_image_desc *)0) {
            diag_printf("No image '%s' found\n", name);
            return;
        }

        flash_addr = img->flash_base;
        length = img->size;
    } else
#endif
      if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = cyg_flash_verify_addr(flash_addr)) ||
         (stat = cyg_flash_verify_addr((flash_addr+length-1))))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }
    if ((stat = cyg_flash_lock(flash_addr, length, &err_addr)) != 0) {
        diag_printf("Error locking at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
    }
}

static void
fis_unlock(int argc, char *argv[])
{
    char *name;
    int stat;
    unsigned long length;
    CYG_ADDRESS flash_addr;
    bool flash_addr_set = false;
    bool length_set = false;
    cyg_flashaddr_t err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void *)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void *)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, &name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
#ifdef CYGOPT_REDBOOT_FIS
    if (name) {
        struct fis_image_desc *img;
        if ((img = fis_lookup(name, NULL)) == (struct fis_image_desc *)0) {
            diag_printf("No image '%s' found\n", name);
            return;
        }

        flash_addr = img->flash_base;
        length = img->size;
    } else
#endif
      if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = cyg_flash_verify_addr(flash_addr)) ||
         (stat = cyg_flash_verify_addr((flash_addr+length-1))))) {
        _show_invalid_flash_address(flash_addr, stat);
        return;
    }

    if ((stat = cyg_flash_unlock(flash_addr, length, &err_addr)) != 0) {
        diag_printf("Error unlocking at %p: %s\n", (void*)err_addr, cyg_flash_errmsg(stat));
    }
}
#endif

// This is set non-zero if the FLASH subsystem has successfully been initialized
int __flash_init;

void
_flash_info(void)
{   
    cyg_uint32 i=0,j;
    cyg_flash_info_t info;
    int ret;
    
    if (!__flash_init) return;

    do {
      ret = cyg_flash_get_info(i, &info);
      if (ret == CYG_FLASH_ERR_OK) {
          diag_printf("FLASH: %p-%p", (void*)info.start, (void*)info.end);
        for (j=0;j < info.num_block_infos; j++) {
          diag_printf(", %d x 0x%x blocks",
                      info.block_info[j].blocks,
                      (unsigned int)info.block_info[j].block_size);
        }
        diag_printf("\n");
      }
      i++;
    } while (ret != CYG_FLASH_ERR_INVALID);
}

/* Returns -1 on failure, 0 on success, 1 if it was successfull
 but a failed fis update was detected  */
int
do_flash_init(void)
{
    int stat, i;
    cyg_flash_info_t info;

#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
    struct fis_image_desc img0;
    struct fis_image_desc img1;
    int fis_update_was_interrupted=0;
    cyg_flashaddr_t err_addr;

    //check the size of fis_valid_info
    CYG_ASSERT((sizeof(struct fis_valid_info)<=sizeof(img0.u.name)), "fis_valid_info size mismatch");
    //try to check the alignment of version_count
    CYG_ASSERT((((unsigned long)&img0.u.valid_info.version_count - (unsigned long)&img0) % sizeof(unsigned long) == 0), "alignment problem");
#endif



    if (!__flash_init) {
        __flash_init = 1;

        cyg_flash_set_global_printf((cyg_flash_printf *)&diag_printf);
        if ((stat = cyg_flash_init(NULL)) != 0) {
            diag_printf("FLASH: driver init failed: %s\n", cyg_flash_errmsg(stat));
            return -1;
        }

#ifdef CYGNUM_REDBOOT_FLASH_BASE
        stat = cyg_flash_get_info_addr(CYGNUM_REDBOOT_FLASH_BASE, &info);
#else
        stat = cyg_flash_get_info(0, &info);
#endif
        if (stat != CYG_FLASH_ERR_OK) {
             diag_printf("FLASH: driver init failed: %s\n", 
                         cyg_flash_errmsg(stat));
             return false;
        }
        flash_start = info.start;
        flash_end = info.end;

        // No bootblock support yet, so we merge any bootblocks we might
        // find into full size blocks
        for (i=0; i < info.num_block_infos; i++) {
          if (info.block_info[i].block_size > flash_block_size) {
            flash_block_size = info.block_info[i].block_size;
          }
        }
        flash_num_blocks = 0;
        for (i=0; i < info.num_block_infos; i++) {
          flash_num_blocks += (info.block_info[i].block_size *
                               info.block_info[i].blocks) /
            flash_block_size;
        }
        
#ifdef CYGOPT_REDBOOT_FIS
        fisdir_size = CYGNUM_REDBOOT_FIS_DIRECTORY_ENTRY_COUNT * CYGNUM_REDBOOT_FIS_DIRECTORY_ENTRY_SIZE;
        fisdir_size = ((fisdir_size + flash_block_size - 1) / flash_block_size) * flash_block_size;
# if defined(CYGPRI_REDBOOT_ZLIB_FLASH) && defined(CYGOPT_REDBOOT_FIS_ZLIB_COMMON_BUFFER)
	fis_work_block = fis_zlib_common_buffer;
	if(CYGNUM_REDBOOT_FIS_ZLIB_COMMON_BUFFER_SIZE < fisdir_size) {
            diag_printf("FLASH: common buffer too small\n");
            return -1;
	}
# else
        workspace_end = (unsigned char *)(workspace_end-fisdir_size);
        fis_work_block = workspace_end;
# endif

        if (CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK < 0) {
            fis_addr = ((CYG_ADDRESS)flash_end + 1 +
                        (CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK*flash_block_size));
        } else {
            fis_addr = ((CYG_ADDRESS)flash_start + 
                        (CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK*flash_block_size));
        }
        
        if (((CYG_ADDRESS)fis_addr + fisdir_size - 1) > (CYG_ADDRESS)flash_end) {
            diag_printf("FIS directory doesn't fit\n");
            return -1;
        }
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS

        if (CYGNUM_REDBOOT_FIS_REDUNDANT_DIRECTORY_BLOCK < 0) {
            redundant_fis_addr = ((CYG_ADDRESS)flash_end + 1 +
                                  (CYGNUM_REDBOOT_FIS_REDUNDANT_DIRECTORY_BLOCK*flash_block_size));
        } else {
            redundant_fis_addr = ((CYG_ADDRESS)flash_start +
                                  (CYGNUM_REDBOOT_FIS_REDUNDANT_DIRECTORY_BLOCK*flash_block_size));
        }

        if (((CYG_ADDRESS)redundant_fis_addr + fisdir_size - 1) > (CYG_ADDRESS)flash_end) {
            diag_printf("Redundant FIS directory doesn't fit\n");
            return -1;
        }
        cyg_flash_read(fis_addr, &img0, sizeof(img0), &err_addr);
        cyg_flash_read(redundant_fis_addr, &img1, sizeof(img1), &err_addr);

        if (strncmp(img0.u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC, CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH)!=0)
        {
           memset(&img0, 0, sizeof(img0));
        }

        if (strncmp(img1.u.valid_info.magic_name, CYG_REDBOOT_RFIS_VALID_MAGIC, CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH)!=0)
        {
           memset(&img1, 0, sizeof(img0));
        }

#ifdef REDBOOT_FLASH_REVERSE_BYTEORDER
        img0.u.valid_info.version_count = CYG_SWAP32(img0.u.valid_info.version_count);
        img1.u.valid_info.version_count = CYG_SWAP32(img1.u.valid_info.version_count);
#endif

        if (fis_get_valid_buf(&img0, &img1, &fis_update_was_interrupted)==1)
        {
           // Valid, so swap primary and secondary
           cyg_flashaddr_t tmp;
           tmp = fis_addr;
           fis_addr = redundant_fis_addr;
           redundant_fis_addr = tmp;
        }
#endif
        fis_read_directory();
#endif
    }
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
    if (fis_update_was_interrupted)
       return 1;
    else
       return 0;
#else
    return 0;
#endif
}

// Wrapper to avoid compiler warnings
static void
_do_flash_init(void)
{
    static int init_done = 0;
    if (init_done) return;
    init_done = 1;
    do_flash_init();
}

RedBoot_init(_do_flash_init, RedBoot_INIT_FIRST);

static void
do_fis(int argc, char *argv[])
{
    struct cmd *cmd;

    if (argc < 2) {
        fis_usage("too few arguments");
        return;
    }
    if (do_flash_init()<0) {
        diag_printf("Sorry, no FLASH memory is available\n");
        return;
    }
    if ((cmd = cmd_search(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, 
                          argv[1])) != (struct cmd *)0) {
        (cmd->fun)(argc, argv);
        return;
    }
    fis_usage("unrecognized command");
}

// EOF flash.c
