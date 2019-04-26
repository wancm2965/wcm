// VIDECYNCAPP.h : VIDECYNC DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CUDAAppApp
// 有关此类实现的信息，请参阅 VIDECYNC.cpp
//

class VIDECYNCApp : public CWinApp
{
public:
	VIDECYNCApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
