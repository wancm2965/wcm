// WBRubberDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRubberDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"

// CWBRubberDlg 对话框

IMPLEMENT_DYNAMIC(CWBRubberDlg, CDialogEx)

CWBRubberDlg::CWBRubberDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBRubberDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBRubberDlg::~CWBRubberDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBRubberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BIG_RUBBERBTN, m_BigRubberBtn);
	DDX_Control(pDX, IDC_CURVE_RUBBERBTN, m_CurveRubberBtn);
	DDX_Control(pDX, IDC_OBJ_RUBBERBTN, m_ObjRubberBtn);
}


BEGIN_MESSAGE_MAP(CWBRubberDlg, CDialogEx)
	ON_BN_CLICKED(IDC_OBJ_RUBBERBTN, &CWBRubberDlg::OnClickedObjRubberbtn)
	ON_BN_CLICKED(IDC_CURVE_RUBBERBTN, &CWBRubberDlg::OnClickedCurveRubberbtn)
	ON_BN_CLICKED(IDC_BIG_RUBBERBTN, &CWBRubberDlg::OnClickedBigRubberbtn)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWBRubberDlg 消息处理程序

BOOL CWBRubberDlg:: OnEraseBkgnd(CDC* pDC)
{    // 设置对话框背景图和画背景图
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	/*m_BigRubberBtn.SetType(TYPE_RUBBER_BIG);
	m_LitterRubberBtn.SetType(TYPE_RUBBER_SMALL);
	m_CurveRubberBtn.SetType(TYPE_RUBBER_CURVE);
	m_ObjRubberBtn.SetType(TYPE_RUBBER_OBJ);*/

	return NULL;
		  
		}
void CWBRubberDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


void CWBRubberDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


BOOL CWBRubberDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Relayout();
	// 设置对话框背景图和画背景图
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	m_BigRubberBtn.SetType(TYPE_RUBBER_BIG);
	m_BigRubberBtn.SetWindowText(GetCaptureFromType(TYPE_RUBBER_BIG));
	m_CurveRubberBtn.SetType(TYPE_RUBBER_CURVE);
	m_CurveRubberBtn.SetWindowText(GetCaptureFromType(TYPE_RUBBER_CURVE));
	m_ObjRubberBtn.SetType(TYPE_RUBBER_OBJ);
	m_ObjRubberBtn.SetWindowText(GetCaptureFromType(TYPE_RUBBER_OBJ));

	return TRUE;  
	
}
void CWBRubberDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//设置对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴的起始坐标

	width = WB_CTRLBTNDLG_WIDTH ;  //子工具条的宽度
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
	m_BigRubberBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_CurveRubberBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_ObjRubberBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);

}
void CWBRubberDlg::SetSkinImage(CString strPath)
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
CString CWBRubberDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_RUBBER_BIG:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RUBBER_BIG"));
			if (BtnText.IsEmpty() )
			{
				return _T("擦子");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_RUBBER_CURVE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RUBBER_CURVE"));
			if (BtnText.IsEmpty() )
			{
				return _T("曲线擦");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_RUBBER_OBJ:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RUBBER_OBJ"));
			if (BtnText.IsEmpty() )
			{
				return _T("擦对象");
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
void CWBRubberDlg::OnClickedObjRubberbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetRubberDlgNUll();
		pToolBar->CloseToolbox();
		CTransparentButtonEx::g_RubberbuttonType = TYPE_RUBBER_OBJ;
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RUBBER_OBJ,m_pParentBtn);
	}
}





void CWBRubberDlg::OnClickedCurveRubberbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetRubberDlgNUll();
		pToolBar->CloseToolbox();
		CTransparentButtonEx::g_RubberbuttonType = TYPE_RUBBER_CURVE;
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RUBBER_CURVE,m_pParentBtn);
	}
}


void CWBRubberDlg::OnClickedBigRubberbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetRubberDlgNUll();
		pToolBar->CloseToolbox();
		CTransparentButtonEx::g_RubberbuttonType = TYPE_RUBBER_BIG;
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RUBBER_BIG,m_pParentBtn);
	}
}


void CWBRubberDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	delete this;
}


BOOL CWBRubberDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}
