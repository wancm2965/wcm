// WBDesktopDrawDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBDesktopDrawDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"


// CWBDesktopDrawDlg �Ի���

IMPLEMENT_DYNAMIC(CWBDesktopDrawDlg, CDialog)

CWBDesktopDrawDlg::CWBDesktopDrawDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBDesktopDrawDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBDesktopDrawDlg::~CWBDesktopDrawDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
}

void CWBDesktopDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DDOPENBTN, m_DDOpenBtn);
	DDX_Control(pDX, IDC_DDEFFECTBTN, m_DDEffectBtn);
//	DDX_Control(pDX, IDC_DDFULLSCREENBTN, m_DDFullScreenBtn);
//	DDX_Control(pDX, IDC_DDCTRLBTN, m_DDCtrlBtn);
	DDX_Control(pDX, IDC_DDCLOSEBTN, m_DDCloseBtn);
}


BEGIN_MESSAGE_MAP(CWBDesktopDrawDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DDOPENBTN, &CWBDesktopDrawDlg::OnBnClickedDdopenbtn)
	ON_BN_CLICKED(IDC_DDEFFECTBTN, &CWBDesktopDrawDlg::OnBnClickedDdeffectbtn)
 //	ON_BN_CLICKED(IDC_DDFULLSCREENBTN, &CWBDesktopDrawDlg::OnBnClickedDdfullscreenbtn)
//	ON_BN_CLICKED(IDC_DDCTRLBTN, &CWBDesktopDrawDlg::OnBnClickedDdctrlbtn)
	ON_BN_CLICKED(IDC_DDCLOSEBTN, &CWBDesktopDrawDlg::OnBnClickedDdclosebtn)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWBDesktopDrawDlg ��Ϣ�������


void CWBDesktopDrawDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}


void CWBDesktopDrawDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}


BOOL CWBDesktopDrawDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// ���öԻ��򱳾�ͼ�ͻ�����ͼ
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));

	m_DDOpenBtn.SetType(TYPE_DDOPEN);
	m_DDOpenBtn.SetWindowText(GetCaptureFromType(TYPE_DDOPEN));
	m_DDEffectBtn.SetType(TYPE_DDEFFECT);
	m_DDEffectBtn.SetWindowText(GetCaptureFromType(TYPE_DDEFFECT));
//	m_DDFullScreenBtn.SetType(TYPE_DDFULLSCREEN);
//	m_DDCtrlBtn.SetType(TYPE_DDCTRL);
	m_DDCloseBtn.SetType(TYPE_DDCLOSE);
	m_DDCloseBtn.SetWindowText(GetCaptureFromType(TYPE_DDCLOSE));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CWBDesktopDrawDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//���öԻ���Ŀ�Ⱥ͸߶�
	int toolYBegin , toolXBegin ;    //�Ի���X,Y�����ʼ����

	width = WB_CTRLBTNDLG_WIDTH ;   //�ӹ������Ŀ��
	height = WB_TOOLBARBTN_SPACE * 4 + WB_TOOLBARBTN_HEIGHT * 3;

	toolXBegin = BtnRc.right +  WB_TOOLBARBTN_SPACE;
	toolYBegin = BtnRc.top;	
	if ((toolYBegin + height) > ToolbarRc.bottom)
	{
		toolYBegin = ToolbarRc.bottom - height;
	}

	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	, height,SWP_SHOWWINDOW);
	HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
	SetWindowRgn(rgnwind,TRUE);
	DeleteObject(rgnwind);
	m_DDOpenBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_DDEffectBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_DDCloseBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
/*	m_DDCtrlBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	
	m_DDCloseBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
			*/
}
void CWBDesktopDrawDlg::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);//��CString����ת����WCHAR
		m_pBitmap = new Bitmap(picturePath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
		m_pBitmap = new Bitmap(picturePath);
	}

	if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
// 		Rect Rt(0, 0, rect.Width(), rect.Height());
// 		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);
		int nBmpWidth = m_pBitmap->GetWidth();
		int nBmpHeight = m_pBitmap->GetHeight();

		SolidBrush brush(Color(255, 255, 255));
		graph.FillRectangle(&brush, 0, 0, rect.Width(), rect.Height());

		const int nRoundWidth = 6;
		//�����ĸ�Բ��
		for (int nRow = 0; nRow != 2; ++nRow)
		{
			for (int nColumn = 0; nColumn != 2; ++nColumn)
				graph.DrawImage(m_pBitmap, nRow * (rect.Width() - nRoundWidth), nColumn * (rect.Height() - nRoundWidth),
				nRow * (nBmpWidth - nRoundWidth), nColumn * (nBmpHeight - nRoundWidth), nRoundWidth, nRoundWidth, UnitPixel);
		}
		//�������±߿�
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(nRoundWidth, i * (rect.Height() - nRoundWidth), rect.Width() - nRoundWidth * 2, nRoundWidth);
			graph.DrawImage(m_pBitmap, rt, nRoundWidth, i * (nBmpHeight - nRoundWidth), 1, nRoundWidth, UnitPixel);
		}
		//�������ұ߿�
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(i * (rect.Width() - nRoundWidth), nRoundWidth, nRoundWidth, rect.Height() - nRoundWidth * 2);
			graph.DrawImage(m_pBitmap, rt, i * (nBmpWidth - nRoundWidth), nRoundWidth, nRoundWidth, 1, UnitPixel);
		}
	}
}
CString CWBDesktopDrawDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_DDOPEN:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DDOPEN"));
			if (BtnText.IsEmpty() )
			{
				return _T("�򿪱�ע");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DDCTRL:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DDCTRL"));
			if (BtnText.IsEmpty() )
			{
				return _T("������");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_FULLSCREEN:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FULLSCREEN"));
			if (BtnText.IsEmpty() )
			{
				return _T("ȫ��");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DDEFFECT:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DDEFFECT"));
			if (BtnText.IsEmpty() )
			{
				return _T("Ч���л�");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_DDCLOSE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DDCLOSE"));
			if (BtnText.IsEmpty() )
			{
				return _T("�˳���ע");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	default:
		{
			return _T("");
		}
		break;
	}
}
BOOL CWBDesktopDrawDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}


void CWBDesktopDrawDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete this;
}


void CWBDesktopDrawDlg::OnBnClickedDdopenbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar) //����һ����ť�����滹�ǹر����������л�
		//�����ڹ�������Ҳ������DDOpenCloseBtn��ֵ���������TYPE_DDOPEN_P�����TYPE_DDCLOSE����TYPE_DDCLOSE_P�������TYPE_DDOPEN
	{	
		pToolBar->SetDesktopDrawDlgNULL();	
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DDOPEN,m_pParentBtn);
	
	}
}


void CWBDesktopDrawDlg::OnBnClickedDdeffectbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->SetDesktopDrawDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DDEFFECT,m_pParentBtn);
		
	}
}


void CWBDesktopDrawDlg::OnBnClickedDdfullscreenbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->SetDesktopDrawDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DDFULLSCREEN,m_pParentBtn);
		
	}
}


void CWBDesktopDrawDlg::OnBnClickedDdctrlbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->SetDesktopDrawDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DDCTRL,m_pParentBtn);
	
	}
}


void CWBDesktopDrawDlg::OnBnClickedDdclosebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->SetDesktopDrawDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DDCLOSE,m_pParentBtn);

	}
}

BOOL CWBDesktopDrawDlg::OnEraseBkgnd( CDC* pDC )
{
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	return TRUE;
}
