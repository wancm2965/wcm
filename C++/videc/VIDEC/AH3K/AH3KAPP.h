// AH3KAPP.h : AH3K DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// AH3KAppApp
// 有关此类实现的信息，请参阅 AH3K.cpp
//

class AH3KApp : public CWinApp
{
public:
	AH3KApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
