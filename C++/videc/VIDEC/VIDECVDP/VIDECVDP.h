// VIDECVDP.h : VIDECVDP DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CVIDECVDPApp
// �йش���ʵ�ֵ���Ϣ������� VIDECVDP.cpp
//

class CVIDECVDPApp : public CWinApp
{
public:
	CVIDECVDPApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
