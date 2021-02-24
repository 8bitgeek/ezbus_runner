#include <syslog_timestamp.h>

extern uint64_t ezbus_platform_get_ms_ticks();

extern  uint64_t  syslog_timestamp(void)
{
  return ezbus_platform_get_ms_ticks();
}  
