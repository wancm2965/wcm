#pragma once

int sqlite3_encode_binary(const unsigned char *in, int n, unsigned char *out);
int sqlite3_decode_binary(const unsigned char *in, unsigned char *out);

//---------------------------------------------------------------------------------------
// SQLiteBinary ¿‡∂®“Â
class AFX_EXT_CLASS SQLiteBinary
{
public:
    SQLiteBinary();
    ~SQLiteBinary();

    void SetBinary(const unsigned char* pBuf, int nLen);
    void SetEncoded(const unsigned char* pBuf);

    const unsigned char* GetEncoded();
    const unsigned char* GetBinary();
    int GetBinaryLength();
    unsigned char* AllocBuffer(int nLen);
    void Clear();

private:
    unsigned char* m_pBuf;
    int m_nBinaryLen;
    int m_nBufferLen;
    int m_nEncodedLen;
    bool m_bEncoded;
};
