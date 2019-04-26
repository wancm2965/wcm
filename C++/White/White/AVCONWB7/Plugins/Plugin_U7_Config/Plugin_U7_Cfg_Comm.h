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
		CString m_strDefCfgSavePath; //����Ĭ�ϱ���·��������XPE�������豸
		void InitCfgFilePath();

	public:
		//////////////////////////////////////////////////////////////////////////
		// �̳�IPlugin_U7_Cfg_Comm�ӿ�
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();

		// �ж��Ƿ�XPE
		virtual bool IsXPE(void);

		// ���ù��������ļ�·����������Ŀ������ã�Ĭ���ڳ����Ŀ¼�£�
		virtual void SetCfgFilePath(CString strFilePath);

		// �����û����������ļ�·�����������ã������޷�Ϊÿ���û�����������ã�
		virtual void SetUserCfgFilePath(CString strFilePath);

		// ��ȡĬ������
		virtual CString GetDefCfgString(CString strSection, CString strKey, CString strDefValue = _T(""));
		virtual int GetDefCfgInt(CString strSection, CString strKey, int nDefValue = 0);
		virtual bool GetDefCfgBool(CString strSection, CString strKey, bool bDefValue = false);
		virtual unsigned long GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0);

		// ��д��������
		virtual void SetCfgString(CString strSection, CString strKey, CString strValue);
		virtual CString GetCfgString(CString strSection, CString strKey, CString strDefValue = _T(""));

		virtual void SetCfgInt(CString strSection, CString strKey, int nValue);
		virtual int GetCfgInt(CString strSection, CString strKey, int nDefValue = 0);

		virtual void SetCfgBool(CString strSection, CString strKey, bool bValue);
		virtual bool GetCfgBool(CString strSection, CString strKey, bool bDefValue = false);

		virtual void SetCfgULong(CString strSection, CString strKey, unsigned long ulValue);
		virtual unsigned long GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0);

		// ��д�û���������
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