// WBEagelEye.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBEagelEye.h"
#include "afxdialogex.h"


// CWBEagelEye �Ի���

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


// CWBEagelEye ��Ϣ�������


BOOL CWBEagelEye::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	// �쳣: OCX ����ҳӦ���� FALSE
}


CWBPreviewSingleView* CWBEagelEye::GetPreviewSingleView()
{
	return m_pWBPreviewSingleView;
}



void CWBEagelEye::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�����Ҫˢ�±���������
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
