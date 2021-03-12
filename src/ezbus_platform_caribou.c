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
#include <ezbus_platform.h>
#include <ezbus_address.h>
#include <caribou/lib/stdio.h>
#include <caribou/dev/uart.h>
#include <caribou/lib/heap.h>
#include <caribou/lib/rand.h>
#include <caribou/lib/uuid.h>
#if defined(EZBUS_USE_FLOW_CALLBACK) && !defined(EZBUS_USE_DEFAULT_FLOW_CALLBACK)
    #include <ezbus_callback.h>
#endif

ezbus_platform_t ezbus_platform;

static void*    ezbus_platform_memset      ( void* dest, int c, size_t n );
static void*    ezbus_platform_memcpy      ( void* dest, const void *src, size_t n );
static void*    ezbus_platform_memmove     ( void* dest, const void *src, size_t n );
static int      ezbus_platform_memcmp      ( const void* dest, const void *src, size_t n );
static char*    ezbus_platform_strcpy      ( char* dest, const char *src );
static char*    ezbus_platform_strcat      ( char* dest, const char *src );
static char*    ezbus_platform_strncpy     ( char* dest, const char *src, size_t n );
static int      ezbus_platform_strcmp      ( const char* s1, const char *s2 );
static int      ezbus_platform_strcasecmp  ( const char* s1, const char *s2 );
static size_t   ezbus_platform_strlen      ( const char* s);
static void*    ezbus_platform_malloc      ( size_t n );
static void*    ezbus_platform_realloc     ( void* src, size_t n );
static void     ezbus_platform_free        ( void *src );

static int      ezbus_platform_rand        ( void );
static void     ezbus_platform_srand       ( unsigned int seed );
static int      ezbus_platform_random      ( int lower, int upper );
static void     ezbus_platform_rand_init   ( void );
static void     ezbus_platform_delay       ( unsigned int ms );
static void     ezbus_platform_set_address ( const ezbus_address_t* address );
static void     ezbus_platform_address     ( ezbus_address_t* address );

static ezbus_ms_tick_t  ezbus_platform_get_ms_ticks();

extern int ezbus_platform_setup(void* cmdline_obj)
{
    ezbus_cmdline_t* cmdline = (ezbus_cmdline_t*)cmdline_obj;

    memset(&ezbus_platform,0,sizeof(ezbus_platform_t));

    ezbus_platform.cmdline = cmdline;

    ezbus_platform.callback_memset       = ezbus_platform_memset;
    ezbus_platform.callback_memcpy       = ezbus_platform_memcpy;
    ezbus_platform.callback_memmove      = ezbus_platform_memmove;
    ezbus_platform.callback_memcmp       = ezbus_platform_memcmp;
    ezbus_platform.callback_strcpy       = ezbus_platform_strcpy;
    ezbus_platform.callback_strcat       = ezbus_platform_strcat;
    ezbus_platform.callback_strncpy      = ezbus_platform_strncpy;
    ezbus_platform.callback_strcmp       = ezbus_platform_strcmp;
    ezbus_platform.callback_strcasecmp   = ezbus_platform_strcasecmp;
    ezbus_platform.callback_strlen       = ezbus_platform_strlen;
    ezbus_platform.callback_malloc       = ezbus_platform_malloc;
    ezbus_platform.callback_realloc      = ezbus_platform_realloc;
    ezbus_platform.callback_free         = ezbus_platform_free;
    ezbus_platform.callback_rand         = ezbus_platform_rand;
    ezbus_platform.callback_srand        = ezbus_platform_srand;
    ezbus_platform.callback_random       = ezbus_platform_random;
    ezbus_platform.callback_rand_init    = ezbus_platform_rand_init;
    ezbus_platform.callback_delay        = ezbus_platform_delay;
    ezbus_platform.callback_get_ms_ticks = ezbus_platform_get_ms_ticks;

    ezbus_platform_rand_init();

    return 0;
}

void* ezbus_platform_memset(void* dest, int c, size_t n)
{
    return memset(dest,c,n);
}

void* ezbus_platform_memcpy(void* dest, const void *src, size_t n)
{
    return memcpy(dest,src,n);
}

void* ezbus_platform_memmove(void* dest, const void *src, size_t n)
{
    return memmove(dest,src,n);
}

int ezbus_platform_memcmp(const void* dest, const void *src, size_t n)
{
    return memcmp(dest,src,n);
}

char* ezbus_platform_strcpy( char* dest, const char *src )
{
    return strcpy( dest, src );
}

char* ezbus_platform_strcat( char* dest, const char *src )
{
    return strcat( dest, src );
}

char* ezbus_platform_strncpy( char* dest, const char *src, size_t n )
{
    return strncpy( dest, src, n );
}

size_t ezbus_platform_strlen( const char* s)
{
    return strlen( s );
}

extern int ezbus_platform_strcmp( const char* s1, const char *s2 )
{
    return strcmp(s1,s2);
}

extern int ezbus_platform_strcasecmp( const char* s1, const char *s2 )
{
    return strcasecmp(s1,s2);
}

void* ezbus_platform_malloc( size_t n)
{
    return malloc(n);
}

void* ezbus_platform_realloc(void* src,size_t n)
{
    return realloc(src,n);
}

void ezbus_platform_free(void *src)
{
    free(src);
}

ezbus_ms_tick_t ezbus_platform_get_ms_ticks()
{
    ezbus_ms_tick_t ticks;
    struct timeval tm;
    gettimeofday(&tm,NULL);
    ticks = ((tm.tv_sec*1000)+(tm.tv_usec/1000));
    return ticks;
}

int ezbus_platform_rand(void)
{
    return rand();
}

void ezbus_platform_srand(unsigned int seed)
{
    srand(seed);
}

int ezbus_platform_random(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

void ezbus_platform_rand_init (void)
{
    ezbus_platform_srand( ezbus_platform_get_ms_ticks()&0xFFFFFFFF );
}

void ezbus_platform_delay(unsigned int ms)
{
    ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
    while ( (ezbus_platform_get_ms_ticks() - start) < ms );
}
