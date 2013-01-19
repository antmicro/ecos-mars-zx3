//=================================================================
//
//        atoi.c
//
//        Testcase for C library atoi()
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     
// Contributors:  
// Date:          2000-04-30
// Description:   Contains testcode for C library atoi() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/system.h>
#include <stdlib.h>
#include <cyg/infra/testcase.h>

// FUNCTIONS

static char *
my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()


static void
test( CYG_ADDRWORD data )
{
    char x[30];

    // Check 1
    my_strcpy(x, "20");
    CYG_TEST_PASS_FAIL( atoi(x) == 20, "atoi(20)");

    // Check 2
    my_strcpy(x, "1972");
    CYG_TEST_PASS_FAIL( atoi(x) == 1972, "atoi(1972)");

    // Check 3
    my_strcpy(x, "-9876");
    CYG_TEST_PASS_FAIL( atoi(x) == -9876, "atoi(-9876)");

    // Check 4
    my_strcpy(x, "  -9876xxx");
    CYG_TEST_PASS_FAIL( atoi(x) == -9876, "atoi(   -9876xxx)");


    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "atoi() function");
} // test()


#ifndef CYGPKG_LIBC_STARTUP
void cyg_user_start(void)
#else
int
main( int argc, char *argv[] )
#endif
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "atoi() function");

    test(0);

    CYG_TEST_FAIL_FINISH("Not reached");

} // main()

// EOF atoi.c
