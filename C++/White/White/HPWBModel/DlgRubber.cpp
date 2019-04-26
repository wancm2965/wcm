// DlgRubber.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "DlgRubber.h"


// CDlgRubber 对话框

IMPLEMENT_DYNAMIC(CDlgRubber, CDialog)

CDlgRubber::CDlgRubber(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRubber::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDlgRubber::~CDlgRubber()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgRubber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_BIGRUBBER, m_btnBigRubber);
	DDX_Control(pDX, IDC_BTN_LITTERRUBBER, m_btnLitterRubber);
	DDX_Control(pDX, IDC_BTN_CLEAROBJECT, m_btnClearObject);
	DDX_Control(pDX, IDC_BTN_CURVERUBBER, m_btnCurveRubber);
}


BEGIN_MESSAGE_MAP(CDlgRubber, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_BIGRUBBER, &CDlgRubber::OnBnClickedBtnBigrubber)
	ON_BN_CLICKED(IDC_BTN_CLEAROBJECT, &CDlgRubber::OnBnClickedBtnClearobject)
	ON_BN_CLICKED(IDC_BTN_CURVERUBBER, &CDlgRubber::OnBnClickedBtnCurverubber)
	ON_BN_CLICKED(IDC_BTN_LITTERRUBBER, &CDlgRubber::OnBnClickedBtnLitterrubber)
END_MESSAGE_MAP()


// CDlgRubber 消息处理程序

BOOL CDlgRubber::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
    
	m_btnBigRubber.SetType(TYPE_RUBBER_BIG );
	m_btnLitterRubber.SetType(TYPE_RUBBER_SMALL);
    m_btnClearObject.SetType(TYPE_RUBBER_OBJ );
    m_btnCurveRubber.SetType(TYPE_RUBBER_CURVE);

	m_btnBigRubber.SetParentWnd(this);
	m_btnLitterRubber.SetParentWnd(this);
    m_btnClearObject.SetParentWnd(this);
    m_btnCurveRubber.SetParentWnd(this);

	//工具提示控件
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnBigRubber,_T("大橡皮擦"));
	m_ToolTip.AddTool(&m_btnLitterRubber,_T("小橡皮擦"));
	m_ToolTip.AddTool(&m_btnClearObject,_T("擦对象"));
	m_ToolTip.AddTool(&m_btnCurveRubber,_T("曲线擦除"));

	return TRUE;  
}
void CDlgRubber::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//加载图片
void CDlgRubber::SetSkinImage(CString strPath)
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
void CDlgRubber::FlashSkin()
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
void CDlgRubber::Relayout()
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
	m_btnBigRubber.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnLitterRubber.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnClearObject.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnCurveRubber.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}
void CDlgRubber::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}
BOOL CDlgRubber::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72按钮");
	SetSkinImage(strPath + _T("\\238皮肤.png"));
    return true;
	//return CDialog::OnEraseBkgnd(pDC);
}
void CDlgRubber::OnBnClickedBtnBigrubber()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RUBBER_BIG);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgRubber::OnBnClickedBtnClearobject()
{
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RUBBER_OBJ);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgRubber::OnBnClickedBtnCurverubber()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RUBBER_CURVE);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgRubber::OnBnClickedBtnLitterrubber()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RUBBER_SMALL);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgRubber::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}

BOOL CDlgRubber::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
