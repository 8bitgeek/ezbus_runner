/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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
// #include <termios.h>
#include <asm/termbits.h>
#include <unistd.h>
#include <ezbus_port.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ezbus_cmdline.h>
#include <ezbus_udp_broadcast.h>
#include <ezbus_udp_listen.h>
#include <ezbus_cmdline_unix.h>
#include <ezbus_platform.h>
#include <ezbus_crc32.h>

typedef struct
{
    char*                   serial_port_name;
    int                     fd;
    ezbus_udp_broadcast_t   udp_broadcast;
    ezbus_udp_listen_t      udp_listen;
} ezbus_platform_port_t;

static int                      ezbus_platform_open         ( ezbus_port_t* port );
static int                      ezbus_platform_send         ( ezbus_port_t* port, void* bytes, size_t size );
static int                      ezbus_platform_recv         ( ezbus_port_t* port, void* bytes, size_t size );
static void                     ezbus_platform_close        ( ezbus_port_t* port );
static void                     ezbus_platform_flush        ( ezbus_port_t* port );
static void                     ezbus_platform_drain        ( ezbus_port_t* port );
static int                      ezbus_platform_getch        ( ezbus_port_t* port );
static int                      ezbus_platform_set_speed    ( ezbus_port_t* port, uint32_t speed );
static uint32_t                 ezbus_platform_get_speed    ( ezbus_port_t* port );
static void                     ezbus_platform_set_address  ( ezbus_port_t* port, const ezbus_address_t* address );
static const ezbus_address_t*   ezbus_platform_get_address  ( ezbus_port_t* port );
static bool                     ezbus_platform_set_tx       ( ezbus_port_t* port, bool enable );

static void                     serial_set_blocking             (int fd, int should_block);
static ezbus_platform_port_t*   ezbus_platform_port_get_private (ezbus_port_t* ezbus_port);

extern int ezbus_port_setup(ezbus_port_t* port)
{
    ezbus_platform_port_t* ezbus_platform_port = (ezbus_platform_port_t*)ezbus_platform.callback_malloc(sizeof(ezbus_platform_port_t));
    
    ezbus_platform.callback_memset(port,0,sizeof(ezbus_port_t));
    ezbus_platform.callback_memset(ezbus_platform_port,0,sizeof(ezbus_platform_port_t));
    
    port->private = ezbus_platform_port;

    port->callback_open         = ezbus_platform_open;
    port->callback_send         = ezbus_platform_send;
    port->callback_recv         = ezbus_platform_recv;
    port->callback_close        = ezbus_platform_close;
    port->callback_flush        = ezbus_platform_flush;
    port->callback_drain        = ezbus_platform_drain;
    port->callback_getch        = ezbus_platform_getch;
    port->callback_set_speed    = ezbus_platform_set_speed;
    port->callback_get_speed    = ezbus_platform_get_speed;
    port->callback_set_address  = ezbus_platform_set_address;
    port->callback_get_address  = ezbus_platform_get_address;
    port->callback_set_tx       = ezbus_platform_set_tx;

    ezbus_platform_get_address(port);

    return 0;
}

extern void ezbus_port_dispose(ezbus_port_t* port)
{
    ezbus_platform_port_t* private = ezbus_platform_port_get_private(port);
    if ( private )
    {
        free( private );
    }
}

static int ezbus_platform_open(ezbus_port_t* port)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);
    ezbus_cmdline_t* cmdline = ezbus_platform_get_cmdline();
    
    if ( ezbus_platform_port_is_udp() )
    {
        ezbus_address_t address;
        address.word = ezbus_cmdline_unix_get_address(cmdline);
        port->callback_set_address(port,&address);

        if ( ezbus_udp_broadcast_setup( &private_port->udp_broadcast, 
                ezbus_cmdline_unix_get_host(cmdline), 
                ezbus_cmdline_unix_get_port(cmdline) ) >= 0 )
        {
            if ( ezbus_udp_listen_setup( &private_port->udp_listen,
                ezbus_cmdline_unix_get_host(cmdline), 
                ezbus_cmdline_unix_get_port(cmdline) ) >= 0 )
            {
                return 0;
            }
        }
    }
    else
    {
        if ( (private_port->fd = open(private_port->serial_port_name,O_RDWR)) >=0 )
        {
            struct serial_rs485 rs485conf = {0};

            rs485conf.flags |= SER_RS485_ENABLED;
            rs485conf.flags |= SER_RS485_RTS_ON_SEND;
            rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);

            ioctl (private_port->fd, TIOCSRS485, &rs485conf);
            ezbus_platform_set_speed(port,ezbus_cmdline_unix_get_serial_baud(cmdline));
            serial_set_blocking(private_port->fd,false);
            return 0;
        }
    }
    perror("ezbus_platform_open");
    return -1;
}

static ezbus_platform_port_t* ezbus_platform_port_get_private(ezbus_port_t* ezbus_port)
{
    ezbus_platform_port_t* private = (ezbus_platform_port_t*)ezbus_port->private;
    return private;
}

static int ezbus_platform_send(ezbus_port_t* port,void* bytes,size_t size)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);
 
     if ( ezbus_platform_port_is_udp() )
    {
        uint8_t* p = (uint8_t*)bytes;
        size_t sent=0;
        do {
            sent += ezbus_udp_broadcast_send(&private_port->udp_broadcast,p,size-sent);
            if ( sent > 0)
                p += sent;
        } while (sent<size&&sent>=0);
        ezbus_platform_flush( port );
        return sent;
    }
    else
    {
        uint8_t* p = (uint8_t*)bytes;
        size_t sent=0;
        do {
            sent += write(private_port->fd,p,size-sent);
            if ( sent > 0)
                p += sent;
        } while (sent<size&&sent>=0);
        ezbus_platform_flush( port );
        return sent;
    }
}

static int ezbus_platform_recv(ezbus_port_t* port,void* bytes,size_t size)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);

    if ( ezbus_platform_port_is_udp() )
    {
        int rc = ezbus_udp_listen_recv(&private_port->udp_listen,bytes,size);
        return rc;
    }
    else
    {
        int rc = read(private_port->fd,bytes,size);
        return rc;
    }
}

static int ezbus_platform_getch(ezbus_port_t* port)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);

    if ( ezbus_platform_port_is_udp() )
    {
        uint8_t ch;
        int rc = ezbus_udp_listen_recv(&private_port->udp_listen,&ch,1);
        if ( rc == 1 )
        {
            #if 1
                if ( ch == 0x55 )
                    fputc('\n',stderr);
                fprintf( stderr, "%02X ", ch );
            #endif
            return (int)ch;
        }
        return -1;
    }
    else
    {
        uint8_t ch;
        int rc = read(private_port->fd,&ch,1);
        if ( rc == 1 )
        {
            return (int)ch;
        }
        return -1;
    }
}

static void ezbus_platform_close(ezbus_port_t* port)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);

    if ( ezbus_platform_port_is_udp() )
    {
        ezbus_udp_listen_close(&private_port->udp_listen);
        ezbus_udp_broadcast_close(&private_port->udp_broadcast);
    }
    else
    {
        close(private_port->fd);
    }
}

static void ezbus_platform_drain(ezbus_port_t* port)
{
    while(ezbus_platform_getch(port)>=0);
}

static int ezbus_platform_set_speed(ezbus_port_t* port,uint32_t baud)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);

    if ( !ezbus_platform_port_is_udp() )
    {
        struct termios2 options;

        if ( private_port->fd >= 0 )
        {
            fcntl(private_port->fd, F_SETFL, 0);

            ioctl(private_port->fd, TCGETS2, &options);

            options.c_cflag &= ~CBAUD;
            options.c_cflag |= BOTHER;
            options.c_ispeed = baud;
            options.c_ospeed = baud;
            
            options.c_cflag |= (CLOCAL | CREAD);
            options.c_cflag &= ~CRTSCTS;
            options.c_cflag &= ~HUPCL;

            options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
            options.c_oflag &= ~OPOST;
            options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            options.c_cflag &= ~(CSIZE | PARENB);
            options.c_cflag |= CS8;

            ioctl(private_port->fd, TCSETS2, &options);

            return 0;
        }
    }
    return -1;
}

static uint32_t ezbus_platform_get_speed( ezbus_port_t* port )
{
    ezbus_cmdline_t* cmdline = ezbus_platform_get_cmdline();
    return ezbus_cmdline_unix_get_serial_baud(cmdline);
}

static void ezbus_platform_set_address ( ezbus_port_t* port, const ezbus_address_t* address )
{
    ezbus_address_copy(&port->self_address,address);
}

static const ezbus_address_t* ezbus_platform_get_address( ezbus_port_t* port )
{
    if ( port->self_address.word == 0 )
    {
        uint32_t words[3];
        words[0] = ezbus_platform.callback_rand();
        words[1] = ezbus_platform.callback_rand();
        words[2] = ezbus_platform.callback_rand();
        port->self_address.word = ezbus_crc32(words,3*sizeof(uint32_t));
    }
    return &port->self_address;
}

static void ezbus_platform_flush(ezbus_port_t* port)
{
    ezbus_platform_port_t* private_port = ezbus_platform_port_get_private(port);

    if ( !ezbus_platform_port_is_udp() )
    {
        fsync(private_port->fd);
    }
}

static void serial_set_blocking (int fd, int should_block)
{
    struct termios2 options;

    if ( fd >= 0 )
    {
        fcntl(fd, F_SETFL, 0);

        ioctl(fd, TCGETS2, &options);

        if ( should_block )
        {
            options.c_cc[VMIN]  = 1;
            options.c_cc[VTIME] = 1;
        }
        else
        {
            options.c_cc[VMIN]  = 0;
            options.c_cc[VTIME] = 0;
        }

        ioctl(fd, TCSETS2, &options);
    }
}

static bool ezbus_platform_set_tx( ezbus_port_t* port, bool enable )
{
    /* FIXME - do something here? */
    return enable;
}
