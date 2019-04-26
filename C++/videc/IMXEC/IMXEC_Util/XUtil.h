// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

unsigned int XGetTimestamp(void);
unsigned short XGetLastSequence(unsigned short usSequence1,unsigned short usSequence2);
unsigned int XGenerateSSRC(void);
void XSleep(unsigned long ulMS);

#endif 