//=================================================================
//
//        strptime.c
//
//        Testcase for C library strptime() function
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  gthomas
// Date:          1999-03-05
// Description:   Contains testcode for C library strptime() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/libc_time.h>          // C library configuration

// INCLUDES

#include <time.h>
#include <cyg/infra/testcase.h>
#include <string.h>                     // strlen()
// HOW TO START TESTS

# define START_TEST( test ) test(0)

// FUNCTIONS

static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()

static void
test( CYG_ADDRWORD data )
{
    struct tm tm1;
    char s[1000], *sp, *dp, *fp;
    size_t size;
    
    dp = "Fri Jan 24 08:33:14 2003";
    fp = "%a %b %d %H:%M:%S %Y";
    sp = strptime(dp, fp, &tm1);
    
    // Set an invalid year day. The following converters don't use
    // this, so it should not cause a problem.
    tm1.tm_yday = 1000;

    CYG_TEST_PASS_FAIL(((sp!=NULL) && (*sp=='\0')), "strptime test #1");
    size = strftime(s, sizeof(s), fp, &tm1);
    CYG_TEST_PASS_FAIL(((size==strlen(dp)) && (my_strcmp(s, dp) == 0)), "strptime test #2");
    
    dp = "Friday January 24 08:33:14 2003";
    fp = "%A %B %d %H:%M:%S %Y";
    sp = strptime(dp, fp, &tm1);    
    CYG_TEST_PASS_FAIL(((sp!=NULL) && (*sp=='\0')), "strptime test #3");
    size = strftime(s, sizeof(s), fp, &tm1);
    CYG_TEST_PASS_FAIL(((size==strlen(dp)) && (my_strcmp(s, dp) == 0)), "strptime test #4");

    dp = "2006:06:13 12:22:01";
    fp = "%x %X";
    sp = strptime(dp, fp, &tm1);    
    CYG_TEST_PASS_FAIL(((sp!=NULL) && (*sp=='\0')), "strptime test #5");
    CYG_TEST_PASS_FAIL((tm1.tm_sec == 01) &&
                       (tm1.tm_min == 22) &&
                       (tm1.tm_hour == 12) &&
                       (tm1.tm_mday == 13) &&
                       (tm1.tm_mon ==  (06 - 1)) &&
                       (tm1.tm_year == (2006 - 1900)), "strptime test #6");
    size = strftime(s, sizeof(s), fp, &tm1);
    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strptime() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strptime() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF strptime.c
