#include "StdAfx.h"
#include ".\rwinifile.h"
#include "ISystem\ISystem.h"
#include "IPlugin\IU7_IM\IPlugin_U7_IM_Friend.h"
using namespace AVCONPlugin;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString g_strCfgFilePath;
CString g_strLangFilePath;
CString g_strOptionFilePath;

CRWIniFile::CRWIniFile(void)
{
}

CRWIniFile::~CRWIniFile(void)
{
}

int CRWIniFile::ReadConfigInt(LPCTSTR selection, LPCTSTR key, int def)
{
	CString strINIPath = GetCfgFilePath();
	return GetPrivateProfileInt(selection, key, def, strINIPath);
}

void CRWIniFile::WriteConfigInt(LPCTSTR selection, LPCTSTR key, int value)
{
	CString strINIPath = GetCfgFilePath();
	WritePrivateProfileInt(selection, key, value, strINIPath);
}

CString CRWIniFile::ReadConfigStr(LPCTSTR selection, LPCTSTR key, CString def)
{
	CString strINIPath = GetCfgFilePath();
	TCHAR strValue[4096]=_T("");
	::GetPrivateProfileString(selection, key, def, strValue, 4096, strINIPath);
	return strValue;
}

void CRWIniFile::WriteConfigStr(LPCTSTR selection, LPCTSTR key, CString value)
{
	CString strINIPath = GetCfgFilePath();
	WritePrivateProfileString(selection, key, value, strINIPath);
}

BOOL CRWIniFile::ReadConfigBool(LPCTSTR selection, LPCTSTR key, BOOL def)
{
	CString strINIPath = GetCfgFilePath();
	int nValue = GetPrivateProfileInt(selection, key, def, strINIPath);
	return  nValue ? true : false;
}

void CRWIniFile::WriteConfigBool(LPCTSTR selection, LPCTSTR key, BOOL value)
{
	CString strINIPath = GetCfgFilePath();
	WritePrivateProfileBool(selection, key, value, strINIPath);
}

//////////////////////////////////////////////////////////////////////////
//СЎПо
BOOL CRWIniFile::ReadOptionConfigBool(LPCTSTR selection, LPCTSTR key, BOOL def, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	int nValue = GetPrivateProfileInt(selection, key, def, strINIPath);
	return  nValue ? true : false;
}

void CRWIniFile::WriteOptionConfigBool(LPCTSTR selection, LPCTSTR key, BOOL value, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	WritePrivateProfileBool(selection, key, value, strINIPath);
}

int CRWIniFile::ReadOptionConfigInt(LPCTSTR selection, LPCTSTR key, int def, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	return GetPrivateProfileInt(selection, key, def, strINIPath);
}

void CRWIniFile::WriteOptionConfigInt(LPCTSTR selection, LPCTSTR key, int value, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	WritePrivateProfileInt(selection, key, value, strINIPath);
}

CString CRWIniFile::ReadOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString def, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	TCHAR strValue[4096]=_T("");
	::GetPrivateProfileString(selection, key, def, strValue, 4096, strINIPath);
	return strValue;
}

void CRWIniFile::WriteOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString value, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
	}
	else
	{
		strINIPath = GetOptionFilePath();
	}
	WritePrivateProfileString(selection, key, value, strINIPath);
}
//////////////////////////////////////////////////////////////////////////

CString CRWIniFile::ReadLangStr(LPCTSTR selection, LPCTSTR key, CString def)
{
	CString strLangPath = GetLangFilePath();
	TCHAR strValue[4096]=_T("");
	::GetPrivateProfileString(selection, key, def, strValue, 4096, strLangPath);
	return strValue;
}

void CRWIniFile::WriteLangStr(LPCTSTR selection, LPCTSTR key, CString value)
{
	CString strLangPath = GetLangFilePath();
	WritePrivateProfileString(selection, key, value, strLangPath);
}

//////////////////////////////////////////////////////////////////////////
void CRWIniFile::WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int value, LPCTSTR lpFileName)
{
	CString strValue;
	strValue.Format(_T("%d"), value);
	WritePrivateProfileString(lpAppName, lpKeyName, strValue, lpFileName);
}

void CRWIniFile::WritePrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL value, LPCTSTR lpFileName)
{
	CString strValue;
	strValue.Format(_T("%d"),value ? 1 : 0);
	WritePrivateProfileString(lpAppName, lpKeyName, strValue, lpFileName);
}

CString CRWIniFile::GetCfgFilePath(void)
{
	if (g_strCfgFilePath.IsEmpty())
	{
		g_strCfgFilePath.Format(_T("%s\\AVCONUCCCfg.ini"), GetExePath());
	}

	return g_strCfgFilePath;
}

CString CRWIniFile::GetLangFilePath(void)
{
	if (g_strLangFilePath.IsEmpty())
	{
		CString strLang = ReadConfigStr(_T("System"), _T("Lang"), _T("cs"));
		g_strLangFilePath.Format(_T("%s\\skin\\AVCONUCCLang_%s.ini"), GetExePath(), strLang);
	}

	return g_strLangFilePath;
}

CString CRWIniFile::GetOptionFilePath(void)
{
	if (g_strOptionFilePath.IsEmpty())
	{
		IPlugin_U7_IM_Friend* pPlugin = (IPlugin_U7_IM_Friend*)g_System->GetPluginByName("Plugin_U7_IM_Friend");
		if (pPlugin == NULL)
			return _T("");
		CString csUserID = _A2W(pPlugin->GetUserID());
		if ( csUserID.IsEmpty())
			return _T("");		
		g_strOptionFilePath.Format(_T("%suser\\%s\\config.ini"), GetExePath(), csUserID);
	}

	return g_strOptionFilePath;
}

int CRWIniFile::ReadLeaveCount()
{
	return CRWIniFile::ReadOptionConfigInt(MSG_LEAV_SECTION, MSG_LEAV_Count, 0);
}

void CRWIniFile::WriteLeaveCount(int nCount)
{
	CRWIniFile::WriteOptionConfigInt(MSG_LEAV_SECTION, MSG_LEAV_Count, nCount);
}

CString CRWIniFile::ReadLeaveMsgTxt(int nIndex)
{
	CString strKey = _T("");
	strKey.Format( _T("%s%d"), MSG_LEAV_Lable, nIndex );
	return CRWIniFile::ReadOptionConfigStr(MSG_LEAV_SECTION, strKey, _T(""));
}

void CRWIniFile::WriteLeaveMsgTxt(int nIndex,CString csTxt)
{
	CString strKey = _T("");
	strKey.Format( _T("%s%d"), MSG_LEAV_Lable, nIndex );
	CRWIniFile::WriteOptionConfigStr(MSG_LEAV_SECTION, strKey, csTxt);
}

