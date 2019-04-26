// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// HPDTS.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

#ifdef _WIN32
#include <MMSystem.h>
#pragma  comment(lib,"winmm.lib")
#endif

unsigned long GenerateSSRC(void)
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

void AvconGet_path(const char* pStr_Path,char* pCrrent_Dir) //�ȵ��ļ���ǰ·��
{
	if (pStr_Path == NULL)
	{
		return;
	}
	while(*(pCrrent_Dir ++) = *(pStr_Path ++));
#ifdef WIN32
	while(*(--pCrrent_Dir) != '\\');
#else
	while(*(--pCrrent_Dir) != '/');
#endif
	*pCrrent_Dir = '\0';
}

void AvconGet_name(const char* pStr_path,char* pFile_name) // �����ļ���
{
	if (pStr_path == NULL)
	{
		return;
	}
	while(*(pStr_path ++));
#ifdef WIN32
	while(*(--pStr_path) != '\\');
#else
	while(*(--pStr_path) != '/');
#endif
	while(*(pFile_name++) = *(++pStr_path));
}