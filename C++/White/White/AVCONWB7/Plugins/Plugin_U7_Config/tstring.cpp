#include "tstring.h"
DRAWSTR ToDRAWStr(const std::wstring p)
{
	return p;
}

DRAWSTR ToDRAWStr(const std::string p)
{
	TCHAR pStr[MAX_PATH] = {0};
	::MultiByteToWideChar(CP_ACP ,NULL,p.c_str(),p.length(),pStr,MAX_PATH);
	return DRAWSTR(pStr);
}

DRAWCHAR* dcscpy(DRAWCHAR* _Dest,const DRAWCHAR* _Source)
{
	return wcscpy(_Dest,_Source);
}

int dcslen( const DRAWCHAR* _Source )
{
	return wcslen(_Source);
}

tstring ToTStr( const std::wstring p )
{
#ifdef UNICODE
	return ToDRAWStr(p);
#else
	return ToStr(p);
#endif
}

tstring ToTStr( const std::string p )
{
#ifdef UNICODE
	return ToDRAWStr(p);
#else
	return ToStr(p);
#endif
}

std::string ToStr( const std::wstring p )
{
	CHAR str[MAX_PATH] = {0};
	WideCharToMultiByte(CP_ACP,NULL,p.c_str(),p.length(),str,MAX_PATH,NULL,NULL);
	return std::string(str);
}

std::string ToStr( const std::string p )
{
	return p;
}

int Utf8ToA(LPCSTR pUtf8Buf,LPSTR pASCIIBuf)
{
	if (NULL==pUtf8Buf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, 0, 0);
	TCHAR *UniCodeStr = new TCHAR[UniCodeLen+1];
	ZeroMemory(UniCodeStr,UniCodeLen);
	MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, UniCodeStr, UniCodeLen);
	DWORD dwALen=WideCharToMultiByte(CP_ACP, 0, UniCodeStr, UniCodeLen , NULL, NULL, NULL, NULL );
	if (NULL!=pASCIIBuf)
	{
		ZeroMemory(pASCIIBuf,dwALen);
		dwALen = WideCharToMultiByte(CP_ACP, 0, UniCodeStr, UniCodeLen , pASCIIBuf, dwALen, NULL, NULL );	
	}
	delete[] UniCodeStr;
	return dwALen;
}
int Utf8ToUnicode(LPCSTR pUtf8Buf,LPWSTR pUNICODEBuf)
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