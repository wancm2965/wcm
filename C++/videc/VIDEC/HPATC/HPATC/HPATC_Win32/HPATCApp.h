// HPATC.h : HPATC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHPATCApp
// �йش���ʵ�ֵ���Ϣ������� HPATC.cpp
//

class CHPATCApp : public CWinApp
{
public:
	CHPATCApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
