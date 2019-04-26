/* * * * * * * * * * * * * * * * * * * *
*名称：win32兼容类字符串操作           *
*功能：增强win32字符串对编码的兼容能力 *
*日期：2013年5月18日                   *
*版本：2.2(正式版)                     *
* * * * * * * * * * * * * * * * * * * */
#pragma once
#include <windows.h>
#include <string>
#include <tchar.h>
#include <assert.h>
#define DRAWTEXT(quote) (L##quote)
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
typedef wchar_t DRAWCHAR;
typedef std::wstring DRAWSTR;
std::string ToStr(const std::wstring &p,UINT CodePage = CP_ACP);
std::string ToStr(const std::string &p,UINT CodePage = CP_ACP);
tstring ToTStr(const std::wstring &p,UINT CodePage = CP_ACP);
tstring ToTStr(const std::string &p,UINT CodePage = CP_ACP);
DRAWSTR ToDRAWStr(const std::wstring &p,UINT CodePage = CP_ACP);
DRAWSTR ToDRAWStr(const std::string &p,UINT CodePage = CP_ACP);
DRAWCHAR* dcscpy(DRAWCHAR* _Dest,const DRAWCHAR* _Source);
int dcslen(const DRAWCHAR* _Source);

int AToUtf8(const char *pASCIIBuf,char* pUtf8Buf = NULL,UINT CodePage = CP_ACP);
int UnicodeToUtf8(const wchar_t *pUNICODEBuf,char *pUtf8Buf = NULL);
int ToUtf8(const char *pASCIIBuf,char* pUtf8Buf = NULL,UINT CodePage = CP_ACP);
int ToUtf8(const wchar_t *pUNICODEBuf,char *pUtf8Buf = NULL,UINT CodePage = CP_ACP);

int Utf8ToA(const char *pUtf8Buf,char* pASCIIBuf = NULL,UINT CodePage = CP_ACP);
int Utf8ToUnicode(const char *pUtf8Buf,wchar_t *pUNICODEBuf = NULL);
//以下函数计算lpszFileName 字符串的hash值，其中dwHashType 为hash的类型，
//其可以取的值为0、1、2；该函数返回lpszFileName 字符串的hash值；  
unsigned long HashString( char *lpszFileName, unsigned long dwHashType );

