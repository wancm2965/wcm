// AHC1KAPP.h : AHC1K DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// AHC1KAppApp
// �йش���ʵ�ֵ���Ϣ������� AHC1K.cpp
//

class AHC1KApp : public CWinApp
{
public:
	AHC1KApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
