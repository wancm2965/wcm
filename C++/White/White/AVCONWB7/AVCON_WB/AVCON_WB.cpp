
// AVCON_WB.cpp : 定义应用程序的类行为。
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

// CAVCON_WBApp 构造

CAVCON_WBApp::CAVCON_WBApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAVCON_WBApp 对象

CAVCON_WBApp theApp;
HANDLE		  g_hAvfone;

// CAVCON_WBApp 初始化

BOOL CAVCON_WBApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	bool rz =  ::SetThreadLocale(0x0804);

	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	CString strDllPath = GetExePath() + _T("System");
	SetDllDirectory(strDllPath);
	HANDLE hMutex = ::CreateMutex(NULL,TRUE, _T("UCC_AVCONWB"));//一个程序只能运行一个实例
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 0;
	}

	//提升权限
	HANDLE hToken;              // 令牌句柄
	TOKEN_PRIVILEGES tkp;       // 令牌结构指针
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
	// 		// TODO: 在此放置处理何时用
	// 		//  “确定”来关闭对话框的代码
	// 	}
	// 	else if (nResponse == IDCANCEL)
	// 	{
	// 		// TODO: 在此放置处理何时用
	// 		//  “取消”来关闭对话框的代码
	// 	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CAVCON_WBApp::ExitInstance()
{

	return CWinAppEx::ExitInstance();
}
