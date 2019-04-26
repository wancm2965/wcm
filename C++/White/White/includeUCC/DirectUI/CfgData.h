// CfgData.h: interface for the CCfgData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFGDATA_H__A40CDB9A_0E44_49E6_A460_505D76BA6414__INCLUDED_)
#define AFX_CFGDATA_H__A40CDB9A_0E44_49E6_A460_505D76BA6414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileEx.h"
#include "ParamParser.h"
#include "stringex.h"
#include "SkinBase.h"

class DIRECTUI_API CCfgData   
{
public:
	CCfgData();
	virtual ~CCfgData();
	bool ReadImageBase(LPCTSTR lpszSection,LPCTSTR lpszImageKey,SkinImageBase& imageBase,LPCTSTR lpszClass = _T(""));
	bool ReadFile(LPCTSTR szFileName);
	bool ReadFile(BYTE * pByte,int nSize);
	CString GetValue(const CString& keyname, const CString& valuename,const CString& strDefault = _T(""));
	int GetValueI(const CString& keyname,const CString& valuename,int nDefault = 0);
	COLORREF ReadColor( CString section, CString key, COLORREF defcolor);
	void AddKeyValue(string& strSection,char* pStart);
	
public:
	CString m_strFullName;
	CString m_strDrive;
	CString m_strDir;
	CString m_strFileName;
		
protected:
	BOOL IsKeyExisted(LPCTSTR lpszSection, LPCTSTR lpszKeyName);
	int GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszKeyName, int nDefault);
	LPCTSTR GetIniString(LPCTSTR lpszSection, LPCTSTR lpszKeyName, LPCTSTR lpszDefault);
	void RemoveAll();
	map<tstring, tstring> m_map;

//	[section]keyname这种存储方式也可以修改一下
//	m_map.insert(pair<tstring, tstring>(strKey, strValue));
		
};

#endif // !defined(AFX_CFGDATA_H__A40CDB9A_0E44_49E6_A460_505D76BA6414__INCLUDED_)

