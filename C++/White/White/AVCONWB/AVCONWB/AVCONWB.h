
// AVCONWB.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAVCONWBApp:
// �йش����ʵ�֣������ AVCONWB.cpp
//

class CAVCONWBApp : public CWinAppEx
{
public:
	CAVCONWBApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAVCONWBApp theApp;