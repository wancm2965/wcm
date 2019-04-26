// WBRollCallShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRollCallShowDlg.h"
#include "afxdialogex.h"


// CWBRollCallShowDlg 对话框

IMPLEMENT_DYNAMIC(CWBRollCallShowDlg, CDialogEx)

CWBRollCallShowDlg::CWBRollCallShowDlg(int left,int top,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBRollCallShowDlg::IDD, pParent)
{
	m_left = left;
	m_top = top;
}

CWBRollCallShowDlg::~CWBRollCallShowDlg()
{
}

void CWBRollCallShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WB_ROLLCALLLISTSTATIC, m_nListStatic);
	DDX_Control(pDX, IDC_WB_ROLLCALLLIST, m_RollCallList);
	DDX_Control(pDX, IDOK, m_okBtn);
	DDX_Control(pDX, IDCANCEL, m_cancleBtn);
}


BEGIN_MESSAGE_MAP(CWBRollCallShowDlg, CDialogEx)
	
	
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CWBRollCallShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString str = ReadConfigText(_T("RollCallDialog"), _T("RollCallResult "));
	if (str.IsEmpty())
	{
		str = _T("点名结果：");
	}
	m_nListStatic.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("OK "));
	if (str.IsEmpty())
	{
		str = _T("确定");
	}
	m_okBtn.SetWindowText(str);
	str =  ReadConfigText(_T("RollCallDialog"), _T("Cancle  "));
	if (str.IsEmpty())
	{
		str = _T("取消");
	}
	m_cancleBtn.SetWindowText(str);
	if (g_pWhiteBoardView->m_RollCallNameList.size() != NULL)
	{
		int nNumber = 0;
		std::vector<string>::iterator it;
		for (it = g_pWhiteBoardView->m_RollCallNameList.begin();it < g_pWhiteBoardView->m_RollCallNameList.end(); it++)
		{
			m_RollCallList.InsertString(nNumber,ToDRAWStr(it->c_str()).c_str());
			nNumber++;
		}
		UpdateData(FALSE);
	}
// 	DWORD dwStyle;
// 	dwStyle = ::GetWindowLong(m_hWnd,GWL_STYLE);
// 	dwStyle &= ~WS_THICKFRAME;
// 	::SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(SW_SHOW);
	MoveWindow(200 + m_left,200 + m_top,500,300);
	return TRUE;
}



// CWBRollCallShowDlg 消息处理程序


void CWBRollCallShowDlg::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(200 + m_left,200 + m_top,500,300);
	//CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CWBRollCallShowDlg::ShowagainDlg( int left,int top )
{
	m_left = left;
	m_top = top;
	CString str = ReadConfigText(_T("RollCallDialog"), _T("RollCallResult "));
	if (str.IsEmpty())
	{
		str = _T("点名结果：");
	}
	m_nListStatic.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("OK "));
	if (str.IsEmpty())
	{
		str = _T("确定");
	}
	m_okBtn.SetWindowText(str);
	str =  ReadConfigText(_T("RollCallDialog"), _T("Cancle  "));
	if (str.IsEmpty())
	{
		str = _T("取消");
	}
	m_cancleBtn.SetWindowText(str);
	if (g_pWhiteBoardView->m_RollCallNameList.size() != NULL)
	{
		int nNumber = 0;
		std::vector<string>::iterator it;
		for (it = g_pWhiteBoardView->m_RollCallNameList.begin();it < g_pWhiteBoardView->m_RollCallNameList.end(); it++)
		{
			m_RollCallList.InsertString(nNumber,ToDRAWStr(it->c_str()).c_str());
			nNumber++;
		}
		UpdateData(FALSE);
	}
	// 	DWORD dwStyle;
	// 	dwStyle = ::GetWindowLong(m_hWnd,GWL_STYLE);
	// 	dwStyle &= ~WS_THICKFRAME;
	// 	::SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(SW_SHOW);
	MoveWindow(200 + m_left,200 + m_top,500,300);
}
