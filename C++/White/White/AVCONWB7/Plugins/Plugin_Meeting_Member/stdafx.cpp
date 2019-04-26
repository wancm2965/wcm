// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_Meeting_Member.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息
#include "stdafx.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

std::string U2A( CString strUnicodeString )
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

CString A2U( const char* strAnsiString )
{
	ASSERT(!::IsBadStringPtrA(strAnsiString,-1));
	int cchStr = (int) strlen(strAnsiString) + 1;
	wchar_t* pwstr = new wchar_t[cchStr];
	if( pwstr != NULL )
		::MultiByteToWideChar(CP_UTF8, 0, strAnsiString, -1, pwstr, cchStr);

	CString str(pwstr);
	delete []pwstr;
	return str;
}

CString A2U( std::string stdstr )
{
	const char* strAnsiString = stdstr.c_str();
	ASSERT(!::IsBadStringPtrA(strAnsiString,-1));
	int cchStr = (int) strlen(strAnsiString) + 1;
	wchar_t* pwstr = new wchar_t[cchStr];
	if( pwstr != NULL )
		::MultiByteToWideChar(CP_UTF8, 0, strAnsiString, -1, pwstr, cchStr);

	CString str(pwstr);
	delete []pwstr;
	return str;

}
CString _A2W( std::string str )
{
	USES_CONVERSION;
	CString strW = A2W(str.c_str());
	return strW;
}

std::string _W2A( CString str )
{
	USES_CONVERSION;
	std::string strA = W2A(str);
	return strA;
}

CString ULONG2STR(unsigned long ulValue)
{
	CString str;
	str.Format(_T("%u"), ulValue);
	return str;
}

CString g_strExePath = _T("");
CString g_strAvconLanguageFile  = _T("");

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
		g_strExePath = CStdString(path);
	}

	return g_strExePath;
}

CString GetFolderPath()
{
// 	CString strSkinFolderPath = _T("");
// 	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(g_System->GetPluginByName("Plugin_U7_ComUI"));
// 	if (pluginU7ComUI != NULL)
// 	{
// 		strSkinFolderPath = pluginU7ComUI->GetSkinFolder();
// 	}
// 
// 	return strSkinFolderPath + _T("Plugin_U7_Meeting_Member\\");
	return CString(CPaintManagerUI::GetInstancePath()) + _T("skin\\Plugin_U7_Meeting_Member\\");
	
}

CString GetComFolderPath()
{
// 	CString strSkinFolder = _T("");
// 	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(g_System->GetPluginByName("Plugin_U7_ComUI"));
// 	if (NULL != pluginU7ComUI )
// 	{
// 		strSkinFolder = pluginU7ComUI->GetSkinFolder();
// 	}
// 
// 	return strSkinFolder + _T("CommRes\\");
	return CString(CPaintManagerUI::GetInstancePath()) + _T("skin\\CommRes\\");
}

//进行语言选择
void GetLangPath()
{
	CString strCfgFilePath = GetExePath() + _T("\U7Config.ini");
	wchar_t strValue[MAX_PATH] = {0};
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, MAX_PATH, strCfgFilePath);
	g_strAvconLanguageFile = GetExePath() + _T("\config") +_T("\\") + strValue + _T("_WBConfig.ini");
}
//读取配置文件
CString ReadConfigText( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[MAX_PATH] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,MAX_PATH,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}