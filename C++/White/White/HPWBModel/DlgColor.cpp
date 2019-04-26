// DlgColor.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "DlgColor.h"


// CDlgColor 对话框

IMPLEMENT_DYNAMIC(CDlgColor, CDialog)

CDlgColor::CDlgColor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColor::IDD, pParent)
{

	m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDlgColor::~CDlgColor()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_RED, m_btnRed);
	DDX_Control(pDX, IDC_BTN_BLACK, m_btnBlack);
	DDX_Control(pDX, IDC_BTN_YELLOW, m_btnYellow);
	DDX_Control(pDX, IDC_BTN_ORANGE, m_btnOrange);
	DDX_Control(pDX, IDC_BTN_BLUE, m_btnblue);
	DDX_Control(pDX, IDC_BTN_PURPLE, m_btnPurple);
}


BEGIN_MESSAGE_MAP(CDlgColor, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PURPLE, &CDlgColor::OnBnClickedBtnPurple)
	ON_BN_CLICKED(IDC_BTN_BLUE, &CDlgColor::OnBnClickedBtnBlue)
	ON_BN_CLICKED(IDC_BTN_ORANGE, &CDlgColor::OnBnClickedBtnOrange)
	ON_BN_CLICKED(IDC_BTN_YELLOW, &CDlgColor::OnBnClickedBtnYellow)
	ON_BN_CLICKED(IDC_BTN_BLACK, &CDlgColor::OnBnClickedBtnBlack)
	ON_BN_CLICKED(IDC_BTN_RED, &CDlgColor::OnBnClickedBtnRed)
END_MESSAGE_MAP()


// CDlgColor 消息处理程序

BOOL CDlgColor::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72按钮");
	SetSkinImage(strPath + _T("\\356皮肤.png"));
	return true;
    
	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgColor::OnOK()
{
	

	//CDialog::OnOK();
}

BOOL CDlgColor::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();

    
	m_btnRed.SetType(TYPE_CLR_REN );
	m_btnBlack.SetType(TYPE_CLR_BLACK);
    m_btnYellow.SetType(TYPE_CLR_YELLOW);
    m_btnOrange.SetType(TYPE_CLR_ORANGE);
	m_btnblue.SetType(TYPE_CLR_BLUE);
	m_btnPurple.SetType(TYPE_CLR_PURPLE);

	m_btnRed.SetParentWnd(this);
	m_btnBlack.SetParentWnd(this);
    m_btnYellow.SetParentWnd(this);
    m_btnOrange.SetParentWnd(this);
	m_btnblue.SetParentWnd(this);
	m_btnPurple.SetParentWnd(this);
   //工具提示控件
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnRed,_T("红色"));
	m_ToolTip.AddTool(&m_btnBlack,_T("黑色"));
	m_ToolTip.AddTool(&m_btnYellow,_T("黄色"));
	m_ToolTip.AddTool(&m_btnOrange,_T("橙色"));
	m_ToolTip.AddTool(&m_btnblue,_T("蓝色"));
	m_ToolTip.AddTool(&m_btnPurple,_T("紫色"));

	return TRUE;  

}

void CDlgColor::OnCancel()
{

	//CDialog::OnCancel();
}

void CDlgColor::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//加载图片
void CDlgColor::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
		m_pBitmap = new Bitmap(strPath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		m_pBitmap = new Bitmap(strPath);
	}
    FlashSkin();
}
//画对话框背景图片
void CDlgColor::FlashSkin()
{
	if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
}
void CDlgColor::Relayout()
{
    CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)); //屏幕矩形
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 266;
	int toolXBegin = rc.right;    //对话框X轴开始
	POINT point;
    GetCursorPos(&point);
	if( point.y < (rc.top + 327) )
	{ 
          toolYBegin = rc.top  + 35;
	}
	if( (rc.right + 70) > screenRC.right)  //不能超出屏幕右边
	{
         toolXBegin = rc.left - 70;
	}
	if((toolYBegin + 356) > screenRC.bottom) //不能超出屏幕底部
	{
		toolYBegin = screenRC.bottom - 360;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,356,SWP_SHOWWINDOW);
	m_btnRed.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnBlack.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnYellow.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnOrange.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnblue.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnPurple.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*5,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}


void CDlgColor::OnBnClickedBtnPurple()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_PURPLE);
		pToolBar->setColorDlgNULL();
	}
}

void CDlgColor::OnBnClickedBtnBlue()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_BLUE);
		pToolBar->setColorDlgNULL();
	}
}

void CDlgColor::OnBnClickedBtnOrange()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_ORANGE);
		pToolBar->setColorDlgNULL();
	}
}

void CDlgColor::OnBnClickedBtnYellow()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_YELLOW);
		pToolBar->setColorDlgNULL();
	}
}

void CDlgColor::OnBnClickedBtnBlack()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_BLACK);
		pToolBar->setColorDlgNULL();
	}
}

void CDlgColor::OnBnClickedBtnRed()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLR_REN);
		pToolBar->setColorDlgNULL();
	}
}

BOOL CDlgColor::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
