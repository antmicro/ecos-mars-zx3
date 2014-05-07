#ifndef CYGONCE_CONFIG_KEYS_H
#define CYGONCE_CONFIG_KEYS_H
// ====================================================================
//
//      config_keys.h
//
//      Device I/O "Keys" for get/put config functions
//
// ====================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas,jskov,grante,jlarmour
// Date:         1999-02-04
// Purpose:      Repository for all get/put config "keys"
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// This file contains all of the 'key' values used by all I/O components.
// It is placed in this single repository to make it easy to reduce conflicts.

// ======== 0x0100 Serial ====================================================
// Get/Set configuration 'key' values for low-level serial I/O

#define CYG_IO_GET_CONFIG_SERIAL_INFO                  0x0101
#define CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN          0x0102
#define CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH          0x0103
#define CYG_IO_SET_CONFIG_SERIAL_OUTPUT_FLUSH          CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH
#define CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH           0x0104
#define CYG_IO_SET_CONFIG_SERIAL_INPUT_FLUSH           CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH
#define CYG_IO_GET_CONFIG_SERIAL_ABORT                 0x0105
#define CYG_IO_GET_CONFIG_SERIAL_BUFFER_INFO           0x0111
#define CYG_IO_GET_CONFIG_SERIAL_FLOW_CONTROL_METHOD   0x0112

#define CYG_IO_SET_CONFIG_SERIAL_INFO                  0x0181
#define CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE   0x0184
#define CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG        0x0185
#define CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_METHOD   0x0186
#define CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE    0x0187
#define CYG_IO_SET_CONFIG_SERIAL_STATUS_CALLBACK       0x0188
#define CYG_IO_SET_CONFIG_SERIAL_HW_BREAK              0x0189

// Compatibility values. Use of these is deprecated, the generic symbols
// should be used instead.
#define CYG_IO_GET_CONFIG_SERIAL_READ_BLOCKING   CYG_IO_GET_CONFIG_READ_BLOCKING
#define CYG_IO_GET_CONFIG_SERIAL_WRITE_BLOCKING  CYG_IO_GET_CONFIG_WRITE_BLOCKING
#define CYG_IO_SET_CONFIG_SERIAL_READ_BLOCKING   CYG_IO_SET_CONFIG_READ_BLOCKING
#define CYG_IO_SET_CONFIG_SERIAL_WRITE_BLOCKING  CYG_IO_SET_CONFIG_WRITE_BLOCKING

// ======== 0x0200 TTY =======================================================
// Get/Set configuration 'key' values for tty-like driver
#define CYG_IO_GET_CONFIG_TTY_INFO       0x0201  // Get channel configuration
#define CYG_IO_SET_CONFIG_TTY_INFO       0x0281  // Set channel configuration


// ======== 0x0300 DSP =======================================================
// Get/Set configuration 'key' values for low-level DSP I/O
#define CYG_IO_GET_CONFIG_DSP_OUTPUT_DRAIN       0x0301
#define CYG_IO_GET_CONFIG_DSP_OUTPUT_FLUSH       0x0302
#define CYG_IO_GET_CONFIG_DSP_INPUT_FLUSH        0x0303
#define CYG_IO_GET_CONFIG_DSP_ABORT              0x0304
#define CYG_IO_GET_CONFIG_DSP_INPUT_OVERFLOW_RESET 0x0307

// Compatibility values. Use of these is deprecated, the generic symbols
// should be used instead.
#define CYG_IO_GET_CONFIG_DSP_READ_BLOCKING      CYG_IO_GET_CONFIG_READ_BLOCKING
#define CYG_IO_GET_CONFIG_DSP_WRITE_BLOCKING     CYG_IO_GET_CONFIG_WRITE_BLOCKING
#define CYG_IO_SET_CONFIG_DSP_READ_BLOCKING      CYG_IO_SET_CONFIG_READ_BLOCKING
#define CYG_IO_SET_CONFIG_DSP_WRITE_BLOCKING     CYG_IO_SET_CONFIG_WRITE_BLOCKING

// ======== 0x400 DSP =======================================================
// Get/Set configuration 'key' values for termios emulation

#define CYG_IO_GET_CONFIG_TERMIOS                0x0400
#define CYG_IO_SET_CONFIG_TERMIOS                0x0401

// ======== 0x600 FLASH =====================================================
// Get/Set configuration 'key' values for FLASH drivers

#define CYG_IO_GET_CONFIG_FLASH_ERASE            0x600
#define CYG_IO_GET_CONFIG_FLASH_QUERY            0x601
#define CYG_IO_GET_CONFIG_FLASH_LOCK             0x602
#define CYG_IO_GET_CONFIG_FLASH_UNLOCK           0x603
#define CYG_IO_GET_CONFIG_FLASH_VERIFY           0x604
#define CYG_IO_GET_CONFIG_FLASH_DEVSIZE          0x605
#define CYG_IO_GET_CONFIG_FLASH_BLOCKSIZE        0x606
#define CYG_IO_GET_CONFIG_FLASH_DEVADDR          0x607

// Note the following is now deprecated. See generic flash package for the replacement.
#define CYG_IO_SET_CONFIG_FLASH_FIS_NAME         0x680

// ======== 0x700 DISK =======================================================
// Get/Set configuration 'key' values for DISK I/O 

#define CYG_IO_GET_CONFIG_DISK_INFO              0x700

#define CYG_IO_SET_CONFIG_DISK_MOUNT             0x781
#define CYG_IO_SET_CONFIG_DISK_UMOUNT            0x782

// ======== 0x800 CAN ========================================================
// Get/Set configuration 'key' values for CAN I/O 
#define CYG_IO_GET_CONFIG_CAN_INFO                  0x0801
#define CYG_IO_GET_CONFIG_CAN_BUFFER_INFO           0x0802
#define CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO           0x0803
#define CYG_IO_GET_CONFIG_CAN_TIMEOUT               0x0804
#define CYG_IO_GET_CONFIG_CAN_HDI                   0x0805
#define CYG_IO_GET_CONFIG_CAN_STATE                 0x0806
#define CYG_IO_GET_CONFIG_CAN_ERR_COUNTERS          0x0807

#define CYG_IO_SET_CONFIG_CAN_INFO                  0x0881
#define CYG_IO_SET_CONFIG_CAN_OUTPUT_DRAIN          0x0882
#define CYG_IO_SET_CONFIG_CAN_OUTPUT_FLUSH          0x0883
#define CYG_IO_SET_CONFIG_CAN_INPUT_FLUSH           0x0884
#define CYG_IO_SET_CONFIG_CAN_TIMEOUT               0x0885
#define CYG_IO_SET_CONFIG_CAN_MSGBUF                0x0886
#define CYG_IO_SET_CONFIG_CAN_MODE                  0x0887
#define CYG_IO_SET_CONFIG_CAN_ABORT                 0x0888
#define CYG_IO_SET_CONFIG_CAN_CALLBACK              0x0889
#define CYG_IO_SET_CONFIG_CAN_RANGE_FILTER          0x088A
#define CYG_IO_SET_CONFIG_CAN_MASK_FILTER           0x088B

// ======== 0x900 ADC ========================================================
// Get/Set configuration 'key' values for ADC I/O

#define CYG_IO_GET_CONFIG_ADC_RATE                  0x0901

#define CYG_IO_SET_CONFIG_ADC_RATE                  0x0981
#define CYG_IO_SET_CONFIG_ADC_ENABLE                0x0982
#define CYG_IO_SET_CONFIG_ADC_DISABLE               0x0983
#define CYG_IO_SET_CONFIG_ADC_DATA_FLUSH            0x0984

// ======== 0x1000 Generic ===================================================
// Get/Set configuration 'key' values that can apply to more than one
// class of device.

#define CYG_IO_GET_CONFIG_READ_BLOCKING         0x1001
#define CYG_IO_GET_CONFIG_WRITE_BLOCKING        0x1002

#define CYG_IO_SET_CONFIG_READ_BLOCKING         0x1081
#define CYG_IO_SET_CONFIG_WRITE_BLOCKING        0x1082

// Close the underlying device - primarily useful for io/fileio's devfs,
// but probably more widely applicable. Note that this is not like UNIX
// close in that there is no reference counting. If that is needed it must
// be done at a higher level.
#define CYG_IO_SET_CONFIG_CLOSE                 0x1100

#endif  /* CYGONCE_CONFIG_KEYS_H */
/* EOF config_keys.h */
