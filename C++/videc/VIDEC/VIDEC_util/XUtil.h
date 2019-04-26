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
void SetMemCanUse(unsigned char* pData);		//����AH2K��ʱ��Ҫ�ɼ���·yuv���ݣ�ÿ�η����ͷŵ����ڴ���Ƭ�Ӷ�����ʧ�ܣ�����˵���ɼ�VCapMemMang.cpp���˵��
#endif

#endif 