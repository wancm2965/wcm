// UCC7-配置读写功能插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7配置读写功能
	class IPlugin_U7_Cfg_Comm : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_Cfg_Comm(){};

	public:
		// 判断是否XPE
		virtual bool IsXPE(void) = 0;

		// 设置公用配置文件路径（根据项目情况设置，默认在程序根目录下）
		virtual void SetCfgFilePath(CString strFilePath) = 0;

		// 设置用户独立配置文件路径（必须设置，否则无法为每个用户保存独立配置）
		virtual void SetUserCfgFilePath(CString strFilePath) = 0;

		// 读取默认配置
		virtual CString GetDefCfgString(CString strSection, CString strKey, CString strDefValue = _T("")) = 0;
		virtual int GetDefCfgInt(CString strSection, CString strKey, int nDefValue = 0) = 0;
		virtual bool GetDefCfgBool(CString strSection, CString strKey, bool bDefValue = false) = 0;
		virtual unsigned long GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0) = 0;

		// 读写公用配置
		virtual void SetCfgString(CString strSection, CString strKey, CString strValue) = 0;
		virtual CString GetCfgString(CString strSection, CString strKey, CString strDefValue = _T("")) = 0;

		virtual void SetCfgInt(CString strSection, CString strKey, int nValue) = 0;
		virtual int GetCfgInt(CString strSection, CString strKey, int nDefValue = 0) = 0;

		virtual void SetCfgBool(CString strSection, CString strKey, bool bValue) = 0;
		virtual bool GetCfgBool(CString strSection, CString strKey, bool bDefValue = false) = 0;

		virtual void SetCfgULong(CString strSection, CString strKey, unsigned long ulValue) = 0;
		virtual unsigned long GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0) = 0;

		// 读写用户独立配置
		virtual void SetUserCfgString(CString strSection, CString strKey, CString strValue) = 0;
		virtual CString GetUserCfgString(CString strSection, CString strKey, CString strDefValue = _T("")) = 0;

		virtual void SetUserCfgInt(CString strSection, CString strKey, int nValue) = 0;
		virtual int GetUserCfgInt(CString strSection, CString strKey, int nDefValue = 0) = 0;

		virtual void SetUserCfgBool(CString strSection, CString strKey, bool bValue) = 0;
		virtual bool GetUserCfgBool(CString strSection, CString strKey, bool bDefValue = false) = 0;

		virtual void SetUserCfgULong(CString strSection, CString strKey, unsigned long ulValue) = 0;
		virtual unsigned long GetUserCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0) = 0;

		virtual CString GetDefCfgSavePath(void) = 0;
	};
}
