// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
#include "comm_layer.h"
#include "posix_comm.h"

/**
 * \file 	posix_comm_layer.c
 * \author 	Olgierd Humenczuk
 * \brief   Implements POSIX _communication layer_ functions [see comm_layer.h]
 */

 /**
  * \brief   Initialise mbed implementation of the _communication layer_
  */
const comm_layer_t* get_comm_layer()
{
    static comm_layer_t __posix_comm_layer =
    {
          &posix_open_connection
        , &posix_send_data
        , &posix_read_data
        , &posix_close_connection
    };

    return &__posix_comm_layer;
}
