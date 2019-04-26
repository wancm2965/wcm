
// AVCON_WB.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CAVCON_WBApp:
// 有关此类的实现，请参阅 AVCON_WB.cpp
//
class CAVCON_WBApp : public CWinAppEx
{
public:
	CAVCON_WBApp();

public:
// 重写
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:

// 实现
	DECLARE_MESSAGE_MAP()

};

extern CAVCON_WBApp theApp;