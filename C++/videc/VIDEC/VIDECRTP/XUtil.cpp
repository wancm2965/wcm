// XUtil.cpp: implementation of the XUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "XUtil.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <errno.h>
#endif


unsigned long XGetTimestamp(void)
{
#ifdef WIN32
	return ::timeGetTime();//毫秒
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

unsigned long XGenerateSSRC(void)
{
#ifdef WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return timeGetTime();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
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
