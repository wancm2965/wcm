// WBColorFillPad.cpp : 实现文件
/****************************
*		图案填充面板		*
*		caoyuqin2012.3.8	*
****************************/

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBDesignFillPad.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CWBDesignFillPad 对话框

IMPLEMENT_DYNAMIC(CWBDesignFillPad, CDialogEx)

CWBDesignFillPad::CWBDesignFillPad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBDesignFillPad::IDD, pParent)
{
}

CWBDesignFillPad::~CWBDesignFillPad()
{
}

void CWBDesignFillPad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN1, m_cClrFillBtnOne);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN2, m_cClrFillBtnTwo);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN3, m_cClrFillBtnThree);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN4, m_cClrFillBtnFour);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN5, m_cClrFillBtnFive);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN6, m_cClrFillBtnSix);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN7, m_cClrFillBtnSeven);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN8, m_cClrFillBtnEight);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN9, m_cClrFillBtnNine);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN10, m_cClrFillBtnTen);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN11, m_cClrFillBtnEleven);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN12, m_cClrFillBtnTwelve);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN13, m_cClrFillBtnThirteen);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN14, m_cClrFillBtnFourteen);
	DDX_Control(pDX, IDC_SINWBCLRFILLBTN15, m_cClrFillBtnFifteen);
}


BEGIN_MESSAGE_MAP(CWBDesignFillPad, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN1, &CWBDesignFillPad::OnBnClickedClrfillbtn1)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN2, &CWBDesignFillPad::OnBnClickedClrfillbtn2)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN3, &CWBDesignFillPad::OnBnClickedClrfillbtn3)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN4, &CWBDesignFillPad::OnBnClickedClrfillbtn4)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN5, &CWBDesignFillPad::OnBnClickedClrfillbtn5)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN6, &CWBDesignFillPad::OnBnClickedClrfillbtn6)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN7, &CWBDesignFillPad::OnBnClickedClrfillbtn7)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN8, &CWBDesignFillPad::OnBnClickedClrfillbtn8)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN9, &CWBDesignFillPad::OnBnClickedClrfillbtn9)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN10, &CWBDesignFillPad::OnBnClickedClrfillbtn10)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN11, &CWBDesignFillPad::OnBnClickedClrfillbtn11)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN12, &CWBDesignFillPad::OnBnClickedClrfillbtn12)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN13, &CWBDesignFillPad::OnBnClickedClrfillbtn13)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN14, &CWBDesignFillPad::OnBnClickedClrfillbtn14)
	ON_BN_CLICKED(IDC_SINWBCLRFILLBTN15, &CWBDesignFillPad::OnBnClickedClrfillbtn15)
END_MESSAGE_MAP()


// CWBDesignFillPad 消息处理程序
BOOL CWBDesignFillPad::OnEraseBkgnd(CDC* pDC)
{
	//对话框背景	
	//CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}
	return TRUE;
}

BOOL CWBDesignFillPad::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_cClrFillBtnOne.MoveWindow(WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnFour.MoveWindow(WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNHEIGHT,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnSeven.MoveWindow(WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNHEIGHT * 2,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnTen.MoveWindow(WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTN_SPACE * 4 + WB_FILLPAD_BTNHEIGHT * 3,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnThirteen.MoveWindow(WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTN_SPACE * 5 + WB_FILLPAD_BTNHEIGHT * 4,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	
	m_cClrFillBtnTwo.MoveWindow(WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnFive.MoveWindow(WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNHEIGHT,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnEight.MoveWindow(WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNHEIGHT * 2,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnEleven.MoveWindow(WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTN_SPACE * 4 + WB_FILLPAD_BTNHEIGHT * 3,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnFourteen.MoveWindow(WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTN_SPACE * 5 + WB_FILLPAD_BTNHEIGHT * 4,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	
	m_cClrFillBtnThree.MoveWindow(WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNWIDTH * 2,WB_FILLPAD_BTN_SPACE,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnSix.MoveWindow(WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNWIDTH * 2,WB_FILLPAD_BTN_SPACE * 2 + WB_FILLPAD_BTNHEIGHT,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnNine.MoveWindow(WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNWIDTH * 2,WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNHEIGHT * 2,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnTwelve.MoveWindow(WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNWIDTH * 2,WB_FILLPAD_BTN_SPACE * 4 + WB_FILLPAD_BTNHEIGHT * 3,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	m_cClrFillBtnFifteen.MoveWindow(WB_FILLPAD_BTN_SPACE * 3 + WB_FILLPAD_BTNWIDTH * 2,WB_FILLPAD_BTN_SPACE * 5 + WB_FILLPAD_BTNHEIGHT * 4,WB_FILLPAD_BTNWIDTH,WB_FILLPAD_BTNHEIGHT);
	return TRUE;
}


void CWBDesignFillPad::OnBnClickedClrfillbtn1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnOne.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\1.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn2()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnTwo.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\2.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	

}


void CWBDesignFillPad::OnBnClickedClrfillbtn3()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnThree.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\3-3.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn4()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnFour.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\4-4.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn5()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnFive.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\5-5.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn6()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnSix.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\6-6.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn7()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnSeven.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\7-7.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn8()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnEight.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\8-8.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn9()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnNine.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\9-9.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn10()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnTen.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\10-10.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn11()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnEleven.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\11-11.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn12()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnTwelve.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\12-12.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn13()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnThirteen.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\13-13.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn14()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnFourteen.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\14-14.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}


void CWBDesignFillPad::OnBnClickedClrfillbtn15()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFillBtnState(m_cClrFillBtnFifteen.GetDlgCtrlID());
	//CString strCurPicPath = GetCurrentPath();
	CString strCurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\15-15.png");
	SetBKGfalse();//删除背景对象
	g_pWBPage->CaclBigPic(&strCurPicPath);//在内存dc里面画一张大图并保存在dc里面
	g_pWhiteBoardView->Invalidate();
	
}

void CWBDesignFillPad::SetFillBtnState( int nBtnID )//设置面板中15个按钮的状态
{
	if (nBtnID == m_cClrFillBtnOne.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(TRUE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnTwo.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(TRUE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnThree.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(TRUE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnFour.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(TRUE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnFive.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(TRUE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnSix.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(TRUE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnSeven.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(TRUE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnEight.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(TRUE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnNine.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(TRUE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnTen.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(TRUE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnEleven.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(TRUE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnTwelve.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(TRUE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnThirteen.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(TRUE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnFourteen.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(TRUE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(FALSE);
	}
	if (nBtnID == m_cClrFillBtnFifteen.GetDlgCtrlID())
	{
		m_cClrFillBtnOne.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwo.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThree.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFour.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFive.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSix.SetIsSelectBtn(FALSE);
		m_cClrFillBtnSeven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEight.SetIsSelectBtn(FALSE);
		m_cClrFillBtnNine.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnEleven.SetIsSelectBtn(FALSE);
		m_cClrFillBtnTwelve.SetIsSelectBtn(FALSE);
		m_cClrFillBtnThirteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFourteen.SetIsSelectBtn(FALSE);
		m_cClrFillBtnFifteen.SetIsSelectBtn(TRUE);
	}
	m_cClrFillBtnOne.Invalidate();
	m_cClrFillBtnTwo.Invalidate();
	m_cClrFillBtnThree.Invalidate();
	m_cClrFillBtnFour.Invalidate();
	m_cClrFillBtnFive.Invalidate();
	m_cClrFillBtnSix.Invalidate();
	m_cClrFillBtnSeven.Invalidate();
	m_cClrFillBtnEight.Invalidate();
	m_cClrFillBtnNine.Invalidate();
	m_cClrFillBtnTen.Invalidate();
	m_cClrFillBtnEleven.Invalidate();
	m_cClrFillBtnTwelve.Invalidate();
	m_cClrFillBtnThirteen.Invalidate();
	m_cClrFillBtnFourteen.Invalidate();
	m_cClrFillBtnFifteen.Invalidate();
}
void CWBDesignFillPad::SetBKGfalse()
{
	CWBObject *pBkgObj = g_pWBPage->GetBkgObj();//得到背景对象
	//CObList * pList = g_pWBPage->GetWBObjList();//得到对象列表
	//POSITION pos;

	if (pBkgObj)
	{
		pBkgObj->SetBkg(FALSE);
		//pos=pList->Find(pBkgObj);//从对象列表中删除已有的背景对象
		//pList->RemoveAt(pos);
	}
}