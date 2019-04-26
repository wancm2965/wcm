// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Plugin_WB_Login.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


#include "io.h"
#include "direct.h"
#include "IPlugin/IU7T.Comm/IPlugin_U7_ComUI.h"



// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
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
		g_strExePath = CString(path);
	}

	return g_strExePath;
}

bool MakeDirEx(const char* lpPath)
{
	CString strPath = CA2W(lpPath);
	if( strPath.Right(1) != "\\" )
		strPath += "\\";
	int nEnd = strPath.ReverseFind('\\');
	int nPt = strPath.Find('\\');
	if (strPath[nPt-1] == ':')
		nPt = strPath.Find('\\', nPt+1);

	CString strTemp = _T("");
	while(nPt != -1 && nPt<=nEnd)
	{
		strTemp = strPath.Left(nPt+1);
		if( -1 == _access(CW2A(strTemp), 0) )
		{
			if ( _mkdir(CW2A(strTemp)) !=  0)
				return false;
		}
		nPt = strPath.Find('\\', nPt+1);
	}
	return true;
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
// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
//��������ѡ��
void GetLangPath()
{
	CString strCfgFilePath = GetExePath() + _T("\U7Config.ini");
	wchar_t strValue[MAX_PATH] = {0};
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, MAX_PATH, strCfgFilePath);
	g_strAvconLanguageFile = GetExePath() + _T("\config") +_T("\\") + strValue + _T("_WBConfig.ini");
}
//��ȡ�����ļ�
CString ReadConfigText( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[MAX_PATH] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,MAX_PATH,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}
void Reboot( void )
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	/* OpenProcessToken() ��������������Ǵ�һ�����̵ķ�������
	GetCurrentProcess() �����������ǵõ������̵ľ��*/
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
		return;
	// LookupPrivilegeValue() ���������޸Ľ��̵�Ȩ��
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set ������������Ȩ
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// AdjustTokenPrivileges������������֪ͨWindows NT�޸ı����̵�Ȩ��
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) //ʧ��
		return;
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0)) //�������������á�ǿ���˳�WINDOWS��EWX_SHUTDOWN����
		return;
}