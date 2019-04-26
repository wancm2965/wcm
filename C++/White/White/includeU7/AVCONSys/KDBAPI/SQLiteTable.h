#pragma once

//---------------------------------------------------------------------------------------
// SQLiteTable ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteTable
{
public:
    SQLiteTable();
    SQLiteTable(const SQLiteTable& rTable);
    SQLiteTable(char** paszResults, int nRows, int nCols);
    virtual ~SQLiteTable();
    SQLiteTable& operator=(const SQLiteTable& rTable);

    int FieldCount();
    int RowCount();
    const char* FieldName(int nCol);
    const char* FieldValue(int nField);
    const char* FieldValue(const char* szField);
    int GetIntField(int nField, int nNullValue=0);
    int GetIntField(const char* szField, int nNullValue=0);
    double GetFloatField(int nField, double fNullValue=0.0);
    double GetFloatField(const char* szField, double fNullValue=0.0);
    const char* GetStringField(int nField, const char* szNullValue="");
    const char* GetStringField(const char* szField, const char* szNullValue="");
    bool FieldIsNull(int nField);
    bool FieldIsNull(const char* szField);
    void SetRow(int nRow);
    void Finalize();

private:
    void CheckResults();
    int m_nCols;
    int m_nRows;
    int m_nCurrentRow;
    char** m_paszResults;
};
