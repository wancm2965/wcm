// AH2KAPP.h : AH2K DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// AH2KAppApp
// 有关此类实现的信息，请参阅 AH2K.cpp
//

class AH2KApp : public CWinApp
{
public:
	AH2KApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
