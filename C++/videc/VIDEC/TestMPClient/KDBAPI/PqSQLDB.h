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
// PgSQL���ݿ�����
class AFX_EXT_CLASS PqSQLDB
{
public:
	PqSQLDB(void);
	virtual ~PqSQLDB(void);

	bool Connect(const std::string& strDBHost,unsigned short usDBPort,const std::string& strDBName,const std::string& strDBUser,const std::string& strDBPass);
	void Disconnect(void);
	bool CheckDB(void);

	/*********************************************************
	 * ����SQL���
	 *********************************************************/
	std::string CreateSQL(const char* cszSQL,...);

	/*********************************************************
	 * ִ��SQL���(INSERT/UPDATE/DELETE)
	 * �������ִ�к�Ӱ�������,���Ϊ-1���ʾִ�д���
	 *********************************************************/
	bool SQLExecute(const std::string& strSQL);

	/*********************************************************
	 * ִ��SQL���(SELECT)
	 * �������ִ�к�Ľ����
	 *********************************************************/
	PqSQLResultSet SQLQuery(const std::string& strSQL);

	/*********************************************************
	 * Escape�ַ���
	 *********************************************************/
	const char* Escape(const std::string& strValue);

	/*********************************************************
	 * ��÷������汾
	 *********************************************************/
	unsigned long GetServerVersion(void);

	/*********************************************************
	 * ���Э��汾
	 *********************************************************/
	unsigned long GetProtocolVersion(void);
	
	/*********************************************************
	 * ��ʾ������Ϣ
	 *********************************************************/
	void ShowLastError(void);

protected:
	PGconn*		m_pDB;

private:
	std::string m_strConnectInfo;
};

#endif //HAS_PQSQL