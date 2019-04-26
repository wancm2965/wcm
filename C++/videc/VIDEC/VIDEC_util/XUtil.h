// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


//#define	USE_AH2K_STATIC_MEMORY


unsigned long XGetTimestamp(void);
unsigned short XGetLastSequence(unsigned short usSequence1,unsigned short usSequence2);
unsigned long XGenerateSSRC(void);
void XSleep(unsigned long ulMS);

#ifdef USE_AH2K_STATIC_MEMORY
void SetMemCanUse(unsigned char* pData);		//由于AH2K有时需要采集多路yuv数据，每次分配释放导致内存碎片从而分配失败，具体说明可见VCapMemMang.cpp类的说明
#endif

#endif 