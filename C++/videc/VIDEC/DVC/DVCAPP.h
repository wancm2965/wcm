// DVCAPP.h : DVC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// DVCAppApp
// �йش���ʵ�ֵ���Ϣ������� DVC.cpp
//

class DVCApp : public CWinApp
{
public:
	DVCApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
