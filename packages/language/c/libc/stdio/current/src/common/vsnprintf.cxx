//===========================================================================
//
//      vsnprintf.cxx
//
//      ANSI Stdio vsnprintf() function
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
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
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

#include <cyg/libc/stdio/io.inl>     // I/O system inlines

// FUNCTIONS

class Cyg_VsnprintfStream: public Cyg_OutputStream
{
public:
    Cyg_VsnprintfStream(char* s): s_(s) {}

    virtual Cyg_ErrNo write( const cyg_uint8 *buffer,
        cyg_ucount32 buffer_length, cyg_ucount32 *bytes_written );

    virtual Cyg_ErrNo get_error( void ) { return ENOERR; }

private:
    char* s_;
};

Cyg_ErrNo
Cyg_VsnprintfStream::write(
    const cyg_uint8 *buffer,
    cyg_ucount32 buffer_length,
    cyg_ucount32 *bytes_written )
{
#ifdef WHEN_MEMCPY_OPTIMISED_IN_ECOS_EVENTUALLY
    memcpy(s_, buffer, buffer_length);
    s_ += buffer_length;
    *bytes_written = buffer_length;
#else
    char *dest = s_;
    char const *src = (char const *)buffer;
    char const *end = src + buffer_length;
    while(src < end)
        *dest++ = *src++;
    s_ = dest;
    *bytes_written = buffer_length;
#endif
    return ENOERR;
}

externC int
vsnprintf( char *s, size_t size, const char *format, va_list arg ) __THROW
{
    int ret;
    Cyg_VsnprintfStream stream(s);
    ret = vfnprintf( (FILE *)(void *)&stream, size, format, arg );
    /* If no error, and string not truncated, then apply null termination in
     * correct place
     */
    if ( (ret >= 0) && ((size_t)ret < size) )
        s[ret] = '\0';
#ifdef CYGIMP_LIBC_STDIO_C99_SNPRINTF
    /* C99 case. If no error, and string truncated, apply null termination
     * in correct place
     */
    if ( (ret >= 0) && ((size_t)ret >= size) && (size > 0) )
        s[size-1] = '\0';
#endif
    return ret;
} // vsnprintf()

// EOF vsnprintf.cxx
