// DlgPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "DlgPage.h"


// CDlgPage �Ի���

IMPLEMENT_DYNAMIC(CDlgPage, CDialog)

CDlgPage::CDlgPage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPage::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDlgPage::~CDlgPage()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_NEWPAGE, m_btnNewPage);
	DDX_Control(pDX, IDC_BTN_PREPAGE, m_btnPrePage);
	DDX_Control(pDX, IDC_BTN_NEXTPAGE, m_btnNextPage);
}


BEGIN_MESSAGE_MAP(CDlgPage, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_NEWPAGE, &CDlgPage::OnBnClickedBtnNewpage)
	ON_BN_CLICKED(IDC_BTN_PREPAGE, &CDlgPage::OnBnClickedBtnPrepage)
	ON_BN_CLICKED(IDC_BTN_NEXTPAGE, &CDlgPage::OnBnClickedBtnNextpage)
END_MESSAGE_MAP()


// CDlgPage ��Ϣ�������



BOOL CDlgPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();

	m_btnNewPage.SetType(TYPE_PAGE );
	m_btnPrePage.SetType(TYPE_PRE_PAGE);
    m_btnNextPage.SetType(TYPE_NEXT_PAGE );

	m_btnNewPage.SetParentWnd(this);
	m_btnPrePage.SetParentWnd(this);
    m_btnNextPage.SetParentWnd(this);

	//������ʾ�ؼ�
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnNewPage,_T("�½�ҳ"));
	m_ToolTip.AddTool(&m_btnPrePage,_T("��һҳ"));
	m_ToolTip.AddTool(&m_btnNextPage,_T("��һҳ"));


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgPage::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//����ͼƬ
void CDlgPage::SetSkinImage(CString strPath)
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
void CDlgPage::FlashSkin()
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
void CDlgPage::Relayout()
{
	CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 387;
	int toolXBegin = rc.right;    //�Ի���X�Ὺʼ
	POINT point;
    GetCursorPos(&point);
	if( point.y < (rc.top + 270))
	{ 
          toolYBegin = rc.top + WIDTH_BTN + 35;
	}
	if( (point.y > (rc.top + 270)) & ( point.y < (rc.top + 440) ))
	{ 
          toolYBegin = rc.top + 275;
	}
	if( (rc.right + 70) > screenRC.right)  //���ܳ�����Ļ�ұ�
	{
         toolXBegin = rc.left - 70;
	}
	if((toolYBegin + 238) > screenRC.bottom) //���ܳ�����Ļ�ײ�
	{
		toolYBegin = screenRC.bottom - 240;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,179,SWP_SHOWWINDOW);
	m_btnNewPage.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnPrePage.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnNextPage.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}

BOOL CDlgPage::PreTranslateMessage(MSG* pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgPage::OnOK()
{
	//CDialog::OnOK();
}

void CDlgPage::OnCancel()
{
	//CDialog::OnCancel();
}
BOOL CDlgPage::OnEraseBkgnd(CDC* pDC)
{
    CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72��ť");
	SetSkinImage(strPath + _T("\\179Ƥ��.png"));
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgPage::OnBnClickedBtnNewpage()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_PAGE);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgPage::OnBnClickedBtnPrepage()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
//	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_PRE_PAGE);
		pToolBar->setRubberDlgNULL();
	}
}

void CDlgPage::OnBnClickedBtnNextpage()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
//	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_NEXT_PAGE);
		pToolBar->setRubberDlgNULL();
	}
}
