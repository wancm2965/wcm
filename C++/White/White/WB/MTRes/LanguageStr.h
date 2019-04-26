#pragma once
#include <map>
#include "tstring.h"

class LanguageStr
{
public:
	LanguageStr();
	~LanguageStr();
	LPCTSTR GetLanguageValue(LPCTSTR key,tstring defaultString);
	BOOL InitLanguageStr();
	tstring GetLanauagePath(){return m_LanConfigPath;}
	void SetLanguagePath(tstring path){m_LanConfigPath = path;}
	tstring GetNodeStr(){return m_strNode;}
	void SetNodeStr(tstring node){m_strNode = node;}
private:
	std::map<tstring,tstring> m_mapKeyValue;
	tstring m_LanConfigPath;
	tstring m_strNode;
};