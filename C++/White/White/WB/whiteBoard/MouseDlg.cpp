
#include "stdafx.h"
#include "whiteBoard.h"
#include "MouseDlg.h"
#include "afxdialogex.h"
#include "ScreenDrawMgr.h"
#include "TouchAPI.h"
#include "ViewMTObject.h"

	// CMouseDlg �Ի���
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
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣
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

// ע��: ������� IID_IMouseDlg ֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {62158673-B0A7-452F-BB3D-B392AA4D7A49}
static const IID IID_IMouseDlg =
{ 0x62158673, 0xB0A7, 0x452F, { 0xBB, 0x3D, 0xB3, 0x92, 0xAA, 0x4D, 0x7A, 0x49 } };

BEGIN_INTERFACE_MAP(CMouseDlg, CDialogEx)
	INTERFACE_PART(CMouseDlg, IID_IMouseDlg, Dispatch)
END_INTERFACE_MAP()


///���߼��ǵ���win7��vista��ϵͳ�£���aeroû��������(��ʼ���ǿ������ˣ���֪��Ϊʲô)
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

// CMouseDlg ��Ϣ�������

//�ô���͸�������ô��ڳ�ʼλ��
BOOL CMouseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//
	if(g_systemVersion>=6)        //�����win7ϵͳ������vistaϵͳ������DWM(���洰�ڹ�����)ʵ��͸�������������ϵͳ��ʹ����һ�ַ���ʵ��͸��
	{
		///����AEROʵ�ֱ߿�ë����Ч��
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CMouseDlg::PreTranslateMessage(MSG* pMsg)
{
	//ɾ����
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		if (g_pWhiteBoardView->DeleteSelectedObjects() > 0)
		{
			//g_pWBFile->AddOperation(_T("ɾ������"));
		}
		return TRUE;	
	}
	//ҳ�е���ҳ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_END)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->LastPage();
		return TRUE;	
	}
	//ҳ�е�ĩҳ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_HOME)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->FirstPage();
		return TRUE;	
	}
	//ҳ�е���һҳ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_PRIOR)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->PrePage();
		return TRUE;	
	}
	//ҳ�е���һҳ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT)
	{

		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		Invalidate();
		g_pWhiteBoardView->NextPage();
		return TRUE;	
	}
	//���μ���Esc��Ϣ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//����������������Ϣ������֪��CScreenDrawDlg
void CMouseDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock) //��������аװ幦�ܱ�����
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

//�����������ɿ���Ϣ������֪CScreenDrawDlg
void CMouseDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->m_MouseDlgLButtonDown = false;
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock) //��������аװ幦�ܱ�����
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

//��������ƶ���Ϣ������֪��CScreenDrawDlg
void CMouseDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock) //��������аװ幦�ܱ�����
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

//��С�����ı�ʱ���ú�ɫ��䴰�ڿͻ�����Ȼ��ʹ��dwm�������ô���͸��
void CMouseDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	this->ChangeSizeOrPos();
	// TODO: �ڴ˴������Ϣ����������
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
			///������Ч��ֻ�ڱ߿��ϳ���
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
	if(g_systemVersion>=6)        //�����win7ϵͳ������vistaϵͳ������DWM(���洰�ڹ�����)ʵ��͸�������������ϵͳ��ʹ����һ�ַ���ʵ��͸��
	{
		static HMODULE hUserDll = AfxCtxLoadLibrary(_T("dwmapi.dll"));
		ENSURE(hUserDll != NULL);

		typedef	HRESULT (__stdcall *DWMAPIFUNCTION)(HWND, PLMARGINS);
		static DWMAPIFUNCTION DwmExtendFrameIntoClientArea = (DWMAPIFUNCTION)GetProcAddress(hUserDll, "DwmExtendFrameIntoClientArea");
		if ( NULL != DwmExtendFrameIntoClientArea )
		{
			///������Ч����չ�������ͻ�����
			LMARGINS margins = { -1 };
			(*DwmExtendFrameIntoClientArea)( GetSafeHwnd(), &margins);
		}
	}
	else
	{
		return;
	}
}
//����CScreenDrawDlg���л�Ч��
void CMouseDlg::GuidesShift(BOOL isTransparent)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->Shift(isTransparent);
}

//����CScreenDrawDlg��ȫ���л�Ч����ȫ��ʱ���ڸ����������˳�ȫ�����л���ȫ��ǰ��λ��
void CMouseDlg::FullScreenShift(bool isFullScreen)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendMessage(WM_CANCELMODE, NULL, NULL);

	CDialogEx::OnDestroy();
	::SendMessage(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->m_hWnd, WM_DESTROYSCREEN, NULL, NULL);
}

void CMouseDlg::OnDestroy()
{
#ifndef SUPPORTMT

	//��ȡ�������ݵ�Э������
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//���Ӱװ����Ƿ�ǿ��ʹ��ϵͳ�Ĵ���
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//�����win8ϵͳǿ��ʹ��ϵͳ����
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_UnRegisterTouchWindow(GetSafeHwnd());
#endif
	CScreenDrawMgr::GetIns()->m_pScreenDrawDlg->DestroyWindow();
}

//�ô��ڲ�����
void CMouseDlg::SetGlass()
{
	this->Glass();
	m_isTranspartentOrNot = false;
}

//�ô���͸����
void CMouseDlg::SetTransparent()
{
	this->Transpart();
	m_isTranspartentOrNot = true;
}
//�л�������͸����Ч��
void CMouseDlg::Shift(BOOL isTransparent)
{
	EnableAero();

	if(isTransparent)
	{
		///���ó�͸��Ч��
		this->SetTransparent();
	}
	else
	{
		///���óɲ���Ч��
		this->SetGlass();
	}
}

void CMouseDlg::OnMove(int x, int y)
{
	m_rectSize.left = x;
	m_rectSize.top = y;
	CDialogEx::OnMove(x, y);
	this->ChangeSizeOrPos();
	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, m_maskCol);
	return TRUE;
}


HBRUSH CMouseDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	//if(nCtlColor == CTLCOLOR_EDIT)
	//{
	//	//pDC->SetBkMode(TRANSPARENT);
	//	pDC->SetBkColor(RGB(0,255,0));sdfa
	//}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

LPCTSTR CMouseDlg::getSystemName()
{

	SYSTEM_INFO info;                                   //��SYSTEM_INFO�ṹ�ж�64λAMD������ 

	GetSystemInfo(&info);                               //����GetSystemInfo�������ṹ 

	OSVERSIONINFOEX os; 

	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   

	if(GetVersionEx((OSVERSIONINFO *)&os))                  

	{ 

		LPCTSTR vname; 

		//������ݰ汾��Ϣ�жϲ���ϵͳ���� 

		switch(os.dwMajorVersion){                        //�ж����汾�� 

		case 4: 

			switch(os.dwMinorVersion){                //�жϴΰ汾�� 

			case 0: 

				if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 

					vname=_T("Microsoft Windows NT 4.0");                //1996��7�·��� 

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

			switch(os.dwMinorVersion){               //�ٱȽ�dwMinorVersion��ֵ 

			case 0: 

				vname=_T("Microsoft Windows 2000");                    //1999��12�·��� 

				break; 

			case 1: 

				vname=_T("Microsoft Windows XP");                    //2001��8�·��� 

				break; 

			case 2: 

				if(os.wProductType==VER_NT_WORKSTATION && 

					info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)

					vname=_T("Microsoft Windows XP Professional x64 Edition"); 

				else if(GetSystemMetrics(SM_SERVERR2)==0) 

					vname=_T("Microsoft Windows Server 2003");        //2003��3�·��� 

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

					vname=_T("Microsoft Windows Server 2008");          //�������汾 

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

			vname=_T("δ֪����ϵͳ"); 

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
	int nMinSize = 100; //������С��С

	if ((pRect->right - pRect->left) < nMinSize)
	{
		if (fwSide == 1) //���
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 2) //�ұ�
		{
			pRect->right = pRect->left + nMinSize;
		}
		if (fwSide == 4) //���Ͻ�
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 5) //���Ͻ�
		{
			pRect->right = pRect->left + nMinSize;
		}
		if (fwSide == 7) //���½�
		{
			pRect->left = pRect->right - nMinSize;
		}
		if (fwSide == 8) //���½�
		{
			pRect->right = pRect->left + nMinSize;
		}
	}
	if ((pRect->bottom - pRect->top) < nMinSize)
	{
		if (fwSide == 3) //�ϱ�
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 6) //�±�
		{
			pRect->bottom = pRect->top + nMinSize;
		}
		if (fwSide == 4) //���Ͻ�
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 5) //���Ͻ�
		{
			pRect->top = pRect->bottom - nMinSize;
		}
		if (fwSide == 7) //���½�
		{
			pRect->bottom = pRect->top + nMinSize;
		}
		if (fwSide == 8) //���½�
		{
			pRect->bottom = pRect->top + nMinSize;
		}
	}

	CDialogEx::OnSizing(fwSide, pRect);
}

void CMouseDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock) //��������аװ幦�ܱ�����
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
*                         ����ϵͳ���ƴ���								*
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
		//��ȡÿ����������Ϣ��Ȼ��ַ����������������С�
		//if(GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
		if( CEncapsulateTouchAPI::U_GetTouchInputInfo(lParam, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
		{
			for(int i = 0; i < iNumContacts; ++i)
			{
				//////ת���ɿͻ�������
				POINT ptInputs;

				ptInputs.x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
				ptInputs.y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);

				//ClientToScreen(&ptInputs);
				// 				CClientDC dc(this);
				//				OnPrepareDC(&dc);//��������ԭ���ƥ��
				// 				dc.DPtoLP(&ptInputs);//����ͼ����ת��Ϊ�ĵ�����
				pInputs[i].x = ptInputs.x;
				pInputs[i].y = ptInputs.y;

				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// ���������¼�
				{
					if (!g_bAvconLock) //��������аװ幦��û�б�����
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
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// �����ƶ��¼�
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->updateMTCursor(it->second); 
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// ���������¼�
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

	// TODO:  �ڴ������ר�õĴ�������
	//��ȡ�������ݵ�Э������
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//���Ӱװ����Ƿ�ǿ��ʹ��ϵͳ�Ĵ���
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//�����win8ϵͳǿ��ʹ��ϵͳ����
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_RegisterTouchWindow(GetSafeHwnd());
#endif
	return 0;
}
