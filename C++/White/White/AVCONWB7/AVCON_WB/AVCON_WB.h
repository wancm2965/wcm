
// AVCON_WB.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAVCON_WBApp:
// �йش����ʵ�֣������ AVCON_WB.cpp
//
class CAVCON_WBApp : public CWinAppEx
{
public:
	CAVCON_WBApp();

public:
// ��д
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:

// ʵ��
	DECLARE_MESSAGE_MAP()

};

extern CAVCON_WBApp theApp;