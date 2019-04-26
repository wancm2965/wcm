
// stdafx.cpp : 只包括标准包含文件的源文件
// Demo_win32.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

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
	g_nLocalVideo=GetPrivateProfileInt(_T("VIDEC"), _T("LocalVideo") , 0, chIniPath);

	TCHAR strValue[128];
	_stprintf(strValue,_T("%d"),g_nMaxDXVAPlayers);		
	WritePrivateProfileString(_T("VIDEC"), _T("MaxDecoder")  , strValue, chIniPath);

	_stprintf(strValue,_T("%d"),g_nMaxEncoderCount);		
	WritePrivateProfileString(_T("VIDEC"), _T("MaxEecoder")  , strValue, chIniPath);
	
	_stprintf(strValue,_T("%d"),g_nLocalVideo);		
	WritePrivateProfileString(_T("VIDEC"), _T("LocalVideo")  , strValue, chIniPath);
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
