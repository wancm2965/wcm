// Plugin_WB_FrameDlg.h : Plugin_WB_FrameDlg DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPlugin_WB_FrameDlgApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_WB_FrameDlg.cpp
//

class CPlugin_WB_FrameDlgApp : public CWinApp
{
public:
	CPlugin_WB_FrameDlgApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
