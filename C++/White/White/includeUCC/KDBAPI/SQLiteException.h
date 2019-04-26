#pragma once

#include "sqlite/sqlite3.h"

#define KSQLITE_ERROR 1000
static const bool DONT_DELETE_MSG=false;

//---------------------------------------------------------------------------------------
// SQLiteException ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteException
{
public:
    SQLiteException(const int nErrCode,char* szErrMess,bool bDeleteMsg=true);
    SQLiteException(const SQLiteException&  e);
    virtual ~SQLiteException();

    const int GetErrorCode(){ return m_nErrCode; }
    const char* GetErrorMessage() { return m_pszErrMess; }
    static const char* GetErrorCodeAsString(int nErrCode);

private:
    int m_nErrCode;
    char* m_pszErrMess;
};
