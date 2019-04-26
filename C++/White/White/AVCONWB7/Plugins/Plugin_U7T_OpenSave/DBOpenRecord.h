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
	// 连接数据库
	bool Connect(CString strMemberID);
	// 断开数据库
	void Disconnect(void);
	// 添加记录
	bool AddOpenRecord( OPENFILE_RECORD& item);
	// 读取记录
	void ReadOpenFileRecord(MAP_OPENFILE_RECORD& map);
	// 删除所有记录
	bool DelAllOpenRecord();

private:
	// 创建表
	bool CreateTable(void);
	// 删除多余记录
	void DeleteMoreRecord();

	IPlugin_U7_SQLiteDB* m_pSQLiteDB;
};
