#include "stdafx.h"
#include "whiteBoard.h"
#include "ButtonCloseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CbuttonCloseDlg 对话框




CbuttonCloseDlg::CbuttonCloseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbuttonCloseDlg::IDD, pParent)
{
	pWBDoc = NULL;
}

void CbuttonCloseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTONCLOSE,m_btd);
}

BEGIN_MESSAGE_MAP(CbuttonCloseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONCLOSE, &CbuttonCloseDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CbuttonCloseDlg 消息处理程序

BOOL CbuttonCloseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    return 0;
}

void CbuttonCloseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbuttonCloseDlg::OnPaint()
{
	CDialogEx::OnPaint();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbuttonCloseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbuttonCloseDlg::OnBnClickedButtonClose()
{
	if (pWBDoc)
	{
		pWBDoc->ButtonClose();
	}
	g_pWhiteBoardView->DestroyFullScreen();
	
}
void CbuttonCloseDlg::setWBDoc( CWBDocument *pDoc )
{
	pWBDoc = pDoc;
}

