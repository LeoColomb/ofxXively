// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file	xi_allocator.c
 * \author  Olgierd Humenczuk
 * \brief   Our custom `alloc()` and `free()` [see xi_allocator.h]
 */

#include <stdlib.h>
#include "xi_allocator.h"

void* xi_alloc( size_t b )
{
    return ( void* ) malloc( b );
}

void xi_free( void* p )
{
    free( p );
}
