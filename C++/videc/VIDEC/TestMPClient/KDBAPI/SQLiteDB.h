/**********************************************************
 * Name: MySQLDB.h
 * Author: zhouxiaochuan@gmail.com
 * Date: 2007-12-28
 **********************************************************/

#pragma once

#ifdef HAS_SQLITE

#include <string>
#include <exception>
#include "sqlite/sqlite3.h"
#include "DBDefine.h"

//---------------------------------------------------------------------------------------
// SQLiteResultSet�ඨ��
class AFX_EXT_CLASS SQLiteResultSet
{
public:
    SQLiteResultSet();
	SQLiteResultSet(const SQLiteResultSet& rs);
    SQLiteResultSet(sqlite3* pDB,sqlite3_stmt* pResultSet,bool bEnd);
    virtual ~SQLiteResultSet();

	void Close();
	bool End();
	void MoveNext();

	int GetFieldCols(void);
	std::string GetStrField(int nCol);
	std::string GetStrField(const std::string& strFieldName);

	int GetIntField(int nCol);
	int GetIntField(const std::string& strFieldName);

	unsigned long GetInt64Field(int nCol);
	unsigned long GetInt64Field(const std::string& strFieldName);

	double GetFloatField(int nCol);
	double GetFloatField(const std::string& strFieldName);

	const unsigned char* GetBlobField(int nCol, int& nLen);
	const unsigned char* GetBlobField(const std::string& strFieldName, int& nLen);

protected:
	sqlite3*		m_pDB;
    sqlite3_stmt*	m_pResultSet;
    bool			m_bEnd;

	int GetFieldCol(const std::string& strFieldName);
};

//---------------------------------------------------------------------------------------
// SQLite���ݿ�����
class AFX_EXT_CLASS SQLiteDB
{
public:
    SQLiteDB();
    virtual ~SQLiteDB();

	bool Connect(const std::string& strDBName);
	void Disconnect(void);
	bool CheckDB(void);

 	/*********************************************************
	 * ����SQL���
	 *********************************************************/
	std::string CreateSQL(const char* cszSQL,...);

	/*********************************************************
	 * ִ��SQL���(INSERT/UPDATE/DELETE)
	 * �������ִ�к�Ӱ�������
	 *********************************************************/
	unsigned long SQLExecute(const std::string& strSQL);
	
	///*********************************************************
	// * ִ��SQL���(SELECT)
	// * �������ִ�к�Ľ����
	// *********************************************************/
	SQLiteResultSet SQLQuery(const std::string& strSQL);

	/*********************************************************
	 * ���INSERT��ļ�¼ID��
	 *********************************************************/
	unsigned long GetLastInsertID(void);

	/*********************************************************
	 * �Ƿ���ڱ�
	 *********************************************************/
	bool HasTable(const std::string& strTableName);

	/*********************************************************
	 * ��÷������汾
	 *********************************************************/
    const char* GetVersion();

protected:
	sqlite3*	m_pDB;
	std::string	m_strSQL;
};

#endif