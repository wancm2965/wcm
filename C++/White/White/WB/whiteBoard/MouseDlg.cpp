
#include "stdafx.h"
#include "whiteBoard.h"
#include "MouseDlg.h"
#include "afxdialogex.h"
#include "ScreenDrawMgr.h"
#include "TouchAPI.h"
#include "ViewMTObject.h"

	// CMouseDlg 对话框
	IMPLEMENT_DYNAMIC(CMouseDlg, CDialogEx)

	CMouseDlg::CMouseDlg(CWnd* pParent)
	: CDialogEx(CMouseDlg::IDD, pParent)
	,m_isFullScreen(false)
	,m_isTranspartentOrNot(true)
	,m_maskCol(0xc8c9ca)
	
{
	EnableAutomation();
	m_MouseDlgLButtonDown = false;
}
CMouseDlg::CMouseDlg()
	:CDialogEx(CMouseDlg::IDD, NULL)
	,m_isFullScreen(false)
	,m_isTranspartentOrNot(true)
	,m_maskCol(0xc8c9ca)
	
{
	m_MouseDlgLButtonDown = false;
}
CMouseDlg::~CMouseDlg()
{
}

void CMouseDlg::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。
	CDialogEx::OnFinalRelease();
}

void CMouseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMouseDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZING()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_GESTURE, OnGesture)
	ON_MESSAGE(WM_TOUCH, OnTouch)
	ON_WM_CREATE()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMouseDlg, CDialogEx)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IMouseDlg 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {62158673-B0A7-452F-BB3D-B392AA4D7A49}
static const IID IID_IMouseDlg =
{ 0x62158673, 0xB0A7, 0x452F, { 0xBB, 0x3D, 0xB3, 0x92, 0xAA, 0x4D, 0x7A, 0x49 } };

BEGIN_INTERFACE_MAP(CMouseDlg, CDialogEx)
	INTERFACE_PART(CMouseDlg, IID_IMouseDlg, Dispatch)
END_INTERFACE_MAP()


///按逻辑是当在win7或vista的系统下，若aero没开启则开启(但始终是开启不了，不知道为什么)
void EnableAero()
{
	if(g_systemVersion>=6)
	{
		BOOL enabled = FALSE;

		static HMODULE hUserDll = AfxCtxLoadLibrary(_T("dwmapi.dll"));
		ENSURE(hUserDll != NULL);

		typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(BOOL*);
		static DWMAPIFUNCTION DwmIsCompositionEnabled = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmIsCompositionEnabled");

		if ( NULL != DwmIsCompositionEnabled )
		{
			(*DwmIsCompositionEnabled)( &enabled);
		}

		if(!enabled)
		{
			typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(UINT);
			static DWMAPIFUNCTION DwmEnableComposition = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmEnableComposition");

			if ( NULL != DwmEnableComposition )
			{
				(*DwmEnableComposition)(DWM_EC_ENABLECOMPOSITION);
			}
		}
	}
	else
	{
		return;
	}
}

// CMouseDlg 消息处理程序

//让窗口透明并设置窗口初始位置
BOOL CMouseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//
	if(g_systemVersion>=6)        //如果是win7系统或者是vista系统则启用DWM(桌面窗口管理器)实现透明，如果是其他系统则使用另一种方法实现透明
	{
		///开启AERO实现边框毛玻璃效果
		EnableAero();

		static HMODULE hUserDll = AfxCtxLoadLibrary(_T("dwmapi.dll"));
		ENSURE(hUserDll != NULL);

		typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(HWND, PLMARGINS);
		static DWMAPIFUNCTION DwmExtendFrameIntoClientArea = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmExtendFrameIntoClientArea");

		if ( NULL != DwmExtendFrameIntoClientArea )
		{
			LMARGINS margins = { 0 };
			(*DwmExtendFrameIntoClientArea)( GetSafeHwnd(), &margins);
		}

		SetWindowLong(this->m_hWnd,GWL_EXSTYLE,GetExStyle() | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(m_hWnd, m_maskCol, 0, LWA_COLORKEY);
	}
	else
	{
		::SetWindowLong(this->m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		VERIFY(::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0), 1, LWA_ALPHA)); 
	}

	//RegisterTouchWindow(TRUE);
	//CEncapsulateTouchAPI::U_RegisterTouchWindow(GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CMouseDlg::PreTranslateMessage(MSG* pMsg)
{
	//删除键
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		if (g_pWhiteBoardView->DeleteSelectedObjects() > 0)
		{
			//g_pWBFile->AddOperation(_T("删除对象"));
		}
		return TRUE;	
	}
	//页中的首页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_END)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->LastPage();
		return TRUE;	
	}
	//页中的末页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_HOME)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->FirstPage();
		return TRUE;	
	}
	//页中的上一页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_PRIOR)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->PrePage();
		return TRUE;	
	}
	//页中的下一页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->NextPage();
		return TRUE;	
	}
	//屏蔽键盘Esc消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//捕获鼠标左键按下消息，并告知给CScreenDrawDlg
void CMouseDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (g_systemVersion >= 6)
	{
		if (CScreenDrawMgr::GetIns())
		{
			CRect rect = m_rectSize;
			SetWindowPos(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	this->m_MouseDlgLButtonDown = true;
	this->Invalidate();

	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->OnLButtonDown(nFlags, point);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

//捕获鼠标左键松开消息，并告知CScreenDrawDlg
void CMouseDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->m_MouseDlgLButtonDown = false;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->OnLButtonUp(nFlags, point);
	if (g_systemVersion >= 6)
	{
		CRect rect = m_rectSize;
		SetWindowPos(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

//捕获鼠标移动消息，并告知给CScreenDrawDlg
void CMouseDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		if (g_nDrawStatus != DRAW_NORMAL)
		{
			g_bAvconLock = FALSE;
			SendMessage(WM_LBUTTONUP, NULL, MAKELPARAM(point.x, point.y));
			g_bAvconLock = TRUE;
			g_nDrawStatus = DRAW_NORMAL;
		}
		return;
	}
	if (this->m_MouseDlgLButtonDown)
	{
		if (g_systemVersion >= 6)
		{
			CRect rect = m_rectSize;
			//this->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(),TRUE);
			SetWindowPos(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->OnMouseMove(nFlags, point);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

//大小发生改变时，用黑色填充窗口客户区，然后使用dwm函数，让窗口透明
void CMouseDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	this->ChangeSizeOrPos();
	// TODO: 在此处添加消息处理程序代码
}

void CMouseDlg::Transpart()
{
	if(g_systemVersion>=6)  
	{
		static HMODULE hUserDll = AfxCtxLoadLibrary(_T("dwmapi.dll"));
		ENSURE(hUserDll != NULL);

		typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(HWND, PLMARGINS);
		static DWMAPIFUNCTION DwmExtendFrameIntoClientArea = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmExtendFrameIntoClientArea");
		if ( NULL != DwmExtendFrameIntoClientArea )
		{
			///将玻璃效果只在边框上呈现
			LMARGINS margins = { 0 };
			(*DwmExtendFrameIntoClientArea)( GetSafeHwnd(), &margins);
		}
	}
	else
	{
		return;
	}
}
void CMouseDlg::Glass()
{
	if(g_systemVersion>=6)        //如果是win7系统或者是vista系统则启用DWM(桌面窗口管理器)实现透明，如果是其他系统则使用另一种方法实现透明
	{
		static HMODULE hUserDll = AfxCtxLoadLibrary(_T("dwmapi.dll"));
		ENSURE(hUserDll != NULL);

		typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(HWND, PLMARGINS);
		static DWMAPIFUNCTION DwmExtendFrameIntoClientArea = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmExtendFrameIntoClientArea");
		if ( NULL != DwmExtendFrameIntoClientArea )
		{
			///将玻璃效果扩展到整个客户区域
			LMARGINS margins = { -1 };
			(*DwmExtendFrameIntoClientArea)( GetSafeHwnd(), &margins);
		}
	}
	else
	{
		return;
	}
}
//调用CScreenDrawDlg的切换效果
void CMouseDlg::GuidesShift(BOOL isTransparent)
{
	// TODO: 在此添加控件通知处理程序代码
	this->Shift(isTransparent);
}

//调用CScreenDrawDlg的全屏切换效果，全屏时不遮盖任务栏，退出全屏是切换回全屏前的位置
void CMouseDlg::FullScreenShift(bool isFullScreen)
{
	// TODO: 在此添加控件通知处理程序代码
	if(isFullScreen)
	{
		HWND screen_hWnd = ::GetDesktopWindow();
		RECT screen_Rect;
		::GetWindowRect(screen_hWnd, &screen_Rect);
		RECT shell_Rect;
		CWnd *shellWnd = FindWindow(L"Shell_TrayWnd",_T(""));
		shellWnd->GetWindowRect(&shell_Rect);
		int height;
		height = shell_Rect.bottom-shell_Rect.top;
		screen_Rect.bottom -= height;
		this->m_isFullScreen = true;
		::MoveWindow(this->GetSafeHwnd(), screen_Rect.left, screen_Rect.top, screen_Rect.right-screen_Rect.left, screen_Rect.bottom, TRUE);
		::MoveWindow(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->GetSafeHwnd(), screen_Rect.left, screen_Rect.top, screen_Rect.right-screen_Rect.left, screen_Rect.bottom, TRUE);
		::MoveWindow(CScreenDrawMgr::GetIns()->m_pAlphaDlg->GetSafeHwnd(), screen_Rect.left, screen_Rect.top, screen_Rect.right-screen_Rect.left, screen_Rect.bottom, TRUE);
	}
	else
	{
		this->m_isFullScreen = false;
		::MoveWindow(this->GetSafeHwnd(), m_rClientRect.left, m_rClientRect.top, m_rClientRect.Width(), m_rClientRect.Height(), TRUE);
		::MoveWindow(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->GetSafeHwnd(), m_rClientRect.left, m_rClientRect.top, m_rClientRect.Width(), m_rClientRect.Height(), TRUE);
		::MoveWindow(CScreenDrawMgr::GetIns()->m_pAlphaDlg->GetSafeHwnd(), m_rClientRect.left, m_rClientRect.top, m_rClientRect.Width(), m_rClientRect.Height(), TRUE);
	}
}

void CMouseDlg::DestroyScreenWindow()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_CANCELMODE, NULL, NULL);

	CDialogEx::OnDestroy();
	::SendMessage(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->m_hWnd, WM_DESTROYSCREEN, NULL, NULL);
}

void CMouseDlg::OnDestroy()
{
#ifndef SUPPORTMT

	//获取触摸数据的协议类型
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//电子白板中是否强制使用系统的触摸
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//如果是win8系统强制使用系统触摸
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_UnRegisterTouchWindow(GetSafeHwnd());
#endif
	CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->DestroyWindow();
}

//让窗口玻璃化
void CMouseDlg::SetGlass()
{
	this->Glass();
	m_isTranspartentOrNot = false;
}

//让窗口透明化
void CMouseDlg::SetTransparent()
{
	this->Transpart();
	m_isTranspartentOrNot = true;
}
//切换玻璃与透明的效果
void CMouseDlg::Shift(BOOL isTransparent)
{
	EnableAero();

	if(isTransparent)
	{
		///设置成透明效果
		this->SetTransparent();
	}
	else
	{
		///设置成玻璃效果
		this->SetGlass();
	}
}

void CMouseDlg::OnMove(int x, int y)
{
	m_rectSize.left = x;
	m_rectSize.top = y;
	CDialogEx::OnMove(x, y);
	this->ChangeSizeOrPos();
	// TODO: 在此处添加消息处理程序代码
}

void CMouseDlg::ChangeSizeOrPos()
{
	CRect rect;
	this->GetClientRect(&rect);
	this->ClientToScreen(&rect);
	if(!m_isFullScreen)
	{
		m_rClientRect.left = rect.left;
		m_rClientRect.right = rect.right;
		m_rClientRect.bottom = rect.bottom;
		m_rClientRect.top = rect.top;
	}
	if(g_systemVersion>=6)
	{
		if(this->m_isTranspartentOrNot)
		{
			this->Transpart();
		}
		else
		{
			this->Glass();
		}
	}
	if (CScreenDrawMgr::GetIns()->m_pScreenDrawDlg != NULL && CScreenDrawMgr::GetIns()->m_pAlphaDlg != NULL)
	{
		CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
		CScreenDrawMgr::GetIns()->m_pAlphaDlg->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	}
	m_rectSize = rect;
}

BOOL CMouseDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, m_maskCol);
	return TRUE;
}


HBRUSH CMouseDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	//if(nCtlColor == CTLCOLOR_EDIT)
	//{
	//	//pDC->SetBkMode(TRANSPARENT);
	//	pDC->SetBkColor(RGB(0,255,0));sdfa
	//}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

LPCTSTR CMouseDlg::getSystemName()
{

	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器 

	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构 

	OSVERSIONINFOEX os; 

	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   

	if(GetVersionEx((OSVERSIONINFO *)&os))                  

	{ 

		LPCTSTR vname; 

		//下面根据版本信息判断操作系统名称 

		switch(os.dwMajorVersion){                        //判断主版本号 

		case 4: 

			switch(os.dwMinorVersion){                //判断次版本号 

			case 0: 

				if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 

					vname=_T("Microsoft Windows NT 4.0");                //1996年7月发布 

				else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 

					vname=_T("Microsoft Windows 95"); 

				break; 

			case 10: 

				vname=_T("Microsoft Windows 98"); 

				break; 

			case 90: 

				vname=_T("Microsoft Windows Me"); 

				break; 

			} 

			break; 

		case 5: 

			switch(os.dwMinorVersion){               //再比较dwMinorVersion的值 

			case 0: 

				vname=_T("Microsoft Windows 2000");                    //1999年12月发布 

				break; 

			case 1: 

				vname=_T("Microsoft Windows XP");                    //2001年8月发布 

				break; 

			case 2: 

				if(os.wProductType==VER_NT_WORKSTATION && 

					info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)

					vname=_T("Microsoft Windows XP Professional x64 Edition"); 

				else if(GetSystemMetrics(SM_SERVERR2)==0) 

					vname=_T("Microsoft Windows Server 2003");        //2003年3月发布 

				else if(GetSystemMetrics(SM_SERVERR2)!=0) 

					vname=_T("Microsoft Windows Server 2003 R2"); 

				break; 

			} 

			break; 

		case 6: 

			switch(os.dwMinorVersion){ 

			case 0: 

				if(os.wProductType==VER_NT_WORKSTATION)

					vname=_T("Microsoft Windows Vista"); 

				else 

					vname=_T("Microsoft Windows Server 2008");          //服务器版本 

				break; 

			case 1: 

				if(os.wProductType==VER_NT_WORKSTATION) 

					vname=_T("Microsoft Windows 7"); 

				else 

					vname=_T("Microsoft Windows Server 2008 R2"); 

				break; 

			} 

			break; 

		default: 

			vname=_T("未知操作系统"); 

		} 

		return vname; 

	} 

	else 

		return _T("");

} 

bool CMouseDlg::PtInScreenDrawDlg(const POINT &pt)const
{
	if ( m_hWnd == ::WindowFromPoint(pt))
	{
		return true;
	}
	else if (CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->GetSafeHwnd() == ::WindowFromPoint(pt))
	{
		return true;
	}

	return false;
}


void CMouseDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	int nMinSize = 100; //窗口最小大小

	if ((pRect->right - pRect->left) < nMinSize)
	{
		if (fwSide == 1) //左边
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 2) //右边
		{
			pRect->right = pRect->left + nMinSize;
		}
		if (fwSide == 4) //左上角
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 5) //右上角
		{
			pRect->right = pRect->left + nMinSize;
		}
		if (fwSide == 7) //左下角
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 8) //右下角
		{
			pRect->right = pRect->left + nMinSize;
		}
	}
	if ((pRect->bottom - pRect->top) < nMinSize)
	{
		if (fwSide == 3) //上边
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 6) //下边
		{
			pRect->bottom = pRect->top + nMinSize;
		}
		if (fwSide == 4) //左上角
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 5) //右上角
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 7) //左下角
		{
			pRect->bottom = pRect->top + nMinSize;
		}
		if (fwSide == 8) //右下角
		{
			pRect->bottom = pRect->top + nMinSize;
		}
	}

	CDialogEx::OnSizing(fwSide, pRect);
}

void CMouseDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (g_systemVersion >= 6)
	{
		if (CScreenDrawMgr::GetIns())
		{
			CRect rect = m_rectSize;
			SetWindowPos(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	this->Invalidate();

	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->OnLButtonDblClk(nFlags, point);
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

/*************************************************************************
*                    CMouseDlg::OnGesture							 *
*                         屏蔽系统手势处理								*
*************************************************************************/
LRESULT CMouseDlg::OnGesture( WPARAM /*wParam*/, LPARAM lParam )
{
	return S_OK;
}


LRESULT CMouseDlg::OnTouch( WPARAM wParam, LPARAM lParam )
{
#ifndef SUPPORTMT

	if (g_bAvconLock) 
	{
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
		return S_OK;
	}

	int iNumContacts = LOWORD(wParam);
	HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;

#ifdef _DEBUG
	PTOUCHINPUT pInputs = new TOUCHINPUT[iNumContacts];
#else
	PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
#endif


	if(pInputs != NULL)
	{
		//获取每个触摸点信息，然后分发到各个处理容器中。
		//if(GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
		if( CEncapsulateTouchAPI::U_GetTouchInputInfo(lParam, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
		{
			for(int i = 0; i < iNumContacts; ++i)
			{
				//////转换成客户区坐标
				POINT ptInputs;

				ptInputs.x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
				ptInputs.y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);

				//ClientToScreen(&ptInputs);
				// 				CClientDC dc(this);
				//				OnPrepareDC(&dc);//进行坐标原点的匹配
				// 				dc.DPtoLP(&ptInputs);//将视图坐标转换为文档作标
				pInputs[i].x = ptInputs.x;
				pInputs[i].y = ptInputs.y;

				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
				{
					if (!g_bAvconLock) //如果会议中白板功能没有被锁定
					{
						std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
						if (m_mapTouchCursor.end() != it)
						{
							delete it->second;
							m_mapTouchCursor.erase(it);
						}
						static long session_id = 1;
						MTCursor *pCursor = new MTCursor(MTTime::getSessionTime(),++session_id, pInputs[i].dwID, (float)pInputs[i].x, (float)pInputs[i].y);
						m_mapTouchCursor.insert(std::pair<DWORD, MTCursor*>(pInputs[i].dwID, pCursor));
						CWBMTObjectManager::GetIns_()->addMTCursor(pCursor);
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->updateMTCursor(it->second); 
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸弹起事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->removeMTCursor(it->second);
						delete it->second;
						m_mapTouchCursor.erase(it);
					}
				}
			}
		}
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
	}

	delete [] pInputs;
#endif
	return S_OK;
}


int CMouseDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifndef SUPPORTMT

	// TODO:  在此添加您专用的创建代码
	//获取触摸数据的协议类型
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//电子白板中是否强制使用系统的触摸
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//如果是win8系统强制使用系统触摸
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_RegisterTouchWindow(GetSafeHwnd());
#endif
	return 0;
}
