
// TestLAVFilters.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestLAVFiltersApp:
// �йش����ʵ�֣������ TestLAVFilters.cpp
//

class CTestLAVFiltersApp : public CWinAppEx
{
public:
	CTestLAVFiltersApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestLAVFiltersApp theApp;