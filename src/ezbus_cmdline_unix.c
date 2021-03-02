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
// #include <ezbus_const.h>
#include <ezbus_cmdline_unix.h>
#include <ezbus_platform.h>
#include <getopt.h>
#include <stdlib.h>

#if !defined(EZBUS_SPEED_DEF)
    #define EZBUS_SPEED_DEF             1152000
#endif

#if !defined(EZBUS_UDP_ADDRESS)
    #define EZBUS_UDP_ADDRESS           "224.0.0.1"
#endif

#if !defined(EZBUS_UDP_PORT)
    #define EZBUS_UDP_PORT              8260
#endif

extern char *optarg;
extern int optind, opterr, optopt;

typedef struct _ezbus_cmline_unix
{
    const char*     serial_device;
    int             serial_baud;
    const char*     host;
    int             port;
    uint32_t        address;
} ezbus_cmdline_unix_t;

static ezbus_cmdline_unix_t     cmdline_private;
static ezbus_cmdline_unix_t*    ezbus_cmdline_get_private(ezbus_cmdline_t* cmdline);

extern int ezbus_cmdline_setup(ezbus_cmdline_t* cmdline, int argc, char* argv[])
{
    memset(cmdline,0,sizeof(ezbus_cmdline_t));
    memset(&cmdline_private,0,sizeof(ezbus_cmdline_unix_t)); 

    ezbus_platform.cmdline = cmdline;
    cmdline->private = &cmdline_private;

    cmdline->argv = argv;
    cmdline->argc = argc;

    cmdline_private.serial_baud   = EZBUS_SPEED_DEF;
    cmdline_private.host          = EZBUS_UDP_ADDRESS;
    cmdline_private.port          = EZBUS_UDP_PORT;

    while ( (ezbus_cmdline_set_opt( 
                                    cmdline,
                                    getopt(
                                            ezbus_cmdline_get_argc(cmdline), 
                                            ezbus_cmdline_get_argv(cmdline), 
                                            "h:p:a:s:b:?")
                                    ) ) >= 0 )
    {
        switch (ezbus_cmdline_get_opt(cmdline)) 
        {
            case 'h':
                cmdline_private.host = optarg;
                break;
            case 'p':
                cmdline_private.port = atoi(optarg);
                break;
            case 'a':
                cmdline_private.address = (uint32_t)atoi(optarg);
                break;
            case 's':
                cmdline_private.serial_device = optarg;
                break;
            case 'b':
                cmdline_private.serial_baud = atoi(optarg);
                break;
            case '?':
            default: /* '?' */
                fprintf(stderr, "Usage: %s "
                                " [-h host-bind-address]"
                                " [-p multicast-udp-port]"
                                " [-a ezbus-address]"
                                " [-s rs485-serial device]"
                                " [-b rs485-serial-baud]\n", ezbus_cmdline_get_argv(cmdline)[0]);
                return -1;
        }
    }

    return 0;
}

static ezbus_cmdline_unix_t* ezbus_cmdline_get_private(ezbus_cmdline_t* ezbus_cmdline)
{
    ezbus_cmdline_unix_t* private = (ezbus_cmdline_unix_t*)ezbus_cmdline->private;
    return private;
}

extern const char* ezbus_cmdline_unix_get_serial_device(ezbus_cmdline_t* cmdline)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    return private->serial_device;
}

extern int ezbus_cmdline_unix_get_serial_baud(ezbus_cmdline_t* cmdline)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    return private->serial_baud;
}

extern const char* ezbus_cmdline_unix_get_host(ezbus_cmdline_t* cmdline)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    return private->host;
}

extern int ezbus_cmdline_unix_get_port(ezbus_cmdline_t* cmdline)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    return private->port;
}

extern uint32_t ezbus_cmdline_unix_get_address(ezbus_cmdline_t* cmdline)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    return private->address;
}

extern void ezbus_cmdline_unix_set_address(ezbus_cmdline_t* cmdline,uint32_t address)
{
    ezbus_cmdline_unix_t* private = ezbus_cmdline_get_private(cmdline);
    private->address = address;
}
