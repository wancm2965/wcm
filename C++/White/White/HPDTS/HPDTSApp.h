// HPDTS.h : HPDTS DLL ����ͷ�ļ�
//

#pragma once

#ifdef WIN32

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHPDTSApp
// �йش���ʵ�ֵ���Ϣ������� HPDTS.cpp
//

class CHPDTSApp : public CWinApp
{
public:
	CHPDTSApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

#endif