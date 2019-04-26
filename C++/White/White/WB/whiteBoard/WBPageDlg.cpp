// WBPageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBPageDlg.h"
#include "afxdialogex.h"
#include "WBToolboxDlg.h"


// CWBPageDlg 对话框
HWND CWBPageDlg::g_WBPageDlgHWND = NULL;
IMPLEMENT_DYNAMIC(CWBPageDlg, CDialog)

CWBPageDlg::CWBPageDlg(CWBToolbarDlg *pToolbar,CWnd* pParent /*=NULL*/)
	: CDialog(CWBPageDlg::IDD, pParent)
{
	m_pToolbar = pToolbar;
}

CWBPageDlg::~CWBPageDlg()
{
}

void CWBPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_FUNCTIONDLGPAGEBTN, m_previewPageBtn);
	DDX_Control(pDX, IDC_PREPAGEBTN, m_prePageBtn);
	DDX_Control(pDX, IDC_NEXTPAGEBTN, m_nextPageBtn);
	DDX_Control(pDX, IDC_PAGENUMBERSTATIC, m_pageNumberStatic);
}


BEGIN_MESSAGE_MAP(CWBPageDlg, CDialog)
	//ON_BN_CLICKED(IDC_FUNCTIONDLGPAGEBTN, &CWBPageDlg::OnBnClickedPreviewpagebtn)
	ON_BN_CLICKED(IDC_PREPAGEBTN, &CWBPageDlg::OnBnClickedPrepagebtn)
	ON_BN_CLICKED(IDC_NEXTPAGEBTN, &CWBPageDlg::OnBnClickedNextpagebtn)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_WB_SHOWPAGENUMBER, OnShowPageNumber)
END_MESSAGE_MAP()

void CWBPageDlg::OnOK()
{
	//	CDialog::OnOK();
}


void CWBPageDlg::OnCancel()
{
	// 	CDialog::OnCancel();
}


BOOL CWBPageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect clientRc;
	GetClientRect(&clientRc);



	





	//功能面板按钮
	//m_previewPageBtn.SetWindowPos(&wndTop,0,0,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_HIDEWINDOW);
	//上一页按钮
	m_prePageBtn.SetWindowPos(&wndTop,0 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	//页码显示
	ShowPageNumber();

	m_pageNumberStatic.SetWindowPos(&wndTop
		,WB_TOOLBOXBTN_SPEC_WIDTH  
		,8
		,50,WB_TOOLBOXBTN_SPEC_HEIGHT -  8 * 2,SWP_SHOWWINDOW);
	//下一页按钮
	m_nextPageBtn.SetWindowPos(&wndTop,WB_TOOLBOXBTN_SPEC_WIDTH  + 50 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	//m_previewPageBtn.SetType(TYPE_FUNCTIONDLG);
    //m_previewPageBtn.SetWindowText(GetCaptureFromType(TYPE_FUNCTIONDLG));
	
	m_prePageBtn.SetType(TYPE_PRE_PAGE);
	m_prePageBtn.SetWindowText(GetCaptureFromType(TYPE_PRE_PAGE));
	m_nextPageBtn.SetType(TYPE_NEXT_PAGE);
	m_nextPageBtn.SetWindowText(GetCaptureFromType(TYPE_NEXT_PAGE));

	g_WBPageDlgHWND = m_hWnd;
	SetPreAndNextBtnStatic();
	return TRUE; 
	
}
CString CWBPageDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_FUNCTIONDLG:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FUNCTIONDLG"));
			if (BtnText.IsEmpty() )
			{
				return _T("功能面板");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_PRE_PAGE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PRE_PAGE"));
			if (BtnText.IsEmpty() )
			{
				return _T("上一页");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_NEXT_PAGE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_NEXT_PAGE"));
			if (BtnText.IsEmpty() )
			{
				return _T("下一页");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	default:
		{
			return _T("");
		}
		break;
	}
}
// CWBPageDlg 消息处理程序


void CWBPageDlg::OnBnClickedPreviewpagebtn()
{
	m_pToolbar->ProcessCmd(TYPE_FUNCTIONDLG);

}


void CWBPageDlg::OnBnClickedPrepagebtn()
{
	m_pToolbar->ProcessCmd(TYPE_PRE_PAGE);
}


void CWBPageDlg::OnBnClickedNextpagebtn()
{
	m_pToolbar->ProcessCmd(TYPE_NEXT_PAGE);
}

BOOL CWBPageDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}




void CWBPageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDialog::OnPaint();



	CRect clientRc;
	GetClientRect(&clientRc);
	CRgn rtClientRgn;
	rtClientRgn.CreateRectRgnIndirect(&clientRc);
	CRgn rgnnew;
	rgnnew.CreateRectRgnIndirect(&clientRc);
	CRgn rgnpre;
	CRgn numrgn;
	CRgn nextrgn;
	rgnpre.CreateRoundRectRgn(0 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH+1,WB_TOOLBOXBTN_SPEC_HEIGHT+1,5,5);
	numrgn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH,8,WB_TOOLBOXBTN_SPEC_WIDTH+51,8 +WB_TOOLBOXBTN_SPEC_HEIGHT -  8 * 2+1,5,5);
	nextrgn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH  + 50 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH  + 50+WB_TOOLBOXBTN_SPEC_WIDTH+1,WB_TOOLBOXBTN_SPEC_HEIGHT+1,5,5);
	rgnnew.CombineRgn(&rgnnew,&rgnpre,RGN_XOR);
	rgnnew.CombineRgn(&rgnnew,&numrgn,RGN_XOR);
	rgnnew.CombineRgn(&rgnnew,&nextrgn,RGN_XOR);
	rtClientRgn.CombineRgn(&rtClientRgn,&rgnnew,RGN_XOR);

	SetWindowRgn((HRGN)rtClientRgn,TRUE);
	rgnnew.DeleteObject();
	rtClientRgn.DeleteObject();
	rgnpre.DeleteObject();
	numrgn.DeleteObject();
	nextrgn.DeleteObject();


// 	RECT re;
// 	//第一个间隔
// 	re.left=WB_TOOLBOXBTN_SPEC_WIDTH + 20 ;
// 	re.top=0;
// 	re.right=WB_TOOLBOXBTN_SPEC_WIDTH + 20;
// 	re.bottom=WB_TOOLBOXBTN_SPEC_HEIGHT;
// 	CRect rtClient;
// 	GetClientRect(&rtClient);
// 
// 	CRgn rtClientRgn; //整个对话框区域
// 	rtClientRgn.CreateRectRgnIndirect(&rtClient);
// 	//rectRgn是第一个间隔区域，topStaticRgn是STATIC的上部区域，bottomStaticRgn是STATIC下部分区域
// 	CRgn rectRgn,topStaticRgn,bottomStaticRgn;
// 	//rectRgn.CreateRectRgnIndirect(&re);
// 
// 	//rtClientRgn.CombineRgn(&rtClientRgn, &rectRgn, RGN_XOR);
// 	//STATIC的上面部分
// 	re.left=WB_TOOLBOXBTN_SPEC_WIDTH;
// 	re.top=0;
// 	re.right= re.left  + 50;
// 	re.bottom=8;
// 	topStaticRgn.CreateRectRgnIndirect(&re);
// 	rtClientRgn.CombineRgn(&rtClientRgn, &topStaticRgn, RGN_XOR);
// 	//STATIC的下面部分
// 	re.left=WB_TOOLBOXBTN_SPEC_WIDTH ;
// 	re.top=WB_TOOLBOXBTN_SPEC_HEIGHT -  8;
// 	re.right= re.left  + 50 ;
// 	re.bottom=WB_TOOLBOXBTN_SPEC_HEIGHT;
// 	bottomStaticRgn.CreateRectRgnIndirect(&re);
// 	rtClientRgn.CombineRgn(&rtClientRgn, &bottomStaticRgn, RGN_XOR);
// 
// 	SetWindowRgn((HRGN)rtClientRgn, TRUE);
// 
// 	rtClientRgn.DeleteObject();
// 	rectRgn.DeleteObject();
// 	topStaticRgn.DeleteObject();
// 	bottomStaticRgn.DeleteObject();
}
void CWBPageDlg::ShowPageNumber()
{
	//页码显示
	CString str;
	str.Format(_T("%d/%d"), g_pWBFile->GetCurPageIndex(), g_pWBFile->GetPageCount());
	m_pageNumberStatic.SetCaption(str);
	/*m_pageNumberStatic.SetWindowText(str);*/
}
LRESULT CWBPageDlg::OnShowPageNumber(WPARAM wParam, LPARAM lParam)
{
	ShowPageNumber();
	return 0;
}

BOOL CWBPageDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CWBPageDlg::SetPreAndNextBtnStatic()
{
	UINT CurentPageNum = g_pWBFile->GetCurPageIndex();
	UINT CountPageNum = g_pWBFile->GetPageCount();
	if (CurentPageNum < CountPageNum)
	{
		m_nextPageBtn.EnableWindow(TRUE);
	} 
	else
	{
		m_nextPageBtn.EnableWindow(FALSE);
	}
	if (CurentPageNum > 1)
	{
	    m_prePageBtn.EnableWindow(TRUE);
	} 
	else
	{
		m_prePageBtn.EnableWindow(FALSE);
	}
	m_prePageBtn.Invalidate();
	m_nextPageBtn.Invalidate();
}
