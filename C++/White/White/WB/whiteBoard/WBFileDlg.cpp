// WBFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFileDlg.h"
#include "afxdialogex.h"
#include "WBToolboxDlg.h"


// CWBFileDlg �Ի���

IMPLEMENT_DYNAMIC(CWBFileDlg, CDialog)

CWBFileDlg::CWBFileDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBFileDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBFileDlg::~CWBFileDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPEN_FILEBTN, m_openBtn);
	DDX_Control(pDX, IDC_COMBINE_FILEBTN, m_combineBtn);
	DDX_Control(pDX, IDC_SAVE_FILEBTN, m_saveBtn);
	DDX_Control(pDX, IDC_RESAVE_FILEBTN, m_resaveBtn);
	DDX_Control(pDX, IDC_PRINT_FILEBTN, m_printBtn);
	DDX_Control(pDX, IDC_CLOSE_FILEBTN, m_closeBtn);
	DDX_Control(pDX, IDC_SAVEPIC_FILEBTN, m_BtnSavePic);
}


BEGIN_MESSAGE_MAP(CWBFileDlg, CDialog)
	ON_BN_CLICKED(IDC_OPEN_FILEBTN, &CWBFileDlg::OnBnClickedOpenFilebtn)
	ON_BN_CLICKED(IDC_COMBINE_FILEBTN, &CWBFileDlg::OnBnClickedCombineFilebtn)
	ON_BN_CLICKED(IDC_SAVE_FILEBTN, &CWBFileDlg::OnBnClickedSaveFilebtn)
	ON_BN_CLICKED(IDC_RESAVE_FILEBTN, &CWBFileDlg::OnBnClickedResaveFilebtn)
	ON_BN_CLICKED(IDC_PRINT_FILEBTN, &CWBFileDlg::OnBnClickedPrintFilebtn)
	ON_BN_CLICKED(IDC_CLOSE_FILEBTN, &CWBFileDlg::OnBnClickedCloseFilebtn)
	ON_BN_CLICKED(IDC_SAVEPIC_FILEBTN, &CWBFileDlg::OnClickedSavepicFilebtn)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWBFileDlg ��Ϣ�������


void CWBFileDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}


void CWBFileDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}


BOOL CWBFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// ���öԻ��򱳾�ͼ�ͻ�����ͼ
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	m_openBtn.SetType(TYPE_OPEN);
	m_openBtn.SetWindowText(GetCaptureFromType(TYPE_OPEN));
	m_combineBtn.SetType(TYPE_COMBINE);
	m_combineBtn.SetWindowText(GetCaptureFromType(TYPE_COMBINE));
	m_saveBtn.SetType(TYPE_SAVE);
	m_saveBtn.SetWindowText(GetCaptureFromType(TYPE_SAVE));
	m_resaveBtn.SetType(TYPE_RESAVE);
	m_resaveBtn.SetWindowText(GetCaptureFromType(TYPE_RESAVE));
	m_printBtn.SetType(TYPE_PRINT);
	m_printBtn.SetWindowText(GetCaptureFromType(TYPE_PRINT));
	m_closeBtn.SetType(TYPE_CLOSE);
	m_closeBtn.SetWindowText(GetCaptureFromType(TYPE_CLOSE));

	m_BtnSavePic.SetType(TYPE_SAVE_PIC);
	m_BtnSavePic.SetWindowText(GetCaptureFromType(TYPE_SAVE_PIC));

	CString staticText;
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_OPEN"));
	if (staticText.IsEmpty())
	{
		staticText = _T("��");
	}
	m_openBtn.SetName(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_CLOSE"));
	if (staticText.IsEmpty())
	{
		staticText = _T("�˳�");
	}
	m_closeBtn.SetName(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_COMBINE"));
	if (staticText.IsEmpty())
	{
		staticText = _T("�ϲ�");
	}
	m_combineBtn.SetName(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SAVE"));
	if (staticText.IsEmpty())
	{
		staticText = _T("����");
	}
	m_saveBtn.SetName(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RESAVE"));
	if (staticText.IsEmpty())
	{
		staticText = _T("���");
	}
	m_resaveBtn.SetName(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PRINT"));
	if (staticText.IsEmpty())
	{
	staticText = _T("��ӡ");
	}
	m_printBtn.SetName(staticText);

	staticText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SAVE_PIC"));
	if (staticText.IsEmpty())
	{
		staticText = _T("����ͼƬ");
	}
	m_BtnSavePic.SetName(staticText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CWBFileDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//���öԻ���Ŀ�Ⱥ͸߶�
	int toolYBegin , toolXBegin ;    //�Ի���X,Y�����ʼ����
	int nBtnCount = g_bSupportU7OpenFile? 5:6;
	if (g_bShowWBNewFun)
	{
		nBtnCount = nBtnCount +1;
	}
	if (g_bU7NewStyle)
	{
		nBtnCount = 1;//ֻ������湦��
	}
	width = WB_CTRLBTNDLG_WIDTH ;   //�ӹ������Ŀ��
	height = WB_TOOLBARBTN_SPACE * (nBtnCount + 1) + WB_TOOLBARBTN_HEIGHT * nBtnCount ;

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
	if (g_bU7NewStyle)//ֻ������湦��
	{
		if (ZSYDZ)
		{
			m_BtnSavePic.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
		}
		else
		{
			m_resaveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
		}
		return;
	}
	m_resaveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_combineBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_saveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_openBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);


	if (g_bSupportU7OpenFile)
	{
		//m_printBtn.SetWindowPos(&wndTop ,WB_WIDTH_SPACE
		//	,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		//	,WB_TOOLBARBTN_WIDTH    //x���
		//	,WB_TOOLBARBTN_HEIGHT   //y���
		//	,SWP_HIDEWINDOW | SWP_NOACTIVATE);
		m_printBtn.ShowWindow(SW_HIDE);
		if (g_bShowWBNewFun)
		{
			m_BtnSavePic.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
			m_closeBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 5/*5 */*(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);	
		}
		else
		{
			m_closeBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 4/*5 */*(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);	
		}
	} 
	else
	{
		m_printBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
			,WB_TOOLBARBTN_SPACE + 4/*5*/ *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
			,WB_TOOLBARBTN_WIDTH    //x���
			,WB_TOOLBARBTN_HEIGHT   //y���
			,SWP_SHOWWINDOW);
		if (g_bShowWBNewFun)
		{
			m_BtnSavePic.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
			m_closeBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 6/*6*/ *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
		}
		else
		{
			m_closeBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 5/*6*/ *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
				,WB_TOOLBARBTN_WIDTH    //x���
				,WB_TOOLBARBTN_HEIGHT   //y���
				,SWP_SHOWWINDOW);
		}
	}
	
}
void CWBFileDlg::SetSkinImage(CString strPath)
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
CString CWBFileDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_SAVE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SAVE"));
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
	case TYPE_PRINT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PRINT"));
			if (BtnText.IsEmpty() )
			{
				return _T("��ӡ");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_CLOSE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_CLOSE"));
			if (BtnText.IsEmpty() )
			{
				return _T("�˳�");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_OPEN:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_OPEN"));
			if (BtnText.IsEmpty() )
			{
				return _T("��");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case  TYPE_COMBINE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_COMBINE"));
			if (BtnText.IsEmpty())
			{
				return _T("�ϲ�");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_RESAVE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RESAVE"));
			if (BtnText.IsEmpty() )
			{
				return _T("���");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_SAVE_PIC:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SAVE_PIC"));
			if (BtnText.IsEmpty() )
			{
				return _T("����ͼƬ");
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

void CWBFileDlg::OnBnClickedOpenFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->CloseToolbox();
		pToolBar->SetFileDlgNULL();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_OPEN,m_pParentBtn);		
		
	}
}


void CWBFileDlg::OnBnClickedSaveFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetFileDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_SAVE,m_pParentBtn);
	}
}


void CWBFileDlg::OnBnClickedResaveFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetFileDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RESAVE,m_pParentBtn);
	}
}


void CWBFileDlg::OnBnClickedPrintFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetFileDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_PRINT,m_pParentBtn);
	}
}


void CWBFileDlg::OnBnClickedCloseFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetFileDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_CLOSE,m_pParentBtn);
	}
}


void CWBFileDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}


BOOL CWBFileDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CWBFileDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	/*m_openBtn.SetType(TYPE_OPEN);
	m_saveBtn.SetType(TYPE_SAVE);
	m_resaveBtn.SetType(TYPE_RESAVE);
	m_printBtn.SetType(TYPE_PRINT);
	m_closeBtn.SetType(TYPE_CLOSE);*/

	return TRUE;
}

void CWBFileDlg::OnBnClickedCombineFilebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->CloseToolbox();
		pToolBar->SetFileDlgNULL();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_COMBINE,m_pParentBtn);		

	}
}


void CWBFileDlg::OnClickedSavepicFilebtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetFileDlgNULL();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_SAVE_PIC,m_pParentBtn);
	}
}
