// SystemSet.cpp : ����Ӧ�ó������ڵ㡣
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
// 	//ͬһ�ն�����ֻ������һ��ʵ��
// 	g_hMutex = CreateMutex(NULL, FALSE, _T("system_tool")); // ����������
// 	DWORD dwLastError = GetLastError();
// 	if (dwLastError != 0)
// 		return 0;
// 
// //  	CPaintManagerUI::SetInstance(hInstance);
// // 	//��ʼ��UI
// // 	mgr_initUI();
// // 	CPaintManagerUI::MessageLoop();
// // 	//����ʼ��UI
// // 	mgr_unitUI();
// 
// 	while (1)
// 	{
// 
// 	}
// 	::CloseHandle(g_hMutex);
// 	return 0;
// }

