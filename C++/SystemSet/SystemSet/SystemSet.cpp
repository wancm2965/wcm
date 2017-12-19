// SystemSet.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SystemSet.h"

HANDLE g_hMutex = NULL;

int _tmain()
{
	while (1)
	{

	}
	
	return 0;
}
// int APIENTRY _tWinMain(HINSTANCE hInstance,
//                      HINSTANCE hPrevInstance,
//                      LPTSTR    lpCmdLine,
//                      int       nCmdShow)
// {
// 	UNREFERENCED_PARAMETER(hPrevInstance);
// 	UNREFERENCED_PARAMETER(lpCmdLine);
// 
// 	//同一终端有且只能启动一个实例
// 	g_hMutex = CreateMutex(NULL, FALSE, _T("system_tool")); // 创建互斥锁
// 	DWORD dwLastError = GetLastError();
// 	if (dwLastError != 0)
// 		return 0;
// 
// //  	CPaintManagerUI::SetInstance(hInstance);
// // 	//初始化UI
// // 	mgr_initUI();
// // 	CPaintManagerUI::MessageLoop();
// // 	//反初始化UI
// // 	mgr_unitUI();
// 
// 	while (1)
// 	{
// 
// 	}
// 	::CloseHandle(g_hMutex);
// 	return 0;
// }

