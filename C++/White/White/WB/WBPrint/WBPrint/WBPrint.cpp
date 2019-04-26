
// WBPrint.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "WBPrint.h"
#include "WBPrintDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE g_eventPrintingBegin = INVALID_HANDLE_VALUE;
HANDLE g_eventPrintingEnd = INVALID_HANDLE_VALUE;


// CWBPrintApp

BEGIN_MESSAGE_MAP(CWBPrintApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWBPrintApp ����

CWBPrintApp::CWBPrintApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	///�����źŶ���
	g_eventPrintingBegin = ::CreateEvent(NULL, FALSE, FALSE, TEXT("print_begin_97B18EEF-BA36-4BAC-BF51-6F6C5A6D243E"));
	ResetEvent(g_eventPrintingBegin);
	g_eventPrintingEnd = ::CreateEvent(NULL, FALSE, FALSE, TEXT("print_end_97B18EEF-BA36-4BAC-BF51-6F6C5A6D243E"));
	ResetEvent(g_eventPrintingEnd);
}

CWBPrintApp::~CWBPrintApp()
{
	if(INVALID_HANDLE_VALUE != g_eventPrintingBegin)
	{
		ReleaseMutex(g_eventPrintingBegin);
		::CloseHandle(g_eventPrintingBegin);
		g_eventPrintingBegin = INVALID_HANDLE_VALUE;
	}
	if(INVALID_HANDLE_VALUE != g_eventPrintingEnd)
	{
		ReleaseMutex(g_eventPrintingEnd);
		::CloseHandle(g_eventPrintingEnd);
		g_eventPrintingEnd = INVALID_HANDLE_VALUE;
	}
}

// Ψһ��һ�� CWBPrintApp ����

CWBPrintApp theApp;


// CWBPrintApp ��ʼ��




BOOL CWBPrintApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	//HWND hWnd = CWnd::FindWindow(NULL,_T("AVCONWB"))->GetSafeHwnd();
	//::EnableWindow(hWnd, FALSE);
	bool bRunOneIns = true;
	CreateMutex(NULL, FALSE, _T("AVCON_WB_PRINT_PROCESS_MUTEX"));
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	///��ӡ��ʼ
	SetEvent(g_eventPrintingBegin);

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CWBPrintDlg dlg; 
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	
	return FALSE;
}



int CWBPrintApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	//PulseEvent(g_eventPrintingEnd);
	///��ӡ�����ź�
	SetEvent(g_eventPrintingEnd);
	return CWinApp::ExitInstance();
}

