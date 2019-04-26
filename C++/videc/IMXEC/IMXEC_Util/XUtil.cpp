// XUtil.cpp: implementation of the XUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "XUtil.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif


unsigned int XGetTimestamp(void)
{
#ifdef WIN32
	return ::timeGetTime();//����
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}


unsigned short XGetLastSequence(unsigned short usSequence1,unsigned short usSequence2)
{
	unsigned short usLastSequence=usSequence2;
	if ((usSequence1>usSequence2 && (unsigned short)(usSequence1-usSequence2)<32768) ||
		(usSequence1<usSequence2 && (unsigned short)(usSequence2-usSequence1)>32768))
	{
		usLastSequence=usSequence1;
	}

	return usLastSequence;
}

unsigned int XGenerateSSRC(void)
{
#ifdef WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//ȡ�߾������м�������Ƶ��
	{
		return timeGetTime();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //���㵽΢����

	return dwRet;//΢��
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec; 
#endif
}

void XSleep(unsigned long ulMS)
{
#ifdef WIN32
	Sleep(ulMS);
#else
	usleep(ulMS*1000);
#endif
}
