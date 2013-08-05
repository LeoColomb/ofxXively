// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_printf.h
 * \author  Olgierd Humenczuk
 * \brief   Our custom `printf()` hook
 *
 *    This is needed on embedded devices, as there is no way to redirect
 *    the output neither it has any logging facilities out of the box.
 *    We currently use a function pointer (`user_print_t`) and and the
 *    user can assign to an external variable (`USER_PRINTF`) to call
 *    whatever device platform may have available.
 */

#ifndef __XI_PRINTF_H__
#define __XI_PRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *user_print_t )( const char* );

extern user_print_t USER_PRINT;

int xi_printf( const char *fmt, ... );

#ifdef __cplusplus
}
#endif

#endif //__XI_PRINTF_H__
