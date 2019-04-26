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
// SQLiteResultSet类定义
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
// SQLite数据库驱动
class AFX_EXT_CLASS SQLiteDB
{
public:
    SQLiteDB();
    virtual ~SQLiteDB();

	bool Connect(const std::string& strDBName);
	void Disconnect(void);
	bool CheckDB(void);

 	/*********************************************************
	 * 创建SQL语句
	 *********************************************************/
	std::string CreateSQL(const char* cszSQL,...);

	/*********************************************************
	 * 执行SQL语句(INSERT/UPDATE/DELETE)
	 * 返回语句执行后影响的行数
	 *********************************************************/
	unsigned long SQLExecute(const std::string& strSQL);
	
	///*********************************************************
	// * 执行SQL语句(SELECT)
	// * 返回语句执行后的结果集
	// *********************************************************/
	SQLiteResultSet SQLQuery(const std::string& strSQL);

	/*********************************************************
	 * 获得INSERT后的记录ID号
	 *********************************************************/
	unsigned long GetLastInsertID(void);

	/*********************************************************
	 * 是否存在表
	 *********************************************************/
	bool HasTable(const std::string& strTableName);

	/*********************************************************
	 * 获得服务器版本
	 *********************************************************/
    const char* GetVersion();

protected:
	sqlite3*	m_pDB;
	std::string	m_strSQL;
};

#endif