// TestDecoder.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTestDecoderApp:
// �йش����ʵ�֣������ TestDecoder.cpp
//

class CTestDecoderApp : public CWinApp
{
public:
	CTestDecoderApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestDecoderApp theApp;
