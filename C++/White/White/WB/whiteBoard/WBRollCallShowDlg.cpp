// WBRollCallShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRollCallShowDlg.h"
#include "afxdialogex.h"


// CWBRollCallShowDlg �Ի���

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
		str = _T("���������");
	}
	m_nListStatic.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("OK "));
	if (str.IsEmpty())
	{
		str = _T("ȷ��");
	}
	m_okBtn.SetWindowText(str);
	str =  ReadConfigText(_T("RollCallDialog"), _T("Cancle  "));
	if (str.IsEmpty())
	{
		str = _T("ȡ��");
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



// CWBRollCallShowDlg ��Ϣ�������


void CWBRollCallShowDlg::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(200 + m_left,200 + m_top,500,300);
	//CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
}

void CWBRollCallShowDlg::ShowagainDlg( int left,int top )
{
	m_left = left;
	m_top = top;
	CString str = ReadConfigText(_T("RollCallDialog"), _T("RollCallResult "));
	if (str.IsEmpty())
	{
		str = _T("���������");
	}
	m_nListStatic.SetWindowText(str);
	str = ReadConfigText(_T("RollCallDialog"), _T("OK "));
	if (str.IsEmpty())
	{
		str = _T("ȷ��");
	}
	m_okBtn.SetWindowText(str);
	str =  ReadConfigText(_T("RollCallDialog"), _T("Cancle  "));
	if (str.IsEmpty())
	{
		str = _T("ȡ��");
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
