// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// HPWBModel.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


CString GetAppPath()
{
	TCHAR szTemp[MAX_PATH] = _T("");
	CString strAppPath;
	GetModuleFileName(NULL, szTemp, MAX_PATH);
	if (wcsrchr(szTemp, _T('\\')) != NULL)
	{
		*wcsrchr(szTemp, _T('\\')) = _T('\0');
	}
	strAppPath.Format(_T("%s"), szTemp);

	return strAppPath;
}
