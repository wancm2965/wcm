// AH5K_UPDATE.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "AH5K_UPDATE.h"
#include "PcieDevEx.h"
#include "LoadFirmFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"") //���ؿ���̨����
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

extern int g_nUpdateCount;


BOOL SystemShutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	/* OpenProcessToken() ��������������Ǵ�һ�����̵ķ�������
	GetCurrentProcess() �����������ǵõ������̵ľ��*/
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
		return FALSE;
	// LookupPrivilegeValue() ���������޸Ľ��̵�Ȩ��
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set ������������Ȩ
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// AdjustTokenPrivileges������������֪ͨWindows NT�޸ı����̵�Ȩ��
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) //ʧ��
		return FALSE;
	if (!ExitWindowsEx(EWX_REBOOT, 0)) //�������������á�ǿ���˳�WINDOWS��EWX_FORCE����
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

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
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
