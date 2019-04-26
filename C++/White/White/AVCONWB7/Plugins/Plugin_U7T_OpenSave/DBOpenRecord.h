#pragma once
#include "IPlugin/IU7_DB/IPlugin_U7_DBRecord.h"

using namespace AVCONPlugin;

typedef struct OPENFILE_RECORD
{
	CString	strFilePath;
	CString	strFileName;
	CString strDateTime;

	OPENFILE_RECORD()
	{
		strFilePath	= _T("");
		strFileName	= _T("");
		strDateTime	= _T("");
	}
}OPENFILE_RECORD,*POPENFILE_RECORD;

typedef std::map<int, OPENFILE_RECORD> MAP_OPENFILE_RECORD;
typedef MAP_OPENFILE_RECORD::iterator MAP_OPENFILE_RECORD_IT;

class CDBOpenRecord
{
public:
	CDBOpenRecord(void);
	~CDBOpenRecord(void);

public:
	// �������ݿ�
	bool Connect(CString strMemberID);
	// �Ͽ����ݿ�
	void Disconnect(void);
	// ��Ӽ�¼
	bool AddOpenRecord( OPENFILE_RECORD& item);
	// ��ȡ��¼
	void ReadOpenFileRecord(MAP_OPENFILE_RECORD& map);
	// ɾ�����м�¼
	bool DelAllOpenRecord();

private:
	// ������
	bool CreateTable(void);
	// ɾ�������¼
	void DeleteMoreRecord();

	IPlugin_U7_SQLiteDB* m_pSQLiteDB;
};
