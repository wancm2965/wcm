// TestAH2K.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTestAH2KApp:
// �йش����ʵ�֣������ TestAH2K.cpp
//

class CTestAH2KApp : public CWinApp
{
public:
	CTestAH2KApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestAH2KApp theApp;
