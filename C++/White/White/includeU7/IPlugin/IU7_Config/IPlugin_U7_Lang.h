// UCC7-语言包读写功能插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7语言包读写功能
	class IPlugin_U7_Lang : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_Lang(){};

	public:
		// 读取当前语种
		virtual CString GetCurLang(void) = 0;
		// 读取通用语言包字符串（多套皮肤库使用:-T,-C）
		virtual CString GetCommLangString(CString strSection, CString strKey, CString strDefValue) = 0;
		// 读取插件自带语言包字符串（只有一套皮肤库使用:-D）
		virtual CString GetCommLangStringD(CString strSection, CString strKey, CString strDefValue) = 0;
		// 读取插件自带语言包字符串（多套皮肤库使用:-T,-C）
		virtual CString GetPluginsLangString(CString strPluginName, CString strKey, CString strDefValue) = 0;
		// 读取插件自带语言包字符串（只有一套皮肤库使用:-D）
		virtual CString GetPluginsLangStringD(CString strPluginName, CString strKey, CString strDefValue) = 0;
		// 获取错误提示
		virtual CString GetErrCodeStr(unsigned long ulErrorCode) = 0;
	};
}
