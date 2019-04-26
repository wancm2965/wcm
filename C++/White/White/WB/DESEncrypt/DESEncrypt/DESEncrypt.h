// DESEncrypt.h : DESEncrypt DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "EncryptAddUnencrypt.h"

// CDESEncryptApp
// 有关此类实现的信息，请参阅 DESEncrypt.cpp
//

class CDESEncryptApp : public CWinApp
{
public:
	CDESEncryptApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
extern  EncryptAddUnencrypt *g_EncryptAddUnencrypt;
extern "C" _declspec(dllexport) void _EncryptCode(CString EncryptData);
extern "C" _declspec(dllexport) void _UnencryptCode(CString FilePath,char* resultData,int &resultDataLenth);
extern "C" _declspec(dllexport) void _SetKeyOriginalData(CString KeyOriginalData);

