// DESEncrypt.h : DESEncrypt DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "EncryptAddUnencrypt.h"

// CDESEncryptApp
// �йش���ʵ�ֵ���Ϣ������� DESEncrypt.cpp
//

class CDESEncryptApp : public CWinApp
{
public:
	CDESEncryptApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
extern  EncryptAddUnencrypt *g_EncryptAddUnencrypt;
extern "C" _declspec(dllexport) void _EncryptCode(CString EncryptData);
extern "C" _declspec(dllexport) void _UnencryptCode(CString FilePath,char* resultData,int &resultDataLenth);
extern "C" _declspec(dllexport) void _SetKeyOriginalData(CString KeyOriginalData);

