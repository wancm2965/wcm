// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// IMXEC.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

void DebugOutput(const char* fmt,...)
{
	va_list args;
	va_start(args, fmt);

	char buffer1[2048]={0};
#ifdef WIN32
	_vsnprintf(buffer1,sizeof(buffer1),fmt,args);
#else
	vsnprintf(buffer1,sizeof(buffer1),fmt,args);
#endif
#ifdef WIN32
	OutputDebugStringA(buffer1);
#endif
	va_end(args);
}

