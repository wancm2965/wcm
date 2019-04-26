#pragma once

#include "sqlite/sqlite3.h"

//---------------------------------------------------------------------------------------
// SQLiteBuffer ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteBuffer
{
public:
    SQLiteBuffer();
    ~SQLiteBuffer();
    const char* Format(const char* szFormat, ...);
    operator const char*() { return m_pBuf; }
    void Clear();

private:
    char* m_pBuf;
};
