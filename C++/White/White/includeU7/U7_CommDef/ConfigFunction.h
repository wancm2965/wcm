#pragma once
#include "..\IPlugin\IU7_Config\IPlugin_U7_Cfg_Comm.h"
#include "..\IPlugin\IU7_Config\IPlugin_U7_Lang.h"

using namespace AVCONPlugin;

class CConfigFuntion
{
private:
	static IPlugin_U7_Cfg_Comm* GetPluginU7CfgComm( void )
	{
		static IPlugin_U7_Cfg_Comm* s_pPluginCfgComm = dynamic_cast<IPlugin_U7_Cfg_Comm*>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
		return s_pPluginCfgComm;
	}

public:
	// ¶ÁÐ´¹«ÓÃÅäÖÃ
	static void SetCfgStr(CString strSection, CString strKey, CString strValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetCfgString(strSection, strKey, strValue);
	}

	static CString GetCfgStr(CString strSection, CString strKey, CString strDefValue /*= _T("")*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return strDefValue;

		CString strReturn = GetPluginU7CfgComm()->GetCfgString(strSection, strKey, strDefValue);
		return strReturn;
	}

	static void SetCfgInt(CString strSection, CString strKey, int nValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetCfgInt(strSection, strKey, nValue);
	}

	static int GetCfgInt(CString strSection, CString strKey, int nDefValue /*= 0*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return nDefValue;

		int nReturn = GetPluginU7CfgComm()->GetCfgInt(strSection, strKey, nDefValue);
		return nReturn;
	}

	static void SetCfgBool(CString strSection, CString strKey, bool bValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetCfgBool(strSection, strKey, bValue);
	}

	static bool GetCfgBool(CString strSection, CString strKey, bool bDefValue /*= false*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return bDefValue;

		bool bReturn = GetPluginU7CfgComm()->GetCfgBool(strSection, strKey, bDefValue);
		return bReturn;
	}

	static void SetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetCfgULong(strSection, strKey, ulDefValue);
	}

	static unsigned long GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return ulDefValue;

		unsigned long ulReturn = GetPluginU7CfgComm()->GetCfgULong(strSection, strKey, ulDefValue);
		return ulReturn;
	}

	// ¶ÁÈ¡Ä¬ÈÏÅäÖÃ
	static CString GetDefCfgStr(CString strSection, CString strKey, CString strDefValue /*= _T("")*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return strDefValue;

		CString strReturn = GetPluginU7CfgComm()->GetDefCfgString(strSection, strKey, strDefValue);
		return strReturn;
	}

	static int GetDefCfgInt(CString strSection, CString strKey, int nDefValue /*= 0*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return nDefValue;

		int nReturn = GetPluginU7CfgComm()->GetDefCfgInt(strSection, strKey, nDefValue);
		return nReturn;
	}

	static bool GetDefCfgBool(CString strSection, CString strKey, bool bDefValue /*= false*/)
	{
		if (GetPluginU7CfgComm() == NULL)
			return bDefValue;

		bool bReturn = GetPluginU7CfgComm()->GetDefCfgBool(strSection, strKey, bDefValue);
		return bReturn;
	}

	static unsigned long GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return ulDefValue;

		unsigned long ulReturn = GetPluginU7CfgComm()->GetDefCfgULong(strSection, strKey, ulDefValue);
		return ulReturn;
	}

	// ¶ÁÐ´ÓÃ»§¶ÀÁ¢ÅäÖÃ
	static void SetUserCfgString(CString strSection, CString strKey, CString strValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetUserCfgString(strSection, strKey, strValue);
	}

	static CString GetUserCfgString(CString strSection, CString strKey, CString strDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return strDefValue;

		CString strReturn = GetPluginU7CfgComm()->GetUserCfgString(strSection, strKey, strDefValue);
		return strReturn;
	}

	static void SetUserCfgInt(CString strSection, CString strKey, int nValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetUserCfgInt(strSection, strKey, nValue);
	}

	static int GetUserCfgInt(CString strSection, CString strKey, int nDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return nDefValue;

		int nReturn = GetPluginU7CfgComm()->GetUserCfgInt(strSection, strKey, nDefValue);
		return nReturn;
	}

	static void SetUserCfgBool(CString strSection, CString strKey, bool bValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetUserCfgBool(strSection, strKey, bValue);
	}

	static bool GetUserCfgBool(CString strSection, CString strKey, bool bDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return bDefValue;

		bool bReturn = GetPluginU7CfgComm()->GetUserCfgBool(strSection, strKey, bDefValue);
		return bReturn;
	}

	static void SetUserCfgULong(CString strSection, CString strKey, unsigned long ulValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return;

		GetPluginU7CfgComm()->SetUserCfgULong(strSection, strKey, ulValue);
	}

	static unsigned long GetUserCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return ulDefValue;

		unsigned long ulReturn = GetPluginU7CfgComm()->GetUserCfgULong(strSection, strKey, ulDefValue);
		return ulReturn;
	}

	static CString GetDefCfgSavePath(CString strDefValue)
	{
		if (GetPluginU7CfgComm() == NULL)
			return strDefValue;

		return GetPluginU7CfgComm()->GetDefCfgSavePath();
	}
};
