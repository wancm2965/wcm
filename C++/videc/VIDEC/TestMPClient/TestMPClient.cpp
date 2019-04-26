
// TestMPClient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TestMPClient.h"
#include "TestMPClientDlg.h"


#include <AUDEC/AUDEC_Engine.h>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_Player.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define _CRTDBG_MAP_ALLOC     
#include <stdlib.h>     
#include <crtdbg.h> 

// CTestMPClientApp

BEGIN_MESSAGE_MAP(CTestMPClientApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestMPClientApp ����

CTestMPClientApp::CTestMPClientApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTestMPClientApp ����

CTestMPClientApp theApp;


// CTestMPClientApp ��ʼ��

BOOL CTestMPClientApp::InitInstance()
{
	//_CrtDumpMemoryLeaks();        
	_CrtSetBreakAlloc(667);    
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  


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

	::CoInitialize(NULL);


	VIDEC_Init();

	IMXEC_CapChanDev::SetVideoAutoFormat(1);

	ReadVidecIni();
	VIDEC_Player::SetMaxDXVAPlayers(GetMaxDXVAPlayers());
	VIDEC_SetHardwareEncoderMaxCount(GetMaxEncoderCount());

	//VIDEC_SetEnableVideoDisplaySmoothly(FALSE);

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CTestMPClientDlg dlg;
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

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CTestMPClientApp::ExitInstance()
{

	VIDEC_Terminate();
	AUDEC_Engine::AUDEC_Terminate();
	::CoUninitialize();


	return CWinAppEx::ExitInstance();
}
