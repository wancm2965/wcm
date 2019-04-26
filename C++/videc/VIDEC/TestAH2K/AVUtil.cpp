// AVUtil.cpp: implementation of the AVUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVUtil.h"
#ifdef _WIN32
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned long AVGetTickCount(void)
{
#ifdef WIN32
	return timeGetTime();//GetTickCount();//毫秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}

unsigned long AVGenerateSSRC(void)
{
#ifdef WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return timeGetTime();//GetTickCount();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();//GetTickCount();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec; 
#endif
}

void AVSleep(unsigned long ulMS)
{
#ifdef WIN32
	Sleep(ulMS);
#else
	usleep(ulMS*1000);
#endif
}

void AVDelay(unsigned long ulMS)
{
#ifdef WIN32
	Sleep(ulMS);
	//LARGE_INTEGER frequence, privious, current, interval;
	//if(!QueryPerformanceFrequency( &frequence))
	//{
	//	Sleep(ulMS);
	//	return;
	//}

	//interval.QuadPart = frequence.QuadPart * ulMS / 1000000;
	//QueryPerformanceCounter( &privious );
	//current = privious;
	//while( current.QuadPart - privious.QuadPart < interval.QuadPart )
	//	QueryPerformanceCounter( &current);
	return;
#else
	int fd=-1;
	unsigned long data=0;
	int tmp=1024;
	struct timeval start, now;

	gettimeofday(&start, NULL);

	fd = open ("/dev/rtc", O_RDONLY);
	if (fd==-1)
	{
		usleep(1);
		return;
	}

	/* Read periodic IRQ rate */
	if (-1==ioctl(fd, RTC_IRQP_READ, &tmp))
	{
		usleep(0);
		close(fd);
		return;
	}

	/* The frequencies 128Hz, 256Hz, ... 8192Hz are only allowed for root. */
	tmp=1024;

	if (-1==ioctl(fd, RTC_IRQP_SET, tmp))
	{
		usleep(0);
		close(fd);
		return;
	}

	if (-1 == ioctl(fd, RTC_PIE_ON, 0))
	{
		usleep(0);
		close(fd);
		return;
	}

	while (true)
	{
		/* This blocks */
		read(fd, &data, sizeof(unsigned long));
		
		gettimeofday(&now, NULL);

		if (now.tv_sec*1000+now.tv_usec/1000-(start.tv_sec*1000+start.tv_usec/1000)>=ulMS)
			break;
	}

	/* Disable periodic interrupts */
	ioctl(fd, RTC_PIE_OFF, 0);
	
	close(fd);
#endif
}

/*
char*AVStringToken(char*szSource,char charKey)
{
	char szTempSource[1024]="";
	strcpy(szTempSource,szSource);
	char*szToken=szTempSource;
	char*pTemp = strchr(szTempSource, charKey);
	if (pTemp)
	{
		strcpy(szSource,pTemp+1);
		*pTemp=0;
		return szTempSource;
	}

	return NULL;
}
*/
