
/*****************************************************************************
* Copyright © 2021 Mike Sharkey <mike@8bitgeek.net>                          *
*                                                                            *
* Permission is hereby granted, free of charge, to any person obtaining a    *
* copy of this software and associated documentation files (the "Software"), *
* to deal in the Software without restriction, including without limitation  *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
* and/or sell copies of the Software, and to permit persons to whom the      *
* Software is furnished to do so, subject to the following conditions:       *
*                                                                            *
* The above copyright notice and this permission notice shall be included in *
* all copies or substantial portions of the Software.                        *
*                                                                            *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
* DEALINGS IN THE SOFTWARE.                                                  *
*****************************************************************************/
#include <ezbus_runner.h>
#include <ezbus.h>
#include <ezbus_flip.h>
#include <ezbus_port.h>
#include <ezbus_setup.h>
#include <syslog.h>
#include <syslog_printf.h>

typedef struct main
{
    ezbus_port_t    port;
    ezbus_t         ezbus;
    syslog_t        syslog;
    ezbus_ms_tick_t timer_start;
} feature_state_t;

static feature_state_t feature_state;


extern int ezbus_runner(ezbus_udp_cmdline_t* ezbus_udp_cmdline)
{
    memset( &feature_state, 0, sizeof(feature_state_t) );
    
    feature_state.timer_start = ezbus_platform_get_ms_ticks();

    syslog_init( &feature_state.syslog, stderr, syslog_fputc );

    ezbus_callback_setup( ezbus_udp_cmdline, &feature_state.port );

    if ( ezbus_port_open( &feature_state.port, EZBUS_SPEED_DEF ) == EZBUS_ERR_OKAY )
    {
        SYSLOG_PRINTF( &feature_state.syslog, SYSLOG_DEBUG, "ezbus init" );
        ezbus_init( &feature_state.ezbus, &feature_state.port );
        SYSLOG_PRINTF( &feature_state.syslog, SYSLOG_DEBUG, "self %s", ezbus_address_string(&ezbus_self_address) );

        for(;;) /* forever... */
        {
            ezbus_run(&feature_state.ezbus);
            if ( (ezbus_platform_get_ms_ticks() - feature_state.timer_start) > 125)
            {
                fputc('*',stderr);
                feature_state.timer_start = ezbus_platform_get_ms_ticks();
            }
        }
    }
    else
    {
        SYSLOG_PRINTF( &feature_state.syslog, SYSLOG_DEBUG, "ezbus open failed" );
    }
    return -1;
}

extern bool ezbus_socket_callback_send ( ezbus_socket_t socket )
{
    return false;
}

extern bool ezbus_socket_callback_recv ( ezbus_socket_t socket )
{
    return true;
}

extern void ezbus_socket_callback_closing ( ezbus_socket_t socket )
{
}
