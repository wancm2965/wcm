// WBGraphicsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBGraphicsDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"

// CWBGraphicsDlg 对话框

IMPLEMENT_DYNAMIC(CWBGraphicsDlg, CDialogEx)

CWBGraphicsDlg::CWBGraphicsDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBGraphicsDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBGraphicsDlg::~CWBGraphicsDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBGraphicsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECTANGLE_GRPHICSBTN, m_rectangleBtn);
	DDX_Control(pDX, IDC_ROUND_GRPHICSBTN, m_roundBtn);
	DDX_Control(pDX, IDC_TRIANGLE_GRPHICSBTN, m_triangleBtn);
	DDX_Control(pDX, IDC_RHOMB_GRPHICSBTN, m_rhombBtn);
	DDX_Control(pDX, IDC_PENTAGON_GRPHICSBTN, m_pentagonBtn);
	DDX_Control(pDX, IDC_HEXAGON_GRPHICSBTN, m_hexagonBtn);
	DDX_Control(pDX, IDC_FIVEPOINTSTART_GRPHICSBTN, m_fivepointstartBtn);
	DDX_Control(pDX, IDC_TRAPEZIUM_GRPHICSBTN, m_trapeziumBtn);
	DDX_Control(pDX, IDC_POLYGON_GRPHICSBTN, m_polygonBtn);
	DDX_Control(pDX, IDC_POLYGON_ATRBTRIANGLE, m_AtrTriangleBtn);
	DDX_Control(pDX, IDC_ISOSCELESTRIANGLE, m_dyTriangleBtn);
	DDX_Control(pDX, IDC_RANGLETRIANGLE, m_zjTriangleBtn);
	DDX_Control(pDX, IDC_RANGLEISOTRIANGLE, m_dyzjTriangleBtn);
	DDX_Control(pDX,IDC_POLYGON_PARALLELOGRAM,m_parallelogamBtn);
}


BEGIN_MESSAGE_MAP(CWBGraphicsDlg, CDialogEx)
//	ON_BN_CLICKED(IDC_POLYGON_GRPHICSBTN, &CWBGraphicsDlg::OnClickedPolygonGrphicsbtn)
	ON_BN_CLICKED(IDC_RECTANGLE_GRPHICSBTN, &CWBGraphicsDlg::OnClickedRectangleGrphicsbtn)
	ON_BN_CLICKED(IDC_RHOMB_GRPHICSBTN, &CWBGraphicsDlg::OnClickedRhombGrphicsbtn)
	ON_BN_CLICKED(IDC_ROUND_GRPHICSBTN, &CWBGraphicsDlg::OnClickedRoundGrphicsbtn)
	ON_BN_CLICKED(IDC_TRIANGLE_GRPHICSBTN, &CWBGraphicsDlg::OnClickedTriangleGrphicsbtn)
	ON_BN_CLICKED(IDC_POLYGON_ATRBTRIANGLE, &CWBGraphicsDlg::OnClickedAtrTrianglebtn)
	ON_BN_CLICKED(IDC_ISOSCELESTRIANGLE, &CWBGraphicsDlg::OnClickedDYTrianglebtn)
	ON_BN_CLICKED(IDC_RANGLETRIANGLE, &CWBGraphicsDlg::OnClickedZJTrianglebtn)
	ON_BN_CLICKED(IDC_RANGLEISOTRIANGLE,&CWBGraphicsDlg::OnClickedDYZJTrianglebtn)
	ON_BN_CLICKED(IDC_POLYGON_PARALLELOGRAM,&CWBGraphicsDlg::OnClickedParallelogambtn)
	ON_BN_CLICKED(IDC_PENTAGON_GRPHICSBTN, &CWBGraphicsDlg::OnClickedPentagonGrphicsbtn)
	ON_BN_CLICKED(IDC_HEXAGON_GRPHICSBTN, &CWBGraphicsDlg::OnClickedHexagonGrphicsbtn)
	ON_BN_CLICKED(IDC_FIVEPOINTSTART_GRPHICSBTN, &CWBGraphicsDlg::OnClickedFivepointstartGrphicsbtn)
	ON_BN_CLICKED(IDC_TRAPEZIUM_GRPHICSBTN, &CWBGraphicsDlg::OnClickedTrapeziumGrphicsbtn)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CWBGraphicsDlg 消息处理程序
 BOOL CWBGraphicsDlg:: OnEraseBkgnd(CDC* pDC)
 {	// 设置对话框背景图和画背景图
	 CString strPath = _T("");
	 strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	 SetSkinImage(strPath + _T("\\DlgBG.png"));
	 /* m_rectangleBtn.SetType(TYPE_RECTANGLE);
	 m_roundBtn.SetType(TYPE_ROUNDNESS);
	 m_triangleBtn.SetType(TYPE_TRIANGLE);
	 m_rhombBtn.SetType(TYPE_RHOMB);*/
    return TRUE;
 }
void CWBGraphicsDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}
void CWBGraphicsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}
BOOL CWBGraphicsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	Relayout();
	// 设置对话框背景图和画背景图
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\DlgBG.png"));
	m_rectangleBtn.SetType(TYPE_RECTANGLE);
	m_rectangleBtn.SetWindowText(GetCaptureFromType(TYPE_RECTANGLE));
	m_roundBtn.SetType(TYPE_ROUNDNESS);
	m_roundBtn.SetWindowText(GetCaptureFromType(TYPE_ROUNDNESS));
	m_triangleBtn.SetType(TYPE_TRIANGLE);
	m_triangleBtn.SetWindowText(GetCaptureFromType(TYPE_TRIANGLE));
	m_rhombBtn.SetType(TYPE_RHOMB);
	m_rhombBtn.SetWindowText(GetCaptureFromType(TYPE_RHOMB));
	m_pentagonBtn.SetType(TYPE_PENTAGON);
	m_pentagonBtn.SetWindowText(GetCaptureFromType(TYPE_PENTAGON));
	m_hexagonBtn.SetType(TYPE_HEXAGON);
	m_hexagonBtn.SetWindowText(GetCaptureFromType(TYPE_HEXAGON));
	m_fivepointstartBtn.SetType(TYPE_FIVEPOINTSTART);
	m_fivepointstartBtn.SetWindowText(GetCaptureFromType(TYPE_FIVEPOINTSTART));
	m_trapeziumBtn.SetType(TYPE_TRAPEZIUM);
	m_trapeziumBtn.SetWindowText(GetCaptureFromType(TYPE_TRAPEZIUM));
	m_AtrTriangleBtn.SetType(TYPE_ATRTRIANGLE);
	m_AtrTriangleBtn.SetWindowText(GetCaptureFromType(TYPE_ATRTRIANGLE));
//	m_polygonBtn.SetType(TYPE_POLYGON);
	m_dyTriangleBtn.SetType(TYPE_DYTRIANGLE);
	m_dyTriangleBtn.SetWindowText(GetCaptureFromType(TYPE_DYTRIANGLE));
	m_dyzjTriangleBtn.SetType(TYPE_DYZJTRIANGLE);
	m_dyzjTriangleBtn.SetWindowText(GetCaptureFromType(TYPE_DYZJTRIANGLE));
	m_zjTriangleBtn.SetType(TYPE_ZJTRIANGLE);
	m_zjTriangleBtn.SetWindowText(GetCaptureFromType(TYPE_ZJTRIANGLE));
	m_parallelogamBtn.SetType(TYPE_PALLELOGAM);
	m_parallelogamBtn.SetWindowText(GetCaptureFromType(TYPE_PALLELOGAM));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CWBGraphicsDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//设置对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴的起始坐标

	width = WB_CTRLBTNDLG_WIDTH ;   //子工具条的宽度
	if (g_bShowWBNewFun)
	{
		height = WB_TOOLBARBTN_SPACE * 14/*9*/ + WB_TOOLBARBTN_HEIGHT *13 /*8*/;
	}
	else
	{
		height = WB_TOOLBARBTN_SPACE * 5/*9*/ + WB_TOOLBARBTN_HEIGHT *4 /*8*/;
	}

	toolXBegin = BtnRc.right +  WB_TOOLBARBTN_SPACE;
	toolYBegin = BtnRc.top;	
	if ((toolYBegin + height) > ToolbarRc.bottom)
	{
		toolYBegin = ToolbarRc.bottom - height;
		if (toolYBegin < 10)
		{
			toolYBegin = 10;
		}
	}

	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	, height,SWP_SHOWWINDOW);
	HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
	SetWindowRgn(rgnwind,TRUE);
	DeleteObject(rgnwind);
	m_rectangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_roundBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_triangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_rhombBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_AtrTriangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);

	//等腰三角形
	m_dyTriangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	//等腰直角三角形
	m_dyzjTriangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 6 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
    //直角三角形
	m_zjTriangleBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 7 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_parallelogamBtn.SetWindowPos(&wndTop, WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 8 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_pentagonBtn.SetWindowPos(&wndTop, WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 9 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_hexagonBtn.SetWindowPos(&wndTop, WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 10 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_fivepointstartBtn.SetWindowPos(&wndTop, WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 11 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_trapeziumBtn.SetWindowPos(&wndTop, WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 12 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
/*	m_polygonBtn.SetWindowPos(&wndTop,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT ) 
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
		*/
}
void CWBGraphicsDlg::SetSkinImage(CString strPath)
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
CString CWBGraphicsDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_RECTANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RECTANGLE"));
			if (BtnText.IsEmpty() )
			{
				return _T("矩形");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_ROUNDNESS:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_ROUNDNESS"));
			if (BtnText.IsEmpty() )
			{
				return _T("圆");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_TRIANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_TRIANGLE"));
			if (BtnText.IsEmpty() )
			{
				return _T("三角形");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_RHOMB:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RHOMB"));
			if (BtnText.IsEmpty() )
			{
				return _T("菱形");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_PENTAGON:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PENTAGON"));
			if (BtnText.IsEmpty() )
			{
				return _T("五边形");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_HEXAGON:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_HEXAGON"));
			if (BtnText.IsEmpty() )
			{
				return _T("六边形");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_FIVEPOINTSTART:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FIVEPOINTSTART"));
			if (BtnText.IsEmpty() )
			{
				return _T("五角星");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_TRAPEZIUM:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_TRAPEZIUM"));
			if (BtnText.IsEmpty() )
			{
				return _T("梯形");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_POLYGON:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_POLYGON"));
			if (BtnText.IsEmpty() )
			{
				return _T("多边形");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_ATRTRIANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_ATRTRIANGLE"));
			if (BtnText.IsEmpty())
			{
				return _T("任意三角形");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DYZJTRIANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DYZJTRIANGLE"));
			if (BtnText.IsEmpty())
			{
				return _T("等腰直角三角形");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DYTRIANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DYTRIANGLE"));
			if (BtnText.IsEmpty())
			{
				return _T("等腰三角形");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_ZJTRIANGLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_ZJTRIANGLE"));
			if (BtnText.IsEmpty())
			{
				return _T("直角三角形");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_PALLELOGAM:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PALLELOGAM"));
			if (BtnText.IsEmpty())
			{
				return _T("平行四边形");
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
void CWBGraphicsDlg::OnClickedPolygonGrphicsbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_POLYGON;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_POLYGON,m_pParentBtn);
	}
}
void CWBGraphicsDlg::OnClickedRectangleGrphicsbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_RECTANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RECTANGLE,m_pParentBtn);
	}
}
void CWBGraphicsDlg::OnClickedRhombGrphicsbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_RHOMB;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_RHOMB,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedPentagonGrphicsbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_PENTAGON;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_PENTAGON,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedHexagonGrphicsbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_HEXAGON;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_HEXAGON,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedFivepointstartGrphicsbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_FIVEPOINTSTART;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_FIVEPOINTSTART,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedTrapeziumGrphicsbtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_TRAPEZIUM;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_TRAPEZIUM,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedRoundGrphicsbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_ROUNDNESS;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_ROUNDNESS,m_pParentBtn);
	}
}
void CWBGraphicsDlg::OnClickedTriangleGrphicsbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_TRIANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_TRIANGLE,m_pParentBtn);
	}
}


void CWBGraphicsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	delete this;
}


BOOL CWBGraphicsDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CWBGraphicsDlg::OnClickedAtrTrianglebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_ATRTRIANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_ATRTRIANGLE,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedDYTrianglebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_DYTRIANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DYTRIANGLE,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedDYZJTrianglebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_DYZJTRIANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DYZJTRIANGLE,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedZJTrianglebtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_ZJTRIANGLE;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_ZJTRIANGLE,m_pParentBtn);
	}
}

void CWBGraphicsDlg::OnClickedParallelogambtn()
{
	if (!g_bShowWBNewFun)
	{
		return;
	}
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetGraphicsDlgNUll();
		CTransparentButtonEx::g_GraphicsbuttonType = TYPE_PALLELOGAM;
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_PALLELOGAM,m_pParentBtn);
	}
}


