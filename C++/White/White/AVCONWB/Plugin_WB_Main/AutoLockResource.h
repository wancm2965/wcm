//////////////////////////////////////////////////////////////////////////
//FileName:			AutoLockResource.h									//
//防止DLL中有资源与外面部分的有冲突																		//
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

