// ConferenceCallTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CConferenceCallTestApp:
// �йش����ʵ�֣������ ConferenceCallTest.cpp
//

class CConferenceCallTestApp : public CWinApp
{
public:
	CConferenceCallTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CConferenceCallTestApp theApp;