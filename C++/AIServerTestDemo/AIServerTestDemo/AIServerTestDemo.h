
// AIServerTestDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAIServerTestDemoApp: 
// �йش����ʵ�֣������ AIServerTestDemo.cpp
//

class CAIServerTestDemoApp : public CWinApp
{
public:
	CAIServerTestDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAIServerTestDemoApp theApp;