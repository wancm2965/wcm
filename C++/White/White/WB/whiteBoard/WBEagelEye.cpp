// WBEagelEye.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBEagelEye.h"
#include "afxdialogex.h"


// CWBEagelEye 对话框

IMPLEMENT_DYNAMIC(CWBEagelEye, CDialog)

CWBEagelEye::CWBEagelEye(CWnd* pParent /*=NULL*/)
	: CDialog(CWBEagelEye::IDD, pParent)
{
	m_rect = NULL;
	m_pWBPreviewSingleView = NULL;
}

CWBEagelEye::~CWBEagelEye()
{
}

void CWBEagelEye::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBEagelEye, CDialog)
	/*ON_WM_ERASEBKGND()*/
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWBEagelEye 消息处理程序


BOOL CWBEagelEye::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->GetClientRect(&m_rect);
	CRect rect = m_rect;
	rect.left -= 2;
	rect.top -= 2;
	rect.right +=2;
	rect.bottom +=2;
	if (m_pWBPreviewSingleView == NULL)
	{
		m_pWBPreviewSingleView =new CWBPreviewSingleView(this->m_hWnd , g_pWBFile->GetCurPage());
		if (NULL == m_pWBPreviewSingleView)
		{
			return FALSE;
		}
		m_pWBPreviewSingleView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW | WS_CLIPCHILDREN,rect,this,NULL);
		m_pWBPreviewSingleView->SetRatio();
		m_pWBPreviewSingleView->SetRectInPreview();
		m_pWBPreviewSingleView->ShowButtonPlace();
		m_pWBPreviewSingleView->SendMessage(WM_PAINT);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


CWBPreviewSingleView* CWBEagelEye::GetPreviewSingleView()
{
	return m_pWBPreviewSingleView;
}



void CWBEagelEye::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	g_pWhiteBoardView->GetClientRect(&rect);
/*	ClientToScreen(&rect);*/
	if (lpwndpos->x < (rect.left +1))
		lpwndpos->x = rect.left+1;

 	if (lpwndpos->x >  (rect.right/*-5 */- m_rect.Width()))
 		lpwndpos->x = rect.right/*-5*/ - m_rect.Width();

	if (lpwndpos->y < (rect.top+1))
		lpwndpos->y = rect.top +1;

	if (lpwndpos->y > (rect.bottom/*-2*/ - m_rect.Height()))
		lpwndpos->y = rect.bottom/*-2 */- m_rect.Height();
	lpwndpos->hwndInsertAfter = wndTop;
	CDialog::OnWindowPosChanging(lpwndpos);
}


BOOL CWBEagelEye::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//算出想要刷新背景的区域
// 	CRect rtClient;
// 	this->GetWindowRect(&rtClient);
// 	ClientToScreen(&rtClient);
// 	CRgn rtClientRgn;
// 	rtClientRgn.CreateRectRgnIndirect(&rtClient);
// 	CRect Viewrect;
// 	m_pWBPreviewSingleView->GetClientRect(&Viewrect);
// 	ClientToScreen(&Viewrect);
// 	CRgn rectRgn;
// 	rectRgn.CreateRectRgnIndirect(&Viewrect);
// 
// 	rtClientRgn.CombineRgn(&rtClientRgn, &rectRgn, RGN_XOR);
// 	CBrush br;
// 	br.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
// 	pDC->FillRgn(&rtClientRgn,&br);
// 	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}
