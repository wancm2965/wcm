// AlphaDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "AlphaDlg.h"
#include <windowsx.h>
#include "afxdialogex.h"

// CAlphaDlg �Ի���

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


// CAlphaDlg ��Ϣ�������


BOOL CAlphaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetExStyle() | WS_EX_LAYERED );
	VERIFY(::SetLayeredWindowAttributes(m_hWnd,RGB(255,255,255), 0, LWA_COLORKEY)); 
	if(rects!=NULL)
	{
		::MoveWindow(m_hWnd, rects->left,rects->top, rects->Width(), rects->Height() ,TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CAlphaDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//this->AlphaChange(this->m_alpha);
	// TODO: �ڴ˴������Ϣ����������
}

HBRUSH CAlphaDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr =  (HBRUSH)GetStockBrush(NULL_BRUSH);
	// TODO:  �ڴ˸��� DC ���κ�����
	this->AlphaChange(this->m_alpha);
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
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
