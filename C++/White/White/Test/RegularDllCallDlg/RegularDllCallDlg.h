
// RegularDllCallDlg.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRegularDllCallDlgApp:
// �йش����ʵ�֣������ RegularDllCallDlg.cpp
//

class CRegularDllCallDlgApp : public CWinApp
{
public:
	CRegularDllCallDlgApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRegularDllCallDlgApp theApp;