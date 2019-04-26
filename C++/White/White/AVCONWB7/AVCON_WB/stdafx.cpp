
// stdafx.cpp : 只包括标准包含文件的源文件
// AVCON_WB.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
CString g_strExePath = _T("");
CString GetExePath(void)
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		int len = GetModuleFileName(NULL,path,MAX_PATH);
		path[len] = 0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if (!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen] = 0;
		g_strExePath = CString(path);
	}
	return g_strExePath;
}

