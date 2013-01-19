//===========================================================================
//
//      vsscanf.cxx
//
//      ANSI Stdio vsscanf() function
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-04-20
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include <cyg/io/devtab.h>          // Device table
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

#include <cyg/libc/stdio/io.inl>     // I/O system inlines

#ifndef CYGPKG_LIBC_STDIO_FILEIO

// FUNCTIONS

static Cyg_ErrNo
str_read(cyg_stdio_handle_t handle, void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *dev = (cyg_devtab_entry_t *)handle;
    cyg_uint8 *str_p = (cyg_uint8 *)dev->priv;
    cyg_ucount32 i;

    // we set str_p to NULL further down when the string has finished being
    // read
    if (str_p == NULL)
    {
        *len = 0;
        return ENOERR;
    } // if

    // I suspect most strings passed to sprintf will be relatively short,
    // so we just take the simple approach rather than have the overhead
    // of calling memcpy etc.

    // copy string until run out of user space, or we reach its end
    for (i = 0; i < *len ; ++i)
    {
        *((cyg_uint8 *)buf + i) = *str_p;

        if (*str_p++ == '\0')
        {
            str_p = NULL;
            ++i;
            break;
        } // if

    } // for

    *len = i;
    dev->priv = (void *)str_p;

    return ENOERR;
    
} // str_read()

static DEVIO_TABLE(devio_table,
                   NULL,            // write
                   str_read,        // read
                   NULL,            // select
                   NULL,            // get_config
                   NULL);           // set_config


externC int
vsscanf( const char *s, const char *format, va_list arg ) __THROW
{
    // construct a fake device with the address of the string we've
    // been passed as its private data. This way we can use the data
    // directly
    DEVTAB_ENTRY_NO_INIT(strdev,
                         "strdev",       // Name
                         NULL,           // Dependent name (layered device)
                         &devio_table,   // I/O function table
                         NULL,           // Init
                         NULL,           // Lookup
                         (void *)s);     // private
    Cyg_StdioStream my_stream( &strdev, Cyg_StdioStream::CYG_STREAM_READ,
                               false, false, _IONBF, 0, NULL );
    
    return vfscanf( (FILE *)&my_stream, format, arg );

} // vsscanf()

#else

externC int
vsscanf( const char *s, const char *format, va_list arg ) __THROW
{
    Cyg_StdioStream my_stream( Cyg_StdioStream::CYG_STREAM_READ,
                               strlen(s), (cyg_uint8 *)s );
    
    return vfscanf( (FILE *)&my_stream, format, arg );

} // vsscanf()

#endif

// EOF vsscanf.cxx
