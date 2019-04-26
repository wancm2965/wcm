#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif




#ifdef WIN32
#include "windows.h"

void gettimeofday(struct timeval *tv, void *tz)
{
    long int l = GetTickCount();
 
    tv->tv_sec = l / 1000;
    tv->tv_usec = (l % 1000) * 1000;
    return;
} 

#endif
