// DlgFile.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "DlgFile.h"


// CDlgFile 对话框

IMPLEMENT_DYNAMIC(CDlgFile, CDialog)

CDlgFile::CDlgFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFile::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDlgFile::~CDlgFile()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BTN_RESAVE, m_btnResave);
	DDX_Control(pDX, IDC_BTN_PRINT, m_btnPrint);
	DDX_Control(pDX, IDC_BTN_CLOSEFLIE, m_btnCloseFile);
}


BEGIN_MESSAGE_MAP(CDlgFile, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_CLOSEFLIE, &CDlgFile::OnBnClickedBtnCloseflie)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CDlgFile::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_PRINT, &CDlgFile::OnBnClickedBtnPrint)
	ON_BN_CLICKED(IDC_BTN_RESAVE, &CDlgFile::OnBnClickedBtnResave)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgFile::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgFile 消息处理程序

BOOL CDlgFile::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    return true;
	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgFile::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
    
	//CDialog::OnOK();
}

BOOL CDlgFile::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();

	m_btnOpen.SetType(TYPE_OPEN );
	m_btnSave.SetType(TYPE_SAVE);
    m_btnResave.SetType(TYPE_RESAVE);
    m_btnPrint.SetType(TYPE_PRINT);
	m_btnCloseFile.SetType(TYPE_CLOSE);

	m_btnOpen.SetParentWnd(this);
	m_btnSave.SetParentWnd(this);
    m_btnResave.SetParentWnd(this);
    m_btnPrint.SetParentWnd(this);
	m_btnCloseFile.SetParentWnd(this);

	 //工具提示控件
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnOpen,_T("打开"));
	m_ToolTip.AddTool(&m_btnSave,_T("保存"));
	m_ToolTip.AddTool(&m_btnResave,_T("另存"));
	m_ToolTip.AddTool(&m_btnPrint,_T("打印"));
	m_ToolTip.AddTool(&m_btnCloseFile,_T("关闭"));


	return TRUE;  // return TRUE unless you set the focus to a control

}
void CDlgFile::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//加载图片
void CDlgFile::SetSkinImage(CString strPath)
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
void CDlgFile::FlashSkin()
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
void CDlgFile::Relayout()
{
    CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 327;//对话框Y轴开始
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
	if((toolYBegin + 296) > screenRC.bottom) //不能超出屏幕底部
	{
		toolYBegin = screenRC.bottom - 300;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,296,SWP_SHOWWINDOW);
	m_btnOpen.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnSave.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnResave.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnPrint.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnCloseFile.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}


void CDlgFile::OnBnClickedBtnCloseflie()
{
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_CLOSE);
		pToolBar->setFileDlgNULL();
	}
}

void CDlgFile::OnBnClickedBtnOpen()
{
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_OPEN);
		pToolBar->setFileDlgNULL();
	}
}

void CDlgFile::OnBnClickedBtnPrint()
{

	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_PRINT);
		pToolBar->setFileDlgNULL();
	}
}

void CDlgFile::OnBnClickedBtnResave()
{
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_RESAVE);
		pToolBar->setFileDlgNULL();
	}
}

void CDlgFile::OnBnClickedBtnSave()
{
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_SAVE);
		pToolBar->setFileDlgNULL();
	}
}

void CDlgFile::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}

BOOL CDlgFile::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
