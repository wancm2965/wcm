// VIDECMKV.h : VIDECMKV DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CVIDECMKVApp
// �йش���ʵ�ֵ���Ϣ������� VIDECMKV.cpp
//

class CVIDECMKVApp : public CWinApp
{
public:
	CVIDECMKVApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
