#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H
//===========================================================================
//
//      var_io.h
//
//      Variant specific registers
//
//===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011, 2013 Free Software Foundation, Inc.                        
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-02-05
// Purpose:       Kinetis variant specific registers
// Description:
// Usage:         #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/hal_cortexm_kinetis.h>

#include <cyg/hal/plf_io.h>

//===========================================================================
// Cortex-M architecture
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Cortex-M architecture overrides
//---------------------------------------------------------------------------
// VTOR - Vector Table Offset Register
#ifndef CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM
#define CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM (BIT_(29) -                       \
                                           CYGHWR_HAL_KINETIS_SRAM_BANK_SIZE)
#endif

//=============================================================================
// Kinetis system configuration
//---------------------------------------------------------------------------
// Utilize Freescale Kinetis flash between startup vectors and 0x400
// for misc funtions.
#ifdef CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION
# define CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR \
         CYGBLD_ATTRIB_SECTION(".kinetis_misc")
#else
# define CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
#endif

//===========================================================================
// KINETIS FLASH configuration field
//===========================================================================

// Note: KINETIS FLASH configuration field must be present in Kinetis flash
//       image and ocupy addresses 0x00000400 to 0x0000040f.

typedef struct cyghwr_hal_kinetis_flash_conf_s {
    cyg_uint8 backdoor_key[8]; // 0x400 .. 0x407
    cyg_uint8 fprot[4];        // 0x408 .. 0x40b
    cyg_uint8 fsec;            // 0x40c
    cyg_uint8 fopt;            // 0x40d
    cyg_uint8 feprot;          // 0x40e
    cyg_uint8 fdprot;          // 0x40f
} cyghwr_hal_kinetis_flash_conf_t;

__externC const cyghwr_hal_kinetis_flash_conf_t *hal_kinetis_flash_conf_p( void );

//===========================================================================
// Kinetis Peripherals
//---------------------------------------------------------------------------
// Internal Flash

typedef volatile struct cyghwr_hal_kinetis_flash_s {
    cyg_uint8 fstat;    // Flash status register
    cyg_uint8 fcnfg;    // Flash configuration register
    cyg_uint8 fsec;     // Flash security register
    cyg_uint8 fopt;     // Flash option register
    cyg_uint8 fccob3;   // Flash common command object registers
    cyg_uint8 fccob2;
    cyg_uint8 fccob1;
    cyg_uint8 fccob0;
    cyg_uint8 fccob7;
    cyg_uint8 fccob6;
    cyg_uint8 fccob5;
    cyg_uint8 fccob4;
    cyg_uint8 fccobB;
    cyg_uint8 fccobA;
    cyg_uint8 fccob9;
    cyg_uint8 fccob8;
    cyg_uint8 fprot3;   // Program flash protection registers
    cyg_uint8 fprot2;
    cyg_uint8 fprot1;
    cyg_uint8 fprot0;
    cyg_uint8 reserved[2];
    cyg_uint8 feprot;   // EEPROM Protection register
    cyg_uint8 fdprot;   // Data flash protection register
} cyghwr_hal_kinetis_flash_t;

#define CYGHWR_HAL_KINETIS_FLASH_P ((cyghwr_hal_kinetis_flash_t *) 0x40020000)

#define CYGHWR_HAL_KINETIS_FLASH_FSTAT_CCIF_M           0x80
#define CYGHWR_HAL_KINETIS_FLASH_FSTAT_RDCOLERR_M       0x40
#define CYGHWR_HAL_KINETIS_FLASH_FSTAT_ACCERR_M         0x20
#define CYGHWR_HAL_KINETIS_FLASH_FSTAT_FPVIOL_M         0x10
#define CYGHWR_HAL_KINETIS_FLASH_FSTAT_MGSTAT0_M        0x01

//---------------------------------------------------------------------------
// Oscillator

#define CYGHWR_HAL_KINETIS_OSC_CR          (0x40065000)
#define CYGHWR_HAL_KINETIS_OSC_CR_P        ((volatile cyg_uint8*) 0x40065000)

#define CYGHWR_HAL_KINETIS_OSC1_CR         (0x400E5000)
#define CYGHWR_HAL_KINETIS_OSC1_CR_P       ((volatile cyg_uint8*) 0x400E5000)

#define CYGHWR_HAL_KINETIS_OSC_CR_SC16P_M      0x01
#define CYGHWR_HAL_KINETIS_OSC_CR_SC16P_S         0
#define CYGHWR_HAL_KINETIS_OSC_CR_SC8P_M       0x02
#define CYGHWR_HAL_KINETIS_OSC_CR_SC8P_S          1
#define CYGHWR_HAL_KINETIS_OSC_CR_SC4P_M       0x04
#define CYGHWR_HAL_KINETIS_OSC_CR_SC4P_S          2
#define CYGHWR_HAL_KINETIS_OSC_CR_SC2P_M       0x08
#define CYGHWR_HAL_KINETIS_OSC_CR_SC2P_S          3
#define CYGHWR_HAL_KINETIS_OSC_CR_EREFSTEN_M   0x20
#define CYGHWR_HAL_KINETIS_OSC_CR_EREFSTEN_S      5
#define CYGHWR_HAL_KINETIS_OSC_CR_ERCLKEN_M    0x80
#define CYGHWR_HAL_KINETIS_OSC_CR_ERCLKEN_S       7

//---------------------------------------------------------------------------
// MCG

typedef volatile struct cyghwr_hal_kinetis_mcg_s {
    cyg_uint8 c1;      // MCG Control 1 Register
    cyg_uint8 c2;      // MCG Control 2 Register
    cyg_uint8 c3;      // MCG Control 3 Register
    cyg_uint8 c4;      // MCG Control 4 Register
    cyg_uint8 c5;      // MCG Control 5 Register
    cyg_uint8 c6;      // MCG Control 6 Register
    cyg_uint8 status;  // MCG Status Register
    cyg_uint8 mcg_res0;
#if CYGHWR_HAL_CORTEXM_KINETIS_REV == 1
    cyg_uint8 atc;     // MCG Auto Trim Control Register
#elif CYGHWR_HAL_CORTEXM_KINETIS_REV == 2
    cyg_uint8 sc;     // MCG Status and Control Register
#endif
    cyg_uint8 mcg_res1;
    cyg_uint8 atcvh;   // MCG Auto Trim Compare Value High Register
    cyg_uint8 atcvl;   // MCG Auto Trim Compare Value Low Register
#if CYGHWR_HAL_CORTEXM_KINETIS_REV == 2
    cyg_uint8 c7;      // MCG Control 7 Register
    cyg_uint8 c8;      // MCG Control 8 Register
#endif
#if CYGINT_HAL_CORTEXM_KINETIS_150
    cyg_uint8 c10;     // MCG Control 10 Register
    cyg_uint8 mcg_res2;
    cyg_uint8 c11;     // MCG Control 11 Register
    cyg_uint8 c12;     // MCG Control 12 Register
    cyg_uint8 s2;      // MCG Status 2 Register
#endif //CYGINT_HAL_CORTEXM_KINETIS_150
} cyghwr_hal_kinetis_mcg_t;

#define CYGHWR_HAL_KINETIS_MCG_P  ((cyghwr_hal_kinetis_mcg_t *)0x40064000)

// C1 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C1_IREFSTEN_M    0x01
#define CYGHWR_HAL_KINETIS_MCG_C1_IREFSTEN_S    0
#define CYGHWR_HAL_KINETIS_MCG_C1_IRCLKEN_M     0x02
#define CYGHWR_HAL_KINETIS_MCG_C1_IRCLKEN_S     1
#define CYGHWR_HAL_KINETIS_MCG_C1_IREFS_M       0x4
#define CYGHWR_HAL_KINETIS_MCG_C1_IREFS_S       2
#define CYGHWR_HAL_KINETIS_MCG_C1_FRDIV_M       0x38
#define CYGHWR_HAL_KINETIS_MCG_C1_FRDIV_S       3
#define CYGHWR_HAL_KINETIS_MCG_C1_FRDIV(_div_)  \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C1_FRDIV_S, _div_)
#define CYGHWR_HAL_KINETIS_MCG_C1_CLKS_M        0xC0
#define CYGHWR_HAL_KINETIS_MCG_C1_CLKS_S        6
#define CYGHWR_HAL_KINETIS_MCG_C1_CLKS(_clks_)  \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C1_CLKS_S, _clks_)
enum {
    CYGHWR_HAL_KINETIS_MCG_C1_CLKS_FLL_PLL,
    CYGHWR_HAL_KINETIS_MCG_C1_CLKS_INT_REF,
    CYGHWR_HAL_KINETIS_MCG_C1_CLKS_EXT_REF,
    CYGHWR_HAL_KINETIS_MCG_C1_CLKS_RESERVED
};
// C2 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C2_IRCS_M        0x01
#define CYGHWR_HAL_KINETIS_MCG_C2_IRCS_S        0
#define CYGHWR_HAL_KINETIS_MCG_C2_LP_M          0x02
#define CYGHWR_HAL_KINETIS_MCG_C2_LP_S          1
#define CYGHWR_HAL_KINETIS_MCG_C2_EREFS_M       0x04
#define CYGHWR_HAL_KINETIS_MCG_C2_EREFS_S       2
#define CYGHWR_HAL_KINETIS_MCG_C2_HGO_M         0x08
#define CYGHWR_HAL_KINETIS_MCG_C2_HGO_S         3
#define CYGHWR_HAL_KINETIS_MCG_C2_RANGE_M       0x30
#define CYGHWR_HAL_KINETIS_MCG_C2_RANGE_S       4
#define CYGHWR_HAL_KINETIS_MCG_C2_RANGE(__v)    \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C2_RANGE_S, __v)

#if CYGHWR_HAL_CORTEXM_KINETIS_REV == 2
#define CYGHWR_HAL_KINETIS_MCG_C2_LOCRE0_M      0x80
#define CYGHWR_HAL_KINETIS_MCG_C2_LOCRE0_S      7
#endif
// C3 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C3_SCTRIM_M      0xFF
#define CYGHWR_HAL_KINETIS_MCG_C3_SCTRIM_S      0
#define CYGHWR_HAL_KINETIS_MCG_C3_SCTRIM(__v)   \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C3_SCTRIM_S, __v)
// C4 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C4_SCFTRIM_M     0x01
#define CYGHWR_HAL_KINETIS_MCG_C4_SCFTRIM_S     0
#define CYGHWR_HAL_KINETIS_MCG_C4_FCTRIM_M      0x1E
#define CYGHWR_HAL_KINETIS_MCG_C4_FCTRIM_S      1
#define CYGHWR_HAL_KINETIS_MCG_C4_FCTRIM(__v)   \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C4_FCTRIM_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C4_DRST_DRS_M    0x60
#define CYGHWR_HAL_KINETIS_MCG_C4_DRST_DRS_S    5
#define CYGHWR_HAL_KINETIS_MCG_C4_DRST_DRS(__v) \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C4_DRST_DRS_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C4_DMX32_M       0x80
#define CYGHWR_HAL_KINETIS_MCG_C4_DMX32_S       7
// C5 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C5_PRDIV_M       0x1F
#define CYGHWR_HAL_KINETIS_MCG_C5_PRDIV_S       0
#define CYGHWR_HAL_KINETIS_MCG_C5_PRDIV(__v)    \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C5_PRDIV_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C5_PLLSTEN_M     0x20
#define CYGHWR_HAL_KINETIS_MCG_C5_PLLSTEN_S     5
#define CYGHWR_HAL_KINETIS_MCG_C5_PLLCLKEN_M    0x40
#define CYGHWR_HAL_KINETIS_MCG_C5_PLLCLKEN_S    6
#if CYGINT_HAL_CORTEXM_KINETIS_HAS_OSC1
# define CYGHWR_HAL_KINETIS_MCG_C5_PLLREFSEL0_M  0x80
# define CYGHWR_HAL_KINETIS_MCG_C5_PLLREFSEL0_S  7
#endif
// C6 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C6_VDIV_M        0x1F
#define CYGHWR_HAL_KINETIS_MCG_C6_VDIV_S        0
#define CYGHWR_HAL_KINETIS_MCG_C6_VDIV(__v)     \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C6_VDIV_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C6_CME_M         0x20
#define CYGHWR_HAL_KINETIS_MCG_C6_CME_S         5
#define CYGHWR_HAL_KINETIS_MCG_C6_PLLS_M        0x40
#define CYGHWR_HAL_KINETIS_MCG_C6_PLLS_S        6
#define CYGHWR_HAL_KINETIS_MCG_C6_LOLIE_M       0x80
#define CYGHWR_HAL_KINETIS_MCG_C6_LOLIE_S       7
// S Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_S_IRCST_M        0x01
#define CYGHWR_HAL_KINETIS_MCG_S_IRCST_S        0
#define CYGHWR_HAL_KINETIS_MCG_S_OSCINIT_M      0x02
#define CYGHWR_HAL_KINETIS_MCG_S_OSCINIT_S      1
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_M        0x0C
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_S        2
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST(__v)     \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_S_CLKST_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_FLL  CYGHWR_HAL_KINETIS_MCG_S_CLKST(0)
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_INT  CYGHWR_HAL_KINETIS_MCG_S_CLKST(1)
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_EXT  CYGHWR_HAL_KINETIS_MCG_S_CLKST(2)
#define CYGHWR_HAL_KINETIS_MCG_S_CLKST_PLL  CYGHWR_HAL_KINETIS_MCG_S_CLKST(3)
#define CYGHWR_HAL_KINETIS_MCG_S_IREFST_M       0x10
#define CYGHWR_HAL_KINETIS_MCG_S_IREFST_S       4
#define CYGHWR_HAL_KINETIS_MCG_S_PLLST_M        0x20
#define CYGHWR_HAL_KINETIS_MCG_S_PLLST_S        5
#define CYGHWR_HAL_KINETIS_MCG_S_LOCK_M         0x40
#define CYGHWR_HAL_KINETIS_MCG_S_LOCK_S         6
#define CYGHWR_HAL_KINETIS_MCG_S_LOLS_M         0x80
#define CYGHWR_HAL_KINETIS_MCG_S_LOLS_S         7

#if CYGHWR_HAL_CORTEXM_KINETIS_REV == 1
// ATC Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATMF_M       0x20
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATMF_S       5
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATMS_M       0x40
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATMS_S       6
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATME_M       0x80
#define CYGHWR_HAL_KINETIS_MCG_ATC_ATME_S       7
#elif CYGHWR_HAL_CORTEXM_KINETIS_REV == 2
// SC Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_SC_LOCS0_M       0x01
#define CYGHWR_HAL_KINETIS_MCG_SC_LOCS0_S       0
#define CYGHWR_HAL_KINETIS_MCG_SC_FCRDIV_M      0x0E
#define CYGHWR_HAL_KINETIS_MCG_SC_FCRDIV_S      1
#define CYGHWR_HAL_KINETIS_MCG_SC_FCRDIV(__v) \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_SC_FCRDIV_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_SC_FLTPRSRV_M    0x10
#define CYGHWR_HAL_KINETIS_MCG_SC_FLTPRSRV_S    4
#define CYGHWR_HAL_KINETIS_MCG_SC_ATMF_M        0x20
#define CYGHWR_HAL_KINETIS_MCG_SC_ATMF_S        5
#define CYGHWR_HAL_KINETIS_MCG_SC_ATMS_M        0x40
#define CYGHWR_HAL_KINETIS_MCG_SC_ATMS_S        6
#define CYGHWR_HAL_KINETIS_MCG_SC_ATME_M        0x80
#define CYGHWR_HAL_KINETIS_MCG_SC_ATME_S        7
#endif //  CYGHWR_HAL_CORTEXM_KINETIS_REV
// ATCVH Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_ATCVH_ATCVH_M    0xFF
#define CYGHWR_HAL_KINETIS_MCG_ATCVH_ATCVH_S    0
#define CYGHWR_HAL_KINETIS_MCG_ATCVH_ATCVH(__v) \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_ATCVH_ATCVH_S, __v)
// ATCVL Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_ATCVL_ATCVL_M    0xFF
#define CYGHWR_HAL_KINETIS_MCG_ATCVL_ATCVL_S    0
#define CYGHWR_HAL_KINETIS_MCG_ATCVL_ATCVL(__v) \
VALUE_(CYGHWR_HAL_KINETIS_MCG_ATCVL_ATCVL_S, __v)

#if CYGHWR_HAL_CORTEXM_KINETIS_REV == 2
// C7 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C7_OSCSEL_M      0x1
#define CYGHWR_HAL_KINETIS_MCG_C7_OSCSEL_S      0
// C8 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C8_LOCS1_M       0x1
#define CYGHWR_HAL_KINETIS_MCG_C8_LOCS1_S       0
#define CYGHWR_HAL_KINETIS_MCG_C8_CME1_M        0x20
#define CYGHWR_HAL_KINETIS_MCG_C8_CME1_S        5
#define CYGHWR_HAL_KINETIS_MCG_C8_LOLRE_M       0x40
#define CYGHWR_HAL_KINETIS_MCG_C8_LOLRE_S       6
#define CYGHWR_HAL_KINETIS_MCG_C8_LOCRE1_M      0x80
#define CYGHWR_HAL_KINETIS_MCG_C8_LOCRE1_S      7
#endif // CYGHWR_HAL_CORTEXM_KINETIS_REV == 2

#if CYGINT_HAL_CORTEXM_KINETIS_150

// C10 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C10_EREFS1_M      0x04
#define CYGHWR_HAL_KINETIS_MCG_C10_EREFS1_S      2
#define CYGHWR_HAL_KINETIS_MCG_C10_HGO1_M        0x08
#define CYGHWR_HAL_KINETIS_MCG_C10_HGO1_S        3
#define CYGHWR_HAL_KINETIS_MCG_C10_RANGE1_M      0x30
#define CYGHWR_HAL_KINETIS_MCG_C10_RANGE1_S      4
#define CYGHWR_HAL_KINETIS_MCG_C10_RANGE1(__v)    \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C10_RANGE1_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C10_LOCRE0_M      0x80
#define CYGHWR_HAL_KINETIS_MCG_C10_LOCRE0_S      7
// C11 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C11_PRDIV1_M       0x7
#define CYGHWR_HAL_KINETIS_MCG_C11_PRDIV1_S       0
#define CYGHWR_HAL_KINETIS_MCG_C11_PRDIV1(__v)    \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C11_PRDIV1_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLCS_M        0x10
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLCS_S        4
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLSTEN1_M     0x20
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLSTEN1_S     5
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLCLKEN1_M    0x40
#define CYGHWR_HAL_KINETIS_MCG_C11_PLLCLKEN1_S    6
# define CYGHWR_HAL_KINETIS_MCG_C11_PLLREFSEL1_M  0x80
# define CYGHWR_HAL_KINETIS_MCG_C11_PLLREFSEL1_S  7
// C12 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_C12_VDIV1_M        0x1F
#define CYGHWR_HAL_KINETIS_MCG_C12_VDIV1_S        0
#define CYGHWR_HAL_KINETIS_MCG_C12_VDIV1(__v)     \
        VALUE_(CYGHWR_HAL_KINETIS_MCG_C12_VDIV1_S, __v)
#define CYGHWR_HAL_KINETIS_MCG_C12_CME2_M         0x20
#define CYGHWR_HAL_KINETIS_MCG_C12_CME2_S         5
#define CYGHWR_HAL_KINETIS_MCG_C12_LOLIE1_M       0x80
#define CYGHWR_HAL_KINETIS_MCG_C12_LOLIE1_S       7
// S2 Bit Fields
#define CYGHWR_HAL_KINETIS_MCG_S2_LOCS2_M        0x01
#define CYGHWR_HAL_KINETIS_MCG_S2_LOCS2_S        0
#define CYGHWR_HAL_KINETIS_MCG_S2_OSCINIT1_M     0x02
#define CYGHWR_HAL_KINETIS_MCG_S2_OSCINIT1_S     1
#define CYGHWR_HAL_KINETIS_MCG_S2_PLLCST_M       0x10
#define CYGHWR_HAL_KINETIS_MCG_S2_PLLCST_S       4
#define CYGHWR_HAL_KINETIS_MCG_S2_LOCK1_M        0x40
#define CYGHWR_HAL_KINETIS_MCG_S2_LOCK1_S        6
#define CYGHWR_HAL_KINETIS_MCG_S2_LOLS1_M        0x80
#define CYGHWR_HAL_KINETIS_MCG_S2_LOLS1_S        7

#endif //CYGINT_HAL_CORTEXM_KINETIS_150


//---------------------------------------------------------------------------
// Real Time Clock

typedef volatile struct cyghwr_hal_kinetis_rtc_s {
    cyg_uint32 tsr;                    // Time Seconds Register
    cyg_uint32 tpr;                    // Time Prescaler Register
    cyg_uint32 tar;                    // Time Alarm Register
    cyg_uint32 tcr;                    // Time Compensation Register
    cyg_uint32 cr;                     // Control Register
    cyg_uint32 sr;                     // Status Register
    cyg_uint32 lr;                     // Lock Register
    cyg_uint32 ier;                    // Enterrupt Enable Register
    cyg_uint32 ttsr;                   // Tamper Times Seconds Register
    cyg_uint32 mer;                    // Monotonic Enable Register
    cyg_uint32 mclr;                   // Monotonic Counter Low Register
    cyg_uint32 mchr;                   // Monotonic Counter High Register
    cyg_uint32 ter;                    // Tamper Enable Register
    cyg_uint32 tdr;                    // Tamper Detect Register
    cyg_uint32 ttr;                    // Tamper Trim Register
    cyg_uint32 tir;                    // Tamper Interrupt Register
    cyg_uint8 reserved[1984];
    cyg_uint32 war;                    // Write Access Register
    cyg_uint32 rar;                    // Read Access Register
} cyghwr_hal_kinetis_rtc_t;

#define CYGHWR_HAL_KINETIS_RTC_P   ((cyghwr_hal_kinetis_rtc_t *)0x4003D000)

#define CYGHWR_HAL_KINETIS_RTC_TCR_TCR_M (0x000000FF)
#define CYGHWR_HAL_KINETIS_RTC_TCR_CIR_S 8
#define CYGHWR_HAL_KINETIS_RTC_TCR_CIR(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_RTC_TCR_CIR_S, _div_)
#define CYGHWR_HAL_KINETIS_RTC_TCR_CIC_S 16
#define CYGHWR_HAL_KINETIS_RTC_TCR_TCV_S 24

#define CYGHWR_HAL_KINETIS_RTC_CR_SWR  BIT_(0)
#define CYGHWR_HAL_KINETIS_RTC_CR_WPE  BIT_(1)
#define CYGHWR_HAL_KINETIS_RTC_CR_SUP  BIT_(2)
#define CYGHWR_HAL_KINETIS_RTC_CR_UM   BIT_(3)
#define CYGHWR_HAL_KINETIS_RTC_CR_OSCE BIT_(8)
#define CYGHWR_HAL_KINETIS_RTC_CR_CLKO BIT_(9)
#define CYGHWR_HAL_KINETIS_RTC_CR_SCP \
        VALUE_(10, (CYGHWR_HAL_CORTEXM_KINETIS_RTC_OSC_CAP/2))

#define CYGHWR_HAL_KINETIS_RTC_SR_TCE BIT_(4)
#define CYGHWR_HAL_KINETIS_RTC_SR_TAF BIT_(2)
#define CYGHWR_HAL_KINETIS_RTC_SR_TOF BIT_(1)
#define CYGHWR_HAL_KINETIS_RTC_SR_TIF BIT_(0)

//---------------------------------------------------------------------------
// Watch dog

// WDOG - Peripheral register structure
typedef volatile struct CygHwr_HAL_Kinetis_wdog_s {
    cyg_uint16 StCtrlH;    // Status and Control Register High
    cyg_uint16 StCtrlL;    // Status and Control Register Low
    cyg_uint16 ToValH;     // Time-out Value Register High
    cyg_uint16 ToValL;     // Time-out Value Register Low
    cyg_uint16 WinH;       // Window Register High
    cyg_uint16 WinL;       // Window Register Low
    cyg_uint16 Refresh;    // Refresh Register
    cyg_uint16 Unlock;     // Unlock Register
    cyg_uint16 TmrOutH;    // Timer Output Register High
    cyg_uint16 TmrOutL;    // Timer Output Register Low
    cyg_uint16 RstCnt;     // Reset Count Register
    cyg_uint16 Presc;      // Prescaler Register
} CygHwr_HAL_Kinetis_wdog_t;

#define CYGHWR_HAL_KINETIS_WDOG_P ((CygHwr_HAL_Kinetis_wdog_t *)0x40052000)

// STCTRLH Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WDOGEN_M          0x0001
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WDOGEN_S          0
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_CLKSRC_M          0x0002
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_CLKSRC_S          1
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_IRQRSTEN_M        0x0004
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_IRQRSTEN_S        2
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WINEN_M           0x0008
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WINEN_S           3
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_ALLOWUPDATE_M     0x0010
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_ALLOWUPDATE_S     4
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_DBGEN_M           0x0020
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_DBGEN_S           5
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_STOPEN_M          0x0040
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_STOPEN_S          6
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WAITEN_M          0x0080
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_WAITEN_S          7
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_STNDBYEN_M        0x0100
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_STNDBYEN_S        8
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_TESTWDOG_M        0x0400
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_TESTWDOG_S        10
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_TESTSEL_M         0x0800
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_TESTSEL_S         11
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_BYTESEL_M         0x3000
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_BYTESEL_S         12
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_BYTESEL(__v)      \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_STCTRLH_BYTESEL_S, __v)
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_DISTESTWDOG_M     0x4000
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLH_DISTESTWDOG_S     14
// STCTRLL Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLL_INTFLG_M          0x8000
#define CYGHWR_HAL_KINETIS_WDOG_STCTRLL_INTFLG_S          15
// TOVALH Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_TOVALH_TOVALHIGH_M        0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_TOVALH_TOVALHIGH_S        0
#define CYGHWR_HAL_KINETIS_WDOG_TOVALH_TOVALHIGH(__v)     \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_TOVALH_TOVALHIGH_S, __v)
// TOVALL Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_TOVALL_TOVALLOW_M         0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_TOVALL_TOVALLOW_S         0
#define CYGHWR_HAL_KINETIS_WDOG_TOVALL_TOVALLOW(__v)      \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_TOVALL_TOVALLOW_S, __v)
// WINH Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_WINH_WINHIGH_M            0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_WINH_WINHIGH_S            0
#define CYGHWR_HAL_KINETIS_WDOG_WINH_WINHIGH(__v)         \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_WINH_WINHIGH_S, __v)
// WINL Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_WINL_WINLOW_M             0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_WINL_WINLOW_S             0
#define CYGHWR_HAL_KINETIS_WDOG_WINL_WINLOW(__v)          \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_WINL_WINLOW_S, __v)
// REFRESH Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_REFRESH_WDOGREFRESH_M     0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_REFRESH_WDOGREFRESH_S     0
#define CYGHWR_HAL_KINETIS_WDOG_REFRESH_WDOGREFRESH(__v)  \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_REFRESH_WDOGREFRESH_S, __v)
// UNLOCK Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_UNLOCK_WDOGUNLOCK_M       0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_UNLOCK_WDOGUNLOCK_S       0
#define CYGHWR_HAL_KINETIS_WDOG_UNLOCK_WDOGUNLOCK(__v)    \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_UNLOCK_WDOGUNLOCK_S, __v)
// TMROUTH Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTH_TIMEROUTHIGH_M    0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTH_TIMEROUTHIGH_S    0
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTH_TIMEROUTHIGH(__v) \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_TMROUTH_TIMEROUTHIGH_S, __v)
// TMROUTL Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTL_TIMEROUTLOW_M     0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTL_TIMEROUTLOW_S     0
#define CYGHWR_HAL_KINETIS_WDOG_TMROUTL_TIMEROUTLOW(__v)  \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_TMROUTL_TIMEROUTLOW_S, __v)
// RSTCNT Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_RSTCNT_RSTCNT_M           0xFFFF
#define CYGHWR_HAL_KINETIS_WDOG_RSTCNT_RSTCNT_S           0
#define CYGHWR_HAL_KINETIS_WDOG_RSTCNT_RSTCNT(__v)        \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_RSTCNT_RSTCNT_S, __v)
// PRESC Bit Fields
#define CYGHWR_HAL_KINETIS_WDOG_PRESC_PRESCVAL_M          0x700
#define CYGHWR_HAL_KINETIS_WDOG_PRESC_PRESCVAL_S          8
#define CYGHWR_HAL_KINETIS_WDOG_PRESC_PRESCVAL(__v)       \
        VALUE(CYGHWR_HAL_KINETIS_WDOG_PRESC_PRESCVAL_S, __v)

#ifndef __ASSEMBLER__

__externC void hal_wdog_unlock(volatile CygHwr_HAL_Kinetis_wdog_t *wdog_p);
__externC void hal_wdog_disable(void);

#endif // __ASSEMBLER__

//---------------------------------------------------------------------------
// SIM - System Integration Module

// SIM - Peripheral register structure
typedef volatile struct cyghwr_hal_kinetis_sim_s {
    cyg_uint32 sopt1;           // System Options Register 1
    cyg_uint8  reserved_0[4096];
    cyg_uint32 sopt2;           // System Options Register 2
    cyg_uint8  reserved_1[4];
    cyg_uint32 sopt4;           // System Options Register 4
    cyg_uint32 sopt5;           // System Options Register 5
    cyg_uint32 sopt6;           // System Options Register 6
    cyg_uint32 sopt7;           // System Options Register 7
    cyg_uint8  Reserved_2[8];
    cyg_uint32 sdid;            // System Device Identification Register
    cyg_uint32 scgc1;           // System Clock Gating Control Register 1
    cyg_uint32 scgc2;           // System Clock Gating Control Register 2
    cyg_uint32 scgc3;           // System Clock Gating Control Register 3
    cyg_uint32 scgc4;           // System Clock Gating Control Register 4
    cyg_uint32 scgc5;           // System Clock Gating Control Register 5
    cyg_uint32 scgc6;           // System Clock Gating Control Register 6
    cyg_uint32 scgc7;           // System Clock Gating Control Register 7
    cyg_uint32 clk_div1;        // System Clock Divider Register 1
    cyg_uint32 clk_div2;        // System Clock Divider Register 2
    cyg_uint32 fcfg1;           // Flash Configuration Register 1
    cyg_uint32 fcfg2;           // Flash Configuration Register 2
    cyg_uint32 uidh;            // Unique Identification Register High
    cyg_uint32 uidmh;           // Unique Identification Register Mid-High
    cyg_uint32 uidml;           // Unique Identification Register Mid Low
    cyg_uint32 uidl;            // Unique Identification Register Low

    cyg_uint32 clkdiv3;         // System Clock Divider Register 3
    cyg_uint32 clkdiv4;         // System Clock Divider Register 4
    cyg_uint32 mcr;             // Misc control register
} cyghwr_hal_kinetis_sim_t;

#define CYGHWR_HAL_KINETIS_SIM_P ((cyghwr_hal_kinetis_sim_t *) 0x40047000)

// SOPT1 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_RAMSIZE_M         0xF000
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_RAMSIZE_S         12
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_RAMSIZE(__val)    \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT1_RAMSIZE_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_OSC32KSEL_M       0x80000
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_OSC32KSEL_S       19
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_MS_M              0x800000
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_MS_S              23
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_USBSTBY_M         0x40000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_USBSTBY_S         30
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_USBREGEN_M        0x80000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT1_USBREGEN_S        31
// SOPT2 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_MCGCLKSEL_M       0x1
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_MCGCLKSEL_S       0
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_FBSL_M            0x300
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_FBSL_S            8
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_FBSL(__val)       \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT2_FBSL_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_CMTUARTPAD_M      0x800
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_CMTUARTPAD_S      11
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_TRACECLKSEL_M     0x1000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_TRACECLKSEL_S     12
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_PLLFLLSEL_M       0x10000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_PLLFLLSEL_S       16
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_USBSRC_M          0x40000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_USBSRC_S          18
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_TIMESRC_M         0x300000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_TIMESRC_S         20
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_TIMESRC(__val)    \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT2_TIMESRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_I2SSRC_M          0x3000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_I2SSRC_S          24
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_I2SSRC(__val)     \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT2_I2SSRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_SDHCSRC_M         0x30000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_SDHCSRC_S         28
#define CYGHWR_HAL_KINETIS_SIM_SOPT2_SDHCSRC(__val)    \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT2_SDHCSRC_S, __val)
// SOPT4 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT0_M        0x1
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT0_S        0
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT1_M        0x2
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT1_S        1
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT2_M        0x4
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0FLT2_S        2
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1FLT0_M        0x10
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1FLT0_S        4
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2FLT0_M        0x100
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2FLT0_S        8
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CH0SRC_M      0xC0000
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CH0SRC_S      18
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CH0SRC(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CH0SRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CH0SRC_M      0x300000
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CH0SRC_S      20
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CH0SRC(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CH0SRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0CLKSEL_M      0x1000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM0CLKSEL_S      24
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CLKSEL_M      0x2000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM1CLKSEL_S      25
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CLKSEL_M      0x4000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT4_FTM2CLKSEL_S      26
// SOPT5 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0TXSRC_M      0x3
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0TXSRC_S      0
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0TXSRC(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0TXSRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0RXSRC_M      0xC
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0RXSRC_S      2
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0RXSRC(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT5_UART0RXSRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UARTTXSRC_M       0x30
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UARTTXSRC_S       4
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UARTTXSRC(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT5_UARTTXSRC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART1RXSRC_M      0xC0
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART1RXSRC_S      6
#define CYGHWR_HAL_KINETIS_SIM_SOPT5_UART1RXSRC(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT5_UART1RXSRC_S, __val)
// SOPT6 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTSEL_M       0x1F000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTSEL_S       24
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTSEL(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTSEL_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTEN_M        0xE0000000
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTEN_S        29
#define CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTEN(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT6_RSTFLTEN_S, __val)
// SOPT7 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0TRGSEL_M      0xF
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0TRGSEL_S      0
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0TRGSEL(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0TRGSEL_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0PRETRGSEL_M   0x10
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0PRETRGSEL_S   4
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0ALTTRGEN_M    0x80
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC0ALTTRGEN_S    7
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1TRGSEL_M      0xF00
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1TRGSEL_S      8
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1TRGSEL(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1TRGSEL_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1PRETRGSEL_M   0x1000
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1PRETRGSEL_S   12
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1ALTTRGEN_M    0x8000
#define CYGHWR_HAL_KINETIS_SIM_SOPT7_ADC1ALTTRGEN_S    15
// SDID Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_SDID_PINID_M            0xF
#define CYGHWR_HAL_KINETIS_SIM_SDID_PINID_S            0
#define CYGHWR_HAL_KINETIS_SIM_SDID_PINID(__val)       \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SDID_PINID_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SDID_FAMID_M            0x70
#define CYGHWR_HAL_KINETIS_SIM_SDID_FAMID_S            4
#define CYGHWR_HAL_KINETIS_SIM_SDID_FAMID(__val)       \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SDID_FAMID_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_SDID_REVID_M            0xF000
#define CYGHWR_HAL_KINETIS_SIM_SDID_REVID_S            12
#define CYGHWR_HAL_KINETIS_SIM_SDID_REVID(__val)       \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_SDID_REVID_S, __val)

//---------------------------------------------------------------------------
// Clock distribution
// The following encodes the control register and clock bit number
// into clock configuration descriptor (CLKCD).
#define CYGHWR_HAL_KINETIS_SIM_SCGC(__reg,__bit) ((((__reg) - 1 ) & 0xF) + \
                                                  (((__bit) << 8) & 0x1F00))

// Macros to extract encoded values.
#define CYGHWR_HAL_KINETIS_SIM_SCGC_REG(__clkcd) (((__clkcd) & 0xF))
#define CYGHWR_HAL_KINETIS_SIM_SCGC_BIT(__clkcd) (((__clkcd) >> 8) & 0x1F)

// Functions and macros to enable/disable clocks.
#define CYGHWR_HAL_SCGC_NONE (0xFFFFFFFF)
__externC void hal_clock_enable(cyg_uint32 clkcd);
__externC void hal_clock_disable(cyg_uint32 clkcd);

#define CYGHWR_HAL_CLOCK_ENABLE(__clkcd) hal_clock_enable(__clkcd)
#define CYGHWR_HAL_CLOCK_DISABLE(__clkcd) hal_clock_disable(__clkcd)

#include <cyg/hal/var_io_clkgat.h>

// CLKDIV1 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV4_M       0xF0000
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV4_S       16
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV4(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV4_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV3_M       0xF00000
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV3_S       20
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV3(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV3_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV2_M       0xF000000
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV2_S       24
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV2(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV2_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV1_M       0xF0000000
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV1_S       28
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV1(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV1_OUTDIV1_S, __val)
// CLKDIV2 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBFRAC_M       0x1
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBFRAC_S       0
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBDIV_M        0xE
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBDIV_S        1
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBDIV(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV2_USBDIV_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SFRAC_M       0xFF00
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SFRAC_S       8
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SFRAC(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SFRAC_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SDIV_M        0xFFF00000
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SDIV_S        20
#define CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SDIV(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_CLKDIV2_I2SDIV_S, __val)
// FCFG1 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_DEPART_M          0xF00
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_DEPART_S          8
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_DEPART(__val)     \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_FCFG1_DEPART_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_EESIZE_M          0xF0000
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_EESIZE_S          16
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_EESIZE(__val)     \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_FCFG1_EESIZE_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_FSIZE_M           0xFF000000
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_FSIZE_S           24
#define CYGHWR_HAL_KINETIS_SIM_FCFG1_FSIZE(__val)      \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_FCFG1_FSIZE_S, __val)
// FCFG2 Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR1_M        0x3F0000
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR1_S        16
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR1(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR1_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_PFLSH_M           0x800000
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_PFLSH_S           23
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR0_M        0x3F000000
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR0_S        24
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR0(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_FCFG2_MAXADDR0_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_SWAPPFLSH_M       0x80000000
#define CYGHWR_HAL_KINETIS_SIM_FCFG2_SWAPPFLSH_S       31
// UIDH Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_UIDH_UID_M              0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_SIM_UIDH_UID_S              0
#define CYGHWR_HAL_KINETIS_SIM_UIDH_UID(__val)         \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_UIDH_UID_S, __val)
// UIDMH Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_UIDMH_UID_M             0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_SIM_UIDMH_UID_S             0
#define CYGHWR_HAL_KINETIS_SIM_UIDMH_UID(__val)        \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_UIDMH_UID_S, __val)
// UIDML Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_UIDML_UID_M             0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_SIM_UIDML_UID_S             0
#define CYGHWR_HAL_KINETIS_SIM_UIDML_UID(__val)        \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_UIDML_UID_S, __val)
// UIDL Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_UIDL_UID_M              0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_SIM_UIDL_UID_S              0
#define CYGHWR_HAL_KINETIS_SIM_UIDL_UID(__val)         \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_UIDL_UID_S, __val)
// MCR Bit Fields
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRCFG_M           0xE000
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRCFG_S           5
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRCFG(__val)      \
        VALUE_(CYGHWR_HAL_KINETIS_SIM_MCR_DDRCFG_S, __val)
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRDQSDIS_M        0x8
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRDQSDIS_S        3
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRPEN_M           0x4
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRPEN_S           2
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRS_M             0x2
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRS_S             1
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRSREN_M          0x1
#define CYGHWR_HAL_KINETIS_SIM_MCR_DDRSREN_S          0



//--------------------------------------------------------------------------
// AXBS - Crossbar switch

#define CYGHWR_HAL_KINETIS_AXBS_SLAVES_K    7
#define CYGHWR_HAL_KINETIS_AXBS_MASTERS_K   8

typedef volatile struct cyghwr_hal_kinetis_axbs_s {
    volatile struct cyghwr_hal_kinetis_axbs_slave_s {
        cyg_uint32 prs;
        cyg_uint32 res0[3];
        cyg_uint32 crs;
        cyg_uint32 res1[59];
    } slave[8];
    volatile struct cyghwr_hal_kinetis_axbs_master_s {
        cyg_uint32 res0[64];
        cyg_uint32 mgprc;
    } master[8];
} cyghwr_hal_kinetis_axbs_t;

#define CYGHWR_HAL_KINETIS_AXBS_P ((cyghwr_hal_kinetis_axbs_t *) 0x40004000)

// PRS Fields
#define CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_S(_master_) ((_master_) * 4)
#define CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_M(_master_) \
            (0x7 << CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_S(_master_))

#define CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_PRIO(_master_,_prs)           \
            (((_prs) & CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_M(_master_)) >>  \
             CYGHWR_HAL_KINETIS_AXBS_PRS_MASTER_S(_master_))
// CRS Fields
#define CYGHWR_HAL_KINETIS_AXBS_CRS_PARK_M     0x7

#define CYGHWR_HAL_KINETIS_AXBS_CRS_PCTL_M     0x30
#define CYGHWR_HAL_KINETIS_AXBS_CRS_PCTL_S     4
#define CYGHWR_HAL_KINETIS_AXBS_CRS_PCTL(_x_)  (((_x_) << CYGHWR_HAL_KINETIS_AXBS_CRS_PCTL_S) & \
                                                         CYGHWR_HAL_KINETIS_AXBS_CRS_PCTL_M)
#define CYGHWR_HAL_KINETIS_AXBS_CRS_ARB_M      0x300
#define CYGHWR_HAL_KINETIS_AXBS_CRS_ARB_S      8
#define CYGHWR_HAL_KINETIS_AXBS_CRS_ARB(_x_)   (((_x_) << CYGHWR_HAL_KINETIS_AXBS_CRS_ARB_S) & \
                                                        CYGHWR_HAL_KINETIS_AXBS_CRS_ARB_M)
#define CYGHWR_HAL_KINETIS_AXBS_CRS_HLP_M      0x40000000
#define CYGHWR_HAL_KINETIS_AXBS_CRS_RO_M       0x80000000

#define CYGHWR_HAL_KINETIS_AXBS_CRS_SET_ARB(_slave_i,_arb)          \
CYG_MACRO_START                                                     \
    cyg_uint32 regval;                                              \
    cyghwr_hal_kinetis_axbs_t* _axbs_p = CYGHWR_HAL_KINETIS_AXBS_P; \
    regval = _axbs_p->slave[_slave_i].crs;                          \
    regval &= ~CYGHWR_HAL_KINETIS_AXBS_CRS_ARB_M;                   \
    regval |= CYGHWR_HAL_KINETIS_AXBS_CRS_ARB(_arb);                \
    _axbs_p->slave[_slave_i].crs = regval;                          \
CYG_MACRO_END

#define CYGHWR_HAL_KINETIS_AXBS_CRS_SET_ARB_RR(_slave_i)    \
            CYGHWR_HAL_KINETIS_AXBS_CRS_SET_ARB((_slave_i), 1)

#define CYGHWR_HAL_KINETIS_AXBS_CRS_SET_ARB_FIX(_slave_i)   \
            CYGHWR_HAL_KINETIS_AXBS_CRS_SET_ARB((_slave_i), 0)

// MGPCR Bit Fields
#define CYGHWR_HAL_KINETIS_AXBS_CRS_AXBS_MGPCR_AULB_M  0x7

//---------------------------------------------------------------------------
// PORT - Peripheral register structure

typedef volatile struct cyghwr_hal_kinetis_port_s {
    cyg_uint32 pcr[32];      // Pin Control Register n, array
    cyg_uint32 gpclr;        // Global Pin Control Low Register
    cyg_uint32 gpchr;        // Global Pin Control High Register
    cyg_uint8 reserved0[24];
    cyg_uint32 isfr;         // Interrupt Status Flag Register
    cyg_uint8 reserved1[28];
    cyg_uint32 dfer;         // Digital Filter Enable Register
    cyg_uint32 dfcr;         // Digital Filter Clock Register
    cyg_uint32 dfwr;         // Digital Filter Width Register
} cyghwr_hal_kinetis_port_t;

// PORT - Peripheral instance base addresses
#define CYGHWR_HAL_KINETIS_PORTA_P  ((cyghwr_hal_kinetis_port_t *)0x40049000)
#define CYGHWR_HAL_KINETIS_PORTB_P  ((cyghwr_hal_kinetis_port_t *)0x4004A000)
#define CYGHWR_HAL_KINETIS_PORTC_P  ((cyghwr_hal_kinetis_port_t *)0x4004B000)
#define CYGHWR_HAL_KINETIS_PORTD_P  ((cyghwr_hal_kinetis_port_t *)0x4004C000)
#define CYGHWR_HAL_KINETIS_PORTE_P  ((cyghwr_hal_kinetis_port_t *)0x4004D000)
#define CYGHWR_HAL_KINETIS_PORTF_P  ((cyghwr_hal_kinetis_port_t *)0x4004E000)

enum {
    CYGHWR_HAL_KINETIS_PORTA, CYGHWR_HAL_KINETIS_PORTB,
    CYGHWR_HAL_KINETIS_PORTC, CYGHWR_HAL_KINETIS_PORTD,
    CYGHWR_HAL_KINETIS_PORTE, CYGHWR_HAL_KINETIS_PORTF
};

#define CYGHWR_HAL_KINETIS_PORT(__port, __reg) \
        (CYGHWR_HAL_KINETIS_PORT##__port##_P)->__reg

// PCR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_PCR_PS_M          0x1
#define CYGHWR_HAL_KINETIS_PORT_PCR_PS_S          0
#define CYGHWR_HAL_KINETIS_PORT_PCR_PE_M          0x2
#define CYGHWR_HAL_KINETIS_PORT_PCR_PE_S          1
#define CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M         0x4
#define CYGHWR_HAL_KINETIS_PORT_PCR_SRE_S         2
#define CYGHWR_HAL_KINETIS_PORT_PCR_PFE_M         0x10
#define CYGHWR_HAL_KINETIS_PORT_PCR_PFE_S         4
#define CYGHWR_HAL_KINETIS_PORT_PCR_ODE_M         0x20
#define CYGHWR_HAL_KINETIS_PORT_PCR_ODE_S         5
#define CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M         0x40
#define CYGHWR_HAL_KINETIS_PORT_PCR_DSE_S         6
#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX_M         0x700
#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX_S         8
#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX(__val)    \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_PCR_MUX_S, __val)
#define CYGHWR_HAL_KINETIS_PORT_PCR_LK_M          0x8000
#define CYGHWR_HAL_KINETIS_PORT_PCR_LK_S          15
#define CYGHWR_HAL_KINETIS_PORT_PCR_IRQC_M        0xF0000
#define CYGHWR_HAL_KINETIS_PORT_PCR_IRQC_S        16
#define CYGHWR_HAL_KINETIS_PORT_PCR_IRQC(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_PCR_IRQC_S, __val)
#define CYGHWR_HAL_KINETIS_PORT_PCR_ISF_M         0x1000000
#define CYGHWR_HAL_KINETIS_PORT_PCR_ISF_S         24

#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX_ANALOG    0
#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX_DIS       0
#define CYGHWR_HAL_KINETIS_PORT_PCR_MUX_GPIO      1

#define CYGHWR_HAL_KINETIS_PORT_PCR_ISFR_CLEAR(__port, __pin) \
        CYGHWR_HAL_KINETIS_PORT(__port, pcr[__pin]) |= BIT_(24)

#define CYGHWR_HAL_KINETIS_PORT_ISFR_CLEAR(__port, __pin)     \
        CYGHWR_HAL_KINETIS_PORT(__port, isfr) |= BIT_(__pin)

#define CYGHWR_HAL_KINETIS_PIN_CFG(__port, __bit, __mux, __irqc, __cnf) \
        ((CYGHWR_HAL_KINETIS_PORT##__port << 20) | ((__bit) << 27)      \
         | CYGHWR_HAL_KINETIS_PORT_PCR_IRQC(__irqc)                     \
         | CYGHWR_HAL_KINETIS_PORT_PCR_MUX(__mux) | (__cnf))

#define CYGHWR_HAL_KINETIS_PIN(__port, __bit, __mux, __cnf) \
        CYGHWR_HAL_KINETIS_PIN_CFG(__port, __bit, __mux, 0, __cnf)

#define CYGHWR_HAL_KINETIS_PIN_PORT(__pin) (((__pin) >> 20) & 0x7)
#define CYGHWR_HAL_KINETIS_PIN_BIT(__pin)  (((__pin) >> 27 ) & 0x1f)
#define CYGHWR_HAL_KINETIS_PIN_FUNC(__pin) ((__pin) & 0x010f8777)
#define CYGHWR_HAL_KINETIS_PIN_NONE (0xffffffff)

// GPCLR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWD_M      0xFFFF
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWD_S      0
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWD(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWD_S, __val)
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWE_M      0xFFFF0000
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWE_S      16
#define CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWE(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_GPCLR_GPWE_S, __val)
// GPCHR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWD_M      0xFFFF
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWD_S      0
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWD(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWD_S, __val)
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWE_M      0xFFFF0000
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWE_S      16
#define CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWE(__val) \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_GPCHR_GPWE_S, __val)
// ISFR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_ISFR_ISF_M        0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_PORT_ISFR_ISF_S        0
#define CYGHWR_HAL_KINETIS_PORT_ISFR_ISF(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_ISFR_ISF_S, __val)
// DFER Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_DFER_DFE_M        0xFFFFFFFF
#define CYGHWR_HAL_KINETIS_PORT_DFER_DFE_S        0
#define CYGHWR_HAL_KINETIS_PORT_DFER_DFE(__val)   \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_DFER_DFE_S, __val)
// DFCR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_DFCR_CS_M         0x1
#define CYGHWR_HAL_KINETIS_PORT_DFCR_CS_S         0
// DFWR Bit Fields
#define CYGHWR_HAL_KINETIS_PORT_DFWR_FILT_M       0x1F
#define CYGHWR_HAL_KINETIS_PORT_DFWR_FILT_S       0
#define CYGHWR_HAL_KINETIS_PORT_DFWR_FILT(__val)  \
        VALUE_(CYGHWR_HAL_KINETIS_PORT_DFWR_FILT_S, __val)

#ifndef __ASSEMBLER__

// Pin configuration related functions
__externC void  hal_set_pin_function(cyg_uint32 pin);
__externC void  hal_dump_pin_function(cyg_uint32 pin);
__externC void  hal_dump_pin_setting(cyg_uint32 pin);

#endif

#define HAL_SET_PINS(_pin_array) \
CYG_MACRO_START \
    const cyg_uint32 *_pin_p; \
    for(_pin_p = &_pin_array[0]; \
        _pin_p < &_pin_array[0] + sizeof(_pin_array)/sizeof(_pin_array[0]); \
        hal_set_pin_function(*_pin_p++)); \
CYG_MACRO_END

//---------------------------------------------------------------------------
// PMC Power Management Controller

typedef volatile struct cyghwr_hal_kinetis_pmc_s {
    cyg_uint8 lvdsc1;   // Low Voltage Detect Status and Control 1 Register
    cyg_uint8 lvdsc2;   // Low Voltage Detect Status and Control 2 Register
    cyg_uint8 regsc;    // Regulator Status and Control Register
} cyghwr_hal_kinetis_pmc_t;

// PMC base address
#define CYGHWR_HAL_KINETIS_PMC_P  ((cyghwr_hal_kinetis_pmc_t *)0x4007D000)

// LVDSC1 Bit Fields
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDV_M      0x3
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDV(__val) \
        ((__val) & CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDV_M)
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDRE_M     0x10
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDRE_S     4
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDIE_M     0x20
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDIE_S     5
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDACK_M    0x40
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDACK_S    6
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDF_M      0x80
#define CYGHWR_HAL_KINETIS_PMC_LVDSC1_LVDF_S      7
// LVDSC2 Bit Fields
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWV_M      0x3
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWV(__val) \
        ((__val) & CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWV_M)
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWIE_M     0x20
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWIE_S     5
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWACK_M    0x40
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWACK_S    6
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWF_M      0x80
#define CYGHWR_HAL_KINETIS_PMC_LVDSC2_LVWF_S      7
// REGSC Bit Fields
#define CYGHWR_HAL_KINETIS_PMC_REGSC_BGBE_M       0x1
#define CYGHWR_HAL_KINETIS_PMC_REGSC_BGBE_S       0
#define CYGHWR_HAL_KINETIS_PMC_REGSC_REGONS_M     0x4
#define CYGHWR_HAL_KINETIS_PMC_REGSC_REGONS_S     2
#define CYGHWR_HAL_KINETIS_PMC_REGSC_ACKISO_M     0x8
#define CYGHWR_HAL_KINETIS_PMC_REGSC_ACKISO_S     3
#define CYGHWR_HAL_KINETIS_PMC_REGSC_BGEN_M       0x10
#define CYGHWR_HAL_KINETIS_PMC_REGSC_BGEN_S       4

//---------------------------------------------------------------------------
// FMC Flash Memory Controller

#define CYGHWR_HAL_KINETIS_FMC_BASE     (0x4001F000)
#define CYGHWR_HAL_KINETIS_FMC_PFAPR    (CYGHWR_HAL_KINETIS_FMC_BASE)
#define CYGHWR_HAL_KINETIS_FMC_PFB0CR   (CYGHWR_HAL_KINETIS_FMC_BASE + 4)
#define CYGHWR_HAL_KINETIS_FMC_PFB1CR   (CYGHWR_HAL_KINETIS_FMC_BASE + 8)

enum {
    CYGHWR_HAL_KINETIS_FMC_CACHE_W0,
    CYGHWR_HAL_KINETIS_FMC_CACHE_W1,
    CYGHWR_HAL_KINETIS_FMC_CACHE_W2,
    CYGHWR_HAL_KINETIS_FMC_CACHE_W3,
    CYGHWR_HAL_KINETIS_FMC_CACHE_WAYS
};

enum {
    CYGHWR_HAL_KINETIS_FMC_CACHE_S0,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S1,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S2,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S3,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S4,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S5,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S6,
    CYGHWR_HAL_KINETIS_FMC_CACHE_S7,
    CYGHWR_HAL_KINETIS_FMC_CACHE_SIDES
};

#define CYGHWR_HAL_KINETIS_FMC_TAG(__way,__side) \
          (CYGHWR_HAL_KINETIS_FMC_BASE + 0x100 + (__way)*0x20 + (__side)*4)
#define CYGHWR_HAL_KINETIS_FMC_DATA_U(__way,side) \
          (CYGHWR_HAL_KINETIS_FMC_BASE + 0x200 + (__way)*0x20 + (__side)*8)
#define CYGHWR_HAL_KINETIS_FMC_DATA_L(__way,side) \
          (CYGHWR_HAL_KINETIS_FMC_BASE + 0x204 + (__way)*0x40 + (__side)*8)

#define CYGHWR_HAL_KINETIS_FMC_PFAPR_MPFD_M(__master) (1 << ((__master) + 16))
enum {
    CYGHWR_HAL_KINETIS_FMC_PFAPR_MAP_NO_ACCESS,
    CYGHWR_HAL_KINETIS_FMC_PFAPR_MAP_RO,
    CYGHWR_HAL_KINETIS_FMC_PFAPR_MAP_WO,
    CYGHWR_HAL_KINETIS_FMC_PFAPR_MAP_RW
};
#define CYGHWR_HAL_KINETIS_FMC_PFAPR_MAP(__master, __access) \
          ((__access) <<(2 * (__master)))

#define CYGHWR_HAL_KINETIS_FMC_PFBCR_RWSC_M (0xf0000000)

#define CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_WAY(__way) ((1 << (__way)) << 24)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_ALL \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W0) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W1) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W2) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CLCK_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W3)

#define CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_WAY(__way) ((1 << (__way)) << 20)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_ALL \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W0) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W1) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W2) | \
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CINV_WAY(CYGHWR_HAL_KINETIS_FMC_CACHE_W3)

#define CYGHWR_HAL_KINETIS_FMC_PFBCR_SBINV (1 << 19)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BMW   (7 << 17)

enum{
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CRC_LRU,
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CRC_res0,
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CRC_IND_LRUW01IF23D,
    CYGHWR_HAL_KINETIS_FMC_PFBCR_CRC_IND_LRUW02ID3D
};
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_CRC   (__cache_repl_con) \
                                       ((__cache_repl_con) << 5)

#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BDCE  (0x10)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BICE  (0x08)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BDPE  (0x04)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BIPE  (0x02)
#define CYGHWR_HAL_KINETIS_FMC_PFBCR_BSEBE (0x01)

//---------------------------------------------------------------------------
// MPU Memory Protection unit

typedef volatile struct cyghwr_hal_kinetis_mpu_s {
    cyg_uint32 cesr;          // Control/Error Status Register
    cyg_uint8 reserved0[12];
    struct {
        cyg_uint32 ear;       // Error Address Register, Slave Port n
        cyg_uint32 edr;       // Error Detail Register, Slave Port n
    } slave_port[5];
    cyg_uint8 reserved1[968];
    struct {  // Region Descriptors, Word 0..Region Descriptor n, Word 3
        cyg_uint32 word[4];
    }reg_desc[12];
    cyg_uint8 reserved2[832];
    cyg_uint32 reg_daac[12];  // Region Descriptor Alternate Access Control n
} cyghwr_hal_kinetis_mpu_t;

#define CYGHWR_HAL_KINETIS_MPU_P  (cyghwr_hal_kinetis_mpu_t *)0x4000d000

//---------------------------------------------------------------------------
// FlexBus
#ifdef CYGPKG_HAL_CORTEXM_KINETIS_FLEXBUS
# include <cyg/hal/var_io_flexbus.h>
#endif

//----------------------------------------------------------------------------
// DDRMC - SDRAM controller
#ifdef CYGPKG_HAL_CORTEXM_KINETIS_DDRMC
# include <cyg/hal/var_io_ddrmc.h>
#endif

//---------------------------------------------------------------------------
// GPIO
#include <cyg/hal/var_io_gpio.h>

//=============================================================================
// DEVS:
// Following macros may also be, and usually are borrwed by some device drivers.
//-----------------------------------------------------------------------------
#include <cyg/hal/var_io_devs.h>

// End Peripherals

// Some miscelaneous function prototypes
// Clock related functions are in kinetis_clocking.c
__externC void hal_start_clocks(void);
__externC void hal_update_clock_var(void);

//-----------------------------------------------------------------------------
// end of var_io.h

#endif // CYGONCE_HAL_VAR_IO_H
