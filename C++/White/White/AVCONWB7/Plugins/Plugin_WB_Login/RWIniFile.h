#pragma once

//读写配置文件

class CRWIniFile
{
public:
	CRWIniFile(void);
	~CRWIniFile(void);

public:
	static int ReadConfigInt(LPCTSTR selection, LPCTSTR key, int def);
	static void WriteConfigInt(LPCTSTR selection, LPCTSTR key, int value);
	static CString ReadConfigStr(LPCTSTR selection, LPCTSTR key, CString def);
	static void WriteConfigStr(LPCTSTR selection, LPCTSTR key, CString value);
	static CString ReadLangStr(LPCTSTR selection, LPCTSTR key, CString def);
	static void WriteLangStr(LPCTSTR selection, LPCTSTR key, CString value);
	static BOOL ReadConfigBool(LPCTSTR selection, LPCTSTR key, BOOL def);
	static void WriteConfigBool(LPCTSTR selection, LPCTSTR key, BOOL value);


	//选项
	static BOOL ReadOptionConfigBool(LPCTSTR selection, LPCTSTR key, BOOL def, CString strDefPath = _T(""));
	static void WriteOptionConfigBool(LPCTSTR selection, LPCTSTR key, BOOL value, CString strDefPath = _T(""));
	static int ReadOptionConfigInt(LPCTSTR selection, LPCTSTR key, int def, CString strDefPath = _T(""));
	static void WriteOptionConfigInt(LPCTSTR selection, LPCTSTR key, int value, CString strDefPath = _T(""));
	static CString ReadOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString def, CString strDefPath = _T(""));
	static void WriteOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString value, CString strDefPath = _T(""));

	//离线状态
	static int		ReadLeaveCount();
	static void		WriteLeaveCount(int nCount);
	static CString	ReadLeaveMsgTxt(int nIndex);
	static void		WriteLeaveMsgTxt(int nIndex,CString csTxt);


protected:
	static void WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int value, LPCTSTR lpFileName);
	static void WritePrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL value, LPCTSTR lpFileName);
	static CString GetCfgFilePath(void);
	static CString GetLangFilePath(void);


};
