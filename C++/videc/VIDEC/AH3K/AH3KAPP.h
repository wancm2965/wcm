// AH3KAPP.h : AH3K DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// AH3KAppApp
// �йش���ʵ�ֵ���Ϣ������� AH3K.cpp
//

class AH3KApp : public CWinApp
{
public:
	AH3KApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
