#include "stdafx.h"
#include "tstring.h"
DRAWSTR ToDRAWStr(const std::wstring &p,UINT CodePage /*= CP_ACP*/)
{
	return p;
}

DRAWSTR ToDRAWStr(const std::string &p,UINT CodePage /*= CP_ACP*/)
{
	DWORD  UniCodeLen=MultiByteToWideChar(CodePage, 0, p.c_str(), -1, 0, 0);
	DRAWCHAR *pStr = new(std::nothrow) DRAWCHAR[UniCodeLen+1];//预留结尾符
	if (pStr != NULL)
	{
		ZeroMemory(pStr,sizeof(DRAWCHAR)*(UniCodeLen+1));
		::MultiByteToWideChar(CodePage ,NULL,p.c_str(),p.length(),pStr,UniCodeLen);
		DRAWSTR dStr = pStr;
		delete[] pStr;
		return dStr;
	}
	return DRAWSTR(L"");
}

DRAWCHAR* dcscpy(DRAWCHAR* _Dest,const DRAWCHAR* _Source)
{
	return wcscpy(_Dest,_Source);
}

int dcslen( const DRAWCHAR* _Source )
{
	return wcslen(_Source);
}

tstring ToTStr( const std::wstring &p ,UINT CodePage /*= CP_ACP*/)
{
#ifdef UNICODE
	return ToDRAWStr(p,CodePage);
#else
	return ToStr(p,CodePage);
#endif
}

tstring ToTStr( const std::string &p ,UINT CodePage /*= CP_ACP*/)
{
#ifdef UNICODE
	return ToDRAWStr(p,CodePage);
#else
	return ToStr(p,CodePage);
#endif
}

std::string ToStr( const std::wstring &p ,UINT CodePage /*= CP_ACP*/)
{
	DWORD dwALen=WideCharToMultiByte(CodePage, 0, p.c_str(), -1 , NULL, NULL, NULL, NULL );
	CHAR *str = new(std::nothrow) CHAR[dwALen+1];
	
	if (str != NULL)
	{
		ZeroMemory(str,sizeof(CHAR)*(dwALen+1));
		WideCharToMultiByte(CodePage,NULL,p.c_str(),p.length(),str,dwALen,NULL,NULL);
		std::string rstr = str;
		delete[] str;
		return rstr;
	}
	return std::string("");
}

std::string ToStr( const std::string &p ,UINT CodePage /*= CP_ACP*/)
{
	return p;
}

int AToUtf8(const char *pASCIIBuf,char *pUtf8Buf /*= NULL*/,UINT CodePage /*= CP_ACP*/)
{
	if (NULL==pASCIIBuf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CodePage, 0, pASCIIBuf, -1, 0, 0);
	wchar_t *UniCodeStr = new(std::nothrow) wchar_t[UniCodeLen+1];
	
	DWORD dwUtf8Len = 0;
	if (UniCodeStr != NULL)
	{
		ZeroMemory(UniCodeStr,sizeof(wchar_t)*(UniCodeLen+1));
		MultiByteToWideChar(CodePage, 0, pASCIIBuf, -1, UniCodeStr, UniCodeLen);
		dwUtf8Len=WideCharToMultiByte(CP_UTF8, 0, UniCodeStr, -1 , NULL, NULL, NULL, NULL );
		if (NULL!=pUtf8Buf)
		{
			ZeroMemory(pUtf8Buf,sizeof(char)*dwUtf8Len);
			dwUtf8Len = WideCharToMultiByte(CP_UTF8, 0, UniCodeStr, UniCodeLen , pUtf8Buf, dwUtf8Len, NULL, NULL );	
		}
		delete[] UniCodeStr;
	}
	return dwUtf8Len;
}

int Utf8ToA(const char *pUtf8Buf,char *pASCIIBuf /*= NULL*/,UINT CodePage /*= CP_ACP*/)
{
	if (NULL==pUtf8Buf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, 0, 0);
	wchar_t *UniCodeStr = new(std::nothrow) wchar_t[UniCodeLen+1];
	
	DWORD dwALen = 0;
	if (UniCodeStr != NULL)
	{
		ZeroMemory(UniCodeStr,sizeof(wchar_t)*(UniCodeLen+1));
		MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, UniCodeStr, UniCodeLen);
		DWORD dwALen=WideCharToMultiByte(CodePage, 0, UniCodeStr, -1 , NULL, NULL, NULL, NULL );
		if (NULL!=pASCIIBuf)
		{
			ZeroMemory(pASCIIBuf,sizeof(char)*dwALen);
			dwALen = WideCharToMultiByte(CodePage, 0, UniCodeStr, UniCodeLen , pASCIIBuf, dwALen, NULL, NULL );	
		}
		delete[] UniCodeStr;
	}
	return dwALen;
}

int Utf8ToUnicode(LPCSTR pUtf8Buf,LPWSTR pUNICODEBuf /*= NULL*/)
{
	if (NULL==pUtf8Buf) return 0;
	DWORD  UniCodeLen=MultiByteToWideChar(CP_UTF8, 0, pUtf8Buf, -1, 0, 0);
	if (NULL != pUNICODEBuf)
	{
		ZeroMemory(pUNICODEBuf,sizeof(wchar_t)*UniCodeLen);
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
		ZeroMemory(pUtf8Buf,sizeof(char)*dwUtf8Len);
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

unsigned long HashString( char *lpszFileName, unsigned long dwHashType )
{   
	static unsigned long cryptTable[0x500] = {0};//保存的下面将会用到的一些数据
	static bool bInitialized = false;
	if (!bInitialized)
	{
		//生成一个长度为0x500（合10进制数：1280）的cryptTable[0x500]  
		unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
		for( index1 = 0; index1 < 0x100; index1++ )  
		{   
			for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 )  
			{   
				unsigned long temp1, temp2;  

				seed = (seed * 125 + 3) % 0x2AAAAB;  
				temp1 = (seed & 0xFFFF) << 0x10;  

				seed = (seed * 125 + 3) % 0x2AAAAB;  
				temp2 = (seed & 0xFFFF);  

				cryptTable[index2] = ( temp1 | temp2 );   
			}   
		} 
		bInitialized = true;
	}

	unsigned char *key  = (unsigned char *)lpszFileName;  
	unsigned long seed1 = 0x7FED7FED;  
	unsigned long seed2 = 0xEEEEEEEE;  
	int ch;  
	while( *key != 0 )  
	{   
		ch = toupper(*key++);  
		seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);  
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;   
	}  
	return seed1;   
} 