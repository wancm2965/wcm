#pragma once
#include "IPlugin/IU7_Config/IPlugin_U7_Cfg_Comm.h"

using namespace AVCONPlugin;
namespace AVCONPlugin
{
	class CPlugin_U7_Cfg_Comm
		: public IPlugin_U7_Cfg_Comm
	{
	public:
		CPlugin_U7_Cfg_Comm(void);
		~CPlugin_U7_Cfg_Comm(void);

	public:
		void WriteCfgString(CString strFile, CString strSection, CString strKey, CString strValue);
		CString ReadCfgString(CString strFile, CString strSection, CString strKey, CString strDefValue = _T(""));

		void WriteCfgInt(CString strFile, CString strSection, CString strKey, int nValue);
		int ReadCfgInt(CString strFile, CString strSection, CString strKey, int nDefValue = 0);

		void WriteCfgBool(CString strFile, CString strSection, CString strKey, bool bValue);
		bool ReadCfgBool(CString strFile, CString strSection, CString strKey, bool bDefValue = false);

		void WriteCfgULong(CString strFile, CString strSection, CString strKey, unsigned long ulValue);
		unsigned long ReadCfgULong(CString strFile, CString strSection, CString strKey, unsigned long ulDefValue = 0);

	private:
		bool		m_bXPE;
		CString m_strDefCfgFilePath;
		CString m_strCfgFilePath;
		CString m_strUserCfgFilePath;
		CString m_strDefCfgSavePath; //程序默认保存路径，区分XPE与其他设备
		void InitCfgFilePath();

	public:
		//////////////////////////////////////////////////////////////////////////
		// 继承IPlugin_U7_Cfg_Comm接口
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

		// 判断是否XPE
		virtual bool IsXPE(void);

		// 设置公用配置文件路径（根据项目情况设置，默认在程序根目录下）
		virtual void SetCfgFilePath(CString strFilePath);

		// 设置用户独立配置文件路径（必须设置，否则无法为每个用户保存独立配置）
		virtual void SetUserCfgFilePath(CString strFilePath);

		// 读取默认配置
		virtual CString GetDefCfgString(CString strSection, CString strKey, CString strDefValue = _T(""));
		virtual int GetDefCfgInt(CString strSection, CString strKey, int nDefValue = 0);
		virtual bool GetDefCfgBool(CString strSection, CString strKey, bool bDefValue = false);
		virtual unsigned long GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0);

		// 读写公用配置
		virtual void SetCfgString(CString strSection, CString strKey, CString strValue);
		virtual CString GetCfgString(CString strSection, CString strKey, CString strDefValue = _T(""));

		virtual void SetCfgInt(CString strSection, CString strKey, int nValue);
		virtual int GetCfgInt(CString strSection, CString strKey, int nDefValue = 0);

		virtual void SetCfgBool(CString strSection, CString strKey, bool bValue);
		virtual bool GetCfgBool(CString strSection, CString strKey, bool bDefValue = false);

		virtual void SetCfgULong(CString strSection, CString strKey, unsigned long ulValue);
		virtual unsigned long GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0);

		// 读写用户独立配置
		virtual void SetUserCfgString(CString strSection, CString strKey, CString strValue);
		virtual CString GetUserCfgString(CString strSection, CString strKey, CString strDefValue = _T(""));

		virtual void SetUserCfgInt(CString strSection, CString strKey, int nValue);
		virtual int GetUserCfgInt(CString strSection, CString strKey, int nDefValue = 0);

		virtual void SetUserCfgBool(CString strSection, CString strKey, bool bValue);
		virtual bool GetUserCfgBool(CString strSection, CString strKey, bool bDefValue = false);

		virtual void SetUserCfgULong(CString strSection, CString strKey, unsigned long ulValue);
		virtual unsigned long GetUserCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0);

		virtual CString GetDefCfgSavePath(void);
		//////////////////////////////////////////////////////////////////////////
	};
}

CPlugin_U7_Cfg_Comm* GetU7CfgComm(void);