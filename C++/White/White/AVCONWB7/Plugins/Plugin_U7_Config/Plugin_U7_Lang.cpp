#include "StdAfx.h"
#include "Plugin_U7_Lang.h"
#include "Plugin_U7_Cfg_Comm.h"
#include "XmlLoad.h"
#include "tstring.h"
#include "ErrorCode.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CPlugin_U7_Lang* g_pPluginU7Lang = NULL;
CPlugin_U7_Lang* GetPluginU7Lang(void)
{
	return g_pPluginU7Lang;
}

CPlugin_U7_Lang::CPlugin_U7_Lang(void)
 :	m_strSkinFolderPath(_T(""))
{

	CString strCfgFilePath = GetExePath() + _T("U7Config.ini");
	if (GetU7CfgComm())
	{
		if (GetU7CfgComm()->IsXPE())
		{
			strCfgFilePath = _T("d:\\AVCON7_XPE\\U7Config.ini");
		}
	}
	TCHAR strValue[1024] = _T("");
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, 1024, strCfgFilePath);
	m_strCurLang = strValue;

	m_strSkinPath = GetExePath() + _T("Skin\\");
	m_strCommLangPath = GetExePath() + _T("Skin\\CommRes\\") + m_strCurLang + _T("_CommLang.ini");

	g_pPluginU7Lang = this;
}

CPlugin_U7_Lang::~CPlugin_U7_Lang(void)
{
	g_pPluginU7Lang = NULL;
	std::map<CString, std::map<CString, CString>*>::iterator it = m_mapLanguage.begin();
	for(;it != m_mapLanguage.end();it++)
	{
		std::map<CString, CString>* plang = it->second;
		plang->clear();
		delete plang;
	}
	m_mapLanguage.clear();
	for(it = m_mapCommLang.begin();it != m_mapCommLang.end();it++)
	{
		std::map<CString, CString>* plang = it->second;
		plang->clear();
		delete plang;
	}
	m_mapCommLang.clear();
}

//////////////////////////////////////////////////////////////////////////
// 继承IPlugin_U7_Lang接口
// 获取插件名称
std::string CPlugin_U7_Lang::Name()
{
	return "Plugin_U7_Lang";
}

// 获取插件功能点
std::string CPlugin_U7_Lang::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_U7_Lang";
}

// 初始化插件
bool CPlugin_U7_Lang::InitPlugin()
{
	CErrorCode::Instance().InitErrorCode();
	return true;
}

// 插件退出处理
bool CPlugin_U7_Lang::ClosePlugin()
{
	CErrorCode::Instance().Release();
	return true;
}

// 读取当前语种
CString CPlugin_U7_Lang::GetCurLang(void)
{
	return m_strCurLang;
}

// 读取通用语言包字符串
CString CPlugin_U7_Lang::GetCommLangString(CString strSection, CString strKey, CString strDefValue)
{
	CString strLangFile = GetSkinFolder() + _T("CommRes\\language.xml");
	return GetLangStringFromMap(strLangFile,_T(""),strSection,strKey,strDefValue);
// 	if (GetU7CfgComm() == NULL)
// 		return strDefValue;
// 
// 	m_strCommLangPath = GetSkinFolder() + _T("CommRes\\") + m_strCurLang + _T("_CommLang.ini");
// 	return GetU7CfgComm()->ReadCfgString(m_strCommLangPath, strSection, strKey, strDefValue);
}

CString AVCONPlugin::CPlugin_U7_Lang::GetCommLangStringD(CString strSection, CString strKey, CString strDefValue)
{
	return GetPluginsLangStringD(_T("CommRes_D"),strKey,strDefValue);
// 	if (GetU7CfgComm() == NULL)
// 		return strDefValue;
// 
// 	CString strLangFile = GetSkinFolderD() + _T("CommRes_D\\") + m_strCurLang + _T("_CommLang.ini");
// 	return GetU7CfgComm()->ReadCfgString(strLangFile, strSection, strKey, strDefValue);
}

// 读取插件自带语言包字符串
CString CPlugin_U7_Lang::GetPluginsLangString(CString strPluginName, CString strKey, CString strDefValue)
{
	m_strSkinPath = GetSkinFolder();
	CString strLangFile = m_strSkinPath + strPluginName + _T("\\") +_T("language.xml");
	return GetLangStringFromMap(strLangFile,strPluginName,_T("LANGUAGE"),strKey,strDefValue);
// 	if (GetU7CfgComm() == NULL)
// 		return strDefValue;
// 
// 	m_strSkinPath = GetSkinFolder();
// 	CString strLangFile = m_strSkinPath + strPluginName + _T("\\") + m_strCurLang + _T("_Lang.ini");;
// 	return GetU7CfgComm()->ReadCfgString(strLangFile, _T("LANGUAGE"), strKey, strDefValue);
}

CString AVCONPlugin::CPlugin_U7_Lang::GetPluginsLangStringD(CString strPluginName, CString strKey, CString strDefValue)
{
	if (GetU7CfgComm() == NULL)
		return strDefValue;
	CString strLangFile = GetSkinFolderD() + strPluginName + _T("\\") + _T("language.xml");
// 	CString strLangFile = GetSkinFolderD() + strPluginName + _T("\\") + m_strCurLang + _T("_Lang.ini");;
// 	return GetU7CfgComm()->ReadCfgString(strLangFile, _T("LANGUAGE"), strKey, strDefValue);
	return GetLangStringFromMap(strLangFile,strPluginName,_T("LANGUAGE"),strKey,strDefValue);
}

//////////////////////////////////////////////////////////////////////////

CString CPlugin_U7_Lang::GetSkinFolder()
{
	if (m_strSkinFolderPath.IsEmpty())
	{
		//m_strSkinFolderPath = GetExePath() + _T("skin\\");
		//CString strTemp = _T("skin_black");
// 		//IPlugin_U7_Cfg_Comm* pIPlugin_U7_Cfg_Comm = dynamic_cast<IPlugin_U7_Cfg_Comm*>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
// 		if (NULL != GetU7CfgComm())
// 		{
// 			strTemp = GetU7CfgComm()->GetCfgString(_T("UISet"), _T("SkinFolder"), _T("skin_black"));
// 		}
// 
// 		if (!strTemp.IsEmpty())
// 		{
// 			m_strSkinFolderPath.Format(_T("%sskin\\%s\\"), GetExePath(), strTemp);
// 		}

		m_strSkinFolderPath.Format(_T("%sskin\\%s\\"), GetExePath(), _T("Language"));
	}
	return m_strSkinFolderPath;
}

CString AVCONPlugin::CPlugin_U7_Lang::GetSkinFolderD()
{
	if (m_strSkinFolderPathD.IsEmpty())
	{
		m_strSkinFolderPathD = GetExePath() + _T("skin\\Language\\");
	}
	return m_strSkinFolderPathD;
}

CString AVCONPlugin::CPlugin_U7_Lang::GetLangStringFromMap( CString &strLangFileS,CString strPluginName,CString strSection,CString &strKey,CString &strDefValue )
{
	BOOL bComPlugin = strPluginName.IsEmpty();
	std::map<CString, std::map<CString, CString>*> *pLanguageMap = NULL;
	CString Langkey = _T("");
	if (bComPlugin)
	{
		pLanguageMap = &m_mapCommLang;
		Langkey = strSection;
	}
	else
	{
		pLanguageMap = &m_mapLanguage;
		Langkey = strPluginName;
	}
	std::map<CString, std::map<CString, CString>*>::iterator it = pLanguageMap->find(Langkey);
	if (pLanguageMap->end() == it)
	{
		//解析xml
// 		CString strLangFile = strLangFileS;
// 		std::string langfilex = ToStr(strLangFile.GetBuffer());
		CMarkup myDocument;
		if (myDocument.LoadFromFile(strLangFileS))
		{
			CMarkupNode root = myDocument.GetRoot();
			if (root.IsValid())
			{
				CMarkupNode LangNation = root.GetChild();
				for(;LangNation.IsValid();LangNation=LangNation.GetSibling())
				{
					if (m_strCurLang == LangNation.GetAttributeValue(TEXT("language")))
					{
						break;
					}
				}
				
				if (LangNation.IsValid())
				{
					CMarkupNode PluginNode = LangNation.GetChild();
					for(;PluginNode.IsValid();PluginNode=PluginNode.GetSibling())
					{
						std::map<CString, CString> *pPluginmap = new std::map<CString, CString>;
						for(CMarkupNode LangNode=PluginNode.GetChild();LangNode.IsValid();LangNode=LangNode.GetSibling())
						{
							CString cName,cValue;
							cName = LangNode.GetAttributeValue(TEXT("name"));
							cValue = LangNode.GetAttributeValue(TEXT("value"));
							pPluginmap->insert(std::pair<CString, CString>(cName,cValue));
						}
						if (bComPlugin)
						{
							Langkey = PluginNode.GetAttributeValue(TEXT("name"));
						}
						pLanguageMap->insert(std::pair<CString, std::map<CString, CString>*>(Langkey, pPluginmap));			
						if (!bComPlugin)
						{
							break;
						}
					}
				}	
			}
		}
	}

	if (bComPlugin)
	{
		Langkey = strSection;
	}
	else
	{
		Langkey = strPluginName;
	}
	it = pLanguageMap->find(Langkey);
	if (pLanguageMap->end() != it)
	{
		std::map<CString, CString>* LanguageMap = it->second;
		std::map<CString, CString>::iterator LanguageIt = LanguageMap->find(strKey);
		if (LanguageIt != LanguageMap->end())
		{
			return LanguageIt->second;
		}
	}
	return strDefValue;
}

CString AVCONPlugin::CPlugin_U7_Lang::GetErrCodeStr(unsigned long ulErrorCode)
{
	return CErrorCode::Instance().GetErrCodeStr(ulErrorCode);
}