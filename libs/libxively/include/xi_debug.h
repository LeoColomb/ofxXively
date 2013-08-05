// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_debug.h
 * \author  Olgierd Humenczuk
 * \brief   Macros to use for debugging (relies on `xi_printf()`)
 */

#ifndef __XI_DEBUG_H__
#define __XI_DEBUG_H__

#include "xi_printf.h"

#ifdef XI_DEBUG_OUTPUT
    #define xi_debug_log_str(...) xi_printf( "[%d@%s] - %s", __LINE__, __FILE__, __VA_ARGS__ )
    #define xi_debug_log_data(...) xi_printf( "%s", __VA_ARGS__ )
    #define xi_debug_log_int(...) xi_printf( "%d", __VA_ARGS__ )
    #define xi_debug_log_endl(...) xi_printf( "\n" )
#else
    #define xi_debug_log_str(...)
    #define xi_debug_log_data(...)
    #define xi_debug_log_int(...)
    #define xi_debug_log_endl(...)
#endif

#endif
