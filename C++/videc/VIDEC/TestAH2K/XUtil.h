// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

unsigned long XGetTimestamp(void);
unsigned short XGetLastSequence(unsigned short usSequence1,unsigned short usSequence2);
unsigned long XGenerateSSRC(void);
void XSleep(unsigned long ulMS);

#endif 