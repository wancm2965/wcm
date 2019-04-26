#include "StdAfx.h"
#include "LanguageStr.h"
#include "LanguageNameDefine.h"
#include "Shlwapi.h"

LanguageStr::LanguageStr()
{
}

LanguageStr::~LanguageStr()
{
	m_mapKeyValue.clear();
}

LPCTSTR LanguageStr::GetLanguageValue( LPCTSTR key,tstring defaultString )
{
	static TCHAR p = TEXT('\0');
	std::map<tstring,tstring>::iterator it = m_mapKeyValue.find(key);
	if (m_mapKeyValue.end() != it)
	{
		return (*it).second.c_str();

	}
	else
	{
		m_mapKeyValue.insert(std::pair<tstring,tstring>(key,defaultString));
		std::map<tstring,tstring>::iterator it = m_mapKeyValue.find(key);
		if (m_mapKeyValue.end() != it)
		{
			return (*it).second.c_str();

		}
	}
	return &p;
}

BOOL LanguageStr::InitLanguageStr()
{
	TCHAR strKeyValue[MAX_PATH];
	TCHAR strNameValue[MAX_PATH];
	TCHAR strFile[MAX_PATH] = {0};
	TCHAR *pToStrPath = NULL;
	wcscpy_s(strFile,m_LanConfigPath.c_str());
	pToStrPath = _tcsrchr(strFile,_T('\\'));
	pToStrPath++;
	tstring iniFile = pToStrPath;
	pToStrPath[0] = _T('\0');
	tstring temporaryFile = strFile;
	temporaryFile += TEXT("~$");
	temporaryFile += iniFile;
	if (PathFileExists(m_LanConfigPath.c_str()))
	{
		CopyFile(m_LanConfigPath.c_str(),temporaryFile.c_str(),FALSE);
		if (PathFileExists(temporaryFile.c_str()) )        //以后要处理拷贝不成功的问题
		{
			DWORD nChar = 0;
			while(TRUE)
			{
				nChar = GetPrivateProfileString(m_strNode.c_str(),NULL,L"",strKeyValue,MAX_PATH,temporaryFile.c_str());
				nChar = GetPrivateProfileString(m_strNode.c_str(),strKeyValue,L"",strNameValue,MAX_PATH,temporaryFile.c_str());
				m_mapKeyValue.insert(std::pair<tstring, tstring>(strKeyValue,strNameValue));
				WritePrivateProfileString(m_strNode.c_str(),strKeyValue,NULL,temporaryFile.c_str());
				if (0 == nChar)
				{
					break;
				}
			}
			DeleteFile(temporaryFile.c_str());
		}
	}
	return TRUE;
}
