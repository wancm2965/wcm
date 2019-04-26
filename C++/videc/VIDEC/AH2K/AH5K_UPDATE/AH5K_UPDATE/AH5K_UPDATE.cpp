// AH5K_UPDATE.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AH5K_UPDATE.h"
#include "PcieDevEx.h"
#include "LoadFirmFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"") //隐藏控制台窗口
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

extern int g_nUpdateCount;


BOOL SystemShutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	/* OpenProcessToken() 这个函数的作用是打开一个进程的访问令牌
	GetCurrentProcess() 函数的作用是得到本进程的句柄*/
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
		return FALSE;
	// LookupPrivilegeValue() 的作用是修改进程的权限
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set 赋给本进程特权
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// AdjustTokenPrivileges（）的作用是通知Windows NT修改本进程的权利
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) //失败
		return FALSE;
	if (!ExitWindowsEx(EWX_REBOOT, 0)) //参数在这里设置。强行退出WINDOWS（EWX_FORCE）。
		return FALSE;
	return TRUE;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	int nDevCount = 0;

	PBYTE pBufAH2K32 = NULL;
	DWORD dwLenAH2K32 = 0;
	g_nUpdateCount = 0;
	
	//system("pause");

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		//nRetCode = 1;
	}
	else
	{
		nDevCount = GetDevCount();
		if (nDevCount<=0)
		{
			return -1;
		}

		HMODULE g_hInstance = ::GetModuleHandle(NULL);

		if (!LoadBin(g_hInstance, IDR_BIN, &pBufAH2K32, dwLenAH2K32))
		{
			return -1;
		}
		
		for (int n=0; n<nDevCount; n++)
		{
			DEV_EX_TYPE device_type;
			GetDevType(n,device_type);
			PcieDevEx::BootLoad(n,pBufAH2K32,dwLenAH2K32,device_type);
		}

		if (g_nUpdateCount>0)
		{
			//SystemShutdown();
		}
	}

	//system("pause");

	//return nRetCode;

	CString strText;
	strText.Format(TEXT("g_nUpdateCount is %d"),g_nUpdateCount);
	OutputDebugString(strText);
	return g_nUpdateCount;
}
