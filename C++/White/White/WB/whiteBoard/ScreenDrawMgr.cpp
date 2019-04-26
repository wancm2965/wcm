#include "ScreenDrawMgr.h"

CScreenDrawMgr* CScreenDrawMgr::s_pScreenDrawMgrIns = NULL;

CScreenDrawMgr::CScreenDrawMgr():m_pAlphaDlg(NULL), m_pMouseDlg(NULL), m_pScreenDrawDlg(NULL)
{
	m_bChangeInvalidateway = false;
}

CScreenDrawMgr::~CScreenDrawMgr()
{
	m_pMouseDlg->DestroyScreenWindow();

	if (m_pScreenDrawDlg)
	{
		delete m_pScreenDrawDlg;
		m_pScreenDrawDlg = NULL;
	}
	if (m_pMouseDlg)
	{
		delete m_pMouseDlg;
		m_pMouseDlg = NULL;
	}
	if (m_pAlphaDlg)
	{
		delete m_pAlphaDlg;
		m_pAlphaDlg = NULL;
	}
}

///获得实例
CScreenDrawMgr* CScreenDrawMgr::GetIns()
{
	return s_pScreenDrawMgrIns;
}

///创建实例
bool CScreenDrawMgr::CreateIns()
{
	if (s_pScreenDrawMgrIns)
	{
		return false;
	}
	s_pScreenDrawMgrIns = new CScreenDrawMgr;

	return true;
}

///用来创建三层窗口
void CScreenDrawMgr::Init(CRect& rect)
{
	m_pAlphaDlg = new CAlphaDlg(&rect);
	m_pAlphaDlg->Create(IDD_ALPHACHANGE,CWBToolbarDlg::FromHandle(CWBToolbarDlg::g_toolBarHWND));
	m_pAlphaDlg->ShowWindow(SW_HIDE);

	m_pMouseDlg = new CMouseDlg;
	m_pMouseDlg->Create(IDD_MOUSEDLG,CWBToolbarDlg::FromHandle(CWBToolbarDlg::g_toolBarHWND));
	m_pMouseDlg->ShowWindow(SW_HIDE);

	m_pScreenDrawDlg =new CScreenDrawDlg(rect); 
	m_pScreenDrawDlg->Create(IDD_SCREENDRAWDLG,CWBToolbarDlg::FromHandle(CWBToolbarDlg::g_toolBarHWND));
	m_pScreenDrawDlg->ShowWindow(SW_HIDE);

	//设置桌面绘图的位置
	m_pMouseDlg->MoveWindow(rect.left + 7, rect.top + 7, rect.Width() - 14,rect.Height() - 14);
	//如果桌面绘图按钮在工具箱中，则将工具箱设在桌面绘图窗口的上面
	if (NULL != CWBToolboxDlg::g_toolBoxHWND)
	{
		m_pScreenDrawDlg->SetWindowPos(CWBToolboxDlg::FromHandle(CWBToolboxDlg::g_toolBoxHWND),rect.left + 7, rect.top + 7, rect.Width() - 14,rect.Height() - 14,SWP_SHOWWINDOW);
	}
	m_pScreenDrawDlg->ShowWindow(SW_HIDE);
	//m_pScreenDrawDlg->MoveWindow(rect.left + 7, rect.top + 7, rect.Width() - 14,rect.Height() - 14);

	m_pAlphaDlg->ShowWindow(SW_SHOW);
	m_pMouseDlg->ShowWindow(SW_SHOW);
	m_pScreenDrawDlg->ShowWindow(SW_SHOW);
	g_bIsDrawScreen = TRUE;
}

///销毁实例
void CScreenDrawMgr::DestroyIns()
{
	if (s_pScreenDrawMgrIns)
	{
		delete s_pScreenDrawMgrIns;
		s_pScreenDrawMgrIns = NULL;
	}
}

void CScreenDrawMgr::addMTCursor(MTCursor *tcur)
{
#ifndef SUPPORTMT

	m_pScreenDrawDlg->addMTCursor(tcur);
#endif
}

void CScreenDrawMgr::updateMTCursor(MTCursor *tcur)
{
#ifndef SUPPORTMT

	m_pScreenDrawDlg->updateMTCursor(tcur);
#endif
}

void CScreenDrawMgr::removeMTCursor(MTCursor *tcur)
{
#ifndef SUPPORTMT

	m_pScreenDrawDlg->removeMTCursor(tcur);
#endif
}

bool CScreenDrawMgr::PtInScreenDrawDlg(const POINT &pt)const
{
	return m_pMouseDlg->PtInScreenDrawDlg(pt);
}

void CScreenDrawMgr::GuidesShift(BOOL isTransparent)
{
	m_pMouseDlg->GuidesShift(isTransparent);
}

void CScreenDrawMgr::DestroyScreenWindow()
{
	m_pMouseDlg->DestroyScreenWindow();
}

void CScreenDrawMgr::FullScreenShift(bool isFullScreen)
{
	m_pMouseDlg->FullScreenShift(isFullScreen);
}

void CScreenDrawMgr::AlphaChange(const int alpha)
{
	m_pAlphaDlg->AlphaChange(alpha);
}

bool CScreenDrawMgr::IsTransparent()
{
	return m_pMouseDlg->IsTransparent();
}

bool CScreenDrawMgr::IsFullScreen()
{
	return m_pMouseDlg->IsFullScreen();
}

void CScreenDrawMgr::Invalidate()
{
	if (m_bChangeInvalidateway)
	{
		::SendMessage(m_pScreenDrawDlg->m_hWnd,WM_PAINT,0,0);
		
	}
	else
	{
		m_pScreenDrawDlg->Invalidate();
	}
	//
	
}

void CScreenDrawMgr::UpdateWindow()
{
	m_pScreenDrawDlg->UpdateWindow();
}

///最小化
void CScreenDrawMgr::SetIsMinSize(BOOL bIsMinSize)
{
	if (bIsMinSize)
	{
		::SendMessage(m_pScreenDrawDlg->GetSafeHwnd(), SC_MINIMIZE, NULL, NULL);
		::SendMessage(m_pMouseDlg->GetSafeHwnd(), SC_MINIMIZE, NULL, NULL);
		::SendMessage(m_pAlphaDlg->GetSafeHwnd(), SC_MINIMIZE, NULL, NULL);

	} 
	else
	{
		::SendMessage(m_pScreenDrawDlg->GetSafeHwnd(), SC_RESTORE, NULL, NULL);
		::SendMessage(m_pMouseDlg->GetSafeHwnd(), SC_RESTORE, NULL, NULL);
		::SendMessage(m_pAlphaDlg->GetSafeHwnd(), SC_RESTORE, NULL, NULL);
	}
}

///设置文本框的父窗口
void CScreenDrawMgr::SetRichEditParent(CWnd **parent)
{
	if (g_systemVersion >= 6)
	{
		*parent = (CWnd*)m_pMouseDlg;
	}
	else
	{
		*parent = (CWnd*)m_pScreenDrawDlg;
	}
}

///将MouseDlg放到ScreenDrawDlg的下面
void CScreenDrawMgr::SetMouseDlgWindowPos()
{
	::SetWindowPos(m_pMouseDlg->GetSafeHwnd(), m_pScreenDrawDlg->GetSafeHwnd(), m_pMouseDlg->GetRectSize().left, m_pMouseDlg->GetRectSize().top, 
		m_pMouseDlg->GetRectSize().Width(), m_pMouseDlg->GetRectSize().Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

void CScreenDrawMgr::SetMoveIsNeedSendData( BOOL MoveIsNeedSendData )
{
	m_pScreenDrawDlg->SetMoveIsNeedSendData(MoveIsNeedSendData);
}

BOOL CScreenDrawMgr::GetMoveIsNeedSendData()
{
	return m_pScreenDrawDlg->GetMoveIsNeedSendData();
}

void CScreenDrawMgr::GetMouseDlgScreenPosition(CPoint& pt)
{
	m_pMouseDlg->ClientToScreen(&pt);
}

void CScreenDrawMgr::GetScreenDrawDlgClientPosition(CPoint& pt)
{
	m_pScreenDrawDlg->ScreenToClient(&pt);
}

///清除m_pScreenDrawDlg中的drawmap
void CScreenDrawMgr::ClearTouchDrawMap(DWORD dwLayerIndex)
{
	m_pScreenDrawDlg->ScreenClearTouchMap(dwLayerIndex);
}

