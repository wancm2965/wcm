
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// AVCON_WB.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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

