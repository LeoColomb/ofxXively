// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    posix_comm.c
 * \author  Olgierd Humenczuk
 * \brief   Implements POSIX _communication layer_ abstraction interface [see comm_layer.h]
 */

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#include "posix_comm.h"
#include "comm_layer.h"
#include "xi_helpers.h"
#include "xi_allocator.h"
#include "posix_comm_layer_data_specific.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_globals.h"


connection_t* posix_open_connection( const char* address, int32_t port )
{
    // PRECONDITIONS
    assert( address != 0 );

    // variables
    posix_comm_layer_data_specific_t* pos_comm_data = 0;
    connection_t* conn                              = 0;

    // allocate memory for the posix data specific structure
    pos_comm_data
        = ( posix_comm_layer_data_specific_t* ) xi_alloc(
                sizeof( posix_comm_layer_data_specific_t ) );

    XI_CHECK_MEMORY( pos_comm_data );

    // allocate memory for the connection layer
    conn
        = ( connection_t* ) xi_alloc(
                sizeof( connection_t ) );

    XI_CHECK_MEMORY( conn );

    // make copy of an address
    conn->address = xi_str_dup( address );
    conn->port = port;

    XI_CHECK_MEMORY( conn->address );

    // initialze the fd for the TCP/IP socket
    pos_comm_data->socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if( pos_comm_data->socket_fd == -1 )
    {
        xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
        return 0;
    }

    // set the timout
    {
        struct timeval timeout;
        timeout.tv_sec  = xi_globals.network_timeout / 1000;
        timeout.tv_usec = ( xi_globals.network_timeout - timeout.tv_sec * 1000 ) * 1000;

        if ( setsockopt( pos_comm_data->socket_fd, SOL_SOCKET
                , SO_RCVTIMEO, ( char * )&timeout,
                  sizeof( timeout ) ) < 0 )
        {
            xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
            goto err_handling;
        }

        if ( setsockopt( pos_comm_data->socket_fd, SOL_SOCKET
                , SO_SNDTIMEO, ( char * )&timeout,
                  sizeof( timeout ) ) < 0 )
        {
            xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
            goto err_handling;
        }
    }


    // remember the layer specific part
    conn->layer_specific = ( void* ) pos_comm_data;

    // socket specific data
    struct sockaddr_in name;
    struct hostent* hostinfo;

    // get the hostaddress
    hostinfo = gethostbyname( conn->address );

    // if null it means that the address has not been founded
    if( hostinfo == NULL )
    {
        xi_set_err( XI_SOCKET_GETHOSTBYNAME_ERROR );
        goto err_handling;
    }

    // set the address and the port for further connection attempt
    memset( &name, 0, sizeof( struct sockaddr_in ) );
    name.sin_family = AF_INET;
    name.sin_addr = *( ( struct in_addr* ) hostinfo->h_addr );
    name.sin_port = htons( port );

    if( connect( pos_comm_data->socket_fd, ( struct sockaddr* ) &name, sizeof( struct sockaddr ) ) == -1 )
    {
        xi_set_err( XI_SOCKET_CONNECTION_ERROR );
        goto err_handling;
    }

    // POSTCONDITIONS
    assert( conn != 0 );
    assert( pos_comm_data->socket_fd != -1 );

    return conn;

err_handling:
    // cleanup the memory
    if( pos_comm_data ) { XI_SAFE_FREE( pos_comm_data ); }
    if( conn ) { XI_SAFE_FREE( conn->address ); }
    XI_SAFE_FREE( conn );

    return 0;
}

int posix_send_data( connection_t* conn, const char* data, size_t size )
{
    // PRECONDITIONS
    assert( conn != 0 );
    assert( conn->layer_specific != 0 );
    assert( data != 0 );
    assert( size != 0 );

    // extract the layer specific data
    posix_comm_layer_data_specific_t* pos_comm_data
        = ( posix_comm_layer_data_specific_t* ) conn->layer_specific;

    int bytes_written = write( pos_comm_data->socket_fd, data, size );

    if( bytes_written == - 1 )
    {
        xi_set_err( XI_SOCKET_WRITE_ERROR );
    }

    // store the value
    conn->bytes_sent += bytes_written;

    return bytes_written;
}

int posix_read_data( connection_t* conn, char* buffer, size_t buffer_size )
{
    // PRECONDITIONS
    assert( conn != 0 );
    assert( conn->layer_specific != 0 );
    assert( buffer != 0 );
    assert( buffer_size != 0 );

    // extract the layer specific data
    posix_comm_layer_data_specific_t* pos_comm_data
        = ( posix_comm_layer_data_specific_t* ) conn->layer_specific;

    memset( buffer, 0, buffer_size );
    int bytes_read = read( pos_comm_data->socket_fd, buffer, buffer_size );

    if( bytes_read == -1 )
    {
        xi_set_err( XI_SOCKET_READ_ERROR );
    }

    // store the value
    conn->bytes_received += bytes_read;

    return bytes_read;
}

void posix_close_connection( connection_t* conn )
{
    // PRECONDITIONS
    assert( conn != 0 );

    // extract the layer specific data
    posix_comm_layer_data_specific_t* pos_comm_data
        = ( posix_comm_layer_data_specific_t* ) conn->layer_specific;

    // shutdown the communication
    if( shutdown( pos_comm_data->socket_fd, SHUT_RDWR ) == -1 )
    {
        xi_set_err( XI_SOCKET_SHUTDOWN_ERROR );
        close( pos_comm_data->socket_fd ); // just in case
        goto err_handling;
    }

    // close the connection & the socket
    if( close( pos_comm_data->socket_fd ) == -1 )
    {
        xi_set_err( XI_SOCKET_CLOSE_ERROR );
        goto err_handling;
    }

err_handling:
    // cleanup the memory
    if( conn ) { XI_SAFE_FREE( conn->layer_specific ); }
    if( conn ) { XI_SAFE_FREE( conn->address ); }
    XI_SAFE_FREE( conn );

    return;
}
