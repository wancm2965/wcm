
// GetAllWin.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGetAllWinApp:
// �йش����ʵ�֣������ GetAllWin.cpp
//

class CGetAllWinApp : public CWinAppEx
{
public:
	CGetAllWinApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGetAllWinApp theApp;