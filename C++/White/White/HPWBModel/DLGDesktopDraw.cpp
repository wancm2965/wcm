// DesktopDrawDLG.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "DLGDesktopDraw.h"


// CDLGDesktopDraw �Ի���

IMPLEMENT_DYNAMIC(CDLGDesktopDraw, CDialog)

CDLGDesktopDraw::CDLGDesktopDraw(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGDesktopDraw::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDLGDesktopDraw::~CDLGDesktopDraw()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDLGDesktopDraw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DDOPEN, m_btnDDOpen);
	DDX_Control(pDX, IDC_BTN_DDFULLSCREEN, m_btnDDFullScreen);
	DDX_Control(pDX, IDC_BTN_DDEFFECT, m_btnDDEffect);
	DDX_Control(pDX, IDC_BTN_DDCTRL, m_btnCtrl);
	DDX_Control(pDX, IDC_BTN_DDCLOSE, m_btnDDClose);
}


BEGIN_MESSAGE_MAP(CDLGDesktopDraw, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_DDOPEN, &CDLGDesktopDraw::OnBnClickedBtnDdopen)
	ON_BN_CLICKED(IDC_BTN_DDFULLSCREEN, &CDLGDesktopDraw::OnBnClickedBtnDdfullscreen)
	ON_BN_CLICKED(IDC_BTN_DDEFFECT, &CDLGDesktopDraw::OnBnClickedBtnDdeffect)
	ON_BN_CLICKED(IDC_BTN_DDCTRL, &CDLGDesktopDraw::OnBnClickedBtnDdctrl)
	ON_BN_CLICKED(IDC_BTN_DDCLOSE, &CDLGDesktopDraw::OnBnClickedBtnDdclose)
END_MESSAGE_MAP()


// CDLGDesktopDraw ��Ϣ�������

void CDLGDesktopDraw::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}

void CDLGDesktopDraw::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}

BOOL CDLGDesktopDraw::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// ���öԻ��򱳾�ͼ�ͻ�����ͼ
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72��ť");
	SetSkinImage(strPath + _T("\\296Ƥ��.png"));

	m_btnDDOpen.SetType(TYPE_DDOPEN );
	m_btnDDFullScreen.SetType(TYPE_DDFULLSCREEN);
    m_btnDDEffect.SetType(TYPE_DDEFFECT);
    m_btnCtrl.SetType(TYPE_DDCTRL);
	m_btnDDClose.SetType(TYPE_DDCLOSE);

	m_btnDDOpen.SetParentWnd(this);
	m_btnDDFullScreen.SetParentWnd(this);
    m_btnDDEffect.SetParentWnd(this);
    m_btnCtrl.SetParentWnd(this);
	m_btnDDClose.SetParentWnd(this);

	 //������ʾ�ؼ�
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnDDOpen,_T("��"));
	m_ToolTip.AddTool(&m_btnDDFullScreen,_T("ȫ��"));
	m_ToolTip.AddTool(&m_btnDDEffect,_T("Ч���л�"));
	m_ToolTip.AddTool(&m_btnCtrl,_T("������"));
	m_ToolTip.AddTool(&m_btnDDClose,_T("�ر�"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CDLGDesktopDraw::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//����ͼƬ
void CDLGDesktopDraw::SetSkinImage(CString strPath)
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
//���Ի��򱳾�ͼƬ
void CDLGDesktopDraw::FlashSkin()
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
void CDLGDesktopDraw::Relayout()
{
    CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 327;//�Ի���Y�Ὺʼ
	int toolXBegin = rc.right;    //�Ի���X�Ὺʼ
	POINT point;
    GetCursorPos(&point);
	if( point.y < (rc.top + 327) )
	{ 
         toolYBegin = rc.top  + 35;
	}
	if( (rc.right + 70) > screenRC.right)  //���ܳ�����Ļ�ұ�
	{
         toolXBegin = rc.left - 70;
	}
	if((toolYBegin + 296) > screenRC.bottom) //���ܳ�����Ļ�ײ�
	{
		toolYBegin = screenRC.bottom - 300;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,296,SWP_SHOWWINDOW);
	m_btnDDOpen.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnDDFullScreen.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnDDEffect.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnCtrl.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnDDClose.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}


BOOL CDLGDesktopDraw::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CDLGDesktopDraw::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CDLGDesktopDraw::OnBnClickedBtnDdopen()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DDOPEN);
		pToolBar->setDesktopDrawDlgNULL();
	}
}

void CDLGDesktopDraw::OnBnClickedBtnDdfullscreen()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DDFULLSCREEN);
		pToolBar->setDesktopDrawDlgNULL();
	}
}

void CDLGDesktopDraw::OnBnClickedBtnDdeffect()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DDEFFECT);
		pToolBar->setDesktopDrawDlgNULL();
	}
}

void CDLGDesktopDraw::OnBnClickedBtnDdctrl()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DDCTRL);
		pToolBar->setDesktopDrawDlgNULL();
	}
}

void CDLGDesktopDraw::OnBnClickedBtnDdclose()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DDCLOSE);
		pToolBar->setDesktopDrawDlgNULL();
	}
}
