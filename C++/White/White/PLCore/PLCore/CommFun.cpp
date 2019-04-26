#include "CommFun.h"
#include <Windows.h>

CString g_strExePath;
CString GetCurrentPath(void)
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		int len = GetModuleFileName(NULL, path, MAX_PATH);
		path[len] = 0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if (!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen] = 0;
		g_strExePath = path;
	}

	return g_strExePath;
}
