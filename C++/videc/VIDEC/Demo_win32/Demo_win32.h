
// Demo_win32.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDemo_win32App:
// �йش����ʵ�֣������ Demo_win32.cpp
//

class CDemo_win32App : public CWinAppEx
{
public:
	CDemo_win32App();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDemo_win32App theApp;