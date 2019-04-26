// AlphaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "AlphaDlg.h"
#include <windowsx.h>
#include "afxdialogex.h"

// CAlphaDlg 对话框

IMPLEMENT_DYNAMIC(CAlphaDlg, CDialogEx)

	CAlphaDlg::CAlphaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAlphaDlg::IDD, pParent)
	,rects(NULL)
	,m_alpha(0)
{

}
CAlphaDlg::CAlphaDlg(const CRect* rect /*=NULL*/)
	: CDialogEx(CAlphaDlg::IDD, NULL)
	,m_alpha(0)
{
	this->rects=rect;
}
CAlphaDlg::~CAlphaDlg()
{
}

void CAlphaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAlphaDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAlphaDlg 消息处理程序


BOOL CAlphaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetExStyle() | WS_EX_LAYERED );
	VERIFY(::SetLayeredWindowAttributes(m_hWnd,RGB(255,255,255), 0, LWA_COLORKEY)); 
	if(rects!=NULL)
	{
		::MoveWindow(m_hWnd, rects->left,rects->top, rects->Width(), rects->Height() ,TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAlphaDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//this->AlphaChange(this->m_alpha);
	// TODO: 在此处添加消息处理程序代码
}

HBRUSH CAlphaDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr =  (HBRUSH)GetStockBrush(NULL_BRUSH);
	// TODO:  在此更改 DC 的任何特性
	this->AlphaChange(this->m_alpha);
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


BOOL CAlphaDlg::AlphaChange(const int alpha)
{
	CRect rect;
	this->GetClientRect(&rect);
	CDC *pdc = NULL;
	pdc = this->GetDC();
	if (NULL !=pdc)
	{
		pdc->FillSolidRect(rect.left,rect.top,rect.right,rect.bottom,RGB(255,255,255));
	}
	//this->GetDC()->FillSolidRect(rect.left,rect.top,rect.right,rect.bottom,RGB(255,255,255));
	::SetLayeredWindowAttributes(this->m_hWnd,RGB(255,255,255),alpha,LWA_ALPHA);
	this->m_alpha=alpha;

	ReleaseDC(pdc);
	return TRUE;
}
