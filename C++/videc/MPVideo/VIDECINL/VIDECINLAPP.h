// VIDECINLAPP.h : VIDECINL DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// VIDECINLAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECINL.cpp
//

class VIDECINLApp : public CWinApp
{
public:
	VIDECINLApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
