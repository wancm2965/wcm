// Plugin_WB_Main.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBMainDlg.h"
using namespace AVCONPlugin;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CPlugin_WB_MainApp

BEGIN_MESSAGE_MAP(CPlugin_WB_MainApp, CWinApp)
END_MESSAGE_MAP()

// CPlugin_WB_MainApp ����

CPlugin_WB_MainApp::CPlugin_WB_MainApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPlugin_WB_MainApp ����

CPlugin_WB_MainApp theApp;


// CPlugin_WB_MainApp ��ʼ��

BOOL CPlugin_WB_MainApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance = m_hInstance;

	return TRUE;
}


// CPlugin_WB_Main ����

CPlugin_WB_Main::CPlugin_WB_Main()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

CPlugin_WB_Main::~CPlugin_WB_Main()
{

}

std::string CPlugin_WB_Main::Name()
{
   return "Plugin_WB_Main";
}

std::string CPlugin_WB_Main::ExtendPoint()
{
   return "AVCONPlugin::IPlugin_WB_Main";
}

bool CPlugin_WB_Main::InitPlugin()
{
	return true;

}

bool CPlugin_WB_Main::ClosePlugin()
{
	return true;

}

bool CPlugin_WB_Main::Init()
{
	return true;

}

bool CPlugin_WB_Main::Run()
{
 	CWBMainDlg MainDlg;
	CWBMainDlg::g_pMainDlg = &MainDlg;
	// ��ʼ��Com��
	HRESULT hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{
		AfxMessageBox( _T( "��ʼ��Com��ʧ��!" ) );		
	}
	AutoLockResource l;
    INT_PTR MainDlgBack = MainDlg.DoModal();
	CoUninitialize();
	return true;
}

bool CPlugin_WB_Main::Close()
{
	return true;
}

void CPlugin_WB_Main::Exit( void )
{
    
}
