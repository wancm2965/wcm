//////////////////////////////////////////////////////////////////////////
//FileName:			AutoLockResource.h									//
//��ֹDLL������Դ�����沿�ֵ��г�ͻ																		//
//																		//
//																		//
//																		//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

#pragma once

extern HINSTANCE	g_hInstance;

class AutoLockResource
{
public:
	AutoLockResource();
	virtual ~AutoLockResource();

private:
	BYTE				bDLL;
	HINSTANCE			appLangDLL;
	HINSTANCE			hCurrentInstanceHandle;
	HINSTANCE			hCurrentResourceHandle;
};

