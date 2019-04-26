#include "StdAfx.h"
#include "RWIniFile.h"

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

	WritePrivateProfileBool(selection, key, value, strINIPath);
}

int CRWIniFile::ReadOptionConfigInt(LPCTSTR selection, LPCTSTR key, int def, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
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
	
	WritePrivateProfileInt(selection, key, value, strINIPath);
}

CString CRWIniFile::ReadOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString def, CString strDefPath /* = _T */)
{
	CString strINIPath = _T("");
	if ( !strDefPath.IsEmpty() )
	{
		strINIPath = strDefPath;
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
		g_strCfgFilePath.Format(_T("%s\\U7Config.ini"), GetExePath());
	}

	return g_strCfgFilePath;
}

CString CRWIniFile::GetLangFilePath(void)
{
	if (g_strLangFilePath.IsEmpty())
	{
		CString strLang = ReadConfigStr(_T("LANGUAGE"), _T("Language"), _T("cs"));
		g_strLangFilePath.Format(_T("%s\\skin\\Plugin_WB_Login\\%s_Lang.ini"), GetExePath(), strLang);
	}

	return g_strLangFilePath;
}


// 
// int CRWIniFile::ReadLeaveCount()
// {
// 	return CRWIniFile::ReadOptionConfigInt(MSG_LEAV_SECTION, MSG_LEAV_Count, 0);
// }
// 
// void CRWIniFile::WriteLeaveCount(int nCount)
// {
// 	CRWIniFile::WriteOptionConfigInt(MSG_LEAV_SECTION, MSG_LEAV_Count, nCount);
// }
// 
// CString CRWIniFile::ReadLeaveMsgTxt(int nIndex)
// {
// 	CString strKey = _T("");
// 	strKey.Format( _T("%s%d"), MSG_LEAV_Lable, nIndex );
// 	return CRWIniFile::ReadOptionConfigStr(MSG_LEAV_SECTION, strKey, _T(""));
// }
// 
// void CRWIniFile::WriteLeaveMsgTxt(int nIndex,CString csTxt)
// {
// 	CString strKey = _T("");
// 	strKey.Format( _T("%s%d"), MSG_LEAV_Lable, nIndex );
// 	CRWIniFile::WriteOptionConfigStr(MSG_LEAV_SECTION, strKey, csTxt);
// }
// 
// 
