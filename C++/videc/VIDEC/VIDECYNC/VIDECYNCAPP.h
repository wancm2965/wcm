// VIDECYNCAPP.h : VIDECYNC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CUDAAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECYNC.cpp
//

class VIDECYNCApp : public CWinApp
{
public:
	VIDECYNCApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
