
// testDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CtestDlg �Ի���




CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CtestDlg ��Ϣ�������

BOOL CtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	::SetWindowPos(GetSafeHwnd(),NULL,50,100,1400,700,SWP_SHOWWINDOW);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc1(this);
		DrawTex(&dc1);
		CDialog::OnPaint();
		
		//CPaintDC* pDC=&dc1;
		 
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestDlg::DrawTex( CDC*pdc )
{
	CDC* pDC=pdc;
	CDC* dc = (CDC*)pDC;
	CRect rect;  
	CBrush brush;  
	brush.CreateSolidBrush(RGB(0,0,0));  
	this->GetClientRect(&rect);  
	dc->FillRect(&rect, &brush);  
	brush.DeleteObject();  

	CFont font;  
	font.CreateFont(  
		50, // nHeight  
		0, // nWidth  
		0, // nEscapement  
		0, // nOrientation  
		FW_NORMAL, // nWeight  
		FALSE, // bItalic  
		FALSE, // bUnderline  
		0, // cStrikeOut  
		ANSI_CHARSET, // nCharSet  
		OUT_DEFAULT_PRECIS, // nOutPrecision  
		CLIP_DEFAULT_PRECIS, // nClipPrecision  
		DEFAULT_QUALITY, // nQuality  
		DEFAULT_PITCH | FF_SWISS,  
		_T("Arial") // nPitchAndFamily Arial  
		);   
	pDC->SelectObject(&font);  
	SetBkColor(pDC->m_hDC, RGB(0,0,0));  
	//SetBkMode(pDC->m_hDC, TRANSPARENT);  
	SetTextColor(pDC->m_hDC, RGB(255,255,255));  

	CString tout = _T("��������У���!");  
	CRect temp = rect;  
	int height = pDC->DrawText(tout, temp, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);  
	rect.DeflateRect(0, (rect.Height() - height)/2);// change rect  
	pDC->DrawText(tout, rect, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
}
