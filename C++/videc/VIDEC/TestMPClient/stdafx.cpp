
// stdafx.cpp : 只包括标准包含文件的源文件
// TestMPClient.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



//---------------------------------------------------------------------------------------
//将UTF8字符串解码成通用字符串
CString KA2W(const std::string& strAnsiString)
{
	/*int nUnicodeCharCont=MultiByteToWideChar(CP_UTF8,0,strAnsiString.c_str(),-1,NULL,0);
	WCHAR* szUnicodeString = new WCHAR[nUnicodeCharCont+1];
	MultiByteToWideChar(CP_UTF8,0,strAnsiString.c_str(),-1,szUnicodeString,nUnicodeCharCont);
	CString result(szUnicodeString);
	delete[]szUnicodeString;
	szUnicodeString=NULL;*/

	CString result = CA2W(strAnsiString.c_str());

	return result;
}

//将通用字符串编码成UTF8字符串
std::string KW2A(CString csUnicodeString)
{
	//int nUnicodeCharCount=WideCharToMultiByte(CP_UTF8,0,csUnicodeString.GetBuffer(),-1,NULL,0,NULL,NULL) ;
	//char* szAnsiString = new char[nUnicodeCharCount+1];
	//WideCharToMultiByte(CP_UTF8,0,(PWSTR)csUnicodeString.GetBuffer(),-1,szAnsiString,nUnicodeCharCount,NULL,NULL) ;
	//std::string result = szAnsiString;
	//delete[]szAnsiString;
	//szAnsiString=NULL;

	std::string result = CW2A(csUnicodeString);
	return result;
}


static int g_nMaxDXVAPlayers	= -1;
static int g_nMaxEncoderCount	= -1;
static int g_nLocalVideo		= -1;

void ReadVidecIni(void)
{
	TCHAR chIniPath[1024];
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	lstrcat(chIniPath,_T("\\VIDEC.ini"));	

	g_nMaxDXVAPlayers=GetPrivateProfileInt(_T("VIDEC"), _T("MaxDecoder") , 32, chIniPath);
	g_nMaxEncoderCount=GetPrivateProfileInt(_T("VIDEC"), _T("MaxEecoder") , 8, chIniPath);
	//g_nLocalVideo=GetPrivateProfileInt(_T("VIDEC"), _T("LocalVideo") , 0, chIniPath);

	TCHAR strValue[128];
	_stprintf(strValue,_T("%d"),g_nMaxDXVAPlayers);		
	WritePrivateProfileString(_T("VIDEC"), _T("MaxDecoder")  , strValue, chIniPath);

	_stprintf(strValue,_T("%d"),g_nMaxEncoderCount);		
	WritePrivateProfileString(_T("VIDEC"), _T("MaxEecoder")  , strValue, chIniPath);

	//_stprintf(strValue,_T("%d"),g_nLocalVideo);		
	//WritePrivateProfileString(_T("VIDEC"), _T("LocalVideo")  , strValue, chIniPath);
}

int  GetMaxDXVAPlayers(void)
{
	return g_nMaxDXVAPlayers;
}

int  GetMaxEncoderCount(void)
{
	return g_nMaxEncoderCount;
}

int  GetLocalVideo(void)
{
	return g_nLocalVideo;
}
