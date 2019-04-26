// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_WB_Main.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CString g_strExePath = _T("");
CString GetExePath(void)
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[8192];
		int len = GetModuleFileName(AfxGetInstanceHandle(),path,8192);
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
void SetHPDTSSavePath( CString LoginInId )
{
	std::string strDtsTempPath = CW2A(GetExePath());
	std::string strLoginID = CW2A(LoginInId);
	// 	if (IsHpAvconXPE())
	// 	{
	// 		strDtsTempPath = "d:\\AVCON6_XPE\\";
	// 	}
	strDtsTempPath += "HPDTS";
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	_tmkdir(CA2W(strLoginID.c_str()));
	strDtsTempPath = strDtsTempPath + "\\" + strLoginID;
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	HPDTSInit(strDtsTempPath.c_str());
	//	IMXEC_PDFConvert::SetTempPath(strDtsTempPath.c_str());
	HPDTSSetSavePath(strDtsTempPath.c_str());
}

void WriteOptionConfigStr( LPCTSTR selection, LPCTSTR key, CString value, CString strDefPath /*= _T("")*/ )
{
	CString strPath = _T("");
	if (strDefPath.IsEmpty())
	{
		strPath = g_strExePath + CONFIG_FILEPATH_OF_LOGIN;
	}
	else
	{
		strPath = strDefPath;
	}
	CString keyValue = value;
	CString strSelection = selection;
	WritePrivateProfileString(strSelection,key,keyValue, strPath);
}

CString ReadOptionConfigStr( LPCTSTR selection, LPCTSTR key,CString strDefPath /*= _T("")*/ )
{
	CString strPath = _T("");
	if (strDefPath.IsEmpty())
	{
		strPath = g_strExePath + CONFIG_FILEPATH_OF_LOGIN;
	}
	else
	{
		strPath = strDefPath;
	}

	CString strSelection = selection; 
	TCHAR strValue[4096]=_T("");
	GetPrivateProfileString(strSelection, key, _T(""),strValue,4096,strPath);
	return strValue;
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

// CString A2U(std::string sstring)
// {
// 	int cchStr = (int) sstring.length() + 1;
// 	wchar_t* pwstr = new wchar_t[cchStr];
// 	if( pwstr != NULL )
// 		::MultiByteToWideChar(CP_UTF8, 0, sstring.c_str(), -1, pwstr, cchStr);
// 
// 	CString strTmp(pwstr);
// 	delete []pwstr;
// 
// 	return strTmp;
// }
// 
// std::string U2A(CString cstring)
// {
// 	CT2CA pszU (cstring);
// 	std::string m_strUserID (pszU);
// 	return m_strUserID;
// }