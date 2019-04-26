// UCC7-���ö�д���ܲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7���ö�д����
	class IPlugin_U7_Cfg_Comm : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_Cfg_Comm(){};

	public:
		// �ж��Ƿ�XPE
		virtual bool IsXPE(void) = 0;

		// ���ù��������ļ�·����������Ŀ������ã�Ĭ���ڳ����Ŀ¼�£�
		virtual void SetCfgFilePath(CString strFilePath) = 0;

		// �����û����������ļ�·�����������ã������޷�Ϊÿ���û�����������ã�
		virtual void SetUserCfgFilePath(CString strFilePath) = 0;

		// ��ȡĬ������
		virtual CString GetDefCfgString(CString strSection, CString strKey, CString strDefValue = _T("")) = 0;
		virtual int GetDefCfgInt(CString strSection, CString strKey, int nDefValue = 0) = 0;
		virtual bool GetDefCfgBool(CString strSection, CString strKey, bool bDefValue = false) = 0;
		virtual unsigned long GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0) = 0;

		// ��д��������
		virtual void SetCfgString(CString strSection, CString strKey, CString strValue) = 0;
		virtual CString GetCfgString(CString strSection, CString strKey, CString strDefValue = _T("")) = 0;

		virtual void SetCfgInt(CString strSection, CString strKey, int nValue) = 0;
		virtual int GetCfgInt(CString strSection, CString strKey, int nDefValue = 0) = 0;

		virtual void SetCfgBool(CString strSection, CString strKey, bool bValue) = 0;
		virtual bool GetCfgBool(CString strSection, CString strKey, bool bDefValue = false) = 0;

		virtual void SetCfgULong(CString strSection, CString strKey, unsigned long ulValue) = 0;
		virtual unsigned long GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue = 0) = 0;

		// ��д�û���������
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
