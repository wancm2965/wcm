#pragma once
#include "IPlugin/IU7_Config/IPlugin_U7_Lang.h"
#include <map>
using namespace AVCONPlugin;
namespace AVCONPlugin
{
	class CPlugin_U7_Lang
		: public IPlugin_U7_Lang
	{
	public:
		CPlugin_U7_Lang(void);
		~CPlugin_U7_Lang(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		// 继承IPlugin_U7_Lang接口
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

		// 读取当前语种
		virtual CString GetCurLang(void);
		// 读取通用语言包字符串
		virtual CString GetCommLangString(CString strSection, CString strKey, CString strDefValue);
		virtual CString GetCommLangStringD(CString strSection, CString strKey, CString strDefValue);
		// 读取插件自带语言包字符串
		virtual CString GetPluginsLangString(CString strPluginName, CString strKey, CString strDefValue);
		virtual CString GetPluginsLangStringD(CString strPluginName, CString strKey, CString strDefValue);

		virtual CString GetErrCodeStr(unsigned long ulErrorCode);
		//////////////////////////////////////////////////////////////////////////

		CString GetSkinFolder();
		CString GetSkinFolderD();
	private:
		CString GetLangStringFromMap(CString &strLangFileS,CString strPluginName,CString strSection,CString &strKey,CString &strDefValue);
	private:
		CString m_strCurLang;
		CString m_strSkinPath;
		CString m_strCommLangPath;

		CString m_strSkinFolderPath;
		CString m_strSkinFolderPathD;
		//content:<section, <key, keyvalue>>
		//std::map<CString, std::map<CString, CString> *> m_mapCommLangD;
		//content:<section, <key, keyvalue>>
		std::map<CString, std::map<CString, CString> *> m_mapCommLang;
		//content:<pluginname, <key, keyvalue>>
		std::map<CString, std::map<CString, CString> *> m_mapLanguage;
	};
}

CPlugin_U7_Lang* GetPluginU7Lang(void);