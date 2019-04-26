#pragma once

#include "sqlite/sqlite3.h"

//---------------------------------------------------------------------------------------
// SQLiteQuery ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteQuery
{
public:
    SQLiteQuery();
    SQLiteQuery(const SQLiteQuery& rQuery);
    SQLiteQuery(sqlite3* pDB,sqlite3_stmt* pStmt,bool bEof,bool bOwnStmt=true);
    SQLiteQuery& operator=(const SQLiteQuery& rQuery);
    virtual ~SQLiteQuery();

    int FieldCount(void);
    int FieldIndex(const char* szField);
    const char* FieldName(int nCol);

    const char* FieldDeclType(int nCol);
    int FieldDataType(int nCol);

    const char* FieldValue(int nField);
    const char* FieldValue(const char* szField);

    int GetIntField(int nField, int nNullValue=0);
    int GetIntField(const char* szField, int nNullValue=0);

    double GetFloatField(int nField, double fNullValue=0.0);
    double GetFloatField(const char* szField, double fNullValue=0.0);

    const char* GetStringField(int nField, const char* szNullValue="");
    const char* GetStringField(const char* szField, const char* szNullValue="");

    const unsigned char* GetBlobField(int nField, int& nLen);
    const unsigned char* GetBlobField(const char* szField, int& nLen);

    bool FieldIsNull(int nField);
    bool FieldIsNull(const char* szField);

    bool Eof();
    void NextRow();
    void Finalize();

private:
	sqlite3* m_pDB;
    sqlite3_stmt* m_pStmt;
    bool m_bEof;
    int m_nCols;
    bool m_bOwnStmt;
};
