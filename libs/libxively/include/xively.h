// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xively.h
 * \brief   Xively C library
 */

#ifndef __XI_H__
#define __XI_H__

#include <stdlib.h>
#include <stdint.h>

#include <time.h>

#include "comm_layer.h"
#include "xi_consts.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------
// TYPES AND STRUCTURES
//-----------------------------------------------------------------------

/**
 * \brief    The protocols currently supported by Xively
 * \note     See source code for details of what's implemented.
 */
typedef enum {
    /** `http://api.xively.com` */
    XI_HTTP,
    /** `https://api.xively.com` */
    XI_HTTPS,
    /** `telnet api.xively.com 8081` */
    XI_TCP,
    /** `openssl s_client -host api.xively.com -port 8091 -tls1` */
    XI_TCPS,
    /** `ws://api.xively.com:8080` */
    XI_WS,
    /** `wss://api.xively.com:8090` */
    XI_WSS,
} xi_protocol_t;

/**
 * \brief   _The context structure_ - it's the first agument for all functions
 *          that communicate with Xively API (_i.e. not helpers or utilities_)
 */
typedef struct {
    char *api_key; /** Xively API key */
    xi_protocol_t protocol; /** Xively protocol */
    int32_t feed_id; /** Xively feed ID */
} xi_context_t;

/**
 * \brief HTTP headers
 */
typedef enum
{
    /** `Date` */
    XI_HTTP_HEADER_DATE = 0,
    /** `Content-Type` */
    XI_HTTP_HEADER_CONTENT_TYPE,
    /** `Content-Length` */
    XI_HTTP_HEADER_CONTENT_LENGTH,
    /** `Connection` */
    XI_HTTP_HEADER_CONNECTION,
    /** `X-Request-Id` */
    XI_HTTP_HEADER_X_REQUEST_ID,
    /** `Cache-Control` */
    XI_HTTP_HEADER_CACHE_CONTROL,
    /** `Vary` */
    XI_HTTP_HEADER_VARY,
    /** `Count` */
    XI_HTTP_HEADER_COUNT,
    /** `Age` */
    XI_HTTP_HEADER_AGE,
    // must go before the last here
    XI_HTTP_HEADER_UNKNOWN,
    // must be the last here
    XI_HTTP_HEADERS_COUNT
} http_header_type_t;

/** Datapoint value types */
typedef enum
{   /** 32-bit signed integer */
    XI_VALUE_TYPE_I32 = 0,
    /** 32-bit floating point number */
    XI_VALUE_TYPE_F32,
    /** any string-econded data */
    XI_VALUE_TYPE_STR,
    XI_VALUE_TYPE_COUNT
} xi_value_type_t;

typedef struct {
    http_header_type_t  header_type;
    char                name[ XI_HTTP_HEADER_NAME_MAX_SIZE ];
    char                value[ XI_HTTP_HEADER_VALUE_MAX_SIZE ];
} http_header_t;

typedef struct {
    int             http_version1;
    int             http_version2;
    int             http_status;
    char            http_status_string[ XI_HTTP_STATUS_STRING_SIZE ];
    http_header_t*  http_headers_checklist[ XI_HTTP_HEADERS_COUNT ];
    http_header_t   http_headers[ XI_HTTP_MAX_HEADERS ];
    size_t          http_headers_size;
    char            http_content[ XI_HTTP_MAX_CONTENT_SIZE ];
} http_response_t;

/**
 * \brief   _The response structure_ - it's the return type for all functions
 *          that communicate with Xively API (_i.e. not helpers or utilities_)
 */
typedef struct {
    http_response_t http;
} xi_response_t;

/**
 * \brief   The datapoint value union
 */
typedef union {
    int32_t i32_value;
    float   f32_value;
    char    str_value[ XI_VALUE_STRING_MAX_SIZE ];
} xi_datapoint_value_t;

/**
 * \brief   The datapoint timestamp
 */
typedef struct {
    time_t timestamp;
    time_t micro;
} xi_timestamp_t;

/**
 * \brief   _Xively datapoint structure_ - it contains value and timestamp
 * \note    A zero-valued timestamp is used by most functions as a convention
 *          to opt for server-side timestamps.
 */
typedef struct {
    xi_datapoint_value_t  value;
    xi_value_type_t       value_type;
    xi_timestamp_t        timestamp;
} xi_datapoint_t;

typedef struct {
    char              datastream_id[ XI_MAX_DATASTREAM_NAME ];
    size_t            datapoint_count;
    xi_datapoint_t    datapoints[ XI_MAX_DATAPOINTS ];
} xi_datastream_t;

/**
 * \brief   _Xively feed structure_ - it contains a fixed array of datastream
 * \note    The implementation is such that user will need to know in advance
 *          how many datastreams there can be, which should be sufficent for
 *          a real-world application. It's also undesired to have some devices
 *          create dozens of datastreams due to a bug.
 */
typedef struct {
    int32_t           feed_id;
    size_t            datastream_count;
    xi_datastream_t   datastreams[ XI_MAX_DATASTREAMS ];
} xi_feed_t;

//-----------------------------------------------------------------------
// HELPER FUNCTIONS
//-----------------------------------------------------------------------

/**
 * \brief   Sets the xi_datapoint_t value field to `int32_t` value
 *
 * \return  Pointer or `0` if an error occurred.
 */
extern xi_datapoint_t* xi_set_value_i32( xi_datapoint_t* dp, int32_t v );

/**
 * \brief   Sets the `xi_datapoint_t` value field to `float` value
 * \return  Pointer or `0` if an error occurred.
 */
extern xi_datapoint_t* xi_set_value_f32( xi_datapoint_t* dp, float v );

/**
 * \brief   Sets the `xi_datapoint_t` value field to zero-terminated string value
 *
 * \return  Pointer or `0` if an error occurred.
 */
extern xi_datapoint_t* xi_set_value_str( xi_datapoint_t* dp, const char* v );

/**
 * \brief   Sets the timeout for network operations
 *
 * \note    The timeout is used by the comunication layer
 *          to determine whenever it should treat the lag
 *          in a connection as an error, so if your device
 *          or your connection is slow, you can try to increase
 *          the timeout for network operations. It only affects the
 *          send/recv operations it does not work with connect but that
 *          behaviour may differ between platforms and communication
 *          layer imlementations.
 */
extern void xi_set_network_timeout( uint32_t milliseconds );

/**
 * \brief   Gets the current network timeout
 */
extern uint32_t xi_get_network_timeout( void );

//-----------------------------------------------------------------------
// MAIN LIBRARY FUNCTIONS
//-----------------------------------------------------------------------

/**
 * \brief   Library context constructor
 *
 *   The purpose of this function is to allocate memory and initialise the
 *   data structures needed in order to use any other library functions.
 *
 * \return  Initialised context structure or `0` if an error occurred
 */
extern xi_context_t* xi_create_context(
          xi_protocol_t protocol, const char* api_key
        , int32_t feed_id );

/**
 * \brief   Library context destructor
 *
 *   The purpose of this fucntion is to free all allocated resources
 *   when the application is intending to terminate or stop using the library.
 */
extern void xi_delete_context( xi_context_t* context );


/**
 * \brief   Update Xively feed
 */
extern const xi_response_t* xi_feed_update(
          xi_context_t* xi
        , const xi_feed_t* value );

/**
 * \brief   Retrieve Xively feed
 */
extern const xi_response_t* xi_feed_get(
          xi_context_t* xi
        , xi_feed_t* value );

/**
 * \brief   Create a datastream with given value using server timestamp
 */
extern const xi_response_t* xi_datastream_create(
          xi_context_t* xi, int32_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* value);

/**
 * \brief   Update a datastream with given datapoint using server or local timestamp
 */
extern const xi_response_t* xi_datastream_update(
          xi_context_t* xi, int32_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* value );

/**
 * \brief   Retrieve latest datapoint from a given datastream
 */
extern const xi_response_t* xi_datastream_get(
          xi_context_t* xi, int32_t feed_id
        , const char * datastream_id, xi_datapoint_t* dp );

/**
 * \brief   Delete datastream
 * \warning This function destroys the data in Xively and there is no way to restore it!
 */
extern const xi_response_t* xi_datastream_delete(
          xi_context_t* xi, int feed_id
          , const char* datastream_id );

/**
 * \brief   Delete datapoint at a given timestamp
 * \warning This function destroys the data in Xively and there is no way to restore it!
 * \note    You need to provide exact timestamp value to guarantee successful response
 *          from the API, i.e. it will respond with error 404 if datapoint didn't exist.
 *          If you need to determine the exact timestamp, it may be easier to call
 *          `xi_datapoint_delete_range()` with short range instead.
 */
extern const xi_response_t* xi_datapoint_delete(
          const xi_context_t* xi, int feed_id
        , const char * datastream_id
        , const xi_datapoint_t* dp );

/**
 * \brief   Delete all datapoints in given time range
 * \warning This function destroys the data in Xively and there is no way to restore it!
 */
extern const xi_response_t* xi_datapoint_delete_range(
          const xi_context_t* xi, int feed_id, const char * datastream_id
        , const xi_timestamp_t* start, const xi_timestamp_t* end );

#ifdef __cplusplus
}
#endif

#endif // __XI_H__
