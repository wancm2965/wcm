// Plugin_WB_Main.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "DlgMainFrame.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

AVCONPlugin::CPlugin_WB_Main::CPlugin_WB_Main()
:m_pMainParent(NULL)
{
}

AVCONPlugin::CPlugin_WB_Main::~CPlugin_WB_Main()
{
}

std::string AVCONPlugin::CPlugin_WB_Main::Name()
{
	return "Plugin_WB_Main";
}

std::string AVCONPlugin::CPlugin_WB_Main::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_WB_Main";
}

bool AVCONPlugin::CPlugin_WB_Main::InitPlugin()
{
	return true;
}

bool AVCONPlugin::CPlugin_WB_Main::ClosePlugin()
{
	return true;
}

bool AVCONPlugin::CPlugin_WB_Main::Init()
{
	return true;
}

bool AVCONPlugin::CPlugin_WB_Main::Run()
{
	GetLangPath();
	//运行程序 加载主皮肤对话框
	HINSTANCE hInstance = GetModuleHandle(NULL);
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetCurrentPath() +_T("\\skin\\Plugin_WB_Main"));
	CString ResPath = CPaintManagerUI::GetCurrentPath() +_T("\\skin\\Plugin_WB_Main");
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))	return 0;
	
	
	// 新建一个窗口，作为主窗口的父窗口
	m_pMainParent = new CMainParentWnd();
	if (m_pMainParent == NULL)
		return false;
	CString strWhiteBoard;
	strWhiteBoard = ReadConfigText(_T("PluginText"), _T("MAIN_WhiteBoard"));
	if (strWhiteBoard.IsEmpty())
	{
		strWhiteBoard = _T("电子白板系统");
	}
	m_pMainParent->Create(::GetDesktopWindow(), strWhiteBoard,  WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_APPWINDOW);
	UICRect rcWorkArea;
	//获取屏幕分辩率
	SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID)&rcWorkArea, 0);
	::SetWindowPos(m_pMainParent->GetHWND(), NULL, 0, 0, rcWorkArea.GetWidth(), rcWorkArea.GetHeight(), SWP_SHOWWINDOW);
 
 	m_pDlgMainFrame = new CDlgMainFrame();
 	if (m_pDlgMainFrame == NULL) 
 		return false;
 	m_pDlgMainFrame->Create(m_pMainParent->GetHWND(), _T(""), UI_WNDSTYLE_FRAME, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
 	::ShowWindow(m_pDlgMainFrame->GetHWND(), SW_HIDE);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();


	return true;
}

bool AVCONPlugin::CPlugin_WB_Main::Close()
{
	return true;
}

void AVCONPlugin::CPlugin_WB_Main::Exit( void )
{
	if (m_pDlgMainFrame)
	{
 		::DestroyWindow(m_pDlgMainFrame->GetHWND());
 		delete m_pDlgMainFrame;
		m_pDlgMainFrame = NULL;
	}
	if (m_pMainParent)
	{
		::DestroyWindow(m_pMainParent->GetHWND());
		delete m_pMainParent;
		m_pMainParent = NULL;
	}
}

HWND AVCONPlugin::CPlugin_WB_Main::GetMainWnd()
{
	return m_pDlgMainFrame == NULL ? NULL : m_pDlgMainFrame->GetHWND();
}

HWND AVCONPlugin::CPlugin_WB_Main::GetExeParentWnd()
{
	return m_pMainParent == NULL ? NULL : m_pMainParent->GetHWND();
}
