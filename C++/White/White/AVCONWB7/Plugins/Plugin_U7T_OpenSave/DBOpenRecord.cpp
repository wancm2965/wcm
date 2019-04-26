#include "StdAfx.h"
#include "DBOpenRecord.h"
#include "ExportPlugin.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CDBOpenRecord::CDBOpenRecord(void)
	: m_pSQLiteDB(NULL)
{
}

CDBOpenRecord::~CDBOpenRecord(void)
{
}

//����SQL������
bool CDBOpenRecord::Connect(CString strMemberID)
{
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(g_System->GetPluginByName("Plugin_U7_DBRecord"));
	if (NULL == pU7DBRecord)
		return false;

	if (NULL == m_pSQLiteDB)
	{
		m_pSQLiteDB = pU7DBRecord->CreateSQLiteDB();
	}

	//hxl 2012 9 14 �����Ƿ���XPE
	CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
	int nIsXPE = ::GetPrivateProfileInt(_T("AVCON_SELECTED"), _T("AVCONXPE"), 0, csINIPath);
	bool bXPE = nIsXPE != 0;

	CString strPath = _T("");
	if (bXPE)
	{
		strPath = _T("d:\\AVCON7_XPE\\user\\")+strMemberID;
	}
	else
	{
		strPath.Format(_T("%suser\\%s"), GetExePath(),strMemberID);
	}
	if ( !MakeDirEx(CW2A(strPath)))
		return false;
	CString strDB = _T("");
	strDB.Format(_T("%s\\AVCONUCC_T.db"), strPath);

	if (!m_pSQLiteDB->Connect(CCommFunction::U2A(strDB)))
	{
		Disconnect();
		return false;
	}

	bool bExsit = m_pSQLiteDB->HasTable("OpenFileRecord");
	if (bExsit)
		return true;

	if (CreateTable())
	{
		return true;
	}

	Disconnect();
	return false;
}

void CDBOpenRecord::Disconnect(void)
{
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord");
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(pIPlugin);
	if (pU7DBRecord == NULL)
		return;

	if (NULL == m_pSQLiteDB)
		return;

	m_pSQLiteDB->Disconnect();
	pU7DBRecord->DestroySQLiteDB(m_pSQLiteDB);
	m_pSQLiteDB = NULL;
}

//��Ӵ�ý���¼
bool CDBOpenRecord::AddOpenRecord( OPENFILE_RECORD& item)
{
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord"));
	if (NULL == pU7DBRecord)
		return false;

	if (NULL == m_pSQLiteDB)
		return false;

	if (item.strFileName.Find(_T("\'")) >= 0)
	{
		item.strFileName.Replace(_T("\'"),_T("\""));
	}
	// ��ѯ��ý���¼�Ƿ����
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM OpenFileRecord WHERE FilePath = '%s' AND FileName = '%s'"), 
		item.strFilePath, item.strFileName);

	IPlugin_U7_SQLiteResultSet* rs = m_pSQLiteDB->SQLQuery(CCommFunction::U2A(strSQL));
	if (!rs->End()) // ��¼�Ѵ���
	{
		strSQL.Format(_T("UPDATE OpenFileRecord SET DateTime = '%s' WHERE FilePath = '%s' AND FileName = '%s'"), 
			item.strDateTime, item.strFilePath, item.strFileName);
	}
	else // ��¼������
	{
		strSQL.Format(_T("INSERT INTO OpenFileRecord(FilePath, FileName, DateTime) VALUES('%s', '%s', '%s')"), 
			item.strFilePath, item.strFileName, item.strDateTime);
	}
	rs->Close();

	long lRet = m_pSQLiteDB->SQLExecute(CCommFunction::U2A(strSQL));
	DeleteMoreRecord();

	if ( lRet > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//��ȡ��ý��ļ�¼
void CDBOpenRecord::ReadOpenFileRecord(MAP_OPENFILE_RECORD& map)
{
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord"));
	if (NULL == pU7DBRecord)
		return;

	if (NULL == m_pSQLiteDB)
		return;

	map.clear();
	CString strSQL = _T("");
	strSQL = _T("SELECT * FROM OpenFileRecord ORDER BY DateTime DESC");
	std::string strTemp = CCommFunction::U2A(strSQL);
	IPlugin_U7_SQLiteResultSet* rs = m_pSQLiteDB->SQLQuery(strTemp);

	int i = 0;
	while (!rs->End())
	{
		OPENFILE_RECORD item;
		item.strFilePath = CCommFunction::A2U(rs->GetStrField("FilePath").c_str());
		item.strFileName = CCommFunction::A2U(rs->GetStrField("FileName").c_str());
		if (item.strFileName.Find(_T("\"")) >= 0)
		{
			item.strFileName.Replace(_T("\""),_T("\'"));
		}
		item.strDateTime = CCommFunction::A2U(rs->GetStrField("DateTime").c_str());
		map[i] = item;
		rs->MoveNext(); //�ƶ�����һ����¼
		i++;
	}
	rs->Close();
}

//ɾ����ý���¼
bool CDBOpenRecord::DelAllOpenRecord()
{
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord"));
	if (NULL == pU7DBRecord)
		return false;

	if (NULL == m_pSQLiteDB)
		return false;

	std::string strSQL = "DROP TABLE OpenFileRecord";
	long lRet = m_pSQLiteDB->SQLExecute(strSQL);
	if ( lRet > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ����ý���¼SQL��
bool CDBOpenRecord::CreateTable(void)
{
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord");
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(pIPlugin);
	if (pU7DBRecord == NULL)
		return false;

	if (NULL == m_pSQLiteDB)
		return false;

	std::string strSQL = "CREATE TABLE OpenFileRecord(FilePath varchar(1024), FileName varchar(255), DateTime varchar(16))";
	unsigned long l = m_pSQLiteDB->SQLExecute(strSQL);

	if (l == 0)
		return true;
	else
		return false;
}

//ɾ������20��ý���¼����ʱ����õ���ɾ��
void CDBOpenRecord::DeleteMoreRecord(void)
{
	IPlugin_U7_DBRecord* pU7DBRecord = dynamic_cast<IPlugin_U7_DBRecord*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_DBRecord"));
	if (NULL == pU7DBRecord)
		return;

	if (NULL == m_pSQLiteDB)
		return;

	CString strSQL = _T("SELECT count(*) FROM OpenFileRecord");
	IPlugin_U7_SQLiteResultSet* rs = m_pSQLiteDB->SQLQuery(CCommFunction::U2A(strSQL));
	int nCount = 0; 
	if (!rs->End())
	{
		nCount = rs->GetIntField("count");
	}
	rs->Close();

	if (nCount>20)
	{
		//ɾ����õĵļ�¼
		strSQL.Format(_T("DELETE FROM OpenFileRecord WHERE DateTime=(SELECT min(DateTime) FROM OpenFileRecord)"));
		IPlugin_U7_SQLiteResultSet* rs = m_pSQLiteDB->SQLQuery(CCommFunction::U2A(strSQL));
		rs->Close();
	}
}
