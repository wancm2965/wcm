// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_WB_FrameDlg.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString g_strExePath =_T("");

CString GetExePath(void)
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[8192];
		int len = GetModuleFileName(GetModuleHandle(NULL),path,8192);
		path[len]=0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if(!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen]=0;
		g_strExePath = CString(path);
	}
	return g_strExePath;
}

CString A2U(std::string sstring)
{
	int cchStr = (int) sstring.length() + 1;
	wchar_t* pwstr = new wchar_t[cchStr];
	if( pwstr != NULL )
		::MultiByteToWideChar(CP_UTF8, 0, sstring.c_str(), -1, pwstr, cchStr);

	CString strTmp(pwstr);
	delete []pwstr;

	return strTmp;
}

std::string U2A(CString strUnicodeString)
{
	ASSERT(!::IsBadStringPtrW(strUnicodeString,-1));
	int cchStr = ::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, wcslen(strUnicodeString), NULL, 0, NULL, NULL);
	char* pstr = new char[cchStr + 1];
	if( pstr != NULL )
		::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, -1, pstr, cchStr, NULL, NULL);
	pstr[cchStr] = '\0';
	std::string str(pstr);
	delete pstr;
	return str;
}
