
// bmp2H264.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cbmp2H264App:
// �йش����ʵ�֣������ bmp2H264.cpp
//

class Cbmp2H264App : public CWinAppEx
{
public:
	Cbmp2H264App();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cbmp2H264App theApp;