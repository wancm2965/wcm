// VIDECICEAPP.h : VIDECICE DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// VIDECICEAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECICE.cpp
//

class VIDECICEApp : public CWinApp
{
public:
	VIDECICEApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
