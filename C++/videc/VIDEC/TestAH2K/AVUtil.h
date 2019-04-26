// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVUTIL_H__
#define __AVUTIL_H__

unsigned long AVGetTickCount(void);
unsigned long AVGenerateSSRC(void);
void AVSleep(unsigned long ulMS);
void AVDelay(unsigned long ulMS);
#define AVStringToULONG(string)		strtoul(string,NULL,10)

#endif 