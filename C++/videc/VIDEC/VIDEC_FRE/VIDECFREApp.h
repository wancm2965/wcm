// VIDECFREAPP.h : VIDECFRE DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CUDAAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECFRE.cpp
//

class VIDECFREApp : public CWinApp
{
public:
	VIDECFREApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
