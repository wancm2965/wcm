
// WBPrint.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWBPrintApp:
// �йش����ʵ�֣������ WBPrint.cpp
//

class CWBPrintApp : public CWinApp
{
public:
	CWBPrintApp();
	~CWBPrintApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CWBPrintApp theApp;