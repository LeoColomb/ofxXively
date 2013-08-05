// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_printf.c
 * \author  Olgierd Humenczuk
 * \brief   Our custom `printf()` hook [see xi_printf.h]
 */

#include <stdarg.h>
#include <stdio.h>

#include "xi_printf.h"
#include "xi_consts.h"

#ifdef __cplusplus
extern "C" {
#endif

user_print_t USER_PRINT = 0;

int xi_printf( const char *fmt, ... )
{
    char buffer[ XI_PRINTF_BUFFER_SIZE ];
    int n = 0;

    va_list ap;
    va_start( ap, fmt );
    vsnprintf( buffer, XI_PRINTF_BUFFER_SIZE, fmt, ap );
    va_end( ap );


    if( USER_PRINT )
    {
        USER_PRINT( buffer );
    }
    else
    {
        printf( "%s", buffer );
    }

    return n;
}

#ifdef __cplusplus
}
#endif
