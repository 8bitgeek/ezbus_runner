
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
#include <ezbus.h>
#include <ezbus_cmdline.h>
#include <ezbus_platform.h>
#include <ezbus_port.h>
#include <ezbus_runner.h>

ezbus_cmdline_t cmdline;
ezbus_port_t    port;
ezbus_t         ezbus;

int main(int argc,char* argv[])
{
    if ( ezbus_cmdline_setup( &cmdline, argc, argv ) >= 0 )
    {
        if ( ezbus_platform_setup( &cmdline ) >= 0 )
        {
            if ( ezbus_port_setup(&port) == 0 )
            {
                if ( ezbus_port_open( &port ) == EZBUS_ERR_OKAY )
                {
                    ezbus_init( &ezbus, &port );
                    return ezbus_runner( &ezbus );
                }
            }
        }
    }
    return -1;
}
