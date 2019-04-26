// DlgResource.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgResource.h"
#include "DlgToolBar.h"


// CDlgResource �Ի���

IMPLEMENT_DYNAMIC(CDlgResource, CDialog)

CDlgResource::CDlgResource(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResource::IDD, pParent)
{
    m_pWndParent = NULL;
    m_pBitmap = NULL;
}

CDlgResource::~CDlgResource()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CDlgResource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PICTURE, m_btnPicture);
	DDX_Control(pDX, IDC_BTN_VIEDO, m_btnViedo);
	DDX_Control(pDX, IDC_BTN_FLASH, m_btnFlash);
	DDX_Control(pDX, IDC_BTN_DOCUMENT, m_btnDocument);
}


BEGIN_MESSAGE_MAP(CDlgResource, CDialog)
	ON_BN_CLICKED(IDC_BTN_PICTURE, &CDlgResource::OnBnClickedBtnPicture)
	ON_BN_CLICKED(IDC_BTN_VIEDO, &CDlgResource::OnBnClickedBtnViedo)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CDlgResource::OnBnClickedBtnFlash)
	ON_BN_CLICKED(IDC_BTN_DOCUMENT, &CDlgResource::OnBnClickedBtnDocument)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgResource ��Ϣ�������

BOOL CDlgResource::OnInitDialog()
{
	CDialog::OnInitDialog();
	
    Relayout();

	m_btnPicture.SetType(TYPE_IMAGE);
	m_btnViedo.SetType(TYPE_VIDEO);
    m_btnFlash.SetType(TYPE_FLASH);
    m_btnDocument.SetType(TYPE_DOCUMENT);

	m_btnPicture.SetParentWnd(this);
	m_btnViedo.SetParentWnd(this);
    m_btnFlash.SetParentWnd(this);
    m_btnDocument.SetParentWnd(this);

	//������ʾ�ؼ�
	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.AddTool(&m_btnPicture,_T("ͼƬ"));
	m_ToolTip.AddTool(&m_btnViedo,_T("��Ƶ"));
	m_ToolTip.AddTool(&m_btnFlash,_T("FLASH"));
	m_ToolTip.AddTool(&m_btnDocument,_T("�ĵ�"));


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgResource::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
//����ͼƬ
void CDlgResource::SetSkinImage(CString strPath)
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
void CDlgResource::FlashSkin()
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
void CDlgResource::Relayout()
{
	CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	int x = (rc.Width() - WIDTH_BTN)/2;
	int toolYBegin = rc.top + 327;
	int toolXBegin = rc.right;    //�Ի���X�Ὺʼ
	POINT point;
    GetCursorPos(&point);
	if( point.y < (rc.top + 327) )
	{ 
          toolYBegin = rc.top + WIDTH_BTN + 35;
	}
	if( (rc.right + 70) > screenRC.right)  //���ܳ�����Ļ�ұ�
	{
         toolXBegin = rc.left - 70;
	}
	if((toolYBegin + 238) > screenRC.bottom) //���ܳ�����Ļ�ײ�
	{
		toolYBegin = screenRC.bottom - 240;
         
	}
	SetWindowPos(&wndTop,toolXBegin,toolYBegin, 68,238,SWP_SHOWWINDOW);
	m_btnPicture.SetWindowPos(&wndTop,x,4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    m_btnViedo.SetWindowPos(&wndTop,x,HEIGHT_BTN +4,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnFlash.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*2,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	m_btnDocument.SetWindowPos(&wndTop,x,(HEIGHT_BTN +4)*3,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}

void CDlgResource::OnBnClickedBtnPicture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_btnPicture.Flash();
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{
		pToolBar->ProcessCmd(TYPE_IMAGE);
		pToolBar->setResourceDlgNULL();
	}
	
}

void CDlgResource::OnBnClickedBtnViedo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_btnViedo.Flash();
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_VIDEO);
		pToolBar->setResourceDlgNULL();
	}


}

void CDlgResource::OnBnClickedBtnFlash()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_btnFlash.Flash();
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_FLASH);
		pToolBar->setResourceDlgNULL();
	}

}

void CDlgResource::OnBnClickedBtnDocument()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_btnDocument.Flash();
	
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	DestroyWindow();
	if (pToolBar)
	{		
		pToolBar->ProcessCmd(TYPE_DOCUMENT);
		pToolBar->setResourceDlgNULL();
	}
		
}

BOOL CDlgResource::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72��ť");
	SetSkinImage(strPath + _T("\\238Ƥ��.png"));
    return true;
	return CDialog::OnEraseBkgnd(pDC);
}
void CDlgResource::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}


void CDlgResource::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}

BOOL CDlgResource::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
