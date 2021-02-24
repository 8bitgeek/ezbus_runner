#ifndef _SYSLOG_TIMESTAMP_H_
#define _SYSLOG_TIMESTAMP_H_

#if defined(_CARIBOU_RTOS_)
    #include <board.h>
#else
    #include <stdint.h>
    #include <stdbool.h>
    #include <stdio.h>
#endif
#include <syslog.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define syslog_timestamp_t  uint64_t

extern  syslog_timestamp_t  syslog_timestamp(void);         

#ifdef __cplusplus
}
#endif

#endif
