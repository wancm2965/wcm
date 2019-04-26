#include "StdAfx.h"
#include "Plugin_U7_Cfg_Comm.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CPlugin_U7_Cfg_Comm* g_Plugin_U7_Cfg_Comm = NULL;
CPlugin_U7_Cfg_Comm* GetU7CfgComm(void)
{
	return g_Plugin_U7_Cfg_Comm;
}

CPlugin_U7_Cfg_Comm::CPlugin_U7_Cfg_Comm(void)
{
	g_Plugin_U7_Cfg_Comm = this;

	m_strDefCfgFilePath = GetExePath() + _T("\\U7DefConfig.ini");
	CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
	int nIsXPE = ReadCfgInt(csINIPath, _T("AVCON_SELECTED"), _T("AVCONXPE"), 0);
	m_bXPE = nIsXPE != 0;
	if (m_bXPE)
	{
		CString strXPEPath=_T("d:\\AVCON7_XPE");
		m_strCfgFilePath = strXPEPath + _T("\\U7Config.ini");
		m_strDefCfgSavePath = strXPEPath + _T("\\");
		// ����ļ����Ƿ����
		WIN32_FIND_DATA wfd;
		BOOL bRet;
		HANDLE hFind;
		hFind = FindFirstFile(_T("d:\\AVCON7_XPE"), &wfd);
		bRet = hFind != INVALID_HANDLE_VALUE;
		FindClose(hFind);
		if (bRet == FALSE)
		{
			CreateDirectory(_T("d:\\AVCON7_XPE"), NULL); // �����ļ���
		}
	}
	else
	{
		m_strCfgFilePath = GetExePath() + _T("U7Config.ini");
		m_strDefCfgSavePath = GetExePath();
	}
}

CPlugin_U7_Cfg_Comm::~CPlugin_U7_Cfg_Comm(void)
{
	g_Plugin_U7_Cfg_Comm = NULL;
}

void CPlugin_U7_Cfg_Comm::WriteCfgString(CString strFile, CString strSection, CString strKey, CString strValue)
{
	// Save as Unicode	wangxin 2012/10/31 16
	FILE *fp;
	fp = _tfopen(strFile, _T("r"));
	if (fp == NULL)
	{
		fp=_tfopen(strFile, _T("w+b"));
		
		if (fp == NULL)
			return;

		TCHAR strUnicode[1];
		strUnicode[0] = TCHAR(0XFEFF);
		fputwc(*strUnicode,fp);
	}
	fclose(fp);

	WritePrivateProfileString(strSection, strKey, strValue, strFile);
}

CString CPlugin_U7_Cfg_Comm::ReadCfgString(CString strFile, CString strSection, CString strKey, CString strDefValue)
{
	TCHAR strValue[4096] = _T("");
	::GetPrivateProfileString(strSection, strKey, strDefValue, strValue, 4096, strFile);
	return strValue;
}

void CPlugin_U7_Cfg_Comm::WriteCfgInt(CString strFile, CString strSection, CString strKey, int nValue)
{
	// Save as Unicode	wangxin 2012/10/31 16
	FILE *fp;
	fp = _tfopen(strFile, _T("r"));
	if (fp == NULL)
	{
		fp=_tfopen(strFile, _T("w+b"));

		if (fp == NULL)
			return;

		TCHAR strUnicode[1];
		strUnicode[0] = TCHAR(0XFEFF);
		fputwc(*strUnicode,fp);
	}
	fclose(fp);

	CString strValue;
	strValue.Format(_T("%d"), nValue);
	WritePrivateProfileString(strSection, strKey, strValue, strFile);
}

int CPlugin_U7_Cfg_Comm::ReadCfgInt(CString strFile, CString strSection, CString strKey, int nDefValue)
{
	return GetPrivateProfileInt(strSection, strKey, nDefValue, strFile);
}

void CPlugin_U7_Cfg_Comm::WriteCfgBool(CString strFile, CString strSection, CString strKey, bool bValue)
{
	// Save as Unicode	wangxin 2012/10/31 16
	FILE *fp;
	fp = _tfopen(strFile, _T("r"));
	if (fp == NULL)
	{
		fp=_tfopen(strFile, _T("w+b"));

		if (fp == NULL)
			return;

		TCHAR strUnicode[1];
		strUnicode[0] = TCHAR(0XFEFF);
		fputwc(*strUnicode,fp);
	}
	fclose(fp);

	int nValue = bValue ? 1 : 0;
	WriteCfgInt(strFile, strSection, strKey, nValue);
}

bool CPlugin_U7_Cfg_Comm::ReadCfgBool(CString strFile, CString strSection, CString strKey, bool bDefValue)
{
	int nDefValue = bDefValue ? 1 : 0;
	int nValue = GetPrivateProfileInt(strSection, strKey, nDefValue, strFile);
	bool bValue = nValue != 0;
	return bValue;
}

void CPlugin_U7_Cfg_Comm::WriteCfgULong(CString strFile, CString strSection, CString strKey, unsigned long ulValue)
{
	// Save as Unicode	wangxin 2012/10/31 16
	FILE *fp;
	fp = _tfopen(strFile, _T("r"));
	if (fp == NULL)
	{
		fp=_tfopen(strFile, _T("w+b"));

		if (fp == NULL)
			return;

		TCHAR strUnicode[1];
		strUnicode[0] = TCHAR(0XFEFF);
		fputwc(*strUnicode,fp);
	}
	fclose(fp);

	CString strValue;
	strValue.Format(_T("%u"), ulValue);
	WritePrivateProfileString(strSection, strKey, strValue, strFile);
}

unsigned long CPlugin_U7_Cfg_Comm::ReadCfgULong(CString strFile, CString strSection, CString strKey, unsigned long ulDefValue)
{
	return GetPrivateProfileInt(strSection, strKey, ulDefValue, strFile);
}

//////////////////////////////////////////////////////////////////////////
// �̳�IPlugin_U7_Cfg_Comm�ӿ�
// ��ȡ�������
std::string CPlugin_U7_Cfg_Comm::Name()
{
	return "Plugin_U7_Cfg_Comm";
}

// ��ȡ������ܵ�
std::string CPlugin_U7_Cfg_Comm::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_U7_Cfg_Comm";
}

// ��ʼ�����
bool CPlugin_U7_Cfg_Comm::InitPlugin()
{
	return true;
}

// ����˳�����
bool CPlugin_U7_Cfg_Comm::ClosePlugin()
{
	return true;
}

bool CPlugin_U7_Cfg_Comm::IsXPE(void)
{
	return m_bXPE;
}

// ���ù��������ļ�·����������Ŀ������ã�Ĭ���ڳ����Ŀ¼�£�
void CPlugin_U7_Cfg_Comm::SetCfgFilePath(CString strFilePath)
{
	if (m_bXPE)
		return;

	m_strCfgFilePath = strFilePath + _T("\\U7Config.ini");
}

// �����û����������ļ�·�����������ã������޷�Ϊÿ���û�����������ã�
void CPlugin_U7_Cfg_Comm::SetUserCfgFilePath(CString strFilePath)
{
	m_strUserCfgFilePath = strFilePath + _T("\\U7UserConfig.ini");
}

// ��ȡĬ������
CString CPlugin_U7_Cfg_Comm::GetDefCfgString(CString strSection, CString strKey, CString strDefValue)
{
	return ReadCfgString(m_strDefCfgFilePath, strSection, strKey, strDefValue);
}

int CPlugin_U7_Cfg_Comm::GetDefCfgInt(CString strSection, CString strKey, int nDefValue)
{
	return ReadCfgInt(m_strDefCfgFilePath, strSection, strKey, nDefValue);
}

bool CPlugin_U7_Cfg_Comm::GetDefCfgBool(CString strSection, CString strKey, bool bDefValue)
{
	return ReadCfgBool(m_strDefCfgFilePath, strSection, strKey, bDefValue);
}

unsigned long CPlugin_U7_Cfg_Comm::GetDefCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
{
	return ReadCfgULong(m_strDefCfgFilePath, strSection, strKey, ulDefValue);
}

// ��д��������
void CPlugin_U7_Cfg_Comm::SetCfgString(CString strSection, CString strKey, CString strValue)
{
	WriteCfgString(m_strCfgFilePath, strSection, strKey, strValue);
}

CString CPlugin_U7_Cfg_Comm::GetCfgString(CString strSection, CString strKey, CString strDefValue)
{
	return ReadCfgString(m_strCfgFilePath, strSection, strKey, strDefValue);
}

void CPlugin_U7_Cfg_Comm::SetCfgInt(CString strSection, CString strKey, int nValue)
{
	WriteCfgInt(m_strCfgFilePath, strSection, strKey, nValue);
}

int CPlugin_U7_Cfg_Comm::GetCfgInt(CString strSection, CString strKey, int nDefValue)
{
	return ReadCfgInt(m_strCfgFilePath, strSection, strKey, nDefValue);
}

void CPlugin_U7_Cfg_Comm::SetCfgBool(CString strSection, CString strKey, bool bValue)
{
	WriteCfgBool(m_strCfgFilePath, strSection, strKey, bValue);
}

bool CPlugin_U7_Cfg_Comm::GetCfgBool(CString strSection, CString strKey, bool bDefValue)
{
	return ReadCfgBool(m_strCfgFilePath, strSection, strKey, bDefValue);
}

void CPlugin_U7_Cfg_Comm::SetCfgULong(CString strSection, CString strKey, unsigned long ulValue)
{
	WriteCfgULong(m_strCfgFilePath, strSection, strKey, ulValue);
}

unsigned long CPlugin_U7_Cfg_Comm::GetCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
{
	return ReadCfgULong(m_strCfgFilePath, strSection, strKey, ulDefValue);
}

// ��д�û���������
void CPlugin_U7_Cfg_Comm::SetUserCfgString(CString strSection, CString strKey, CString strValue)
{
	WriteCfgString(m_strUserCfgFilePath, strSection, strKey, strValue);
}

CString CPlugin_U7_Cfg_Comm::GetUserCfgString(CString strSection, CString strKey, CString strDefValue)
{
	return ReadCfgString(m_strUserCfgFilePath, strSection, strKey, strDefValue);
}

void CPlugin_U7_Cfg_Comm::SetUserCfgInt(CString strSection, CString strKey, int nValue)
{
	WriteCfgInt(m_strUserCfgFilePath, strSection, strKey, nValue);
}

int CPlugin_U7_Cfg_Comm::GetUserCfgInt(CString strSection, CString strKey, int nDefValue)
{
	return ReadCfgInt(m_strUserCfgFilePath, strSection, strKey, nDefValue);
}

void CPlugin_U7_Cfg_Comm::SetUserCfgBool(CString strSection, CString strKey, bool bValue)
{
	WriteCfgBool(m_strUserCfgFilePath, strSection, strKey, bValue);
}

bool CPlugin_U7_Cfg_Comm::GetUserCfgBool(CString strSection, CString strKey, bool bDefValue)
{
	return ReadCfgBool(m_strUserCfgFilePath, strSection, strKey, bDefValue);
}

void CPlugin_U7_Cfg_Comm::SetUserCfgULong(CString strSection, CString strKey, unsigned long ulValue)
{
	WriteCfgULong(m_strUserCfgFilePath, strSection, strKey, ulValue);
}

unsigned long CPlugin_U7_Cfg_Comm::GetUserCfgULong(CString strSection, CString strKey, unsigned long ulDefValue)
{
	return ReadCfgULong(m_strUserCfgFilePath, strSection, strKey, ulDefValue);
}
//////////////////////////////////////////////////////////////////////////

CString AVCONPlugin::CPlugin_U7_Cfg_Comm::GetDefCfgSavePath( void )
{
	return m_strDefCfgSavePath;
}
