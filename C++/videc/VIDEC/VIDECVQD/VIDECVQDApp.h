// VIDECVQDAPP.h : VIDECVQD DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CUDAAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECVQD.cpp
//

class VIDECVQDApp : public CWinApp
{
public:
	VIDECVQDApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
