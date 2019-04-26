// WBColorPaletteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBColorPaletteDlg.h"
#include "afxdialogex.h"
#include "WMBtn.h"
#include "ScreenDrawMgr.h"

#define Func(pt,pt1,pt2) pt.y-pt.x*(pt2.y-pt1.y)/(pt2.x-pt1.x)-pt1.y+pt1.x*(pt2.y-pt1.y)/(pt2.x-pt1.x)


// CWBColorPaletteDlg 对话框

IMPLEMENT_DYNAMIC(CWBColorPaletteDlg, CDialog)

CWBColorPaletteDlg::CWBColorPaletteDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBColorPaletteDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
	m_color=RGB(255,255,255);
	m_changecolor = RGB(255,255,255);
	m_nParentBtnID = m_pParentBtn->GetDlgCtrlID();
	m_ParentBtnType = m_pParentBtn->GetType();
}

CWBColorPaletteDlg::~CWBColorPaletteDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	/*if(NULL != m_pPaletteDlgClose)
	{
		delete m_pPaletteDlgClose;
		m_pPaletteDlgClose = NULL;
	}*/
}

void CWBColorPaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUTOCOLOR_BTN, m_autoColorBtn);
	DDX_Control(pDX, IDC_ONECOLOR_BTN, m_oneColorBtn);
	DDX_Control(pDX, IDC_TWOCOLOR_BTN, m_twoColorBtn);
	DDX_Control(pDX, IDC_THREECOLOR_BTN, m_threeColorBtn);
	DDX_Control(pDX, IDC_FORCOLOR_BTN, m_fourColorBtn);
	DDX_Control(pDX, IDC_FIVECOLOR_BTN, m_fiveColorBtn);
	DDX_Control(pDX, IDC_SIXCOLOR_BTN, m_sixColorBtn);
	DDX_Control(pDX, IDC_SEVENCOLOR_BTN, m_sevenColorBtn);
	DDX_Control(pDX, IDC_EIGHTCOLOR_BTN, m_eightColorBtn);
	DDX_Control(pDX, IDC_NINECOLOR_BTN, m_nineColorBtn);
	DDX_Control(pDX, IDC_OTHERCOLOR_BTN, m_otherColorBtn);
}


BEGIN_MESSAGE_MAP(CWBColorPaletteDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_ONECOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedOnecolorBtn)
	ON_BN_CLICKED(IDC_TWOCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedTwocolorBtn)
	ON_BN_CLICKED(IDC_THREECOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedThreecolorBtn)
	ON_BN_CLICKED(IDC_FORCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedForcolorBtn)
	ON_BN_CLICKED(IDC_FIVECOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedFivecolorBtn)
	ON_BN_CLICKED(IDC_SIXCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedSixcolorBtn)
	ON_BN_CLICKED(IDC_SEVENCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedSevencolorBtn)
	ON_BN_CLICKED(IDC_EIGHTCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedEightcolorBtn)
	ON_BN_CLICKED(IDC_NINECOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedNinecolorBtn)
	ON_BN_CLICKED(IDC_OTHERCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedOthercolorBtn)
	ON_BN_CLICKED(IDC_AUTOCOLOR_BTN, &CWBColorPaletteDlg::OnBnClickedAutocolorBtn)
	ON_WM_ACTIVATE()
    ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CWBColorPaletteDlg 消息处理程序


void CWBColorPaletteDlg::OnCancel()
{	
	//CDialog::OnCancel();
}
void CWBColorPaletteDlg::OnOK()
{	
	//CDialog::OnOK();
}
BOOL CWBColorPaletteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    Relayout();
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\");
	SetSkinImage(strPath + _T("ColorDlgBk.png"));
	//CRect clientRect;
	//GetClientRect(&clientRect);
	
	//创建关闭按钮
    
	//CRect rt(235,1,265,40);
	//CRect rt(clientRect.right-31,clientRect.top+1,clientRect.bottom,clientRect.top+35);
	//m_pPaletteDlgClose = new CWMBtn(this, BTN_CLOSECOLORPALETTEDLG_ID);
	//m_pPaletteDlgClose->Create(_T("关闭"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS, rt, this, BTN_CLOSECOLORPALETTEDLG_ID);
	//pos = TRUE;//初始状态表明在左
	//m_pPaletteDlgClose->SetButtonImage(_T("ColorpalleteDlgClose.png"));

  
	
	
	
	// 设置对话框背景图和画背景图
	

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CWBColorPaletteDlg::Relayout()
{
	/*CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetClientRect(ToolbarRc);
	
	int width,height;//调色板对话框的宽度和高度
	int toolYBegin=0, toolXBegin=0 ;    //对话框X,Y轴开始,初始化
	width = WB_TOOLBARBTN_SPACE * 4 + WB_TOOLBARBTN_WIDTH * 3 ;
	height = WB_TOOLBARBTN_SPACE * 5 + WB_TOOLBARBTN_WIDTH * 3 + WB_TOOLBOXBTN_SPEC_WIDTH ;
	if (m_pParentBtn->GetType() == TYPE_DLG_COLORPALETTE)
	{
		toolXBegin = BtnRc.right + WB_TOOLBARBTN_SPACE;
		toolYBegin = BtnRc.top;	
		if ((toolYBegin + height) > ToolbarRc.bottom)
		{
			toolYBegin = ToolbarRc.bottom - height;
		}
		//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrWBCurObjColor);
	}
	if (m_pParentBtn->GetDlgCtrlID() == IDC_MULTIATRIBUTE_LINECOLORBUTTON || m_pParentBtn->GetDlgCtrlID() == IDC_SINATRIBUTE_LINECOLORBUTTON)
	{
		toolXBegin = BtnRc.left;
		toolYBegin = BtnRc.top + 30;
		//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrWBCurObjColor);
	}
	if (m_pParentBtn->GetDlgCtrlID() == IDC_MULTIATRIBUTE_SOLIDCOLORBUTTON || m_pParentBtn->GetDlgCtrlID() == IDC_SINATRIBUTE_SOLIDCOLORBUTTON)
	{
		toolXBegin = BtnRc.left;
		toolYBegin = BtnRc.top + 30;
		//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrSolid);
	}
	
	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	,height	,SWP_SHOWWINDOW);
	//第一行
	m_oneColorBtn.SetBackgroundColor(RGB(0,0,0));//黑色
	m_oneColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_twoColorBtn.SetBackgroundColor(RGB(64,0,64));//
	m_twoColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_threeColorBtn.SetBackgroundColor(RGB(255,0,0));//红色
	m_threeColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 2* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	//第二行
	m_fourColorBtn.SetBackgroundColor(RGB(0,0,255));//蓝色
	m_fourColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_fiveColorBtn.SetBackgroundColor(RGB(255,255,0));//黄色
	m_fiveColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_sixColorBtn.SetBackgroundColor(RGB(255,140,26));//橙色
	m_sixColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 2* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	//第三行
	m_sevenColorBtn.SetBackgroundColor(RGB(170,85,255));//紫色
	m_sevenColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 2 * (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_eightColorBtn.SetBackgroundColor(RGB(80,240,240));//绿色
	m_eightColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 2* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_nineColorBtn.SetBackgroundColor(RGB(0,128,0));//
	m_nineColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 2* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 2* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_autoColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 0* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 3* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH   //x轴宽50
		,WB_TOOLBOXBTN_SPEC_WIDTH   //y轴高45
		,SWP_SHOWWINDOW);
	//其他颜色按钮
	m_otherColorBtn.SetBackgroundColor(RGB(192,192,192));
	CString	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("ColorPaletteOtherColorBtn"));
	if (BtnText.IsEmpty() )
	{
		BtnText = _T("其它颜色");
	} 
	m_otherColorBtn.SetShowText(BtnText);
	m_otherColorBtn.SetWindowPos(&wndTop
		,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
		,WB_TOOLBARBTN_SPACE + 3* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH* 2   //x轴宽
		,WB_TOOLBOXBTN_SPEC_WIDTH   //y轴高45
		,SWP_SHOWWINDOW);*/

	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//调色板对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴开始
	width = WB_TOOLBARBTN_SPACE * 4 + WB_TOOLBARBTN_WIDTH * 3 -21+100;
	height = WB_TOOLBARBTN_SPACE * 5 + WB_TOOLBARBTN_WIDTH * 3 + WB_TOOLBOXBTN_SPEC_WIDTH +50;
	CRect parDlgRect;
	((CWhiteBoardDlg*)g_whiteboardDlg)->GetClientRect(&parDlgRect);//获得白板大小	
	if (m_nParentBtnID == IDC_ATRIBUTE_LINECOLORBUTTON)
	{
		toolXBegin = BtnRc.left-50;
		toolYBegin = BtnRc.top + 30;
	//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrWBCurObjColor);
	}
	if (m_nParentBtnID == IDC_ATRIBUTE_SOLIDCOLORBUTTON)
	{
		toolXBegin = BtnRc.left-50;
		toolYBegin = BtnRc.top + 30;
	//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrSolid);
	}
	if(m_nParentBtnID==IDC_WB_BGDLG_RADIOFORCLRPAD)
	{
		toolXBegin = BtnRc.left - 70;
		//toolXBegin = BtnRc.left-50;
		toolYBegin = BtnRc.top + 30;
	}
	if (m_nParentBtnID == IDC_BTN_A
		||m_nParentBtnID == IDC_BTN_B
		||m_nParentBtnID == IDC_BTN_C
		||m_nParentBtnID == IDC_BTN_D
		||m_nParentBtnID == IDC_BTN_E
		||m_nParentBtnID == IDC_BTN_F
		||m_nParentBtnID == IDC_BTN_G
		||m_nParentBtnID == IDC_BTN_H		
		||m_nParentBtnID == IDC_BTN_J)
	{
		toolXBegin = BtnRc.right + WB_TOOLBARBTN_SPACE;
		toolYBegin = BtnRc.bottom -height;
		//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrSolid);
	}
	if (m_nParentBtnID == IDC_BTNTWO
		||m_nParentBtnID == IDC_BTNTHREE
		||m_nParentBtnID == IDC_BTNFOUR
		||m_nParentBtnID == IDC_BTNFIVE
		||m_nParentBtnID == IDC_BTNSIX
		||m_nParentBtnID == IDC_BTNSEVEN
		||m_nParentBtnID == IDC_BTNEIGHT)
	{
// 		CRect temprect = parDlgRect;
// 		ClientToScreen(&temprect);
		toolXBegin = BtnRc.right + WB_TOOLBARBTN_SPACE ;
		toolYBegin = BtnRc.top;	
		int temp=BtnRc.top;
		if ((toolYBegin + height) > ToolbarRc.bottom)
		{
			toolYBegin = ToolbarRc.bottom - height;
		}
		if ((temp + height)>parDlgRect.bottom)
		{
			//ClientToScreen(parDlgRect);
			toolYBegin =ToolbarRc.bottom-height /*- WB_TOOLBOXBTN_SPEC_WIDTH*/;
		}
		//自动颜色按钮
		m_autoColorBtn.SetBackgroundColor(g_clrWBCurObjColor);
	}
	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	,height	,SWP_SHOWWINDOW);
	HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
	SetWindowRgn(rgnwind,TRUE);
	DeleteObject(rgnwind);
//第一行
	m_oneColorBtn.SetBackgroundColor(RGB(0,0,0));//黑色
	m_oneColorBtn.SetWindowPos(&wndTop
		,0+20-10
		,230
		,25   //x轴宽
		,25   //y轴高
		,SWP_SHOWWINDOW);
	m_twoColorBtn.SetBackgroundColor(RGB(64,0,64));//
	m_twoColorBtn.SetWindowPos(&wndTop
		,25+20-10
		,230
		,25   //x轴宽
		,25   //y轴高
		,SWP_SHOWWINDOW);
	m_threeColorBtn.SetBackgroundColor(RGB(255,0,0));//红色
	m_threeColorBtn.SetWindowPos(&wndTop
		,50+20-10
		,230
		,25   //x轴宽
		,25  //y轴高
		,SWP_SHOWWINDOW);
	//第二行
	m_fourColorBtn.SetBackgroundColor(RGB(0,0,255));//蓝色
	m_fourColorBtn.SetWindowPos(&wndTop
		,75+20-10
		,230
		,25   //x轴宽
		,25  //y轴高  //y轴高
		,SWP_SHOWWINDOW);
	m_fiveColorBtn.SetBackgroundColor(RGB(255,255,0));//黄色
	m_fiveColorBtn.SetWindowPos(&wndTop
		,100+20-10
		,230
		,25   //x轴宽
		,25  //y轴高  //y轴高
		,SWP_SHOWWINDOW);
	m_sixColorBtn.SetBackgroundColor(RGB(255,140,26));//橙色
	m_sixColorBtn.SetWindowPos(&wndTop
		,125+20-10
		,230
		,25   //x轴宽
		,25  //y轴高  //y轴高
		,SWP_SHOWWINDOW);
//第三行
	m_sevenColorBtn.SetBackgroundColor(RGB(170,85,255));//紫色
	m_sevenColorBtn.SetWindowPos(&wndTop
		,150+20-10
		,230
		,25   //x轴宽
		,25  //y轴高 //y轴高
		,SWP_SHOWWINDOW);
	m_eightColorBtn.SetBackgroundColor(RGB(80,240,240));//绿色
	m_eightColorBtn.SetWindowPos(&wndTop
		,175+20-10
		,230
		,25   //x轴宽
		,25  //y轴高  //y轴高
		,SWP_SHOWWINDOW);
	m_nineColorBtn.SetBackgroundColor(RGB(0,128,0));//
	m_nineColorBtn.SetWindowPos(&wndTop
		,200+20-10
		,230
		,25   //x轴宽
		,25  //y轴高  //y轴高
		,SWP_SHOWWINDOW);
	m_autoColorBtn.SetWindowPos(&wndTop
		,135+20-10
		,230
		,15   //x轴宽
		,15  //y轴高   //y轴高45
		,SWP_HIDEWINDOW);
	//其他颜色按钮
	m_otherColorBtn.SetBackgroundColor(RGB(192,192,192));
	CString	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("ColorPaletteOtherColorBtn"));
	if (BtnText.IsEmpty() )
	{
		BtnText = _T("其它颜色");
	} 
	m_otherColorBtn.SetShowText(BtnText);
	m_otherColorBtn.SetWindowPos(&wndTop
	,WB_TOOLBARBTN_SPACE + 1* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_WIDTH)
	,WB_TOOLBARBTN_SPACE + 3* (WB_TOOLBARBTN_SPACE +WB_TOOLBARBTN_HEIGHT)
	,WB_TOOLBARBTN_WIDTH* 2   //x轴宽
	,WB_TOOLBOXBTN_SPEC_WIDTH   //y轴高45
	,SWP_HIDEWINDOW);



}
void CWBColorPaletteDlg::SetSkinImage(CString strPath)
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
// 		graph.DrawImage(m_pBitmap, Rt, 1, 1, rect.Width(), rect.Height(), UnitPixel);	
		int nBmpWidth = m_pBitmap->GetWidth();
		int nBmpHeight = m_pBitmap->GetHeight();

		SolidBrush brush(Color(255, 255, 255));
		graph.FillRectangle(&brush, 0, 0, rect.Width(), rect.Height()-2);


		graph.DrawImage(m_pBitmap,0,0,rect.Width(), rect.Height());

		const int nRoundWidth = 6;
		//绘制四个圆角
		/*for (int nRow = 0; nRow != 2; ++nRow)
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
		}*/
	}
}


BOOL CWBColorPaletteDlg::OnEraseBkgnd(CDC* pDC)
{
	//CString strPath = _T("");
	//strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\");
	//SetSkinImage(strPath + _T("DlgBG.png"));
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}


void CWBColorPaletteDlg::OnBnClickedOnecolorBtn()
{	
	
	m_color=m_oneColorBtn.GetBackgroundColor();
// 	UINT BtnID;
// 	BtnID = m_pParentBtn->GetDlgCtrlID();
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_oneColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_oneColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);
				


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_oneColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);
				
			}
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	
	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}		
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedTwocolorBtn()
{
	/*UINT BtnID;*/
	
	m_color=m_twoColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_twoColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_twoColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);
				


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_twoColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}

}


void CWBColorPaletteDlg::OnBnClickedThreecolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_threeColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_threeColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_threeColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);
				


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_threeColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedForcolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_fourColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_fourColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_fourColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_fourColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedFivecolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_fiveColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_fiveColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_fiveColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_fiveColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}		
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedSixcolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_sixColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_sixColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_sixColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_sixColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}
	
	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedSevencolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_sevenColorBtn.GetBackgroundColor();
/*	BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_sevenColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid  = m_sevenColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_sevenColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedEightcolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_eightColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_eightColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_eightColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_eightColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedNinecolorBtn()
{
	/*UINT BtnID;*/
	m_color=m_nineColorBtn.GetBackgroundColor();
	/*BtnID = m_pParentBtn->GetDlgCtrlID();*/
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_nineColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_nineColorBtn.GetBackgroundColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();

			int a=(int)GetRValue(color);
			int b=(int)GetRValue(color);
			int c=(int)GetRValue(color);
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
				// pos=pObjList->Find(pBkgObj);
				//pObjList->RemoveAt(pos);
				//delete pBkgObj;
				// pBkgObj = NULL;
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				//g_pWhiteBoardView->Invalidate();
				//g_pWhiteBoardView->UpdateWindow();
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);


			}
			CWBBackgroundDlg  *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			if (pBackGroundDlg)
			{		
				pBackGroundDlg->CloseColorPaletteDlg();
			}
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_nineColorBtn.GetBackgroundColor();
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();//设置父窗口中指针为空
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}
	//if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	//{
	//	g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
	//	CScreenDrawMgr::GetIns()->Invalidate();
	//}
	//else
	//{
	//	g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
	//	g_pWhiteBoardView->Invalidate();
	//	g_pWhiteBoardView->UpdateWindow();
	//	g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	//}
}


void CWBColorPaletteDlg::OnBnClickedOthercolorBtn()
{
	CColorDialog   ColorDlgBox(0,0,this); 
	if   (   ColorDlgBox.DoModal()  !=   IDOK) 
	{ 
		return;
	}
	
// 	UINT BtnID;
// 	BtnID = m_pParentBtn->GetDlgCtrlID();
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = ColorDlgBox.GetColor(); 	
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = ColorDlgBox.GetColor();
			m_pParentBtn->Invalidate();
			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
			if (pAttributeDlg)
			{		
				pAttributeDlg->CloseColorPaletteDlg();
			}
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	default:
		{
			g_clrWBCurObjColor = ColorDlgBox.GetColor(); 
			//m_pParentBtn->Invalidate();
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			DestroyWindow();
			pToolBar->SetColorPatetteDlgNULL();
			g_pWhiteBoardView->UpdateObjColor();
		}		
		break;
	
	}
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}


void CWBColorPaletteDlg::OnBnClickedAutocolorBtn()
{
	// TODO: 在此添加控件通知处理程序代码
}


 void CWBColorPaletteDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
 {
	 __super::OnActivate(nState,pWndOther,bMinimized);
	 if (nState == WA_INACTIVE)
	 {
		 if (m_nParentBtnID == IDC_WB_BGDLG_RADIOFORCLRPAD)
		 {
			 CWBBackgroundDlg *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
			 if (pBackGroundDlg)
			 {	
				 g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);			
				 ShowWindow(SW_HIDE);
				 pBackGroundDlg->CloseColorPaletteDlg();//失去焦点时直接删除掉调色板
			 }
		 }
	 }
 	// TODO: 在此处添加消息处理程序代码
 	//CDialog::OnActivate(nState, pWndOther, bMinimized);
 	//if(nState == WA_INACTIVE)//失去激活的时候   因U7最小化而颜色对话框刷新有问题。
 	//{
 	//	switch (m_nParentBtnID)
 	//	{
 	//	case IDC_ATRIBUTE_LINECOLORBUTTON:
 	//		{
 	//			
 	//			CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
 	//			if (pAttributeDlg)
		//		{		
		//			//pAttributeDlg->CloseColorPaletteDlg();//失去焦点时直接删除掉调色板
		//			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);			
		//			ShowWindow(SW_HIDE);
 	//			}	
 	//		}
 	//		break;
		//case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		//	{

		//		CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pParentDlg;
		//		if (pAttributeDlg)
		//		{		
		//			//pAttributeDlg->CloseColorPaletteDlg();
		//			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);			
		//			ShowWindow(SW_HIDE);
		//		}
		//	}
		//	break;
 	//	case IDC_WB_BGDLG_RADIOFORCLRPAD:
 	//		{
 	//			CWBBackgroundDlg *pBackGroundDlg = (CWBBackgroundDlg*)m_pParentDlg;
 	//			if (pBackGroundDlg)
 	//			{	
		//			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);			
		//			ShowWindow(SW_HIDE);
 	//				//pBackGroundDlg->CloseColorPaletteDlg();//失去焦点时直接删除掉调色板
 	//			}
 	//		}
 	//		break;
 	//	default:
		//	{	
		//		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);			
		//		ShowWindow(SW_HIDE);
 	//		}
 	//		break;
 	//	}
 	//}	
 }

void CWBColorPaletteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDC *dc = GetDC();
	BOOL binTran = bInTriangle(point);
	if (!binTran)
	{
		m_color = ::GetPixel (dc->GetSafeHdc(), point.x, point.y);
		m_changecolor = m_color;
		g_clrWBPageColor=m_color;
		ColorFunction();
		
	}
	else
	{
		m_color = ::GetPixel (dc->GetSafeHdc(), point.x, point.y);		
		g_clrWBPageColor=m_color;
		ColorFunction();
	}
	
	//获得颜色后做一些动作
	CClientDC Adc( this );
	////路径渐变单色画刷，画一个三角形
	Graphics graphic(Adc);//创建一个GDI+对象	
	PointF points[]={PointF(90,175),PointF(85,45),PointF(200,110)};//设定路径上的三个坐标点

	GraphicsPath Path;//创建一个路径
	Path.AddLines(points,3);//路径上添加直线

	PathGradientBrush pthGradientBrush(&Path);//定义路径渐变画刷
	pthGradientBrush.SetCenterPoint(Point(205,110));
	if(m_ParentBtnType == TYPE_DLG_COLORPALETTE||m_nParentBtnID == IDC_ATRIBUTE_LINECOLORBUTTON || m_nParentBtnID == IDC_WB_BGDLG_RADIOFORCLRPAD || m_nParentBtnID == IDC_ATRIBUTE_SOLIDCOLORBUTTON)
	{
		pthGradientBrush.SetCenterColor(Color((int)GetRValue(m_changecolor),(int)GetGValue(m_changecolor),(int)GetBValue(m_changecolor)));//设置画刷中心点坐标
	}

	int count=1;
	Color col[]={Color(Color::White)};
	pthGradientBrush.SetSurroundColors(col,&count);//设置路径终点颜色
	graphic.FillPath(&pthGradientBrush,&Path);//画图
	CRect rect(85,45,200,175);
	InvalidateRect(&rect);//进行区域刷新
	ReleaseDC(dc);
}

void  CWBColorPaletteDlg::ColorFunction()
{
// 	UINT BtnID;
// 	BtnID = m_pParentBtn->GetDlgCtrlID();
	switch (m_nParentBtnID)
	{
	case IDC_ATRIBUTE_LINECOLORBUTTON:
		{
			g_clrWBCurObjColor = m_color;
			m_pParentBtn->Invalidate();
			((CWBToolbarDlg *)(FromHandle(CWBToolbarDlg::g_toolBarHWND)))->ReflashBtn(TYPE_DLG_COLORPALETTE);
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	case IDC_ATRIBUTE_SOLIDCOLORBUTTON:
		{
			g_clrSolid = m_color;
			m_pParentBtn->Invalidate();
			g_pWhiteBoardView->UpdateObjSolidColor();
		}
		break;
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		{
			COLORREF color=RGB(255, 255, 255);
			color=GetColor();
			g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色

			CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
			if(pBkgObj)
			{
				pBkgObj->SetBkg(FALSE);
			}
			CDC *dc=GetDC();
			if(dc)
			{
				CDC pDC;
				CBitmap  bitmap;
				bitmap.CreateCompatibleBitmap(dc,g_nWidth,g_nHeight);
				pDC.DeleteDC();
				pDC.CreateCompatibleDC(dc);
				pDC.SelectObject(bitmap);
				pDC.FillSolidRect(0,0,g_nWidth,g_nHeight,color);
				g_pWBPage->SetPageDC(&pDC);	 

				//触发重绘事件
				g_pWhiteBoardView->SetColorPaletteDlg(true);//控制鹰眼的时时刷新
				bitmap.DeleteObject();
				pDC.DeleteDC();
				ReleaseDC(dc);
			}
			
		}
		break;
	default:
		{
			g_clrWBCurObjColor = m_color;
// 			if (m_pParentBtn )
// 			{
// 				m_pParentBtn->Invalidate();
// 			}
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar)
			{		
				pToolBar->CloseToolbox();
			}
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{		
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_PALETTECOLOR,0,0);

			}
			g_pWhiteBoardView->UpdateObjColor();
		}
		break;
	}
	
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
		g_pWhiteBoardView->SetCurTextClr();		
	}

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
		g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	}
}

COLORREF CWBColorPaletteDlg::GetColor()
{
	return m_color;
}

BOOL CWBColorPaletteDlg::bInTriangle( CPoint point )
{
	//PointF(90,175),PointF(90,45),PointF(200,110)
	CPoint pt = point;
	CPoint pt2 = CPoint(90,175);
	CPoint pt1 = CPoint(85,45);
	CPoint pt3 = CPoint(200,110);
//  	int a=Func(pt,pt1,pt2);
// 	int b=Func(pt,pt2,pt3);
// 	int c=Func(pt,pt1,pt3);
	if((Func(pt,pt1,pt2)<=0)&&(Func(pt,pt2,pt3)<=0)&&(Func(pt,pt1,pt3)>=0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void CWBColorPaletteDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
	__super::OnShowWindow(bShow,nStatus);
	if (!bShow)
	{
		m_autoColorBtn.ShowWindow(FALSE);
		m_oneColorBtn.ShowWindow(FALSE);
		m_twoColorBtn.ShowWindow(FALSE);
		m_threeColorBtn.ShowWindow(FALSE);
		m_fourColorBtn.ShowWindow(FALSE);
		m_fiveColorBtn.ShowWindow(FALSE);
		m_sixColorBtn.ShowWindow(FALSE);
		m_sevenColorBtn.ShowWindow(FALSE);
		m_eightColorBtn.ShowWindow(FALSE);
		m_nineColorBtn.ShowWindow(FALSE);
		m_otherColorBtn.ShowWindow(FALSE);
	}
}

