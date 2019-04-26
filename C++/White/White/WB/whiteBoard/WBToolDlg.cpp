// WBToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBToolDlg.h"
#include "afxdialogex.h"


// CWBToolDlg �Ի���

IMPLEMENT_DYNAMIC(CWBToolDlg, CDialog)

CWBToolDlg::CWBToolDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBToolDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBToolDlg::~CWBToolDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAQNBTN, m_MAQNBtn);
	DDX_Control(pDX, IDC_SPOTLIGHTBTN, m_SpotLightBtn);
	DDX_Control(pDX, IDC_BAFFLEPLATEBTN, m_BafflePlateBtn);
	DDX_Control(pDX, IDC_PROTRACTOR, m_ProtractorBtn);
	DDX_Control(pDX, IDC_COMPASSES, m_CompassesBtn);
	DDX_Control(pDX, IDC_RULER, m_RulerBtn);

}


BEGIN_MESSAGE_MAP(CWBToolDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MAQNBTN, &CWBToolDlg::OnBnClickedMaqnbtn)
	ON_BN_CLICKED(IDC_SPOTLIGHTBTN, &CWBToolDlg::OnBnClickedSpotlightbtn)
	ON_BN_CLICKED(IDC_BAFFLEPLATEBTN, &CWBToolDlg::OnBnClickedBaffleplatebtn)
	ON_BN_CLICKED(IDC_PROTRACTOR, &CWBToolDlg::OnBnClickedProtractorbtn)
	ON_BN_CLICKED(IDC_COMPASSES, &CWBToolDlg::OnBnClickedCompassesbtn)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_RULER, &CWBToolDlg::OnBnClickedRuler)
END_MESSAGE_MAP()


// CWBToolDlg ��Ϣ�������


void CWBToolDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}


void CWBToolDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}


BOOL CWBToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// ���öԻ��򱳾�ͼ�ͻ�����ͼ
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	m_MAQNBtn.SetType(TYPE_MAQN);
	m_MAQNBtn.SetWindowText(GetCaptureFromType(TYPE_MAQN));
	m_SpotLightBtn.SetType(TYPE_SPOTLIGHT);
	m_SpotLightBtn.SetWindowText(GetCaptureFromType(TYPE_SPOTLIGHT));
	m_BafflePlateBtn.SetType(TYPE_BAFFLEPLATE);
	m_BafflePlateBtn.SetWindowText(GetCaptureFromType(TYPE_BAFFLEPLATE));
	m_ProtractorBtn.SetType(TYPE_PROTRACTOR);
	m_ProtractorBtn.SetWindowText(GetCaptureFromType(TYPE_PROTRACTOR));
	m_CompassesBtn.SetType(TYPE_COMPASSES);
	m_CompassesBtn.SetWindowText(GetCaptureFromType(TYPE_COMPASSES));
	m_RulerBtn.SetType(TYPE_RULER);
	m_RulerBtn.SetWindowText(GetCaptureFromType(TYPE_RULER));
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CWBToolDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//���öԻ���Ŀ�Ⱥ͸߶�
	int toolYBegin , toolXBegin ;    //�Ի���X,Y�����ʼ����

	width = WB_CTRLBTNDLG_WIDTH;   //�ӹ������Ŀ��
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
	m_MAQNBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_SpotLightBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_BafflePlateBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_ProtractorBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_CompassesBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_RulerBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
}
void CWBToolDlg::SetSkinImage(CString strPath)
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
		//Rect Rt(0, 0, rect.Width(), rect.Height());
		//graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
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
CString CWBToolDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_MAQN:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_MAQN"));
			if (BtnText.IsEmpty() )
			{
				return _T("�Ŵ�");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_SPOTLIGHT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SPOTLIGHT"));
			if (BtnText.IsEmpty() )
			{
				return _T("�۹��");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_BAFFLEPLATE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_BAFFLEPLATE"));
			if (BtnText.IsEmpty() )
			{
				return _T("����");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case  TYPE_PROTRACTOR:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PROTRACTOR"));
			if (BtnText.IsEmpty())
			{
				return _T("������");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_COMPASSES:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_COMPASSES"));
			if (BtnText.IsEmpty())
			{
				return _T("Բ��");
			}
			else
			{
				return BtnText;
			}
				
		}
		break;
	case TYPE_RULER:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RULER"));
			if (BtnText.IsEmpty())
			{
				return _T("ֱ��");
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
BOOL CWBToolDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CWBToolDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}


void CWBToolDlg::OnBnClickedMaqnbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_MAQN,m_pParentBtn);
	}
}


void CWBToolDlg::OnBnClickedSpotlightbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_SPOTLIGHT,m_pParentBtn);
	}
}


void CWBToolDlg::OnBnClickedBaffleplatebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_BAFFLEPLATE,m_pParentBtn);
	}
}




BOOL CWBToolDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	/*m_MAQNBtn.SetType(TYPE_MAQN);
	m_SpotLightBtn.SetType(TYPE_SPOTLIGHT);
	m_BafflePlateBtn.SetType(TYPE_BAFFLEPLATE);*/

	return TRUE; 
}

void CWBToolDlg::OnBnClickedProtractorbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_PROTRACTOR,m_pParentBtn);
	}
}

void CWBToolDlg::OnBnClickedCompassesbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_COMPASSES,m_pParentBtn);
	}
}


void CWBToolDlg::OnBnClickedRuler()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetToolDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RULER,m_pParentBtn);
	}
}
