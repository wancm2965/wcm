// VIDECRTP.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CVIDECRTPApp:
// �йش����ʵ�֣������ VIDECRTP.cpp
//

class CVIDECRTPApp : public CWinApp
{
public:
	CVIDECRTPApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CVIDECRTPApp theApp;
