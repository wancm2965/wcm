// WBLineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBLineDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"

// CWBLineDlg 对话框

IMPLEMENT_DYNAMIC(CWBLineDlg, CDialog)

	CWBLineDlg::CWBLineDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBLineDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBLineDlg::~CWBLineDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LINEBTN, m_lineBtn);
	DDX_Control(pDX, IDC_CURVVEBTN, m_curveBtn);
	DDX_Control(pDX, IDC_ICOCURVEBTN, m_icocurveBtn);
	DDX_Control(pDX, IDC_MAGCURVEBTN, m_magcurveBtn);
}
BEGIN_MESSAGE_MAP(CWBLineDlg, CDialog)
	ON_BN_CLICKED(IDC_LINEBTN, &CWBLineDlg::OnBnClickedLinebtn)
	ON_BN_CLICKED(IDC_CURVVEBTN, &CWBLineDlg::OnBnClickedCurvvebtn)
	ON_BN_CLICKED(IDC_ICOCURVEBTN, &CWBLineDlg::OnBnClickedIcoCurvebtn)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_MAGCURVEBTN, &CWBLineDlg::OnBnClickedMagcurvebtn)
END_MESSAGE_MAP()


// CWBLineDlg 消息处理程序

BOOL CWBLineDlg::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	//m_lineBtn.SetType(TYPE_LINE);
	//m_curveBtn.SetType(TYPE_BIGHT);
	return TRUE;

}
void CWBLineDlg::OnOK()
{
	///CDialog::OnOK();
}
void CWBLineDlg::OnCancel()
{
	//CDialog::OnCancel();
}
BOOL CWBLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// 设置对话框背景图和画背景图
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	m_lineBtn.SetType(TYPE_LINE);
	m_lineBtn.SetWindowText(GetCaptureFromType(TYPE_LINE));
	m_curveBtn.SetType(TYPE_BIGHT);
	m_curveBtn.SetWindowText(GetCaptureFromType(TYPE_BIGHT));
	m_icocurveBtn.SetType(TYPE_ICOCURVE);
	m_icocurveBtn.SetWindowText(GetCaptureFromType(TYPE_ICOCURVE));
	m_magcurveBtn.SetType(TYPE_MAGCURVE);
	m_magcurveBtn.SetWindowText(GetCaptureFromType(TYPE_MAGCURVE));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CWBLineDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//设置对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴的起始坐标

	width = WB_CTRLBTNDLG_WIDTH;  //子工具条的宽度
	if (g_bShowWBNewFun)
	{
		height = WB_TOOLBARBTN_SPACE * 5 + WB_TOOLBARBTN_HEIGHT * 4;
	}
	else
	{
		height = WB_TOOLBARBTN_SPACE * 3 + WB_TOOLBARBTN_HEIGHT * 2;
	}

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
	m_lineBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE 
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_curveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_icocurveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_magcurveBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
}
void CWBLineDlg::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);//将CString类型转换成WCHAR
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
		//绘制四个圆角
		for (int nRow = 0; nRow != 2; ++nRow)
		{
			for (int nColumn = 0; nColumn != 2; ++nColumn)
				graph.DrawImage(m_pBitmap, nRow * (rect.Width() - nRoundWidth), nColumn * (rect.Height() - nRoundWidth),
				nRow * (nBmpWidth - nRoundWidth), nColumn * (nBmpHeight - nRoundWidth), nRoundWidth, nRoundWidth, UnitPixel);
		}
		//绘制上下边框
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(nRoundWidth, i * (rect.Height() - nRoundWidth), rect.Width() - nRoundWidth * 2, nRoundWidth);
			graph.DrawImage(m_pBitmap, rt, nRoundWidth, i * (nBmpHeight - nRoundWidth), 1, nRoundWidth, UnitPixel);
		}
		//绘制左右边框
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(i * (rect.Width() - nRoundWidth), nRoundWidth, nRoundWidth, rect.Height() - nRoundWidth * 2);
			graph.DrawImage(m_pBitmap, rt, i * (nBmpWidth - nRoundWidth), nRoundWidth, nRoundWidth, 1, UnitPixel);
		}
	}
}
CString CWBLineDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_LINE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_LINE"));
			if (BtnText.IsEmpty() )
			{
				return _T("直线");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_BIGHT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_BIGHT"));
			if (BtnText.IsEmpty() )
			{
				return _T("曲线");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_ICOCURVE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_ICOCURVE"));
			if (BtnText.IsEmpty())
			{
				return  _T("图标曲线");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_MAGCURVE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardMenuListText"),_T("magicpen"));
			if (BtnText.IsEmpty())
			{
				return _T("魔术线");
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

void CWBLineDlg::OnBnClickedLinebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{	
		pToolBar->SetLineDlgNUll();
		pToolBar->CloseToolbox();
		CTransparentButtonEx::g_LineTypebuttonType = TYPE_LINE;
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_LINE,m_pParentBtn);

	}
}


void CWBLineDlg::OnBnClickedCurvvebtn()
{
	
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetLineDlgNUll();
		pToolBar->CloseToolbox();
		CTransparentButtonEx::g_LineTypebuttonType = TYPE_BIGHT;
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_BIGHT,m_pParentBtn);
	}
}

void CWBLineDlg::OnBnClickedIcoCurvebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_bShowWBNewFun)
	{
		CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
		if (pToolBar)
		{	
			pToolBar->SetLineDlgNUll();
			pToolBar->CloseToolbox();
			CTransparentButtonEx::g_LineTypebuttonType = TYPE_ICOCURVE;
			OnDestroy();
			pToolBar->ProcessCmd(TYPE_ICOCURVE,m_pParentBtn);
		}
	}
}

void CWBLineDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}


BOOL CWBLineDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}


void CWBLineDlg::OnBnClickedMagcurvebtn()
{
	if (g_bShowWBNewFun)
	{
		CWBToolbarDlg* pToolbar = (CWBToolbarDlg*)m_pParentDlg;
		if (pToolbar)
		{
			pToolbar->SetLineDlgNUll();
			pToolbar->CloseToolbox();
			CTransparentButtonEx::g_LineTypebuttonType = TYPE_MAGCURVE;
			OnDestroy();
			pToolbar->ProcessCmd(TYPE_MAGCURVE,m_pParentBtn);
		}
	}
	// TODO: 在此添加控件通知处理程序代码
}
