#include "tstring.h"

DRAWSTR ToDRAWStr(const std::wstring p,UINT CodePage /*= CP_ACP*/)
{
	return p;
}

DRAWSTR ToDRAWStr(const std::string p,UINT CodePage /*= CP_ACP*/)
{
	if (p.empty())
	{
		return _T("");
	}
	DWORD  UniCodeLen=MultiByteToWideChar(CodePage, 0, p.c_str(), -1, 0, 0);
	DRAWCHAR *pStr = new DRAWCHAR[UniCodeLen+1];//Ô¤Áô½áÎ²·û
	::MultiByteToWideChar(CodePage ,NULL,p.c_str(),-1,pStr,UniCodeLen);
	DRAWSTR dStr = pStr;
	delete[] pStr;
	return dStr;
}

DRAWCHAR* dcscpy(DRAWCHAR* _Dest,const DRAWCHAR* _Source)
{
	return wcscpy(_Dest,_Source);
}

int dcslen( const DRAWCHAR* _Source )
{
	return wcslen(_Source);
}

tstring ToTStr( const std::wstring p ,UINT CodePage /*= CP_ACP*/)
{
#ifdef UNICODE
	return ToDRAWStr(p,CodePage);
#else
	return ToStr(p,CodePage);
#endif
}

tstring ToTStr( const std::string p ,UINT CodePage /*= CP_ACP*/)
{
#ifdef UNICODE
	return ToDRAWStr(p,CodePage);
#else
	return ToStr(p,CodePage);
#endif
}

std::string ToStr( const std::wstring p ,UINT CodePage /*= CP_ACP*/)
{
	if (p.empty())
	{
		return "";
	}
	DWORD dwALen=WideCharToMultiByte(CodePage, 0, p.c_str(), -1 , NULL, NULL, NULL, NULL );
	CHAR *str = new CHAR[dwALen+1];
	WideCharToMultiByte(CodePage,NULL,p.c_str(),-1,str,dwALen,NULL,NULL);
	std::string rstr = str;
	delete[] str;
	return rstr;
}

std::string ToStr( const std::string p ,UINT CodePage /*= CP_ACP*/)
{
	return p;
}

// CString A2U(std::string sstring)
// {
// 	if (sstring.empty())
// 	{
// 		return _T("");
// 	}
// 	int cchStr = (int) sstring.length() + 1;
// 	wchar_t* pwstr = new wchar_t[cchStr];
// 	if( pwstr != NULL )
// 		::MultiByteToWideChar(CP_UTF8, 0, sstring.c_str(), -1, pwstr, cchStr);
// 
// 	CString strTmp(pwstr);
// 	delete []pwstr;
// 
// 	return strTmp;
// }
// 
// std::string U2A(CString strUnicodeString)
// {
// 	if (strUnicodeString.IsEmpty())
// 	{
// 		return "";
// 	}
// 	int cchStr = ::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, wcslen(strUnicodeString), NULL, 0, NULL, NULL);
// 	char* pstr = new char[cchStr + 1];
// 	if( pstr != NULL )
// 		::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, -1, pstr, cchStr, NULL, NULL);
// 	pstr[cchStr] = '\0';
// 	std::string str(pstr);
// 	delete pstr;
// 	return str;
// }

int AToUtf8(const char *pASCIIBuf,char *pUtf8Buf /*= NULL*/,UINT CodePage /*= CP_ACP*/)
{
	if (NULL==pASCIIBuf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CodePage, 0, pASCIIBuf, -1, 0, 0);
	wchar_t *UniCodeStr = new wchar_t[UniCodeLen+1];
	ZeroMemory(UniCodeStr,UniCodeLen);
	MultiByteToWideChar(CodePage, 0, pASCIIBuf, -1, UniCodeStr, UniCodeLen);
	DWORD dwUtf8Len=WideCharToMultiByte(CP_UTF8, 0, UniCodeStr, UniCodeLen , NULL, NULL, NULL, NULL );
	if (NULL!=pUtf8Buf)
	{
		ZeroMemory(pUtf8Buf,dwUtf8Len);
		dwUtf8Len = WideCharToMultiByte(CP_UTF8, 0, UniCodeStr, UniCodeLen , pUtf8Buf, dwUtf8Len, NULL, NULL );	
	}
	delete[] UniCodeStr;
	return dwUtf8Len;
}

int Utf8ToA(const char *pUtf8Buf,char *pASCIIBuf /*= NULL*/,UINT CodePage /*= CP_ACP*/)
{
	if (NULL==pUtf8Buf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, 0, 0);
	wchar_t *UniCodeStr = new wchar_t[UniCodeLen+1];
	ZeroMemory(UniCodeStr,UniCodeLen);
	MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, UniCodeStr, UniCodeLen);
	DWORD dwALen=WideCharToMultiByte(CodePage, 0, UniCodeStr, UniCodeLen , NULL, NULL, NULL, NULL );
	if (NULL!=pASCIIBuf)
	{
		ZeroMemory(pASCIIBuf,dwALen);
		dwALen = WideCharToMultiByte(CodePage, 0, UniCodeStr, UniCodeLen , pASCIIBuf, dwALen, NULL, NULL );	
	}
	delete[] UniCodeStr;
	return dwALen;
}

int Utf8ToUnicode(LPCSTR pUtf8Buf,LPWSTR pUNICODEBuf /*= NULL*/)
{
	if (NULL==pUtf8Buf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, 0, 0);
	if (NULL != pUNICODEBuf)
	{
		ZeroMemory(pUNICODEBuf,UniCodeLen);
		MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, pUNICODEBuf, UniCodeLen);
	}
	return UniCodeLen;
}

int UnicodeToUtf8( const wchar_t *pUNICODEBuf,char *pUtf8Buf /*= NULL*/ )
{
	if (NULL==pUNICODEBuf) return 0;
	DWORD dwUtf8Len=WideCharToMultiByte(CP_UTF8, 0, pUNICODEBuf, -1 , NULL, NULL, NULL, NULL );
	if (NULL!=pUtf8Buf)
	{
		ZeroMemory(pUtf8Buf,dwUtf8Len);
		dwUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pUNICODEBuf, -1 , pUtf8Buf, dwUtf8Len, NULL, NULL );	
	}
	return dwUtf8Len;
}

int ToUtf8( const char *pASCIIBuf,char* pUtf8Buf /*= NULL*/ ,UINT CodePage /*= CP_ACP*/)
{
	return AToUtf8(pASCIIBuf,pUtf8Buf,CodePage);
}

int ToUtf8( const wchar_t *pUNICODEBuf,char *pUtf8Buf /*= NULL*/,UINT CodePage /*= CP_ACP*/ )
{
	return UnicodeToUtf8(pUNICODEBuf,pUtf8Buf);
}
