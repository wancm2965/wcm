// VIDECD3DAPP.h : VIDECD3D DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CUDAAppApp
// �йش���ʵ�ֵ���Ϣ������� VIDECD3D.cpp
//

class VIDECD3DApp : public CWinApp
{
public:
	VIDECD3DApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
