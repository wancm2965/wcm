// WBRollCallDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRollCallDialog.h"
#include "afxdialogex.h"
#define  ROLLCALLTIMER 198910

// CWBRollCallDialog 对话框

IMPLEMENT_DYNAMIC(CWBRollCallDialog, CDialogEx)

CWBRollCallDialog::CWBRollCallDialog(int left,int top,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBRollCallDialog::IDD, pParent)
	,m_nTimer(30)
{
	m_left = left;
	m_top = top;
}

CWBRollCallDialog::~CWBRollCallDialog()
{
}

void CWBRollCallDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROLLCALLSTATIC, m_nStringStatic);
	DDX_Control(pDX, IDC_ROLLCALLOKBTN, m_nOKBtn);
	DDX_Control(pDX, IDC_ROLLCALLCANCLEBTN, m_nCancleBtn);
}


BEGIN_MESSAGE_MAP(CWBRollCallDialog, CDialogEx)
	ON_BN_CLICKED(IDC_ROLLCALLCANCLEBTN, &CWBRollCallDialog::OnBnClickedRollcallcanclebtn)
	ON_BN_CLICKED(IDC_ROLLCALLOKBTN, &CWBRollCallDialog::OnBnClickedRollcallokbtn)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CWBRollCallDialog 消息处理程序


void CWBRollCallDialog::OnBnClickedRollcallcanclebtn()
{

	KillRollCallTimer();
}


void CWBRollCallDialog::OnBnClickedRollcallokbtn()
{
	
	KillRollCallTimer();
	g_pWhiteBoardView->SendRollCallBroadCast(1);
	
}

BOOL CWBRollCallDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
// 	CString str = _T("点名倒计时：");
// 	CString strtime;
// 	strtime.Format(_T("%d"),m_nTimer);
// 	str += strtime;
// 	m_nStringStatic.SetWindowText(str);
// 	MoveWindow(250 + g_pWhiteBoardView->getXnPos(),150 + g_pWhiteBoardView->getYnPos(),250,150);
// 	m_nOKBtn.SetWindowText(_T("签到"));
// 	m_nCancleBtn.SetWindowText(_T("取消"));
	CString str =ReadConfigText(_T("RollCallDialog"), _T("RollCallcountdown"));
	if (str.IsEmpty())
	{
		str = _T("点名倒计时：");

	}
	CString strtime;
	strtime.Format(_T("%d"),m_nTimer);
	str += strtime;
	m_nStringStatic.SetWindowText(str);
	MoveWindow(250 + m_left,150 + m_top,250,150);
	str = ReadConfigText(_T("RollCallDialog"), _T("singin "));
	if (str.IsEmpty())
	{
		str = _T("签到");
	}
	m_nOKBtn.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("cancle "));
	if (str.IsEmpty())
	{
		str = _T("取消");
	}
	m_nCancleBtn.SetWindowText(str);
	m_nOKBtn.MoveWindow(110,90,50,30);
	m_nCancleBtn.MoveWindow(170,90,50,30);
// 	DWORD dwStyle;
// 	dwStyle = ::GetWindowLong(m_hWnd,GWL_STYLE);
// 	dwStyle &= ~WS_THICKFRAME;
// 	::SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(SW_SHOW);
	SetTimer(ROLLCALLTIMER,1000,NULL);
	return TRUE;

}

void CWBRollCallDialog::OnTimer( UINT_PTR nIDEvent )
{
	if (nIDEvent == ROLLCALLTIMER)
	{
		m_nStringStatic.UpdateData(FALSE);
		CString str =ReadConfigText(_T("RollCallDialog"), _T("RollCallcountdown"));
		if (str.IsEmpty())
		{
			str = _T("点名倒计时：");
			
		}	
		CString strtime;
		strtime.Format(_T("%d"),m_nTimer);
		str += strtime;
		m_nStringStatic.SetWindowText(str);
		if (m_nTimer == 0)
		{
			ShowWindow(SW_HIDE);
			KillTimer(ROLLCALLTIMER);
			return;
		}
		m_nTimer--;
		m_nStringStatic.UpdateData();
		
	}
}

void CWBRollCallDialog::OnClose()
{
	KillTimer(ROLLCALLTIMER);
	CDialogEx::OnClose();
	delete this;
}

void CWBRollCallDialog::KillRollCallTimer()
{
	ShowWindow(SW_HIDE);
	KillTimer(ROLLCALLTIMER);
	
}

void CWBRollCallDialog::SetRollCallShow(int left,int top)
{
	m_left = left;
	m_top = top;
	m_nTimer = 30;
	CString str =ReadConfigText(_T("RollCallDialog"), _T("RollCallcountdown"));
	if (str.IsEmpty())
	{
		str = _T("点名倒计时：");
		
	}
	CString strtime;
	strtime.Format(_T("%d"),m_nTimer);
	str += strtime;
	m_nStringStatic.SetWindowText(str);
	MoveWindow(250 + m_left ,150 + m_top ,250,150);
	str = ReadConfigText(_T("RollCallDialog"), _T("singin "));
	if (str.IsEmpty())
	{
		str = _T("签到");
	}
	m_nOKBtn.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("cancle "));
	if (str.IsEmpty())
	{
		str = _T("取消");
	}
	m_nCancleBtn.SetWindowText(str);
	m_nOKBtn.MoveWindow(110,90,50,30);
	m_nCancleBtn.MoveWindow(170,90,50,30);
	ShowWindow(SW_SHOW);
	SetTimer(ROLLCALLTIMER,1000,NULL);
}


void CWBRollCallDialog::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	rc.SetRect(0,0,250,150);
	MoveWindow(250 + m_left,150 + m_top,250,150);
	//CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}
