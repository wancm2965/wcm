// IMXECApp.h : IMXEC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIMXECApp
// �йش���ʵ�ֵ���Ϣ������� IMXEC.cpp
//

class CIMXECApp : public CWinApp
{
public:
	CIMXECApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
