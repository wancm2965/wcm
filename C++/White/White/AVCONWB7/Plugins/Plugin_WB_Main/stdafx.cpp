// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_WB_Main.pch 将作为预编译头
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

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
void ShutDown( void )
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		TRACE(L"OpenProcessToken");

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0);

	// Cannot test the return value of AdjustTokenPrivileges.
	if (GetLastError() != ERROR_SUCCESS)
		TRACE(L"AdjustTokenPrivileges");

	// Shut down the system and force all applications to close.
	if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
		TRACE(L"ExitWindowsEx");
}


void Reboot( void )
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	/* OpenProcessToken() 这个函数的作用是打开一个进程的访问令牌
	GetCurrentProcess() 函数的作用是得到本进程的句柄*/
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
		return;
	// LookupPrivilegeValue() 的作用是修改进程的权限
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set 赋给本进程特权
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// AdjustTokenPrivileges（）的作用是通知Windows NT修改本进程的权利
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) //失败
		return;
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0)) //参数在这里设置。强行退出WINDOWS（EWX_SHUTDOWN）。
		return;
}

void SetHPDTSSavePath( CString LoginId )
{
	std::string strDtsTempPath = CW2A(GetExePath());
	std::string strLoginID = CW2A(LoginId);
	strDtsTempPath += "HPDTS";
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	_tmkdir(CA2W(strLoginID.c_str()));
	strDtsTempPath = strDtsTempPath + "\\" + strLoginID;
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	HPDTSInit(strDtsTempPath.c_str());
	HPDTSSetSavePath(strDtsTempPath.c_str());
}

CString g_strExePath = _T("");
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

CString g_strAvconLanguageFile  = _T("");
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

CString GetFolderPath( void )
{
return CString(CPaintManagerUI::GetInstancePath() + _T("skin\\Plugin_WB_Main\\"));
}

IPlugin_U7_Cfg_Comm* GetPluginCfgComm( void )
{
	static IPlugin_U7_Cfg_Comm* s_pPluginCfgComm = dynamic_cast<IPlugin_U7_Cfg_Comm*>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
	return s_pPluginCfgComm;
}
