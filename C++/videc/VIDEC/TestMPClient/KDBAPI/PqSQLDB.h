/**********************************************************
 * Name: PqSQLDB.h
 * Author: zhouxiaochuan@gmail.com
 * Date: 2009-12-15
 **********************************************************/

#pragma once

#include <map>
#include <string>

#ifdef HAS_PQSQL

#include "pqsql/libpq-fe.h"
#include "DBDefine.h"

typedef std::map<std::string,std::string> PqSQLRow;
typedef std::map<unsigned long,PqSQLRow> PqSQLResultSet;

//---------------------------------------------------------------------------------------
// PgSQL数据库驱动
class AFX_EXT_CLASS PqSQLDB
{
public:
	PqSQLDB(void);
	virtual ~PqSQLDB(void);

	bool Connect(const std::string& strDBHost,unsigned short usDBPort,const std::string& strDBName,const std::string& strDBUser,const std::string& strDBPass);
	void Disconnect(void);
	bool CheckDB(void);

	/*********************************************************
	 * 创建SQL语句
	 *********************************************************/
	std::string CreateSQL(const char* cszSQL,...);

	/*********************************************************
	 * 执行SQL语句(INSERT/UPDATE/DELETE)
	 * 返回语句执行后影响的行数,如果为-1则表示执行错误
	 *********************************************************/
	bool SQLExecute(const std::string& strSQL);

	/*********************************************************
	 * 执行SQL语句(SELECT)
	 * 返回语句执行后的结果集
	 *********************************************************/
	PqSQLResultSet SQLQuery(const std::string& strSQL);

	/*********************************************************
	 * Escape字符串
	 *********************************************************/
	const char* Escape(const std::string& strValue);

	/*********************************************************
	 * 获得服务器版本
	 *********************************************************/
	unsigned long GetServerVersion(void);

	/*********************************************************
	 * 获得协议版本
	 *********************************************************/
	unsigned long GetProtocolVersion(void);
	
	/*********************************************************
	 * 显示错误信息
	 *********************************************************/
	void ShowLastError(void);

protected:
	PGconn*		m_pDB;

private:
	std::string m_strConnectInfo;
};

#endif //HAS_PQSQL