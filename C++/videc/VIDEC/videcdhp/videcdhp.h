// videcdhp.h : videcdhp DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CvidecdhpApp
// �йش���ʵ�ֵ���Ϣ������� videcdhp.cpp
//

class CvidecdhpApp : public CWinApp
{
public:
	CvidecdhpApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
