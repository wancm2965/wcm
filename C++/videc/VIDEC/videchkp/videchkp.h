// videchkp.h : videchkp DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CvidechkpApp
// �йش���ʵ�ֵ���Ϣ������� videchkp.cpp
//

class CvidechkpApp : public CWinApp
{
public:
	CvidechkpApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
