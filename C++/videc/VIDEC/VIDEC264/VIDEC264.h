// VIDEC264.h : VIDEC264 DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CVIDEC264App
// �йش���ʵ�ֵ���Ϣ������� VIDEC264.cpp
//

class CVIDEC264App : public CWinApp
{
public:
	CVIDEC264App();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
