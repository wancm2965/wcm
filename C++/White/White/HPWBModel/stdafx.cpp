// stdafx.cpp : 只包括标准包含文件的源文件
// HPWBModel.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

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
