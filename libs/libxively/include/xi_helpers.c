// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

// This file also containes code from MINIX C library, which is under MINIX license
// Copyright (c) 1987, 1997, 2006, Vrije Universiteit, Amsterdam, The Netherlands

/**
 * \file    xi_helpers.c
 * \author  Olgierd Humenczuk
 * \brief   General helpers used by the library [see xi_helpers.h]
 */

#include <string.h>
#include <assert.h>

#include "xi_helpers.h"
#include "xi_allocator.h"

char* xi_str_dup( const char* s )
{
    // PRECONDITIONS
    assert( s != 0 );

    size_t len = strlen( s );
    char * ret = xi_alloc( len + 1 );
    if( ret == 0 ) { return 0; }
    memcpy( ret, s, len + 1 );
    return ret;
}

int xi_str_copy_untiln( char* dst, size_t dst_size, const char* src, char delim )
{
    // PRECONDITIONS
    assert( dst != 0 );
    assert( dst_size > 1 );
    assert( src != 0 );

    size_t counter = 0;
    size_t real_size = dst_size - 1;

    while( *src != delim && counter < real_size && *src != '\0' )
    {
        *dst++ = *src++;
        counter++;
    }

    *dst = '\0';
    return counter;
}

// used by the xi_mktime and xi_gmtime
#define YEAR0               1900  /* the first year */
#define EPOCH_YR            1970  /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY            (24L * 60L * 60L)
#define LEAPYEAR(year)      (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)      (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)    (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)    (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX            ULONG_MAX
#define ABB_LEN             3

// used by the xi_mktime and xi_gmtime
static const int _ytab[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
      { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

time_t xi_mktime(register struct tm *timep)
{
    register long day, year;
    register int tm_year;
    int yday, month;
    register signed long seconds;
    int overflow;

    timep->tm_min += timep->tm_sec / 60;
    timep->tm_sec %= 60;
    if (timep->tm_sec < 0) {
         timep->tm_sec += 60;
         timep->tm_min--;
    }
    timep->tm_hour += timep->tm_min / 60;
    timep->tm_min = timep->tm_min % 60;
    if (timep->tm_min < 0) {
         timep->tm_min += 60;
         timep->tm_hour--;
    }
    day = timep->tm_hour / 24;
    timep->tm_hour= timep->tm_hour % 24;
    if (timep->tm_hour < 0) {
         timep->tm_hour += 24;
         day--;
    }
    timep->tm_year += timep->tm_mon / 12;
    timep->tm_mon %= 12;
    if (timep->tm_mon < 0) {
         timep->tm_mon += 12;
         timep->tm_year--;
    }
    day += (timep->tm_mday - 1);
    while (day < 0) {
         if(--timep->tm_mon < 0) {
                 timep->tm_year--;
                 timep->tm_mon = 11;
         }
         day += _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon];
    }
    while (day >= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon]) {
         day -= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon];
         if (++(timep->tm_mon) == 12) {
                 timep->tm_mon = 0;
                 timep->tm_year++;
         }
    }
    timep->tm_mday = day + 1;
    year = EPOCH_YR;
    if (timep->tm_year < year - YEAR0) return (time_t)-1;
    seconds = 0;
    day = 0;
    overflow = 0;

    /*
     * Assume that when day becomes negative, there will certainly be overflow on seconds.
     * The check for overflow needs not to be done for leapyears divisible by 400.
     * The code only works when year (1970) is not a leapyear.
     */
    #if EPOCH_YR != 1970
    #error EPOCH_YR != 1970
    #endif

    tm_year = timep->tm_year + YEAR0;

    if (LONG_MAX / 365 < tm_year - year) overflow++;
    day = (tm_year - year) * 365;
    if (LONG_MAX - day < (tm_year - year) / 4 + 1) overflow++;
    day += (tm_year - year) / 4
         + ((tm_year % 4) && tm_year % 4 < year % 4);
    day -= (tm_year - year) / 100
         + ((tm_year % 100) && tm_year % 100 < year % 100);
    day += (tm_year - year) / 400
         + ((tm_year % 400) && tm_year % 400 < year % 400);

    yday = month = 0;
    while (month < timep->tm_mon) {
         yday += _ytab[LEAPYEAR(tm_year)][month];
         month++;
    }
    yday += (timep->tm_mday - 1);
    if (day + yday < 0) overflow++;
    day += yday;

    timep->tm_yday = yday;
    timep->tm_wday = (day + 4) % 7;

    seconds = ( ( timep->tm_hour * 60L ) + timep->tm_min ) * 60L + timep->tm_sec;

    if ( ( TIME_MAX - seconds ) / SECS_DAY < ( unsigned long ) day ) overflow++;
    seconds += day * SECS_DAY;

    if ( overflow ) return ( time_t ) - 1;

    if ( ( time_t ) seconds != seconds) return ( time_t ) - 1;
    return ( time_t ) seconds;
}

struct tm* xi_gmtime( register const time_t *timer )
{
    static struct tm br_time;
    register struct tm *timep = &br_time;
    time_t time = *timer;
    register unsigned long dayclock, dayno;
    int year = EPOCH_YR;

    dayclock = (unsigned long)time % SECS_DAY;
    dayno = (unsigned long)time / SECS_DAY;

    timep->tm_sec = dayclock % 60;
    timep->tm_min = (dayclock % 3600) / 60;
    timep->tm_hour = dayclock / 3600;
    timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */

    while (dayno >= YEARSIZE(year)) {
            dayno -= YEARSIZE(year);
            year++;
    }

    timep->tm_year = year - YEAR0;
    timep->tm_yday = dayno;
    timep->tm_mon = 0;

    while (dayno >= _ytab[LEAPYEAR(year)][timep->tm_mon]) {
            dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
            timep->tm_mon++;
    }

    timep->tm_mday = dayno + 1;
    timep->tm_isdst = 0;

    return timep;
}

char* xi_replace_with(
      char p, char r
    , char* buffer
    , size_t max_chars )
{
    char* c = buffer;

    while( *c != '\0' && max_chars-- )
    {
        if( *c == p ) { *c = r; }
        c++;
    }

    return c;
}
