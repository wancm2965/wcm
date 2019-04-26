// XUtil.h: interface for the XUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <NETEC/NETEC_Export.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string>

NETEC_API uint32_t XGetTimestamp(void);
NETEC_API unsigned short XGetLastSequence(unsigned short usSequence1,unsigned short usSequence2);
NETEC_API uint32_t XGenerateSSRC(void);
NETEC_API void XSleep(uint32_t ulMS);

#endif 