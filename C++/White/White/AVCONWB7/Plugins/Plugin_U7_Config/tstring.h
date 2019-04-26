/* * * * * * * * * * * * * * * * * * * *
*名称：win32兼容类字符串操作           *
*功能：增强win32字符串对编码的兼容能力 *
*日期：2013年4月15日                   *
* * * * * * * * * * * * * * * * * * * */
#include <windows.h>
#include <string>
#include <assert.h>
#define DRAWTEXT(quote) (L##quote)
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
typedef TCHAR DRAWCHAR;
typedef std::wstring DRAWSTR;
std::string ToStr(const std::wstring p);
std::string ToStr(const std::string p);
tstring ToTStr(const std::wstring p);
tstring ToTStr(const std::string p);
DRAWSTR ToDRAWStr(const std::wstring p);
DRAWSTR ToDRAWStr(const std::string p);
DRAWCHAR* dcscpy(DRAWCHAR* _Dest,const DRAWCHAR* _Source);
int dcslen(const DRAWCHAR* _Source);
int Utf8ToA(LPCSTR pUtf8Buf,LPSTR pASCIIBuf=NULL);
int Utf8ToUnicode(LPCSTR pUtf8Buf,LPWSTR pUNICODEBuf=NULL);