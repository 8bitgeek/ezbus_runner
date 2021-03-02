
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
#include <ezbus_port.h>
#include <ezbus_platform.h>

typedef struct main
{
    ezbus_t* ezbus;
    ezbus_ms_tick_t timer_start;
} feature_state_t;

static feature_state_t feature_state;


extern int ezbus_runner(ezbus_t* ezbus)
{    
    memset( &feature_state, 0, sizeof(feature_state_t) );

    feature_state.ezbus = ezbus;
    feature_state.timer_start = ezbus_platform.callback_get_ms_ticks();
    
    fprintf( stderr, "self %s", ezbus_address_string(ezbus_port_get_address(ezbus_port(ezbus) ) ) );

    for(;;)
    {
        ezbus_run(feature_state.ezbus);
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
