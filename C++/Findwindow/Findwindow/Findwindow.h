
// Findwindow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFindwindowApp:
// �йش����ʵ�֣������ Findwindow.cpp
//

class CFindwindowApp : public CWinAppEx
{
public:
	CFindwindowApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFindwindowApp theApp;