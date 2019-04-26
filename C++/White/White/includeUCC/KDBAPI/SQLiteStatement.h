#pragma once

#include "SQLiteQuery.h"

//---------------------------------------------------------------------------------------
// SQLiteStatement ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteStatement
{
public:
    SQLiteStatement();
    SQLiteStatement(const SQLiteStatement& rStatement);
	SQLiteStatement(sqlite3* pDB, sqlite3_stmt* pVM);
    virtual ~SQLiteStatement();
    SQLiteStatement& operator=(const SQLiteStatement& rStatement);

    int ExecDML();

    SQLiteQuery ExecQuery();

    void Bind(int nParam, const char* szValue);
    void Bind(int nParam, const int nValue);
    void Bind(int nParam, const double dwValue);
    void Bind(int nParam, const unsigned char* blobValue, int nLen);
    void BindNull(int nParam);

    void Reset();

    void Finalize();

private:

    void CheckDB();
    void CheckVM();

    sqlite3* m_pDB;
    sqlite3_stmt* m_pVM;
};
