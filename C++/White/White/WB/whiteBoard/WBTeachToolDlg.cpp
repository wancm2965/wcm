// WBTeachToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBTeachToolDlg.h"
#include "afxdialogex.h"
#include "WBFunctionDlg.h"


// CWBTeachToolDlg 对话框

IMPLEMENT_DYNAMIC(CWBTeachToolDlg, CDialogEx)

CWBTeachToolDlg::CWBTeachToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBTeachToolDlg::IDD, pParent)
{

}

CWBTeachToolDlg::~CWBTeachToolDlg()
{
}

void CWBTeachToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WB_TTBTN1, m_cTeachToolBtnOne);
	DDX_Control(pDX, IDC_WB_TTBTN2, m_cTeachToolBtnTwo);
	DDX_Control(pDX, IDC_WB_TTBTN3, m_cTeachToolBtnThree);
	DDX_Control(pDX, IDC_WB_TTBTN4, m_cTeachToolBtnFour);
	DDX_Control(pDX, IDC_WB_TTBTN5, m_cTeachToolBtnFive);
	DDX_Control(pDX, IDC_STATICTT, m_cTTStatic);
}


BEGIN_MESSAGE_MAP(CWBTeachToolDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_DOUBLECLICKED(IDC_WB_TTBTN1, &CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn1)
	ON_BN_DOUBLECLICKED(IDC_WB_TTBTN2, &CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn2)
	ON_BN_DOUBLECLICKED(IDC_WB_TTBTN3, &CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn3)
	ON_BN_DOUBLECLICKED(IDC_WB_TTBTN4, &CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn4)
	ON_BN_DOUBLECLICKED(IDC_WB_TTBTN5, &CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn5)
END_MESSAGE_MAP()


// CWBTeachToolDlg 消息处理程序


void CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTTBtnState(m_cTeachToolBtnOne.GetDlgCtrlID());
	WB_Ruler();
}


void CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn2()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTTBtnState(m_cTeachToolBtnTwo.GetDlgCtrlID());
	WB_Compass();
}


void CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn3()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTTBtnState(m_cTeachToolBtnThree.GetDlgCtrlID());
	WB_SetSquare();
}


void CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn4()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTTBtnState(m_cTeachToolBtnFour.GetDlgCtrlID());
	WB_Protractor();
}

void CWBTeachToolDlg::OnBnDoubleclickedWbTtbtn5()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTTBtnState(m_cTeachToolBtnFour.GetDlgCtrlID());
	if(g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
	{
		g_pWhiteBoardView->SendRollCallBroadCast(0);
	}
}

BOOL CWBTeachToolDlg::OnEraseBkgnd( CDC* pDC )
{
	CString CurPicPath = GetCurrentPath();
	CImage img;
	CRect cr;
	GetClientRect(&cr);
	CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load(CurPicPath);
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,cr);
	}
	return TRUE;
}

BOOL CWBTeachToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect nParentRc;
	((CWBFunctionDlg *)GetParent())->GetClientRect(&nParentRc);
	MoveWindow(nParentRc.left+10,nParentRc.top+30,WB_FUNCTION_DLG_WIDTH-15,nParentRc.bottom - nParentRc.top -65);
	m_cTTStatic.MoveWindow(10,35,80,20);
	CString staticText = _T("");
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("STATICTT"));
	if (staticText.IsEmpty())
	{
		staticText = _T("教学工具：");

	}
	m_cTTStatic.SetCaption(staticText);
	m_cTeachToolBtnOne.MoveWindow(20,75,70,70);
	m_cTeachToolBtnTwo.MoveWindow(110,75,70,70);
	m_cTeachToolBtnThree.MoveWindow(20,165,70,70);
	m_cTeachToolBtnFour.MoveWindow(110,165,70,70);
	m_cTeachToolBtnFive.MoveWindow(20,255,70,70);
	if (WB_SINGLERVER == g_WBType)
	{
		if (!g_pWhiteBoardView->GetIsLocalWhiteboard())
		{			
			m_cTeachToolBtnFive.ShowWindow(SW_SHOW);
		}
		else
		{
			m_cTeachToolBtnFive.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_cTeachToolBtnFive.ShowWindow(SW_HIDE);
	}
	return TRUE;
}

void CWBTeachToolDlg::SetTTBtnState( int nBtnID )
{
	if (nBtnID == m_cTeachToolBtnOne.GetDlgCtrlID())
	{
		m_cTeachToolBtnOne.SetIsSelectBtn(TRUE);
		m_cTeachToolBtnTwo.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnThree.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFour.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFive.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cTeachToolBtnTwo.GetDlgCtrlID())
	{
		m_cTeachToolBtnOne.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnTwo.SetIsSelectBtn(TRUE);
		m_cTeachToolBtnThree.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFour.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFive.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cTeachToolBtnThree.GetDlgCtrlID())
	{
		m_cTeachToolBtnOne.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnTwo.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnThree.SetIsSelectBtn(TRUE);
		m_cTeachToolBtnFour.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFive.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cTeachToolBtnFour.GetDlgCtrlID())
	{
		m_cTeachToolBtnOne.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnTwo.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnThree.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFour.SetIsSelectBtn(TRUE);
		m_cTeachToolBtnFive.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cTeachToolBtnFive.GetDlgCtrlID())
	{
		m_cTeachToolBtnOne.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnTwo.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnThree.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFour.SetIsSelectBtn(FALSE);
		m_cTeachToolBtnFive.SetIsSelectBtn(TRUE);
	}
}

void CWBTeachToolDlg::ShowFiveBtnVisable(bool bshow)
{
	if (WB_SINGLERVER == g_WBType)
	{
		if (bshow)
		{			
			m_cTeachToolBtnFive.ShowWindow(SW_SHOW);
		}
		else
		{
			m_cTeachToolBtnFive.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_cTeachToolBtnFive.ShowWindow(SW_HIDE);
	}
}
