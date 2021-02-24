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
#include <ezbus_setup.h>
#include <ezbus_port.h>
#include <ezbus_mac_pause.h>

extern void ezbus_callback_setup( ezbus_udp_cmdline_t* ezbus_udp_cmdline, ezbus_port_t* port )
{
    ezbus_port_init_struct( port );

    ezbus_platform_port_set_name(port,NULL);
    ezbus_platform_port_set_handle(port,0);

    port->platform_port.udp_cmdline = ezbus_udp_cmdline;
}

extern bool ezbus_platform_set_tx( ezbus_platform_port_t* port, bool enable )
{
    // if ( enable )
    // {
    //     caribou_gpio_set( ezbus_platform_port_get_dir_gpio_tx(port) );
    // }
    // else
    // {
    //     ezbus_platform_drain(port);
    //     caribou_gpio_reset( ezbus_platform_port_get_dir_gpio_tx(port) );
    // }
    return enable;
}

extern bool ezbus_pause_callback( ezbus_mac_t* mac )
{
    switch( ezbus_mac_pause_get_state( mac ) )
    {
        case ezbus_pause_state_start:
            break;

        case ezbus_pause_state_half_duration_timeout:  
            break;
        
        case ezbus_pause_state_duration_timeout:
            break;
        
        case ezbus_pause_state_finish:
            break;

        default:
            break;
    }
    return true;
}
