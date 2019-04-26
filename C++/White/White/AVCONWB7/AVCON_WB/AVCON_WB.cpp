
// AVCON_WB.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AVCON_WB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CAVCON_WBApp

BEGIN_MESSAGE_MAP(CAVCON_WBApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CAVCON_WBApp ����

CAVCON_WBApp::CAVCON_WBApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAVCON_WBApp ����

CAVCON_WBApp theApp;
HANDLE		  g_hAvfone;

// CAVCON_WBApp ��ʼ��

BOOL CAVCON_WBApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	bool rz =  ::SetThreadLocale(0x0804);

	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	CString strDllPath = GetExePath() + _T("System");
	SetDllDirectory(strDllPath);
	HANDLE hMutex = ::CreateMutex(NULL,TRUE, _T("UCC_AVCONWB"));//һ������ֻ������һ��ʵ��
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 0;
	}

	//����Ȩ��
	HANDLE hToken;              // ���ƾ��
	TOKEN_PRIVILEGES tkp;       // ���ƽṹָ��
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set   
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);  

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return FALSE;
	
	HMODULE hLibrary = ::LoadLibrary(_T("PLCore.dll"));
	if(!hLibrary)
	{
		DWORD dwError = GetLastError();
		return 0;
	}

	typedef int(*_MainCore)(const char* );
	_MainCore fun = (_MainCore)(void*)::GetProcAddress(hLibrary,"MainCore");
	if(!fun)
	{
		::FreeLibrary(hLibrary);
		return 0;
	}

	int nFun = fun("");
	::FreeLibrary(hLibrary);

	::OleUninitialize();
	::CoUninitialize();

	::CloseHandle(g_hAvfone);
	return FALSE;

	// 	CAVCON_WBDlg dlg;
	// 	m_pMainWnd = &dlg;
	// 	INT_PTR nResponse = dlg.DoModal();
	// 	if (nResponse == IDOK)
	// 	{
	// 		// TODO: �ڴ˷��ô����ʱ��
	// 		//  ��ȷ�������رնԻ���Ĵ���
	// 	}
	// 	else if (nResponse == IDCANCEL)
	// 	{
	// 		// TODO: �ڴ˷��ô����ʱ��
	// 		//  ��ȡ�������رնԻ���Ĵ���
	// 	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CAVCON_WBApp::ExitInstance()
{

	return CWinAppEx::ExitInstance();
}
