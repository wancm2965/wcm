// WBDicomp.h : WBDicomp DLL ����ͷ�ļ�
//

#pragma once
#include "stdafx.h"
#include "DicomToBmp.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWBDicompApp
// �йش���ʵ�ֵ���Ϣ������� WBDicomp.cpp
//

class CWBDicompApp : public CWinApp
{
public:
	CWBDicompApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern  CDicomToBmp*g_Dicomtobmp;

extern "C" _declspec(dllexport) int DicomToBmpDll(CString strFilePath, CString g_strAvconTempPath);
//��ȡDicom����BMPͼƬ���·��
extern "C" _declspec(dllexport) BOOL GetDicomBmpPath(char *pszPath, int nLen);