/**********************************************************
 * Name: MySQLDB.h
 * Author: zhouxiaochuan@gmail.com
 * Date: 2006-10-09
 **********************************************************/

#pragma once

#include <map>
#include <string>

#ifdef HAS_MYSQL

#ifdef WIN32
#include "mysql/mysql.h"
#else
#include "mysql.h"
#endif
#include "DBDefine.h"

typedef std::map<std::string,std::string> MySQLRow;
typedef std::map<unsigned long,MySQLRow> MySQLResultSet;

typedef struct _MYSQL_CONNECTION
{
	std::string	dbhost;
	unsigned short dbport;
	std::string dbname;
	std::string dbuser;
	std::string dbpass;
	_MYSQL_CONNECTION()
	{
		dbport=3306;
	};
}MYSQL_CONNECTION;

//---------------------------------------------------------------------------------------
// MySQL���ݿ�����
class AFX_EXT_CLASS MySQLDB
{
public:
	MySQLDB(void);
	virtual ~MySQLDB(void);

	bool Connect(MYSQL_CONNECTION SingleConnection,bool bUTF8=false,bool bStoreResult=false);
	bool Connect(MYSQL_CONNECTION WriteConnection,MYSQL_CONNECTION ReadConnection,bool bUTF8=false,bool bStoreResult=false);
	void Disconnect(void);

	/*********************************************************
	 * ������ݿ�(������ݿ�û���������Զ�����)
	 *********************************************************/
	bool CheckDB(void);

	/*********************************************************
	 * ѡ�����ݿ�
	 *********************************************************/
	void SelectDB(const std::string& strDBName);

	/*********************************************************
	 * ����SQL���
	 *********************************************************/
	std::string CreateSQL(const char* cszSQL,...);

	/*********************************************************
	 * ִ��SQL���(INSERT/UPDATE/DELETE)
	 * �������ִ�к�Ӱ�������,���Ϊ-1���ʾִ�д���
	 *********************************************************/
	int SQLExecute(const std::string& strSQL);

	/*********************************************************
	 * ִ��SQL���(SELECT)
	 * �������ִ�к�Ľ����
	 *********************************************************/
	int SQLQuery(const std::string& strSQL,MySQLResultSet& res);

	/*********************************************************
	 * ���INSERT��ļ�¼ID��
	 *********************************************************/
	unsigned long GetLastInsertID(void);

	/*********************************************************
	 * Escape�ַ���
	 *********************************************************/
	const char* Escape(const std::string& strValue);

	/*********************************************************
	 * PING������
	 * ������������������Ч����0��������ִ��󣬷��ط�0ֵ��
	 * ���صķ�0ֵ����ʾMySQL�����������Ƿ��ѹرգ�
	 * ���ӿ���������ԭ���նˣ�����������ȡ�
	 *********************************************************/
	int PingSingleDB(void);
	int PingWriteDB(void);
	int PingReadDB(void);

	/*********************************************************
	 * ��÷������汾
	 * ��ʾMySQL�������汾����ֵ����ʽ���£�
	 * major_version*10000 + minor_version *100 + sub_version
	 * ���磬����5.0.12������500012
	 *********************************************************/
	unsigned long GetSingleServerVersion(void);
	unsigned long GetWriteServerVersion(void);
	unsigned long GetReadServerVersion(void);

	/*********************************************************
	 * ��ÿͻ��˰汾
	 * ��ʾMySQL�ͻ��˰汾����ֵ����ʽ���£�
	 * major_version*10000 + minor_version *100 + sub_version
	 * ���磬����5.0.12������500012
	 *********************************************************/
	unsigned long GetClientVersion(void);
	
protected:
	MYSQL*		m_pSingleDB;
	MYSQL*		m_pWriteDB;
	MYSQL*		m_pReadDB;
	void ShowLastError(MYSQL* pDB,const std::string& strMysql="");

private:
	bool				m_bSingle;
	MYSQL_CONNECTION	m_SingleConnection;
	MYSQL_CONNECTION	m_WriteConnection;
	MYSQL_CONNECTION	m_ReadConnection;
	bool				m_bUTF8;
	bool				m_bStoreResult;
};

#endif //HAS_MYSQL