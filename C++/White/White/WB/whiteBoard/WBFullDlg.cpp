// WBFullDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFullDlg.h"
#include "afxdialogex.h"


// CWBFullDlg 对话框

IMPLEMENT_DYNAMIC(CWBFullDlg, CDialogEx)

CWBFullDlg::CWBFullDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBFullDlg::IDD, pParent)
{

}

CWBFullDlg::~CWBFullDlg()
{
}

void CWBFullDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBFullDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CWBFullDlg 消息处理程序


/*************************************************************************
*                         CWBFullDlg::OnInitDialog()                     *
*                             全屏视图初始化工作                         *
*************************************************************************/
BOOL CWBFullDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将"关于 "菜单项添加到系统菜单中。


	// TODO: 在此添加额外的初始化代码

	//创建工具栏
/*	CBitmap bmp;
	UINT ButtonIDArray[2]={IDC_WB_FILEB, IDC_WB_FILEB};      //定义按钮ID数组

	//添加图像到图像列表
	for(int n=0; n<2; n++){
		bmp.LoadBitmap(IDB_WB_FILE + n);
		m_ImageList.Add(&bmp, RGB(255, 255, 255));
		bmp.DeleteObject();
	}
	m_Toolbar.Create(this);
	m_Toolbar.SetButtons(ButtonIDArray, 2);
	m_Toolbar.SetSizes(CSize(60,56),CSize(40, 40));
	m_Toolbar.GetToolBarCtrl().SetImageList(&m_ImageList);
	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);*/


	//自定义主视图区域创建与设置
	m_whiteboard_fullview = (CWhiteBoardView*) RUNTIME_CLASS(CWhiteBoardView)->CreateObject();
	//在目标位置动态创建视图
	if (NULL == m_whiteboard_fullview)  
	{  
		return FALSE;  
	}
	ShowWindow(SW_SHOWMAXIMIZED);
	//m_fullrect.top += 50;
	m_whiteboard_fullview->Create(NULL, NULL, WS_VISIBLE | AFX_WS_DEFAULT_VIEW, m_fullrect, this, NULL); 

	//全屏窗口重生成，将文件中的数据读入到全屏窗口
	//	if ()//判断条件为还原窗口是否有数据，有读取数据，无则不进行操作
	//	{
	//		m_whiteboard_dlg->SendDataFileFull(){}

	//	}
	return TRUE;  // 除非将焦点设置到控件，否则返回TRUE
}

void CWBFullDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	GetClientRect(&m_fullrect);
}
