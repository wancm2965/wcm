// WBDicomp.h : WBDicomp DLL 的主头文件
//

#pragma once
#include "stdafx.h"
#include "DicomToBmp.h"

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CWBDicompApp
// 有关此类实现的信息，请参阅 WBDicomp.cpp
//

class CWBDicompApp : public CWinApp
{
public:
	CWBDicompApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern  CDicomToBmp*g_Dicomtobmp;

extern "C" _declspec(dllexport) int DicomToBmpDll(CString strFilePath, CString g_strAvconTempPath);
//获取Dicom生成BMP图片后的路径
extern "C" _declspec(dllexport) BOOL GetDicomBmpPath(char *pszPath, int nLen);