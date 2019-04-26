// WBTextDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBTextDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"

// CWBTextDlg �Ի���

IMPLEMENT_DYNAMIC(CWBTextDlg, CDialog)

CWBTextDlg::CWBTextDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBTextDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBTextDlg::~CWBTextDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CWBTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_H_TEXTBTN, m_HTextBtn);
	DDX_Control(pDX, IDC_V_TEXTBTN, m_VTextBtn);
}


BEGIN_MESSAGE_MAP(CWBTextDlg, CDialog)
	ON_BN_CLICKED(IDC_H_TEXTBTN, &CWBTextDlg::OnBnClickedHTextbtn)
	ON_BN_CLICKED(IDC_V_TEXTBTN, &CWBTextDlg::OnBnClickedVTextbtn)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CWBTextDlg ��Ϣ�������


void CWBTextDlg::OnOK()
{
	// CDialog::OnOK();
}


void CWBTextDlg::OnCancel()
{
	//CDialog::OnCancel();
}


BOOL CWBTextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	Relayout();
	// ���öԻ��򱳾�ͼ�ͻ�����ͼ
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhileBroad");
	//SetSkinImage(strPath + _T("\\238DlgBG.png"));
	m_HTextBtn.SetType(TYPE_T_H);
	m_VTextBtn.SetType(TYPE_T_V);

	return TRUE;  // return TRUE unless you set the focus to a control
	
}
void CWBTextDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//���öԻ���Ŀ�Ⱥ͸߶�
	int toolYBegin , toolXBegin ;    //�Ի���X,Y�����ʼ����

	width = WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_WIDTH ;  //�ӹ������Ŀ��
	height = WB_TOOLBARBTN_SPACE * 3 + WB_TOOLBARBTN_HEIGHT * 2;

	toolXBegin = BtnRc.right +  WB_TOOLBARBTN_SPACE;
	toolYBegin = BtnRc.top;	
	if ((toolYBegin + height) > ToolbarRc.bottom)
	{
		toolYBegin = ToolbarRc.bottom - height;
	}

	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	, height,SWP_SHOWWINDOW);
	m_HTextBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE 
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
	m_VTextBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x���
		,WB_TOOLBARBTN_HEIGHT   //y���
		,SWP_SHOWWINDOW);
}
void CWBTextDlg::SetSkinImage(CString strPath)
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
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
}
void CWBTextDlg::OnBnClickedHTextbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		
		pToolBar->SetTextDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_T_H,m_pParentBtn);
	}
}
void CWBTextDlg::OnBnClickedVTextbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		
		pToolBar->SetTextDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_T_V,m_pParentBtn);
	}
}


void CWBTextDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}
