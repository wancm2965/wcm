
// TestTemplate.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestTemplateApp:
// �йش����ʵ�֣������ TestTemplate.cpp
//

class CTestTemplateApp : public CWinAppEx
{
public:
	CTestTemplateApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestTemplateApp theApp;