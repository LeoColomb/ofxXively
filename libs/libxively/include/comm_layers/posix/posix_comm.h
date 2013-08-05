// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file 	posix_comm.h
 * \author 	Olgierd Humenczuk
 * \brief   Implements POSIX _communication layer_ functions [see comm_layer.h and posix_comm.c]
 */

#ifndef __POSIX_COMM_H__
#define __POSIX_COMM_H__

#include "connection.h"

connection_t* posix_open_connection( const char* address, int32_t port );

int posix_send_data( connection_t* conn, const char* data, size_t size );

int posix_read_data( connection_t* conn, char* buffer, size_t buffer_size );

void posix_close_connection( connection_t* conn );

#endif // __POSIX_COMM_H__
