// Usingffmpeg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Usingffmpeg.h"
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include<typeinfo>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <atlbase.h>
#include <atlwin.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

//CWinApp theApp;

using namespace std;

//int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
int main()
{
	int nRetCode = 0;
	/*ShowWindow(GetConsoleWindow(), SW_HIDE);*/
	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}
	
	TCHAR path[MAX_PATH];
	int len = GetModuleFileName(NULL,path,MAX_PATH);
	path[len] = 0;
	TCHAR* ret = _tcsrchr(path,'\\');
	if (!ret)
		return 0;
	int idxlen = (int)(ret - path + 1);
	path[idxlen] = 0;
	CString strExePath = CString(path);
	CString strPath = strExePath+_T("ffmpeg.exe");
	
	CString strCMD;
	CString strFirstTime;
	CString strEndTime;
	CString strSrcFile;
	CString strDesFile;
	strFirstTime = L"0";
	strEndTime= L"00:01:00";
	strSrcFile = L"E:\\TC128.mp4";
	strDesFile = L"E:\\11.mp4";

	strPath = L"E:\\git\\wcm\\C++\\Usingffmpeg\\Debug\\ffmpeg.exe";
	
	strCMD.Format(L" -ss %s -t %s -i %s -acodec copy -vcodec copy %s",strFirstTime.GetBuffer(),strEndTime.GetBuffer(),strSrcFile.GetBuffer(),strDesFile.GetBuffer());
	

	//strCMD=L"-ss 00:00:00  -i E:\\123.flv  -vcodec copy -acodec copy -t 00:00:53  E:\\2.flv";
	HINSTANCE hInst = ShellExecute(0, L"open", strPath, strCMD, NULL, SW_HIDE);
	
	//strSrcFile = L"E:\\123.flv";
	strDesFile = L"E:\\test123.png";
	strCMD.Format(L"-i %s  -y -f image2 -ss 8 -t 0.001 -s 960*480 %s",strSrcFile.GetBuffer(),strDesFile.GetBuffer());

	hInst = ShellExecute(0, L"open", strPath, strCMD, NULL, SW_HIDE);
	//ShellExecute(0, L"open",L"E:\\BAIDUDOWNLOAD\\ffmpeg-latest-win32-static\\ffmpeg-latest-win32-static\\bin\\ffmpeg.exe",L"-ss 00:00:00  -i E:\\123.flv  -vcodec copy -acodec copy -t 00:00:36  E:\\1.flv",NULL, SW_SHOW);
	//ShellExecute(0, L"open",L"E:\\git\\wcm\\C++\\Usingffmpeg\\Debug\\ffmpeg.exe",L"-ss 00:00:00  -i E:\\123.flv  -vcodec copy -acodec copy -t 00:00:53  E:\\2.flv",NULL, SW_SHOW);
	while(true)
	{

	}
	return nRetCode;
}
