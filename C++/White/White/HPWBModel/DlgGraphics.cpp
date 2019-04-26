// DlgGraphics.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgGraphics.h"
#include "DlgToolBar.h"


// CDlgGraphics 对话框

IMPLEMENT_DYNAMIC(CDlgGraphics, CDialog)

CDlgGraphics::CDlgGraphics(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGraphics::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;

}

CDlgGraphics::~CDlgGraphics()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgGraphics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_RECT, m_btmRect);
	DDX_Control(pDX, IDC_BTN_RHOMBUS, m_btmRhombus);
	DDX_Control(pDX, IDC_BTN_ELLIPSE, m_btmEllipse);
	DDX_Control(pDX, IDC_BTN_TRIANGLE, m_btmTriangle);
	DDX_Control(pDX, IDC_BTN_POLYGON, m_btmPolygon);
}


BEGIN_MESSAGE_MAP(CDlgGraphics, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_ELLIPSE, &CDlgGraphics::OnBnClickedBtnEllipse)
	ON_BN_CLICKED(IDC_BTN_POLYGON, &CDlgGraphics::OnBnClickedBtnPolygon)
	ON_BN_CLICKED(IDC_BTN_RECT, &CDlgGraphics::OnBnClickedBtnRect)
	ON_BN_CLICKED(IDC_BTN_RHOMBUS, &CDlgGraphics::OnBnClickedBtnRhombus)
	ON_BN_CLICKED(IDC_BTN_TRIANGLE, &CDlgGraphics::OnBnClickedBtnTriangle)
END_MESSAGE_MAP()


// CDlgGraphics 消息处理程序

BOOL CDlgGraphics::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72按钮");
	//SetSkinImage(strPath + _T("\\296皮肤.png"));
    SetSkinImage(strPath + _T("\\238皮肤.png"));
    return true;
	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgGraphics::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
}

BOOL CDlgGraphics::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();

	m_btmRect.SetType(TYPE_RECTANGLE);
	m_btmRhombus.SetType(TYPE_RHOMB);
    m_btmEllipse.SetType(TYPE_ROUNDNESS);
    m_btmTriangle.SetType(TYPE_TRIANGLE );
	//m_btmPolygon.SetType(TYPE_POLYGON );

	m_btmRect.SetParentWnd(this);
	m_btmRhombus.SetParentWnd(this);
    m_btmEllipse.SetParentWnd(this);
    m_btmTriangle.SetParentWnd(this);
	//m_btmPolygon.SetParentWnd(this);
 
	 //工具提示控件
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btmRect,_T("矩形"));
	m_ToolTip.AddTool(&m_btmRhombus,_T("菱形"));
	m_ToolTip.AddTool(&m_btmEllipse,_T("圆"));
	m_ToolTip.AddTool(&m_btmTriangle,_T("三角形"));
	//m_ToolTip.AddTool(&m_btmPolygon,_T("多边形"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CDlgGraphics::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//加载图片
void CDlgGraphics::SetSkinImage(CString strPath)
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
void CDlgGraphics::FlashSkin()
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
void CDlgGraphics::Relayout()
{
    CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 327;
	int toolXBegin = rc.right;    //对话框X轴开始
	POINT point;
    GetCursorPos(&point);
	if( point.y < (rc.top + 327) )
	{ 
          toolYBegin = rc.top + WIDTH_BTN + 35;
	}
	if( (rc.right + 70) > screenRC.right)  //不能超出屏幕右边
	{
         toolXBegin = rc.left - 70;
	}
	if((toolYBegin + 238) > screenRC.bottom) //不能超出屏幕底部
	{
		toolYBegin = screenRC.bottom - 240;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,238,SWP_SHOWWINDOW);
	//if( point.y < (rc.top + 327) )
	//{ 
 //         toolYBegin = rc.top  + 35;
	//}
	//if( (rc.right + 70) > screenRC.right)  //不能超出屏幕右边
	//{
 //        toolXBegin = rc.left - 70;
	//}
	//if((toolYBegin + 296) > screenRC.bottom) //不能超出屏幕底部
	//{
	//	toolYBegin = screenRC.bottom - 300;
 //        
	//}
	//SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,296,SWP_SHOWWINDOW);
	m_btmRect.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btmRhombus.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btmEllipse.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btmTriangle.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	//m_btmPolygon.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}


void CDlgGraphics::OnBnClickedBtnEllipse()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_ROUNDNESS);
		pToolBar->setGraphicsDlgNULL();
	}
}

void CDlgGraphics::OnBnClickedBtnPolygon()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_POLYGON);
		pToolBar->setGraphicsDlgNULL();
	}
}

void CDlgGraphics::OnBnClickedBtnRect()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RECTANGLE);
		pToolBar->setGraphicsDlgNULL();
	}
}

void CDlgGraphics::OnBnClickedBtnRhombus()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RHOMB);
		pToolBar->setGraphicsDlgNULL();
	}
}

void CDlgGraphics::OnBnClickedBtnTriangle()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_TRIANGLE);
		pToolBar->setGraphicsDlgNULL();
	}
}

void CDlgGraphics::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}

BOOL CDlgGraphics::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
