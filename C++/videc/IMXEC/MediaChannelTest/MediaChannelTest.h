// MediaChannelTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMediaChannelTestApp:
// �йش����ʵ�֣������ MediaChannelTest.cpp
//

class CMediaChannelTestApp : public CWinApp
{
public:
	CMediaChannelTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMediaChannelTestApp theApp;