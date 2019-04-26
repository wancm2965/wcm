// UCC7-IM登录功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_SQLiteResultSet 
	{
	public:
		virtual ~IPlugin_U7_SQLiteResultSet(){};

	public:
		virtual void Close() = 0;
		virtual bool End() = 0;
		virtual void MoveNext() = 0;

 		virtual int GetFieldCols(void) = 0;
 		virtual std::string GetStrField(int nCol) = 0;
 		virtual std::string GetStrField(const std::string& strFieldName) = 0;
 
 		virtual int GetIntField(int nCol) = 0;
 		virtual int GetIntField(const std::string& strFieldName) = 0;
 
 		virtual unsigned long GetInt64Field(int nCol) = 0;
 		virtual unsigned long GetInt64Field(const std::string& strFieldName) = 0;
 
 		virtual double GetFloatField(int nCol) = 0;
 		virtual double GetFloatField(const std::string& strFieldName) = 0;
 
 		virtual const unsigned char* GetBlobField(int nCol, int& nLen) = 0;
 		virtual const unsigned char* GetBlobField(const std::string& strFieldName, int& nLen) = 0;
	};

	class IPlugin_U7_SQLiteDB
	{
	public:
		virtual ~IPlugin_U7_SQLiteDB(){};

	public:
		virtual bool Connect(const std::string& strDBName) = 0;
		virtual void Disconnect(void) = 0;
		virtual bool CheckDB(void) = 0;

		// 创建SQL语句
		virtual std::string CreateSQL(const char* cszSQL,...) = 0;

		// 执行SQL语句(INSERT/UPDATE/DELETE)
		// 返回语句执行后影响的行数
		virtual unsigned long SQLExecute(const std::string& strSQL) = 0;

		// 获得INSERT后的记录ID号
		virtual unsigned long GetLastInsertID(void) = 0;

		// * 执行SQL语句(SELECT)
		// * 返回语句执行后的结果集
		virtual IPlugin_U7_SQLiteResultSet* SQLQuery(const std::string& strSQL) = 0;

		// 是否存在表
		virtual bool HasTable(const std::string& strTableName) = 0;
	};

	class IPlugin_U7_DBRecord
		: public IPlugin
	{
	public:
		virtual ~IPlugin_U7_DBRecord(){};

	public:
		virtual IPlugin_U7_SQLiteDB* CreateSQLiteDB(void) = 0;
		virtual void DestroySQLiteDB(IPlugin_U7_SQLiteDB* pSQLiteDB) = 0;
	};
}
