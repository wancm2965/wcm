// AH2KAPP.h : AH2K DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// AH2KAppApp
// �йش���ʵ�ֵ���Ϣ������� AH2K.cpp
//

class AH2KApp : public CWinApp
{
public:
	AH2KApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
