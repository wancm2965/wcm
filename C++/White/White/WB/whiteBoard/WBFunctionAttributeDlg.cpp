// WBAttributeDlg.cpp : 实现文件


// WBAttributeDlg 消息处理程序
#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFunctionAttributeDlg.h"
#include "afxdialogex.h"
#include "WhiteBoardDlg.h"
#include "WhiteBoardView.h"
#include "WBToolbarDlg.h"
#include "WBFunctionDlg.h"
#include "ScreenDrawMgr.h"
#include "WBToolTip.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//属性面板排版起点，宽度最大值wangxiaobin
#define FUNCTIONATTRIBUTE_X 21
#define FUNCTIONATTRIBUTE_WIDTH 160

// WBAttributeDlg 对话框

IMPLEMENT_DYNAMIC(CWBFunctionAttributeDlg, CDialog)

	CWBFunctionAttributeDlg::CWBFunctionAttributeDlg(int nWidth,COLORREF clr,int nLineType,BOOL nBIsSolid,int nArrow,int nTranspar,
	CWnd* pParent,CString nFontFace,int nFontSize)
	: CDialog(CWBFunctionAttributeDlg::IDD, pParent)
{	
	//m_nCurWidth = nWidth;
	//m_nLineWidth = nWidth;
	m_clrLine = clr;
	m_nLineType = nLineType;
	m_bIsSolid = nBIsSolid;
	m_nCurArrow = nArrow;
	//m_nTranspar = "0";
	m_nTranspar.Format(_T("%d"),g_nTranspar);
	m_nFontFace = nFontFace;
	m_strFontSize = "28";
	//m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
	m_strBrushSize = "30";
	m_nPreFontSize = 28;
	m_pAttriButeColorPatetteDlg = NULL;
	m_LineColorButton = NULL;
	m_SolidColorButton = NULL;
	m_pAttriButeSolidPatetteDlg = NULL;
    m_bTextIsEdit = false;}

CWBFunctionAttributeDlg::~CWBFunctionAttributeDlg()
{
	g_bAttriDlginitialize = FALSE;
	if (m_pAttriButeColorPatetteDlg || m_pAttriButeSolidPatetteDlg)
	{
		CloseColorPaletteDlg();
	}
	if (m_LineColorButton)
	{
		delete m_LineColorButton;
		m_LineColorButton = NULL;
	}
	if (m_SolidColorButton)
	{
		delete m_SolidColorButton;
		m_SolidColorButton = NULL;
	}
}

void CWBFunctionAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ATRIBUTE_LINETYPECOMBOBOXEX, m_LineTypeCombx);
	DDX_Control(pDX, IDC_ATRIBUTE_ARROWCOMBOBOXEX, m_ArrowCombx);
	DDX_Control(pDX, IDC_ATRIBUTE_LINEWIDTHCOMBOBOXEX, m_LineWidthCombx);
	DDX_Text(pDX, IDC_ATRIBUTE_LINEWIDTHEDIT, m_nLineWidth);
	DDX_Text(pDX, IDC_ATRIBUTE_TRANSPAREDIT, m_nTranspar);
	DDX_Text(pDX,IDC_WB_FONTSIZECOMBOX,m_strFontSize);
	DDX_Text(pDX,IDC_ATTRIBUTEBRUSHSIZECOMBO,m_strBrushSize);
	DDX_Control(pDX,IDC_SOLIDCHECKSTATIC,m_SolidCheckStatic);
	DDX_Control(pDX, IDC_ATRIBUTE_SLIDER, m_TransparSlider);
	DDX_Control(pDX, IDC_SOLIDCOLORSTATIC, m_SolidColorStatic);
	DDX_Control(pDX, IDC_LINECOLORSTATIC, m_LineColorStatic);
	DDX_Control(pDX, IDC_LINEWIDTHSTATIC, m_LineWidthStatic);
	DDX_Control(pDX, IDC_BRUSHSIZESTATIC, m_BrushSizeStatic);
	DDX_Control(pDX, IDC_ATTRIBUTEBRUSHSIZECOMBO, m_BrushSizeCombx);
	DDX_Control(pDX, IDC_LINETYPESTATIC, m_LineTypeStatic);
	DDX_Control(pDX, IDC_ARROWSTATIC, m_ArrowStatic);
	DDX_Control(pDX, IDC_ATRIBUTE_CHECK_SOLID, m_SolidCheckBtn);
	DDX_Control(pDX, IDC_ATRIBUTE_LINEWIDTHEDIT, m_LineWidthEdit);
	DDX_Control(pDX, IDC_ATRIBUTE_TRANSPAREDIT, m_TransparEdit);
	DDX_Control(pDX, IDC_WB_FONTSTATIC, m_FontStatic);
	DDX_Control(pDX, IDC_WB_FONTFACECOMBOX, m_FontFaceComBox);
	DDX_Control(pDX, IDC_WB_FONTSIZECOMBOX, m_FontSizeComBox);
	DDX_Control(pDX,IDC_WB_FONTBOLDBTN,m_FontBoldBtn);
	DDX_Control(pDX,IDC_WB_FONTITALICBTN,m_FontItalicBtn);
	DDX_Control(pDX,IDC_WB_FONTUNDERLINEBTN,m_FontUnderlineBtn);
	DDX_Control(pDX,IDC_WB_VERTICALBTN,m_VerticalBtn);
	DDX_Control(pDX, IDC_ATTRIBUTE_TEXTSTATIC, m_cTextStatic);
	DDX_Control(pDX, IDC_ATTRIBUTE_EDBOX, m_TextEditBox);
	DDX_Text(pDX,IDC_ATTRIBUTE_EDBOX,m_Textstr);
	DDV_MaxChars(pDX,m_Textstr,256);
	DDX_Control(pDX, IDC_ATTRIBUTE_TRANSPERSTATIC, m_cAttributeTransperStatic);
	DDX_Control(pDX, IDC_ATRIBUTE_REDUCETRANSPARBTN, m_cReduceTransperBtn);
	DDX_Control(pDX, IDC_ATRIBUTE_ADDTRANSPARBTN, m_cAddTransparBtn);
	DDX_Control(pDX, IDC_TRANSPARSTATIC,m_TransparStatic);
	DDX_Control(pDX, IDC_ATTRIBUTE_PERSTATIC,m_PerStatic);
	DDX_Control(pDX, IDC_WB_MT_OPERATE, m_cAttMTOperate);
	DDX_Control(pDX, IDC_WB_MT_ROTATE, m_btnMTRotate);
	DDX_Control(pDX, IDC_WB_MT_ZOOM, m_btnMTZoom);
	DDX_Control(pDX, IDC_ICOSTYLESTATIC, m_IcoStyleStatic);
	DDX_Control(pDX, IDC_ATRIBUTE_ICOSTYLECOMBOBOXEX, m_IcoStyleCombx);
	DDX_Control(pDX, IDC_DEFAULTBACKGROUND, m_DefaultBackGround);
}


BEGIN_MESSAGE_MAP(CWBFunctionAttributeDlg, CDialog)
	ON_EN_CHANGE(IDC_ATRIBUTE_LINEWIDTHEDIT, &CWBFunctionAttributeDlg::OnEnChangeLinewidthedit)
	ON_CBN_SELCHANGE(IDC_ATRIBUTE_LINEWIDTHCOMBOBOXEX, &CWBFunctionAttributeDlg::OnCbnSelchangeLinewidthcomboboxex)
	ON_BN_CLICKED(IDC_ATRIBUTE_CHECK_SOLID, &CWBFunctionAttributeDlg::OnBnClickedCheckSolid)
	ON_BN_CLICKED(IDC_ATRIBUTE_LINECOLORBUTTON, &CWBFunctionAttributeDlg::OnClickedCbtnLinecolor)
	ON_CBN_SELCHANGE(IDC_ATRIBUTE_LINETYPECOMBOBOXEX, &CWBFunctionAttributeDlg::OnCbnSelchangeLinetypecomboboxex)
	ON_CBN_SELCHANGE(IDC_ATRIBUTE_ARROWCOMBOBOXEX, &CWBFunctionAttributeDlg::OnCbnSelchangeArrowcomboboxex)
	ON_BN_CLICKED(IDC_ATRIBUTE_SOLIDCOLORBUTTON, &CWBFunctionAttributeDlg::OnBnClickedCbtnSolidcolor)
	ON_EN_CHANGE(IDC_ATRIBUTE_TRANSPAREDIT, &CWBFunctionAttributeDlg::OnEnChangeTRANSPAREDIT)
	ON_BN_CLICKED(IDC_ATRIBUTE_REDUCETRANSPARBTN, &CWBFunctionAttributeDlg::OnBnClickedReducetranspar)
	ON_BN_CLICKED(IDC_ATRIBUTE_ADDTRANSPARBTN, &CWBFunctionAttributeDlg::OnBnClickedAddtranspar)
	ON_BN_CLICKED(IDC_WB_FONTBOLDBTN, &CWBFunctionAttributeDlg::OnBnClickedWbFontboldbtn)
	ON_BN_CLICKED(IDC_WB_FONTITALICBTN, &CWBFunctionAttributeDlg::OnBnClickedWbFontitalicbtn)
	ON_BN_CLICKED(IDC_WB_FONTUNDERLINEBTN, &CWBFunctionAttributeDlg::OnBnClickedWbFontunderlinebtn)
	ON_BN_CLICKED(IDC_WB_VERTICALBTN, &CWBFunctionAttributeDlg::OnBnClickedWbVerticalBtn)
	ON_BN_CLICKED(IDC_WB_MT_ROTATE, &CWBFunctionAttributeDlg::OnBnClickedWbMTRotateBtn)
	ON_BN_CLICKED(IDC_WB_MT_ZOOM, &CWBFunctionAttributeDlg::OnBnClickedWbMTZoomBtn)
	ON_EN_CHANGE(IDC_ATTRIBUTE_EDBOX, &CWBFunctionAttributeDlg::OnEnChangeAttributeEdbox)
	ON_EN_KILLFOCUS(IDC_ATRIBUTE_LINEWIDTHEDIT, &CWBFunctionAttributeDlg::OnEnKillfocusAtributeLinewidthedit)
	ON_EN_KILLFOCUS(IDC_ATRIBUTE_TRANSPAREDIT, &CWBFunctionAttributeDlg::OnEnKillfocusAtributeTransparedit)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_CLOSE_TOOBAR_MES, OnCloseColorPaletteMessage)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_ATTRIBUTEBRUSHSIZECOMBO, &CWBFunctionAttributeDlg::OnCbnSelchangeAttributebrushsizecombo)
	ON_CBN_SELCHANGE(IDC_WB_FONTFACECOMBOX, &CWBFunctionAttributeDlg::OnCbnSelchangeWbFontfacecombox)
	ON_CBN_SELCHANGE(IDC_WB_FONTSIZECOMBOX, &CWBFunctionAttributeDlg::OnCbnSelchangeWbFontsizecombox)

	ON_CBN_SELCHANGE(IDC_ATRIBUTE_ICOSTYLECOMBOBOXEX, &CWBFunctionAttributeDlg::OnCbnSelchangeAtributeIcostylecomboboxex)

	ON_CBN_EDITCHANGE(IDC_ATTRIBUTEBRUSHSIZECOMBO, &CWBFunctionAttributeDlg::OnCbnEditchangeAttributebrushsizecombo)
	ON_CBN_EDITCHANGE(IDC_WB_FONTSIZECOMBOX, &CWBFunctionAttributeDlg::OnCbnEditchangeWbFontsizecombox)
	//ON_EN_KILLFOCUS(IDC_ATTRIBUTE_EDBOX, &CWBFunctionAttributeDlg::OnEnKillfocusAttributeEdbox)
	ON_BN_CLICKED(IDC_DEFAULTBACKGROUND, &CWBFunctionAttributeDlg::OnBnClickedDefaultbackground)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CWBFunctionAttributeDlg 消息处理程序
BOOL CWBFunctionAttributeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
	//SetDlgItemText(IDC_ATRIBUTE_TRANSPAREDIT,m_nTranspar);//会引发控件的编辑消息
	m_LineColorButton = new CTransparentButtonEx;
	m_LineColorButton->Create(_T(""),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,CRect(21,112,156,30),this,IDC_ATRIBUTE_LINECOLORBUTTON);
	m_LineColorButton->SetType(TYPE_LINECOLORBUTTON);
	m_LineColorButton->ShowWindow(SW_SHOW);
	m_SolidColorButton = new CTransparentButtonEx;
	m_SolidColorButton->Create(_T(""),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,CRect(21,302,56,30),this,IDC_ATRIBUTE_SOLIDCOLORBUTTON);
	m_SolidColorButton->SetType(TYPE_SOLIDCOLORBUTTON);
	m_SolidColorButton->ShowWindow(SW_SHOW);

	//设置线宽
	SetComboxItem(m_LineWidthicon,&m_LineWidthImageList,7, IDI_LINEWIDTH1,&m_LineWidthCombx);

	//设置图标线样式
	SetComboxItem(m_IcoStyleicon, &m_IcoStyleImageList, 7, IDI_ICOSTYLE1, &m_IcoStyleCombx);
	//SetComboxItem();

	// 设置线型
	SetComboxItem(m_LineTypeicon,&m_LineTypeImageList,5, IDI_LINETYPE1,&m_LineTypeCombx);
	//设置箭头
	SetComboxItem(m_Arrowicon,&m_ArrowImageList,10, IDI_NOARROW,&m_ArrowCombx);
	//设置字号
 	InsertStr(&m_FontSizeComBox,_T("字号"));
 	//设置字体
 	InsertStr(&m_FontFaceComBox,_T("字体"));
	//m_FontSizeComBox.SetCurSel(12);
	//SetDlgItemInt(IDC_WB_FONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);  //会引发控件的编辑消息
	m_FontFaceComBox.SetCurSel(16);

	HWND hEdit = ::GetWindow(m_FontSizeComBox.GetSafeHwnd(),GW_CHILD);
	if(IsWindow(hEdit))
	{
		ModifyStyle(hEdit,0,ES_NUMBER,0);
	}

	if (g_bIsSolid)
	{
		m_SolidCheckBtn.SetIsSelect(TRUE);
	}
	else
	{
		m_SolidCheckBtn.SetIsSelect(FALSE);
	}
	if (g_isBold)
	{
		m_FontBoldBtn.SetIsSelect(TRUE);
	} 
	else
	{
		m_FontBoldBtn.SetIsSelect(FALSE);
	}
	if (g_isItalic)
	{
		m_FontItalicBtn.SetIsSelect(TRUE);
	}
	else
	{
		m_FontItalicBtn.SetIsSelect(FALSE);
	}
	if (g_isUnderline)
	{
		m_FontUnderlineBtn.SetIsSelect(TRUE);
	} 
	else
	{
		m_FontUnderlineBtn.SetIsSelect(FALSE);
	}
	if (g_bIsVertical)
	{
		m_VerticalBtn.SetIsSelect(TRUE);
	} 
	else
	{
		m_VerticalBtn.SetIsSelect(FALSE);
	}
	SetLineType(g_nWBLineType);
	SetLineCap(g_nStartLineCap, g_nEndLineCap);
	SetLineWidth(g_nWBCurObjWidth);
	m_TransparSlider.SetRange(0,100);
	m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
	m_SolidColorButton->UpdateWindow();
	m_SolidColorButton->EnableWindow(g_bIsSolid);
	m_cAttributeTransperStatic.ShowWindow(FALSE);
	CString staticText = _T("");
	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgTransparStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("透明度");

	}
	m_TransparStatic.SetCaption(staticText);
	m_TransparStatic.SetTextPosition(TRUE);
	m_TransparStatic.SetWindowText(staticText);
	
	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgPerStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("%");
	}
	m_PerStatic.SetCaption(staticText);
	m_PerStatic.SetTextPosition(TRUE);
	m_PerStatic.SetWindowText(staticText);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgLineColorStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("颜色");
	}
	m_LineColorStatic.SetCaption(staticText);
	m_LineColorStatic.SetTextPosition(TRUE);
	m_LineColorStatic.SetWindowText(staticText);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgLinewidthStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("线宽");
	}
	m_LineWidthStatic.SetWindowText(staticText);
	m_LineWidthStatic.SetCaption(staticText);
	m_LineWidthStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgIcostyleStatic")); //图标样式
	if (staticText.IsEmpty())
	{
		staticText = _T("图标样式");
	}
	m_IcoStyleStatic.SetWindowText(staticText);
	m_IcoStyleStatic.SetCaption(staticText);
	m_IcoStyleStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgLineTypeStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("线型");
	}
	m_LineTypeStatic.SetWindowText(staticText);
	m_LineTypeStatic.SetCaption(staticText);
	m_LineTypeStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgContentTextStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("文本内容");
	}
	m_cTextStatic.SetWindowText(staticText);
	m_cTextStatic.SetCaption(staticText);
	m_cTextStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgSolidColorStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("填充色");
	}
	m_SolidColorStatic.SetWindowText(staticText);
	m_SolidColorStatic.SetCaption(staticText);
	m_SolidColorStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgSolidCheckStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("填充");
	}
	m_SolidCheckStatic.SetWindowText(staticText);
	m_SolidCheckStatic.SetCaption(staticText);
	m_SolidCheckStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgArrowStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("线帽");
	}
	m_ArrowStatic.SetWindowText(staticText);
 	m_ArrowStatic.SetCaption(staticText);
	m_ArrowStatic.SetTextPosition(TRUE);

	staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgMTOperateStatic"));
	if (staticText.IsEmpty())
	{
		staticText = _T("触摸操作");
	}
	m_cAttMTOperate.SetWindowText(staticText);
	m_cAttMTOperate.SetCaption(staticText);
	m_cAttMTOperate.SetTextPosition(TRUE);
	m_cAttMTOperate.ShowWindow(FALSE);

	if (g_bIsMTRotate)
	{
		m_btnMTRotate.SetIsSelect(TRUE);
		m_btnMTZoom.SetIsSelect(FALSE);
	} 
	else
	{
		m_btnMTRotate.SetIsSelect(FALSE);
		m_btnMTZoom.SetIsSelect(TRUE);
	}
	m_btnMTRotate.ShowWindow(FALSE);
	m_btnMTZoom.ShowWindow(FALSE);
	SetAttributeBrushSize();
	//设置m_IcostyleCombox初始化
	m_IcoStyleCombx.SetCurSel(0);
	m_DefaultBackGround.SetWindowText(_T("删除背景"));
	if (g_pWBPage->GetIsExistBackGround())
	{
		m_DefaultBackGround.ShowWindow(SW_SHOW);
	}
	else
	{
		m_DefaultBackGround.ShowWindow(SW_HIDE);
	}

	if (g_bU7NewStyle)
	{
		m_FontStatic.ShowWindow(SW_HIDE);
		m_FontFaceComBox.ShowWindow(SW_HIDE);
		m_FontSizeComBox.ShowWindow(SW_HIDE);
		m_FontBoldBtn.ShowWindow(SW_HIDE);
		m_FontItalicBtn.ShowWindow(SW_HIDE);
		m_FontUnderlineBtn.ShowWindow(SW_HIDE);
		m_VerticalBtn.ShowWindow(SW_HIDE);
	}
	return TRUE;
}
void CWBFunctionAttributeDlg::PreInitDialog()
{
	//加载图像数组
	CDialog::PreInitDialog();
}
//要求图标的ID号是相连的
void CWBFunctionAttributeDlg::SetComboxItem(HICON *iconArray, CImageList *imList,int iconCount, UINT cionResForstId,CComboBoxEx *comboxData)
{
	int iconC;
	COMBOBOXEXITEM TempCmbItem;
	//装载位图表数组
	for(iconC = 0;iconC < iconCount;iconC++)
	{
		iconArray[iconC] = AfxGetApp()->LoadIcon(cionResForstId+ iconC);
	}

	imList->Create(132,12,ILC_COLOR,1,1);
	for (iconC = 0;iconC < iconCount;iconC++)
	{
		imList->Add(iconArray[iconC]);
	}

	comboxData->SetImageList(imList);
	//添加组合框项和设置组合框COMBOBOXEXITEM中每项数据	
	for (iconC = 0;iconC < iconCount;iconC++)
	{
		TempCmbItem.iItem = iconC;
		TempCmbItem.iImage = iconC;
		TempCmbItem.iSelectedImage = iconC;
		TempCmbItem.iOverlay = iconC;
		TempCmbItem.mask=CBEIF_IMAGE|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		TempCmbItem.pszText=NULL;
		comboxData->InsertItem(&TempCmbItem);
		comboxData->ShowWindow(SW_SHOW);
	}

}
//装载字号下拉框列表
void CWBFunctionAttributeDlg::InsertStr(CComboBoxEx *comBoxData,CString str)
{
	CString FontSizeString[37]={L"8",L"9",L"10",L"11",L"12",L"14",L"16",L"18",L"20",L"22",L"24",L"26",L"28",L"36",L"48",L"72",L"100"};
	CString FontFaceString[46]={L"方正舒体",L"方正姚体",L"仿宋",L"黑体",L"华文彩云",L"华文仿宋",L"华文行楷",L"华文琥珀",L"华文楷体",L"华文隶书",
		L"华文宋体",L"华文细黑",L"华文新魏",L"华文中宋",L"楷体",L"隶书",L"宋体",L"微软雅黑",L"新宋体",L"幼圆",L"Agency FB",L"Aharoni",L"Algerian",
		L"Andalus",L"Angsana New",L"AngsanaUPC",L"Aparajita",L"Arabic Typesetting",L"Arial",L"Arial Black",L"Arial Narrow",L"Arial Rounded MT Bold",
		L"Arial Unicode MS",L"Baskerville Old Face",L"Batang",L"BatangChe",L"Bauhaus 93",L"Bell MT",L"Berlin Sans FB",L"Berlin Sans FB Demi",
		L"Bernard MT Condensed",L"Blackadder ITC",L"Bodoni MT",L"Bodoni MT Black",L"Bodoni MT Condensed",L"Bodoni MT Poster Compressed"};
	comBoxData->SetImageList(NULL);
	if (str == _T("字号"))
	{
		for(int i=0;i<17;i++)
		{
			COMBOBOXEXITEM     cbi;
			CString            str;
			int                nItem;

			//	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |	CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
			cbi.mask =  CBEIF_IMAGE|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
			cbi.iItem = i;
			str.Format(FontSizeString[i]);
			cbi.pszText = (LPTSTR)(LPCTSTR)str;
			cbi.cchTextMax = str.GetLength();
			cbi.iImage = i;
			cbi.iSelectedImage = i;
			cbi.iOverlay = i;
			cbi.iIndent = 10;// (i & 0x03);   //Set indentation according


			nItem = comBoxData->InsertItem(&cbi);
			//comBoxData->AddString(FontSizeString[i]);
		}
	}
	if (str == _T("字体"))
	{
		for(int i=0;i<46;i++)
		{

			COMBOBOXEXITEM     cbi;
			CString            str;
			int                nItem;

			//cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |	CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
			cbi.mask =  CBEIF_IMAGE|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
			cbi.iItem = i;
			str.Format(FontFaceString[i]);
			cbi.pszText = (LPTSTR)(LPCTSTR)str;
			cbi.cchTextMax = str.GetLength();
			cbi.iImage = i;
			cbi.iSelectedImage = i;
			cbi.iOverlay = i;
			cbi.iIndent = 10;//(i & 0x03);   //Set indentation according


			nItem = comBoxData->InsertItem(&cbi);
			//comBoxData->AddString(FontFaceString[i]);
		}
	}
}
void CWBFunctionAttributeDlg::SetLineType(int PenLineType)
{
	switch (PenLineType)
	{
	case  PS_SOLID:
		m_LineTypeCombx.SetCurSel(0);
		break;
	case PS_DASH:
		m_LineTypeCombx.SetCurSel(1);
		break;
	case  PS_DOT:
		m_LineTypeCombx.SetCurSel(2);
		break;
	case  PS_DASHDOT:
		m_LineTypeCombx.SetCurSel(3);
		break;
	case  PS_DASHDOTDOT:
		m_LineTypeCombx.SetCurSel(4);
		break;
	}
}
void CWBFunctionAttributeDlg::SetLineWidth(int PenLineWidth)
{
	if (PenLineWidth <= 3)
	{
		m_LineWidthCombx.SetCurSel(0);
	}
	else if (PenLineWidth <=7)
	{
		m_LineWidthCombx.SetCurSel(1);
	}
	else if (PenLineWidth <= 11)
	{
		m_LineWidthCombx.SetCurSel(2);
	}
	else if (PenLineWidth <= 15)
	{
		m_LineWidthCombx.SetCurSel(3);
	}
	else if (PenLineWidth <= 19)
	{
		m_LineWidthCombx.SetCurSel(4);
	}
	else if (PenLineWidth <= 23)
	{
		m_LineWidthCombx.SetCurSel(5);
	}
	else
		m_LineWidthCombx.SetCurSel(6);		
	m_nLineWidth.Format(_T("%d"),PenLineWidth);
	UpdateData(FALSE);
}
void CWBFunctionAttributeDlg::SetLineCap(LineCap nStartLineCap, LineCap nEndLineCap)
{
	if (nStartLineCap == LineCapNoAnchor && nEndLineCap == LineCapNoAnchor)
	{
		m_ArrowCombx.SetCurSel(0);
	}
	if (nStartLineCap == LineCapArrowAnchor  && nEndLineCap == LineCapArrowAnchor)
	{
		m_ArrowCombx.SetCurSel(3);
	}
	if (nStartLineCap == LineCapArrowAnchor  && nEndLineCap ==LineCapNoAnchor)
	{
		m_ArrowCombx.SetCurSel(1);
	}
	if (nStartLineCap == LineCapNoAnchor && nEndLineCap == LineCapArrowAnchor)
	{
		m_ArrowCombx.SetCurSel(2);
	}
	if (nStartLineCap == LineCapRoundAnchor && nEndLineCap == LineCapNoAnchor)
	{
		m_ArrowCombx.SetCurSel(4);
	}
	if (nStartLineCap == LineCapRoundAnchor && nEndLineCap == LineCapRoundAnchor)
	{
		m_ArrowCombx.SetCurSel(5);
	}
	if (nStartLineCap == LineCapNoAnchor && nEndLineCap == LineCapRoundAnchor)
	{
		m_ArrowCombx.SetCurSel(6);
	}
	if (nStartLineCap == LineCapSquareAnchor && nEndLineCap == LineCapNoAnchor)
	{
		m_ArrowCombx.SetCurSel(7);
	}
	if (nStartLineCap == LineCapSquareAnchor && nEndLineCap == LineCapSquareAnchor)
	{
		m_ArrowCombx.SetCurSel(8);
	}
	if (nStartLineCap == LineCapNoAnchor && nEndLineCap == LineCapSquareAnchor)
	{
		m_ArrowCombx.SetCurSel(9);
	}
	return;
}
void CWBFunctionAttributeDlg::OnEnChangeLinewidthedit()
{
	// TODO:  在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	UpdateData(TRUE);
	if (m_nLineWidth.IsEmpty())
	{
		return;
	}
	int nLineWidth = _ttoi(m_nLineWidth);
	if (nLineWidth < 1)
	{
		nLineWidth = 1;
		m_LineWidthCombx.SetCurSel(0);		
	}
	if (nLineWidth >= 1 && nLineWidth <= 3)
	{
		m_LineWidthCombx.SetCurSel(0);
	}
	if (nLineWidth > 3 && nLineWidth <= 7)
	{
		m_LineWidthCombx.SetCurSel(1);
	}
	if (nLineWidth > 7 && nLineWidth <= 11)
	{
		m_LineWidthCombx.SetCurSel(2);
	}
	if (nLineWidth > 11 && nLineWidth <= 15)
	{
		m_LineWidthCombx.SetCurSel(3);
	}
	if (nLineWidth > 15 && nLineWidth <= 19)
	{
		m_LineWidthCombx.SetCurSel(4);
	}
	if (nLineWidth > 19 && nLineWidth <= 23)
	{
		m_LineWidthCombx.SetCurSel(5);
	}
	if (nLineWidth > 23 && nLineWidth <= 99)
	{
		m_LineWidthCombx.SetCurSel(6);
	}
	if (nLineWidth > 99)
	{
		nLineWidth = 99;
		m_LineWidthCombx.SetCurSel(6);
	}
	m_nLineWidth.Format(_T("%d"),nLineWidth);
	UpdateData(FALSE);
	m_LineWidthEdit.SetSel(m_LineWidthEdit.LineLength(),m_LineWidthEdit.LineLength());//光标到末尾
	g_nWBCurObjWidth = nLineWidth;
	g_pWhiteBoardView->UpdateObjLineWidth();
	
	m_LineWidthEdit.SetFocus();
}
void CWBFunctionAttributeDlg::OnEnChangeTRANSPAREDIT()
{
	// TODO:  在此添加控件通知处理程序代码	
	CloseColorPaletteDlg();
	UpdateData(TRUE);

	if (m_nTranspar.IsEmpty())
	{
		return;
	}
	int iTranspar = _ttoi(m_nTranspar);
	if (iTranspar <= 100)
	{	
		m_nTranspar.Format(_T("%d"),iTranspar);
		UpdateData(FALSE);
		m_TransparSlider.SetPos(iTranspar);
		g_nTranspar = (float)(255 - 2.55 * (float)iTranspar);
	}
	else
	{
		iTranspar = 100;
		m_nTranspar.Format(_T("%d"),iTranspar);
		UpdateData(FALSE);
		m_TransparSlider.SetPos(100);
		g_nTranspar = 0;
	}
	//g_nTranspar = (float)(255 - 2.55 * (float)iTranspar);
	m_TransparEdit.SetSel(m_TransparEdit.LineLength(),m_TransparEdit.LineLength());//光标到末尾
	g_pWhiteBoardView->UpdateObjAlpha();
	
	m_TransparEdit.SetFocus();
}
void CWBFunctionAttributeDlg::OnCbnSelchangeLinewidthcomboboxex()
{
	CloseColorPaletteDlg();
	int nLineWidth = m_LineWidthCombx.GetCurSel();
	switch (nLineWidth)
	{
	case  0:
		{
			g_nWBCurObjWidth = 2;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case 1:
		{
			g_nWBCurObjWidth  =4;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case  2:
		{
			g_nWBCurObjWidth = 8;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case  3:
		{
			g_nWBCurObjWidth = 12;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case  4:
		{
			g_nWBCurObjWidth = 16;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case  5:
		{
			g_nWBCurObjWidth = 20;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	case  6:
		{
			g_nWBCurObjWidth = 24;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;
	default:
		{
			g_nWBCurObjWidth = 1;
			m_nLineWidth.Format(_T("%d"),g_nWBCurObjWidth);
		}
		break;   
	}
	UpdateData(FALSE);
	g_pWhiteBoardView->UpdateObjLineWidth();
}
void CWBFunctionAttributeDlg::OnBnClickedCheckSolid()
{
	if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg() 
		&&((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->IsWindowVisible())
	{
		if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->GetClickSolidBtn())
		{
			((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->SetClickSolidBtn(false);
			return;
		}
	}
	CloseColorPaletteDlg();
	if(!m_SolidCheckBtn.GetIsSelect())
	{
		m_SolidCheckBtn.SetIsSelect(TRUE);
		g_bIsSolid = TRUE;
	}
	else
	{
		m_SolidCheckBtn.SetIsSelect(FALSE);
		g_bIsSolid = FALSE;
	}

	m_SolidColorButton->EnableWindow(g_bIsSolid);
	if (g_bIsSolid)
	{
		g_clrSolid = RGB(255,255,255);
		m_SolidColorButton->Invalidate();
	}
	m_SolidCheckBtn.Invalidate();
	g_pWhiteBoardView->UpdateObjSolidCheck();
}
void CWBFunctionAttributeDlg::OnClickedCbtnLinecolor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (_tcscmp(m_strFontSize, L"") == 0)
	{
		m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		UpdateData(FALSE);
	}
	if (_tcscmp(m_strBrushSize, L"") == 0)
	{
		m_strBrushSize.Format(_T("%d"),g_nBrushSize);
		UpdateData(FALSE);
	}

	if (NULL != m_pAttriButeSolidPatetteDlg)
	{
		delete m_pAttriButeSolidPatetteDlg;
		m_pAttriButeSolidPatetteDlg = NULL;
	}
	if (NULL != m_pAttriButeColorPatetteDlg)
	{
		delete m_pAttriButeColorPatetteDlg;
		m_pAttriButeColorPatetteDlg = NULL;
	}
	if (!m_pAttriButeColorPatetteDlg)
	{
		m_pAttriButeColorPatetteDlg = new CWBColorPaletteDlg(m_LineColorButton,this);
		m_pAttriButeColorPatetteDlg->Create(CWBColorPaletteDlg::IDD,this);
		m_pAttriButeColorPatetteDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		if(SW_HIDE == m_pAttriButeColorPatetteDlg->IsWindowVisible())
		{

			CRect colPalletrect;
			m_pAttriButeColorPatetteDlg->GetClientRect(&colPalletrect);

			int height=colPalletrect.Height();
			int width=colPalletrect.Width();
			int colorPalletDlgX=0;
			int colorPalletDlgY=0;
			CRect lineColBtnRect;
			m_LineColorButton->GetClientRect(&lineColBtnRect);//获得按钮的位置及大小
			ClientToScreen(&lineColBtnRect);//将按钮的位置坐标转换为屏幕坐标
			colorPalletDlgY=lineColBtnRect.bottom+100+15;

			colorPalletDlgX=lineColBtnRect.right-width+80-4;//根据按钮的屏幕坐标设定调色板的坐标
			m_pAttriButeColorPatetteDlg->MoveWindow(colorPalletDlgX,colorPalletDlgY,width,height,TRUE);
			m_pAttriButeColorPatetteDlg->ShowWindow(SW_SHOW);
		}
		else
		{
			CloseColorPaletteDlg();
		}	
	}
 	if (NULL != g_pWhiteBoardView->GetCurEdit())
 	{
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
 
 		g_pWhiteBoardView->SetCurTextClr();		
 	}
}
void CWBFunctionAttributeDlg::OnCbnSelchangeLinetypecomboboxex()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	g_nWBLineType = m_LineTypeCombx.GetCurSel();
	g_pWhiteBoardView->UpdateObjLineType();
}
void CWBFunctionAttributeDlg::OnCbnSelchangeArrowcomboboxex()
{
	//TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	int nLineCap = m_ArrowCombx.GetCurSel();
	switch (nLineCap)
	{
	case 0:
		g_nStartLineCap = LineCapNoAnchor;
		g_nEndLineCap = LineCapNoAnchor;
		break;
	case 1:
		g_nStartLineCap = LineCapArrowAnchor;
		g_nEndLineCap = LineCapNoAnchor;
		break;
	case 2:
		g_nStartLineCap = LineCapNoAnchor;
		g_nEndLineCap = LineCapArrowAnchor;
		break;
	case 3:
		g_nStartLineCap = g_nEndLineCap = LineCapArrowAnchor;
		break;
	case 4:
		g_nStartLineCap = LineCapRoundAnchor;
		g_nEndLineCap = LineCapNoAnchor;
		break;
	case 5:
		g_nStartLineCap = g_nEndLineCap = LineCapRoundAnchor;
		break;
	case 6:
		g_nStartLineCap = LineCapNoAnchor;
		g_nEndLineCap = LineCapRoundAnchor;
		break;
	case 7:
		g_nStartLineCap = LineCapSquareAnchor;
		g_nEndLineCap = LineCapNoAnchor;
		break;
	case 8:
		g_nStartLineCap = g_nEndLineCap = LineCapSquareAnchor;
		break;
	case 9:
		g_nStartLineCap = LineCapNoAnchor;
		g_nEndLineCap = LineCapSquareAnchor;
		break;
	}
	g_pWhiteBoardView->UpdateObjLineArrow();
}
void CWBFunctionAttributeDlg::OnBnClickedCbtnSolidcolor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (_tcscmp(m_strFontSize, L"") == 0)
	{
		m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		UpdateData(FALSE);
	}
	if (_tcscmp(m_strBrushSize, L"") == 0)
	{
		m_strBrushSize.Format(_T("%d"),g_nBrushSize);
		UpdateData(FALSE);
	}

	if (NULL != m_pAttriButeColorPatetteDlg)
	{
		delete m_pAttriButeColorPatetteDlg;
		m_pAttriButeColorPatetteDlg = NULL;
	}
	if (NULL != m_pAttriButeSolidPatetteDlg)
	{
		delete m_pAttriButeSolidPatetteDlg;
		m_pAttriButeSolidPatetteDlg = NULL;
	}
	if(g_bIsSolid)     //如果选中控制按钮  laihuan 20120529
	{
		if (!m_pAttriButeSolidPatetteDlg)
		{
			m_pAttriButeSolidPatetteDlg = new CWBColorPaletteDlg(m_SolidColorButton,this);
			m_pAttriButeSolidPatetteDlg->Create(CWBColorPaletteDlg::IDD,this);
			m_pAttriButeSolidPatetteDlg->ShowWindow(SW_SHOW);
		}
		else
		{
			CloseColorPaletteDlg();
		}	

	}
}
void CWBFunctionAttributeDlg::OnBnClickedReducetranspar()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	int iTranspar = _ttoi(m_nTranspar);	
	iTranspar -= 20;
	if (iTranspar <= 0)
	{
		iTranspar = 0;
	}
	m_nTranspar.Format(_T("%d"),iTranspar);
	m_TransparSlider.SetPos(iTranspar);
	g_nTranspar = float(255 - 2.55 * (float)iTranspar);
	UpdateData(FALSE);
	g_pWhiteBoardView->UpdateObjAlpha();
}
void CWBFunctionAttributeDlg::OnBnClickedAddtranspar()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	int iTranspar = _ttoi(m_nTranspar);
	iTranspar += 20;
	if (iTranspar >= 100)
	{
		iTranspar = 100;
	}
	m_nTranspar.Format(_T("%d"),iTranspar);
	m_TransparSlider.SetPos(iTranspar);
	g_nTranspar = (float)(255 - 2.55 * (float)iTranspar);
	UpdateData(FALSE);
	g_pWhiteBoardView->UpdateObjAlpha();
}

void CWBFunctionAttributeDlg::OnOK()
{
	return;
}
// void CWBFunctionAttributeDlg::OnCbnSelchangeWbFontfacecombox()
// {
// 	// TODO: 在此添加控件通知处理程序代码
// 	CloseColorPaletteDlg();
// 	int i = m_FontFaceComBox.GetCurSel();
// 	m_FontFaceComBox.GetLBText(i,g_nWBCurObjFontFaceName);
// 
// 	if (NULL != g_pWhiteBoardView->GetCurEdit())
// 	{
// 		int iFontWeight;
// 		if (g_isBold)
// 		{
// 			iFontWeight = FW_BOLD;
// 		}
// 		else
// 			iFontWeight = FW_NORMAL;
// 		if (m_Font.GetSafeHandle())
// 		{
// 			m_Font.Detach();
// 		}
// 		m_Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
// 			, OUT_DEFAULT_PRECIS
// 			, CLIP_DEFAULT_PRECIS
// 			, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
// 		g_pWhiteBoardView->GetCurEdit()->SetFont(&m_Font);
// 
// 		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
// 
// 		g_pWhiteBoardView->SetCurTextFont();
// 	}
// 	g_pWhiteBoardView->UpdateObjFontFace();
// 
// 	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
// 	{
// 		if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontFaceComBox())
// 		{
// 			g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontFaceComBox()->SetCurSel(i);
// 		}
// 	}
// }
//void CWBFunctionAttributeDlg::OnCbnSelchangeWbFontsizecombox()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CloseColorPaletteDlg();
//	switch (int i = m_FontSizeComBox.GetCurSel())
//	{
//	case 0:
//		g_nWBCurObjFontSize = 8;
//		break;
//	case 1:
//		g_nWBCurObjFontSize = 9;
//		break;
//	case 2:
//		g_nWBCurObjFontSize = 10;
//		break;
//	case 3:
//		g_nWBCurObjFontSize = 11;
//		break;
//	case 4:
//		g_nWBCurObjFontSize = 12;
//		break;
//	case 5:
//		g_nWBCurObjFontSize = 14;
//		break;
//	case 6:
//		g_nWBCurObjFontSize = 16;
//		break;
//	case 7:
//		g_nWBCurObjFontSize = 18;
//		break;
//	case 8:
//		g_nWBCurObjFontSize = 20;
//		break;
//	case 9:
//		g_nWBCurObjFontSize = 22;
//		break;
//	case 10:
//		g_nWBCurObjFontSize = 24;
//		break;
//	case 11:
//		g_nWBCurObjFontSize = 26;
//		break;
//	case 12:
//		g_nWBCurObjFontSize = 28;
//		break;
//	case 13:
//		g_nWBCurObjFontSize = 36;
//		break;
//	case 14:
//		g_nWBCurObjFontSize = 48;
//		break;
//	case 15:
//		g_nWBCurObjFontSize = 72;
//		break;
//	case 16:
//		g_nWBCurObjFontSize = 100;
//		break;
//	}
//	m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
//	UpdateData(FALSE);
//	if (NULL != g_pWhiteBoardView->GetCurEdit())
//	{
//		int iFontWeight;
//		if (g_isBold)
//		{
//			iFontWeight = FW_BOLD;
//		}
//		else
//			iFontWeight = FW_NORMAL;
//		if (m_Font.GetSafeHandle())
//		{
//			m_Font.Detach();
//		}
//		m_Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
//			, OUT_DEFAULT_PRECIS
//			, CLIP_DEFAULT_PRECIS
//			, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
//		g_pWhiteBoardView->GetCurEdit()->SetFont(&m_Font);
//
//		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
//
//		g_pWhiteBoardView->SetCurTextFont();
//	}
//	g_pWhiteBoardView->UpdateObjFontSize();
//
//	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
//	{
//		if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
//		{
//			g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
//			g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
//		}
//	}
//}
//void CWBFunctionAttributeDlg::OnCbnEditchangeWbFontsizecombox()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	int nWBCurObjFontSize = g_nWBCurObjFontSize;
//	g_nWBCurObjFontSize = atoi(m_strFontSize);
//
//	if (strcmp(m_strFontSize, "") == 0)
//	{
//		g_nWBCurObjFontSize = nWBCurObjFontSize;
//		return;
//	}
//
//	if (g_nWBCurObjFontSize < 1)
//	{
//		m_FontSizeComBox.SetWindowText("1");
//		m_FontSizeComBox.SetDlgItemInt(IDC_WB_FONTSIZECOMBOX,1,FALSE);
//		g_nWBCurObjFontSize = 1;		
//		if (NULL != g_pWhiteBoardView->GetCurEdit())
//		{
//			int iFontWeight;
//			if (g_isBold)
//			{
//				iFontWeight = FW_BOLD;
//			}
//			else
//				iFontWeight = FW_NORMAL;
//			if (m_Font.GetSafeHandle())
//			{
//				m_Font.Detach();
//			}
//			m_Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
//				, OUT_DEFAULT_PRECIS
//				, CLIP_DEFAULT_PRECIS
//				, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
//			g_pWhiteBoardView->GetCurEdit()->SetFont(&m_Font);
//
//			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
//
//			g_pWhiteBoardView->SetCurTextFont();
//		}		
//	}
//	if (g_nWBCurObjFontSize > 600)
//	{
//		m_FontSizeComBox.SetWindowText("600");	
//		m_FontSizeComBox.SetDlgItemInt(IDC_WB_FONTSIZECOMBOX,600,FALSE);
//		g_nWBCurObjFontSize = 600;
//		if (NULL != g_pWhiteBoardView->GetCurEdit())
//		{
//			int iFontWeight;
//			if (g_isBold)
//			{
//				iFontWeight = FW_BOLD;
//			}
//			else
//				iFontWeight = FW_NORMAL;
//			if (m_Font.GetSafeHandle())
//			{
//				m_Font.Detach();
//			}
//			m_Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
//				, OUT_DEFAULT_PRECIS
//				, CLIP_DEFAULT_PRECIS
//				, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
//			g_pWhiteBoardView->GetCurEdit()->SetFont(&m_Font);
//
//			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
//		}
//	}
//	if ((g_nWBCurObjFontSize >= 1)&(g_nWBCurObjFontSize <= 600))
//	{
//		m_FontSizeComBox.SetDlgItemInt(IDC_WB_FONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
//		if (NULL != g_pWhiteBoardView->GetCurEdit())
//		{
//			int iFontWeight;
//			if (g_isBold)
//			{
//				iFontWeight = FW_BOLD;
//			}
//			else
//				iFontWeight = FW_NORMAL;
//			if (m_Font.GetSafeHandle())
//			{
//				m_Font.Detach();
//			}
//			m_Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
//				, OUT_DEFAULT_PRECIS
//				, CLIP_DEFAULT_PRECIS
//				, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
//			g_pWhiteBoardView->GetCurEdit()->SetFont(&m_Font);
//
//			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());
//
//			g_pWhiteBoardView->SetCurTextFont();
//		}
//	}
//	m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
//	UpdateData(FALSE);
//	g_pWhiteBoardView->UpdateObjFontSize();
//
//	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
//	{
//		if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
//		{
//			g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
//			g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
//		}
//	}
//}
void CWBFunctionAttributeDlg::OnBnClickedWbFontboldbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	if(!m_FontBoldBtn.GetIsSelect())
	{
		m_FontBoldBtn.SetIsSelect(TRUE);
		g_isBold = TRUE;
	}
	else
	{
		m_FontBoldBtn.SetIsSelect(FALSE);
		g_isBold = FALSE;
	}
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
	{
		CWBFunctionBtn *ptempBtn = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontBoldBtn();
		ptempBtn->SetIsSelect(g_isBold);
		ptempBtn->Invalidate();
		ptempBtn = NULL;
	}

	m_FontBoldBtn.SetIsSelect(g_isBold);
	m_FontBoldBtn.Invalidate();

	m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	/*CFont font;
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
			iFontWeight = FW_NORMAL;

	
		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		g_pWhiteBoardView->SetCurTextFont();
	}*/
	g_pWhiteBoardView->UpdateObjFontBold();
	//font.DeleteObject();
}
void CWBFunctionAttributeDlg::OnBnClickedWbFontitalicbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	if(!m_FontItalicBtn.GetIsSelect())
	{
		m_FontItalicBtn.SetIsSelect(TRUE);
		g_isItalic = TRUE;
	}
	else
	{
		m_FontItalicBtn.SetIsSelect(FALSE);
		g_isItalic = FALSE;
	}
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
	{
		CWBFunctionBtn *ptempBtn = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontItalicBtn();
		ptempBtn->SetIsSelect(g_isItalic);
		ptempBtn->Invalidate();
		ptempBtn = NULL;
	}

	m_FontItalicBtn.SetIsSelect(g_isItalic);
	m_FontItalicBtn.Invalidate();

	m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	//CFont font;
	//if (NULL != g_pWhiteBoardView->GetCurEdit())
	//{
	//	int iFontWeight;
	//	if (g_isBold)
	//	{
	//		iFontWeight = FW_BOLD;
	//	}
	//	else
	//		iFontWeight = FW_NORMAL;
	//	
	//	font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
	//		, OUT_DEFAULT_PRECIS
	//		, CLIP_DEFAULT_PRECIS
	//		, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
	//	g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

	//	g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

	//	g_pWhiteBoardView->SetCurTextFont();
	//}
	g_pWhiteBoardView->UpdateObjFontItalic();
	//font.DeleteObject();
}
void CWBFunctionAttributeDlg::OnBnClickedWbFontunderlinebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	if(!m_FontUnderlineBtn.GetIsSelect())
	{
		m_FontUnderlineBtn.SetIsSelect(TRUE);
		g_isUnderline = TRUE;
	}
	else
	{
		m_FontUnderlineBtn.SetIsSelect(FALSE);
		g_isUnderline = FALSE;
	}
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
	{
		CWBFunctionBtn* ptempBtn = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontUnderlineBtn();
		ptempBtn->SetIsSelect(g_isUnderline);
		ptempBtn->Invalidate();
		ptempBtn = NULL;
	}

	m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
	m_FontUnderlineBtn.Invalidate();

	m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	//CFont font;
	//if (NULL != g_pWhiteBoardView->GetCurEdit())
	//{
	//	int iFontWeight;
	//	if (g_isBold)
	//	{
	//		iFontWeight = FW_BOLD;
	//	}
	//	else
	//		iFontWeight = FW_NORMAL;
	//	
	//	font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
	//		, OUT_DEFAULT_PRECIS
	//		, CLIP_DEFAULT_PRECIS
	//		, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
	//	g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

	//	g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

	//	g_pWhiteBoardView->SetCurTextFont();
	//}
	g_pWhiteBoardView->UpdateObjUnderline();

	//font.DeleteObject();
}

void CWBFunctionAttributeDlg::OnBnClickedWbVerticalBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	if(!m_VerticalBtn.GetIsSelect())
	{
		m_VerticalBtn.SetIsSelect(TRUE);
		g_bIsVertical = TRUE;
	}
	else
	{
		m_VerticalBtn.SetIsSelect(FALSE);
		g_bIsVertical = FALSE;
	}

	//CFont font1;
	//按了竖排输出按钮后改变编辑框
	if (g_pWhiteBoardView->GetCurEdit())
	{
		CWBRichEdit *pEdit = g_pWhiteBoardView->GetCurEdit();
		CWBPromptFontSetDlg *pPromptFontSetDlg = g_pWhiteBoardView->GetPromptFontSetDlg();
		if (pEdit == NULL || pPromptFontSetDlg == NULL)
		{
			return;
		}
		CString str;
		pEdit->GetWindowText(str);

		CRect rectText = g_pWhiteBoardView->GetEditRect();
		CPoint pt;
		if (g_bIsVertical)//从横变竖
		{
			pt.x = rectText.right;
			pt.y = rectText.top;
		} 
		else//从竖变横
		{
			pt = rectText.TopLeft();
		}
		g_pWhiteBoardView->SetEditTopLeft(pt);

		if (g_pWhiteBoardView->GetIsEditing())
		{
			PWBTextItem pTextItem = new WBTextItem;
			CWBText *pText = NULL;
			if ((g_pWhiteBoardView->GetEditObj())->GetType() == WB_OBJ_ANNOTATIONS)
			{
				CWBAnnotations *pAnnotations = (CWBAnnotations *)(g_pWhiteBoardView->GetEditObj());
				pText = pAnnotations->GetText();
			} 
			else
			{
				pText = (CWBText *)(g_pWhiteBoardView->GetEditObj());
			}

			pText->GetItem(pTextItem);
			pTextItem->ptTopLeft = pt;
			pText->SetItem(pTextItem);
			pText->SetCalRectText();
			delete pTextItem;
		}

		CWBRichEditMgr::GetInst_()->SetRichEditShow(false);
		CWBRichEditMgr::GetInst_()->SetBeginPoint(rectText.left, rectText.top, MOUSE_ID);
		CWBRichEditMgr::GetInst_()->SetEndPoint(rectText.right, rectText.bottom, MOUSE_ID);
		CWBRichEditMgr::GetInst_()->SetRichEditShow(true);

		CWnd *pParent = NULL;
		if(NULL != CScreenDrawMgr::GetIns()) //如果是在桌面绘图，则将文本框加载CMouseDlg上，否则加在CWhiteBoardView上
		{
			///设置文本框的父窗口,如果是win7 vista以上则是mousedlg,否则为screendrawdlg
			CScreenDrawMgr::GetIns()->SetRichEditParent(&pParent);
		}
		else
		{
			pParent = g_pWhiteBoardView;
		}
		CWBRichEditMgr::GetInst_()->SetIsSaveText(false);
		CWBRichEditMgr::GetInst_()->ShowWBRichEdit(MOUSE_ID, g_bIsVertical, pParent, true);
		CWBRichEditMgr::GetInst_()->SetIsSaveText(true);

		pEdit = g_pWhiteBoardView->GetCurEdit();
		//		pEdit->MoveWindow(&rectText);
		pEdit->SetWindowText(str);
		pEdit->SetFocus();
		(pEdit->GetRichEditCtrl()).LimitText(1024);
		(pEdit->GetRichEditCtrl()).SetSel(-1, -1);//光标到末尾

		m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
		//设置文字字体样式
		//int iFontWeight;
		//if (g_isBold)
		//{
		//	iFontWeight = FW_BOLD;
		//}
		//else
		//{
		//	iFontWeight = FW_NORMAL;
		//}
		//font1.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
		//	, OUT_DEFAULT_PRECIS
		//	, CLIP_DEFAULT_PRECIS
		//	, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));		
		//pEdit->SetFont(&font1);

		//g_pWhiteBoardView->SetEditCharFormat(pEdit);
		//g_pWhiteBoardView->SetCurTextFont();

		if (NULL != pPromptFontSetDlg)
		{
			pPromptFontSetDlg->GetFontVerticalBtn()->SetIsSelect(g_bIsVertical);
			pPromptFontSetDlg->GetFontVerticalBtn()->Invalidate();
		}
	}

	m_VerticalBtn.SetIsSelect(g_bIsVertical);
	m_VerticalBtn.Invalidate();

	g_pWhiteBoardView->UpdateObjVertical();
	//font1.DeleteObject();
}

void CWBFunctionAttributeDlg::OnBnClickedWbMTRotateBtn()
{
	CloseColorPaletteDlg();
	m_btnMTRotate.SetIsSelect(TRUE);
	m_btnMTRotate.Invalidate();
	m_btnMTZoom.SetIsSelect(FALSE);
	m_btnMTZoom.Invalidate();
	g_bIsMTRotate = TRUE;
	g_pWhiteBoardView->UpdateMTOperate();
}

void CWBFunctionAttributeDlg::OnBnClickedWbMTZoomBtn()
{
	CloseColorPaletteDlg();
	m_btnMTZoom.SetIsSelect(TRUE);
	m_btnMTZoom.Invalidate();
	m_btnMTRotate.SetIsSelect(FALSE);
	m_btnMTRotate.Invalidate();
	g_bIsMTRotate = FALSE;
	g_pWhiteBoardView->UpdateMTOperate();
}

void CWBFunctionAttributeDlg::OnEnChangeAttributeEdbox()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	GetDlgItemText(IDC_ATTRIBUTE_EDBOX,g_cTextstr);
	SetIsEditingText(true);
	g_pWhiteBoardView->UpdateObjTextStr();
	SetIsEditingText(true);
	m_TextEditBox.SetFocus();	
}
BOOL CWBFunctionAttributeDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		clicentRc.top -= 3;
		clicentRc.right += 3;
		clicentRc.bottom += 3;

		img.Draw(pDC->m_hDC,clicentRc);
	}	
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
void CWBFunctionAttributeDlg::SetSliderCtrlPos()
{
	CloseColorPaletteDlg();
	m_nTranspar.Format(_T("%d"),m_TransparSlider.GetPos());
	g_nTranspar = (float)(255 - 2.55 * (float)m_TransparSlider.GetPos());
	UpdateData(FALSE);
	g_pWhiteBoardView->UpdateObjAlpha();
}

void CWBFunctionAttributeDlg::SetBtnState(int BtnId,WB_MOUSE_STATE MouseState)
{
	switch (BtnId)
	{
	case IDC_ATRIBUTE_REDUCETRANSPARBTN:
		m_cReduceTransperBtn.SetMouseState(MouseState);
		break;
	case IDC_ATRIBUTE_ADDTRANSPARBTN:
		m_cAddTransparBtn.SetMouseState(MouseState);
		break;
	case IDC_WB_FONTBOLDBTN:	
		m_FontBoldBtn.SetMouseState(MouseState);
		break;
	case IDC_WB_FONTITALICBTN:
		m_FontItalicBtn.SetMouseState(MouseState);
		break;
	case IDC_WB_FONTUNDERLINEBTN:
		m_FontUnderlineBtn.SetMouseState(MouseState);
		break;
	case IDC_WB_VERTICALBTN:
		m_VerticalBtn.SetMouseState(MouseState);
		break;
	case IDC_ATRIBUTE_CHECK_SOLID:
		m_SolidCheckBtn.SetMouseState(MouseState);
		break;
	case IDC_DEFAULTBACKGROUND:
		m_DefaultBackGround.SetMouseState(MouseState);
		break;
	default:
		break;
	}
}

void CWBFunctionAttributeDlg::OnCancel()
{
	return;
}

CWBFunctionBtn *CWBFunctionAttributeDlg::GetFontBoldBtn()
{
	return &m_FontBoldBtn;
}
CWBFunctionBtn *CWBFunctionAttributeDlg::GetFontItalicBtn()
{
	return &m_FontItalicBtn;
}
CWBFunctionBtn *CWBFunctionAttributeDlg::GetFontUnderlineBtn()
{
	return &m_FontUnderlineBtn;
}
CWBFunctionBtn *CWBFunctionAttributeDlg::GetFontVerticalBtn()
{
	return &m_VerticalBtn;
}
LRESULT CWBFunctionAttributeDlg::OnCloseColorPaletteMessage(WPARAM wParam, LPARAM lParam)
{
	CloseColorPaletteDlg();
	return 0;
}

void CWBFunctionAttributeDlg::CloseColorPaletteDlg()
{
	if (NULL != m_pAttriButeColorPatetteDlg)
	{
		delete m_pAttriButeColorPatetteDlg;
		m_pAttriButeColorPatetteDlg = NULL;
	}
	if (NULL != m_pAttriButeSolidPatetteDlg)
	{
		delete m_pAttriButeSolidPatetteDlg;
		m_pAttriButeSolidPatetteDlg = NULL;
	}
	if (_tcscmp(m_strFontSize, L"") == 0)
	{
		m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		UpdateData(FALSE);
	}
	if (_tcscmp(m_strBrushSize, L"") == 0)
	{
		m_strBrushSize.Format(_T("%d"),g_nBrushSize);
		UpdateData(FALSE);
	}
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg() && NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
	{
		CComboBoxEx *pTempCom = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox();
		if (NULL != pTempCom)
		{
			CString str;
			pTempCom->GetWindowText(str);
			if (_tcscmp(str, L"") == 0)
			{
				pTempCom->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
				g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
			}
		}
	}
}


void CWBFunctionAttributeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);
	CloseColorPaletteDlg();
}


void CWBFunctionAttributeDlg::OnEnKillfocusAtributeLinewidthedit()
{
	if (!g_pWhiteBoardView->GetSafeHwnd())
	{
		return;
	}
	if (m_nLineWidth.IsEmpty())
	{
		m_nLineWidth.Format(_T("%d"),(int)g_nWBCurObjWidth);
		UpdateData(FALSE);
		//m_LineWidthEdit.SetWindowText(_T("4"));
		m_LineWidthEdit.Invalidate();
		m_LineWidthEdit.UpdateWindow();
	}
}

void CWBFunctionAttributeDlg::OnEnKillfocusAtributeTransparedit()
{
	if (!g_pWhiteBoardView->GetSafeHwnd())
	{
		return;
	}
	if (m_nTranspar.IsEmpty())
	{
		int iTranspar =100  - g_nTranspar/255 * 100;
		m_nTranspar.Format(_T("%d"),iTranspar);
		UpdateData(FALSE);
		//m_TransparEdit.SetWindowText(_T("0"));
		m_TransparEdit.Invalidate();
		m_TransparEdit.UpdateWindow();
	}
}

void CWBFunctionAttributeDlg::WBFunctionAttributeShowWindow( WBFunctionAttribute functionattribute )
{
	m_LineColorStatic.ShowWindow(functionattribute.LineColorStatic);
	m_LineColorButton->ShowWindow(functionattribute.LineColorButton);
	m_SolidColorStatic.ShowWindow(functionattribute.SolidColorStatic);
	m_SolidCheckBtn.ShowWindow(functionattribute.SolidCheckBtn);
	m_SolidCheckStatic.ShowWindow(functionattribute.SolidCheckStatic);
	m_SolidColorButton->ShowWindow(functionattribute.SolidColorButton);
	m_LineWidthStatic.ShowWindow(functionattribute.LineWidthStatic);
	m_LineWidthEdit.ShowWindow(functionattribute.LineWidthEdit);
	m_LineWidthCombx.ShowWindow(functionattribute.LineWidthCombx);
	m_LineTypeStatic.ShowWindow(functionattribute.LineTypeStatic);
	m_LineTypeCombx.ShowWindow(functionattribute.LineTypeCombx);
	m_ArrowStatic.ShowWindow(functionattribute.ArrowStatic);
	m_ArrowCombx.ShowWindow(functionattribute.ArrowCombx);
	m_FontStatic.ShowWindow(functionattribute.FontStatic);
	m_FontBoldBtn.ShowWindow(functionattribute.FontBoldBtn);
	m_FontItalicBtn.ShowWindow(functionattribute.FontItalicBtn);
	m_FontUnderlineBtn.ShowWindow(functionattribute.FontUnderlineBtn);
	m_VerticalBtn.ShowWindow(functionattribute.VerticalBtn);
	m_cTextStatic.ShowWindow(functionattribute.cTextStatic);
	m_TextEditBox.ShowWindow(functionattribute.TextEditBox);
	m_FontFaceComBox.ShowWindow(functionattribute.FontFaceComBox);
	m_FontSizeComBox.ShowWindow(functionattribute.FontSizeComBox);

	m_IcoStyleStatic.ShowWindow(functionattribute.IcoStyleStatic); //显示StyleStatic状态
	m_IcoStyleCombx.ShowWindow(functionattribute.IcoStyleComBox); //显示StyleCombx状态

	
	if (g_bU7NewStyle)
	{
		m_FontStatic.ShowWindow(SW_HIDE);
		m_FontFaceComBox.ShowWindow(SW_HIDE);
		m_FontSizeComBox.ShowWindow(SW_HIDE);
		m_FontBoldBtn.ShowWindow(SW_HIDE);
		m_FontItalicBtn.ShowWindow(SW_HIDE);
		m_FontUnderlineBtn.ShowWindow(SW_HIDE);
		m_VerticalBtn.ShowWindow(SW_HIDE);

 		m_TransparEdit.ShowWindow(SW_SHOW);
 		m_cReduceTransperBtn.ShowWindow(SW_SHOW);
 		m_cAddTransparBtn.ShowWindow(SW_SHOW);
 		m_TransparSlider.ShowWindow(SW_SHOW);
 		m_TransparStatic.ShowWindow(SW_SHOW);
	}
}

void CWBFunctionAttributeDlg::WBFunctionAttributeHideWindow()
{
	m_LineColorStatic.ShowWindow(SW_HIDE);
	if (m_LineColorButton)
	{
		m_LineColorButton->ShowWindow(SW_HIDE);
	}	
	m_SolidColorStatic.ShowWindow(SW_HIDE);
	m_SolidCheckBtn.ShowWindow(SW_HIDE);
	m_SolidCheckStatic.ShowWindow(SW_HIDE);
	if (m_SolidColorButton)
	{
		m_SolidColorButton->ShowWindow(SW_HIDE);
	}
	m_LineWidthStatic.ShowWindow(SW_HIDE);

	m_LineWidthEdit.ShowWindow(SW_HIDE);
	m_LineWidthCombx.ShowWindow(SW_HIDE);
	m_LineTypeStatic.ShowWindow(SW_HIDE);
	m_LineTypeCombx.ShowWindow(SW_HIDE);
	m_ArrowStatic.ShowWindow(SW_HIDE);
	m_ArrowCombx.ShowWindow(SW_HIDE);
	m_FontStatic.ShowWindow(SW_HIDE);
	m_FontBoldBtn.ShowWindow(SW_HIDE);
	m_FontItalicBtn.ShowWindow(SW_HIDE);
	m_FontUnderlineBtn.ShowWindow(SW_HIDE);
	m_VerticalBtn.ShowWindow(SW_HIDE);
	m_cTextStatic.ShowWindow(SW_HIDE);
	m_TextEditBox.ShowWindow(SW_HIDE);
	m_FontFaceComBox.ShowWindow(SW_HIDE);
	m_FontSizeComBox.ShowWindow(SW_HIDE);
	m_TransparEdit.ShowWindow(SW_HIDE);

	if (!g_bU7NewStyle)
	{
		m_cReduceTransperBtn.ShowWindow(SW_HIDE);
		m_cAddTransparBtn.ShowWindow(SW_HIDE);
		m_TransparSlider.ShowWindow(SW_HIDE);
		m_TransparStatic.ShowWindow(SW_HIDE);
	}
	
	m_PerStatic.ShowWindow(SW_HIDE);

	m_IcoStyleStatic.ShowWindow(SW_HIDE); //隐藏StyleStatic状态
	m_IcoStyleCombx.ShowWindow(SW_HIDE); //隐藏StyleCombx状态

	m_DefaultBackGround.ShowWindow(SW_HIDE);
}

void CWBFunctionAttributeDlg::WBFunctionAttributeMoveWindowUnSelect()
{
	CSize nsize;
	m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
	m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
	m_SolidColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,283);
	nsize = m_SolidColorStatic.GetStringLength();
	m_SolidCheckBtn.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,280,20,20);
	m_SolidCheckStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 30 ,283);
	m_SolidColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,302,FUNCTIONATTRIBUTE_WIDTH,30);
	m_LineWidthStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,164);
	nsize = m_LineWidthStatic.GetStringLength();
	m_LineWidthEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,160,40,20);
	m_LineWidthCombx.MoveWindow(FUNCTIONATTRIBUTE_X,184,FUNCTIONATTRIBUTE_WIDTH,30);
	m_LineTypeStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,224);
	m_LineTypeCombx.MoveWindow(FUNCTIONATTRIBUTE_X,244,FUNCTIONATTRIBUTE_WIDTH,30);
	m_ArrowStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,342);
	m_ArrowCombx.MoveWindow(FUNCTIONATTRIBUTE_X,360,FUNCTIONATTRIBUTE_WIDTH,30);
	m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,400,FUNCTIONATTRIBUTE_WIDTH,58);
	m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,402,100,400);
	m_FontSizeComBox.MoveWindow(123,402,52,400);
	m_FontBoldBtn.MoveWindow(30,429,28,28);
	m_FontItalicBtn.MoveWindow(68,429,28,28);
	m_FontUnderlineBtn.MoveWindow(106,429,28,28);
	m_VerticalBtn.MoveWindow(144,429,28,28);
	m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
	nsize = m_TransparStatic.GetStringLength();
	m_TransparEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,31,40,20);
	m_PerStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 50,35);
	m_cReduceTransperBtn.MoveWindow(FUNCTIONATTRIBUTE_X,58,30,30);
	m_cAddTransparBtn.MoveWindow(145,58,30,30);
	m_TransparSlider.MoveWindow(53,58,90,30);
	m_DefaultBackGround.MoveWindow(FUNCTIONATTRIBUTE_X,470,90,30);
}

void CWBFunctionAttributeDlg::WBFunctionAttributeMoveWindowDrawLine()
{
	CSize sized;
	m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
	m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
	m_LineWidthStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,164);
	sized = m_LineWidthStatic.GetStringLength();
	m_LineWidthEdit.MoveWindow(FUNCTIONATTRIBUTE_X + sized.cx + 5,160,46,20);
	m_LineWidthCombx.MoveWindow(FUNCTIONATTRIBUTE_X,184,FUNCTIONATTRIBUTE_WIDTH,30);
	m_LineTypeStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,224);
	m_LineTypeCombx.MoveWindow(FUNCTIONATTRIBUTE_X,244,FUNCTIONATTRIBUTE_WIDTH,30);
	m_ArrowStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,282);
	m_ArrowCombx.MoveWindow(FUNCTIONATTRIBUTE_X,302,FUNCTIONATTRIBUTE_WIDTH,30);

	//g_nWBCurObjWidth = 4;
	//SetLineWidth(g_nWBCurObjWidth); //add by jiangchao 2013.5.2
}

void CWBFunctionAttributeDlg::WBFunctionAttributeMoveWindowDrawPology()
{
	CSize size;
	m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
	m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
	m_SolidColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,225);
	size = m_SolidColorStatic.GetStringLength();
	m_SolidCheckBtn.MoveWindow(FUNCTIONATTRIBUTE_X + size.cx + 5,222,20,20);
	m_SolidCheckStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + size.cx + 30,225);
	m_SolidColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,244,FUNCTIONATTRIBUTE_WIDTH,30);
	m_LineWidthStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,164);
	size = m_LineWidthStatic.GetStringLength();
	m_LineWidthEdit.MoveWindow(FUNCTIONATTRIBUTE_X + size.cx + 5,160,46,20);
	m_LineWidthCombx.MoveWindow(FUNCTIONATTRIBUTE_X,184,FUNCTIONATTRIBUTE_WIDTH,30);
}
 _WBFunctionAttribute::_WBFunctionAttribute()
 {
	 LineColorStatic = 0;
	 LineColorButton = 0;
	 LineWidthStatic = 0;
	 LineWidthEdit = 0;
	 LineWidthCombx = 0;
	 LineTypeStatic = 0;
	 LineTypeCombx = 0;
	 SolidColorStatic = 0;
	 SolidCheckBtn = 0;
	 SolidCheckStatic = 0;
	 SolidColorButton = 0;
	 ArrowStatic = 0;
	 ArrowCombx = 0;
	 cTextStatic = 0;
	 TextEditBox = 0;
	 FontStatic = 0;
	 FontFaceComBox = 0;
	 FontSizeComBox = 0;
	 FontBoldBtn = 0;
	 FontItalicBtn = 0;
	 FontUnderlineBtn = 0;
	 VerticalBtn = 0;
	 BrushSizeComBox = 0;
	 BrushSizeStatic = 0;

	 /*新增Ico属性赋初值*/
	 IcoStyleStatic = 0;
	 IcoStyleComBox = 0;
 }

_WBFunctionAttribute::_WBFunctionAttribute( const _WBFunctionAttribute &pItem )
{
	this->LineColorStatic = pItem.LineColorStatic;
	this->LineColorButton = pItem.LineColorButton;
	LineWidthStatic = pItem.LineWidthStatic;
	LineWidthEdit = pItem.LineWidthEdit;
	LineWidthCombx = pItem.LineWidthCombx;
	LineTypeStatic = pItem.LineTypeStatic;
	LineTypeCombx = pItem.LineTypeCombx;
	SolidColorStatic = pItem.SolidColorStatic;
	SolidCheckBtn = pItem.SolidCheckBtn;
	SolidCheckStatic = pItem.SolidCheckStatic;
	SolidColorButton = pItem.SolidColorButton;
	ArrowStatic = pItem.ArrowStatic;
	ArrowCombx = pItem.ArrowCombx;
	cTextStatic = pItem.cTextStatic;
	TextEditBox = pItem.TextEditBox;
	FontStatic = pItem.FontStatic;
	FontFaceComBox = pItem.FontFaceComBox;
	FontSizeComBox = pItem.FontSizeComBox;
	FontBoldBtn = pItem.FontBoldBtn;
	FontItalicBtn = pItem.FontItalicBtn;
	FontUnderlineBtn = pItem.FontUnderlineBtn;
	VerticalBtn = pItem.VerticalBtn;
	BrushSizeComBox = pItem.BrushSizeComBox;
	BrushSizeStatic = pItem.BrushSizeStatic;

    /*新增Ico属性赋值*/
	IcoStyleStatic = pItem.IcoStyleStatic;
	IcoStyleComBox = pItem.IcoStyleComBox;
}

void CWBFunctionAttributeDlg::OnCbnSelchangeWbFontfacecombox()
{
	// TODO: 在此添加控件通知处理程序代码

	CloseColorPaletteDlg();
	int i = m_FontFaceComBox.GetCurSel();
	m_FontFaceComBox.GetLBText(i,g_nWBCurObjFontFaceName);

	m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	//CFont font;
	//if (NULL != g_pWhiteBoardView->GetCurEdit())
	//{
	//	int iFontWeight;
	//	if (g_isBold)
	//	{
	//		iFontWeight = FW_BOLD;
	//	}
	//	else
	//		iFontWeight = FW_NORMAL;
	//
	//	font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
	//		, OUT_DEFAULT_PRECIS
	//		, CLIP_DEFAULT_PRECIS
	//		, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
	//	g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

	//	g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

	//	g_pWhiteBoardView->SetCurTextFont();
	//}
	g_pWhiteBoardView->UpdateObjFontFace();
	CWBPromptFontSetDlg* pTempFontdlg = g_pWhiteBoardView->GetPromptFontSetDlg();
	if (NULL != pTempFontdlg && NULL != pTempFontdlg->GetFontFaceComBox())
	{
		pTempFontdlg->GetFontFaceComBox()->SetCurSel(i);
	}
	pTempFontdlg = NULL;
	//font.DeleteObject();
}

void CWBFunctionAttributeDlg::OnCbnSelchangeWbFontsizecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseColorPaletteDlg();
	int i = m_FontSizeComBox.GetCurSel();
	switch (i)
	{
	case 0:
		g_nWBCurObjFontSize = 8;
		break;
	case 1:
		g_nWBCurObjFontSize = 9;
		break;
	case 2:
		g_nWBCurObjFontSize = 10;
		break;
	case 3:
		g_nWBCurObjFontSize = 11;
		break;
	case 4:
		g_nWBCurObjFontSize = 12;
		break;
	case 5:
		g_nWBCurObjFontSize = 14;
		break;
	case 6:
		g_nWBCurObjFontSize = 16;
		break;
	case 7:
		g_nWBCurObjFontSize = 18;
		break;
	case 8:
		g_nWBCurObjFontSize = 20;
		break;
	case 9:
		g_nWBCurObjFontSize = 22;
		break;
	case 10:
		g_nWBCurObjFontSize = 24;
		break;
	case 11:
		g_nWBCurObjFontSize = 26;
		break;
	case 12:
		g_nWBCurObjFontSize = 28;
		break;
	case 13:
		g_nWBCurObjFontSize = 36;
		break;
	case 14:
		g_nWBCurObjFontSize = 48;
		break;
	case 15:
		g_nWBCurObjFontSize = 72;
		break;
	case 16:
		g_nWBCurObjFontSize = 100;
		break;
	}
	m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
	UpdateData(FALSE);
	m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	//CFont font;
	//if (NULL != g_pWhiteBoardView->GetCurEdit())
	//{
	//	int iFontWeight;
	//	if (g_isBold)
	//	{
	//		iFontWeight = FW_BOLD;
	//	}
	//	else
	//		iFontWeight = FW_NORMAL;
	//
	//	font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
	//		, OUT_DEFAULT_PRECIS
	//		, CLIP_DEFAULT_PRECIS
	//		, DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
	//	g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

	//	g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

	//	g_pWhiteBoardView->SetCurTextFont();
	//}
	g_pWhiteBoardView->UpdateObjFontSize();
	CWBPromptFontSetDlg* pTempFontdlg = g_pWhiteBoardView->GetPromptFontSetDlg();
	if (NULL != pTempFontdlg && NULL != pTempFontdlg->GetFontSizeComBox())
	{
		pTempFontdlg->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		pTempFontdlg->UpdateData(FALSE);
	}
	pTempFontdlg = NULL;
	//font.DeleteObject();
}

void CWBFunctionAttributeDlg::SetAttributeBrushSize()
 {
	 CString staticText;
	 staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("AttributeDlgBrushSizeStatic"));
	 if (staticText.IsEmpty())
	 {
		 staticText = _T("擦子大小");
	 }
	 m_BrushSizeStatic.SetCaption(staticText);
	 m_BrushSizeCombx.SetImageList(NULL);
	 CString str[10] ={_T("10"),_T("20"),_T("30"),_T("40"),_T("50"),_T("60"),_T("70"),_T("80"),_T("90"),_T("100")};
	 for(int i = 0;i<10; i++)
	 {
		 COMBOBOXEXITEM cbi;
		 CString Str;
		 
		 cbi.mask = CBEIF_IMAGE|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		 cbi.iItem = i;
		 Str.Format(str[i]);
		 cbi.pszText = Str.GetBuffer(Str.GetLength());
		 cbi.cchTextMax = Str.GetLength();
		 cbi.iImage = i;
		 cbi.iSelectedImage = i;
		 cbi.iOverlay = i;
		 cbi.iIndent = 3;

		 m_BrushSizeCombx.InsertItem(&cbi);
		 Str.ReleaseBuffer();
	 }
	 m_BrushSizeCombx.SetCurSel(0);
	 m_nDrawPattern = 0;
	 g_nBrushSize = 10;
	 m_BrushSizeStatic.ShowWindow(SW_HIDE);
	 m_BrushSizeCombx.ShowWindow(SW_HIDE);
 }
 
 void CWBFunctionAttributeDlg::OnCbnSelchangeAttributebrushsizecombo()
 {

	 // TODO: 在此添加控件通知处理程序代码
	 int index = m_BrushSizeCombx.GetCurSel();
	 if(index != CB_ERR)
	 {
		 m_nDrawPattern = index;
		 switch(index)
		 {
		 case 0:
			 {
				 g_nBrushSize = 10;
			 }
			 break;
		 case  1:
			 {
				 g_nBrushSize = 20;
			 }
			 break;
		 case  2:
			 {
				 g_nBrushSize = 30;
			 }
			 break;
		 case 3:
			 {
				 g_nBrushSize = 40;
			 }
			 break;
		 case 4:
			 {
				 g_nBrushSize = 50;
			 }
			 break;
		 case 5:
			 {
				 g_nBrushSize = 60;
					
			 }
			 break;
		 case 6:
			 {
				 g_nBrushSize = 70;
			 }
			 break;
		 case 7:
			 {
				 g_nBrushSize = 80;
			 }
			 break;
		 case 8:
			 {
				 g_nBrushSize = 90;
			 }
			 break;
		 case 9:
			 {
				 g_nBrushSize = 100;
			 }
			 break;
		 default:
			 break;

		 }
		m_strBrushSize.Format(_T("%d"),g_nBrushSize);
		UpdateData(FALSE);
	 }
 }

 void CWBFunctionAttributeDlg::WBFunctionAttributeSelectEraser()
 {
	 m_BrushSizeStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
	 m_BrushSizeCombx.MoveWindow(FUNCTIONATTRIBUTE_X,51,100,180);
	 m_BrushSizeCombx.SetCurSel(m_nDrawPattern);
	 m_BrushSizeStatic.ShowWindow(SW_SHOW);
	 m_BrushSizeCombx.ShowWindow(SW_SHOW);
 }


 void CWBFunctionAttributeDlg::WBFuncitonAttributeMoveWindowDrawIcoCurve() //Add by jiangchao 2013.4.18
 {
	 m_LineWidthStatic.MoveWindow(21,98,36,14);
	 m_LineWidthEdit.MoveWindow(63,94,46,20);
	 m_LineWidthCombx.MoveWindow(21,120,156,30);
	 m_IcoStyleStatic.MoveWindow(21,154,64,14);
	 m_IcoStyleCombx.MoveWindow(21,174,156,30);

	 //g_nWBCurObjWidth = 24;
	//SetLineWidth(g_nWBCurObjWidth); //add by jiangchao 2013.5.2
 }

 void CWBFunctionAttributeDlg::OnCbnSelchangeAtributeIcostylecomboboxex()
 {
	 // TODO: 在此添加控件通知处理程序代码
	 UpdateData(TRUE);
	 CloseColorPaletteDlg();
	 int nIcoStyle = m_IcoStyleCombx.GetCurSel();

	 CString strPath = GetCurrentPath() + _T("\\skin\\WhiteBoard\\image\\functiondlg\\");
	 switch (nIcoStyle)
	 {
	 case  0:
		 {
			 g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE1;
			 g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case 1:
		 {
			g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE2;
			g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case  2:
		 {
			g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE3;
			g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case  3:
		 {
			g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE4;
			g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case  4:
		 {
			 g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE5;
			 g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case  5:
		 {
             g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE6;
			 g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case  6:
		 {
			g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE7;
			g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;
	 case 7:
		 {
			 if(g_bSupportU7OpenFile)
			 {
				 wchar_t szFilter[MAX_PATH];
				 CString strFilter = _T("图片");
				 MultiByteToWideChar(CP_ACP, 0, strFilter+" (*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.bmp;*.dib;*.tif;*.tiff;*.png;*.ico)|*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.bmp;*.dib;*.tif;*.tiff;*.png;*.ico||", -1, szFilter, MAX_PATH);
				 char* sFilePath = new char[MAX_PATH];
				 bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, (LPCTSTR)szFilter, this->GetSafeHwnd());
				 if (g_pWhiteBoardView == NULL)
				 {
					 return;
				 }
				 if(bOpen)
				 {
					 CString strPath = (LPCTSTR)A2U(sFilePath);
					 if (!strPath.IsEmpty()) //路径不为空
					 {
						 if (g_pWhiteBoardView)
						 {
							 g_pWhiteBoardView->SendCurvePicFromFile(sFilePath);
						 }
						 g_nWBCurObjIcoFilePath = strPath;
						 g_bIsWBLoadIcoFromFile = TRUE;
					 }
				 }
                 delete[] sFilePath;
			 }
		 }
		 break;
	 default:
		 {
			 g_nWBCurObjIcoFilePath = strPath + WBICOCURVE_STYLE1;
			 g_bIsWBLoadIcoFromFile = FALSE;
		 }
		 break;   
	 }
	 UpdateData(FALSE);
	 g_pWhiteBoardView->UpdateObjIcoStyle();
 }

 void CWBFunctionAttributeDlg::SetIcoStyle(CString strIcoStyle)
 {
	 int find = strIcoStyle.ReverseFind('\\');
	 if(find != -1)
	 {
		 strIcoStyle = strIcoStyle.Right(strIcoStyle.GetLength() - find - 1);
	 }
	 
	 if(strIcoStyle == WBICOCURVE_STYLE1)
	 {
		 m_IcoStyleCombx.SetCurSel(0);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE2)
	 {
		 m_IcoStyleCombx.SetCurSel(1);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE3)
	 {
		 m_IcoStyleCombx.SetCurSel(2);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE4)
	 {
		 m_IcoStyleCombx.SetCurSel(3);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE5)
	 {
		 m_IcoStyleCombx.SetCurSel(4);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE6)
	 {
		 m_IcoStyleCombx.SetCurSel(5);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else if(strIcoStyle == WBICOCURVE_STYLE7)
	 {
		 m_IcoStyleCombx.SetCurSel(6);
		 g_bIsWBLoadIcoFromFile = FALSE;
	 }
	 else
	 {
		 m_IcoStyleCombx.SetCurSel(7);
		 g_bIsWBLoadIcoFromFile = TRUE;
	 }
	 UpdateData(FALSE);
 }

 void CWBFunctionAttributeDlg::OnCbnEditchangeAttributebrushsizecombo()
 {
	 // TODO: 在此添加控件通知处理程序代码
	 UpdateData(TRUE);
	 int nBrushSize = g_nBrushSize;
	 g_nBrushSize = _ttoi(m_strBrushSize);
	 if (_tcscmp(m_strBrushSize, L"") == 0)
	 {
		 g_nBrushSize = nBrushSize;
		 return;
	 }
	 if (g_nBrushSize < 1)
	 {
		 m_BrushSizeCombx.SetWindowText(L"10");
		 m_BrushSizeCombx.SetDlgItemInt(IDC_ATTRIBUTEBRUSHSIZECOMBO,10,FALSE);
		 g_nBrushSize = 10;

	 }
	 if (g_nBrushSize > 100)
	 {
		 m_BrushSizeCombx.SetWindowText(L"100");
		 m_BrushSizeCombx.SetDlgItemInt(IDC_ATTRIBUTEBRUSHSIZECOMBO,100,FALSE);
		 g_nBrushSize = 100;
	 }
	 if ((g_nBrushSize>= 10)&(g_nBrushSize<= 100))
	 {
		 m_BrushSizeCombx.SetDlgItemInt(IDC_ATTRIBUTEBRUSHSIZECOMBO,g_nBrushSize,FALSE);
	 }
	 m_strBrushSize.Format(_T("%d"),g_nBrushSize);
	 UpdateData(FALSE);
 }


void CWBFunctionAttributeDlg::SetComboxItem()
{
	CString strPath = GetCurrentPath() +  _T("\\skin\\WhiteBoard\\image\\functiondlg\\icostyle8.bmp");

	//判断文件是否存在
    if(!PathFileExists(strPath))
	{
		CDC * pDC = GetDC();
		if(!SaveBitmap(pDC, CRect(0,0,132,12), ToStr(strPath.GetBuffer(strPath.GetLength())).c_str()))
		{
			strPath.ReleaseBuffer();
			ReleaseDC(pDC);
			return;
		}
		strPath.ReleaseBuffer();
		ReleaseDC(pDC);
	}

	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	CBitmap * m_Bitmap = new CBitmap();
	m_Bitmap->Attach(hBitmap);

	m_IcoStyleImageList.Add(m_Bitmap, RGB(0,0,0));

	COMBOBOXEXITEM TempCmbItem;
	TempCmbItem.iItem = 7;
	TempCmbItem.iImage = 7;
	TempCmbItem.iSelectedImage = 7;
	TempCmbItem.iOverlay = 7;
	TempCmbItem.mask=CBEIF_IMAGE|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
	TempCmbItem.pszText=NULL;
	m_IcoStyleCombx.InsertItem(&TempCmbItem);
	m_IcoStyleCombx.ShowWindow(SW_SHOW);

	m_Bitmap->Detach();
	delete m_Bitmap;
	m_Bitmap = NULL;
}

BOOL CWBFunctionAttributeDlg::SaveBitmap(CDC *pDC, CRect rect, const char *filename)
{
	int left=rect.left,top=rect.top,width=rect.Width(),heigth=rect.Height();
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC,width,heigth);
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	CBitmap*pOldBitmap=memdc.SelectObject(&bitmap);
	memdc.StretchBlt(0,0,width,heigth,pDC,left,top,width,heigth,SRCCOPY);
	memdc.FillSolidRect(&rect, RGB(255, 255, 255));

	CFont font;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = 99;
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = 134;
	::lstrcpy(lf.lfFaceName, _T("宋体"));

	font.CreatePointFontIndirect(&lf);
	CFont *pOldFont = memdc.SelectObject(&font);
	CString strTitle = _T("Load Ico From File");
	memdc.DrawText(strTitle,CRect(0, 0, 132, 12),DT_CENTER|DT_VCENTER);

	BITMAP btm;
	bitmap.GetBitmap(&btm);

	DWORD size=btm.bmWidthBytes*btm.bmHeight;
	LPSTR lpData=(LPSTR)::GlobalAlloc(GPTR,size);
	/////////////////////////////////////////////
	BITMAPINFOHEADER bih;
	bih.biBitCount=btm.bmBitsPixel;
	bih.biClrImportant=0;
	bih.biClrUsed=0;
	bih.biCompression=0;
	bih.biHeight=btm.bmHeight;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);
	bih.biSizeImage=size;
	bih.biWidth=btm.bmWidth;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	GetDIBits(memdc,bitmap,0,bih.biHeight,lpData,(BITMAPINFO*)&bih,DIB_RGB_COLORS);

	CString name;
	name=filename;//保存文件名
	BITMAPFILEHEADER bfh;
	bfh.bfReserved1=bfh.bfReserved2=0;
	bfh.bfType=((WORD)('M'<< 8)|'B');
	bfh.bfSize=54+size;
	bfh.bfOffBits=54;
	CFile bf;
	bool ret=false;
	if(bf.Open(name,CFile::modeCreate|CFile::modeWrite))
	{
		bf.Write(&bfh,sizeof(BITMAPFILEHEADER));
		bf.Write(&bih,sizeof(BITMAPINFOHEADER));
		bf.Write(lpData,size);
		bf.Close();
		ret = true;
	}
	::GlobalFree(lpData);

	memdc.SelectObject(pOldBitmap);
	memdc.SelectObject(pOldFont);
	bitmap.DeleteObject();
	font.DeleteObject();
	memdc.DeleteDC();
	return ret;
}

 void CWBFunctionAttributeDlg::OnCbnEditchangeWbFontsizecombox()
 {
	 // TODO: 在此添加控件通知处理程序代码
	  CloseColorPaletteDlg();
	 UpdateData(TRUE);
     //CString str = _T("");
	 //GetDlgItemText(IDC_WB_FONTSIZECOMBOX,str)
	 //int nFontSize = g_nWBCurObjFontSize;	 
	 g_nWBCurObjFontSize = _ttoi(m_strFontSize);

	 if (g_nWBCurObjFontSize < 1)
	 {
		 g_nWBCurObjFontSize = m_nPreFontSize;
	 }
	 if (g_nWBCurObjFontSize > 100)
	 {
		 // m_BrushSizeCombx.SetDlgItemInt(IDC_WB_FONTSIZECOMBOX,100,FALSE);
		 g_nWBCurObjFontSize = 100;
	 }
	 m_nPreFontSize = g_nWBCurObjFontSize;  
	 m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
	 UpdateData(FALSE);

	 m_pAttriButePromptFontSetDlg->CreateFontAndSetFont();
	 //CFont font;
	 //if (NULL != g_pWhiteBoardView->GetCurEdit())
	 //{
		// int iFontWeight;
		// if (g_isBold)
		// {
		//	 iFontWeight = FW_BOLD;
		// }
		// else
		//	 iFontWeight = FW_NORMAL;

		// font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
		//	 , OUT_DEFAULT_PRECIS
		//	 , CLIP_DEFAULT_PRECIS
		//	 , DEFAULT_QUALITY,FF_DONTCARE,_T(g_nWBCurObjFontFaceName));	
		// g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

		// g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		// g_pWhiteBoardView->SetCurTextFont();
	 //}
	// m_TransparEdit.SetSel(m_TransparEdit.LineLength(),m_TransparEdit.LineLength());//光标到末尾
	 g_pWhiteBoardView->UpdateObjFontSize();
     g_pWhiteBoardView->SetCurTextFont();

	 CWBPromptFontSetDlg* pTempFontdlg = g_pWhiteBoardView->GetPromptFontSetDlg();
	 if (NULL != pTempFontdlg && NULL != pTempFontdlg->GetFontSizeComBox())
	 {
		 pTempFontdlg->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		 pTempFontdlg->UpdateData(FALSE);
	 }
	 pTempFontdlg = NULL;

	 CComboBoxEx* pEdit = (CComboBoxEx*)GetDlgItem(IDC_WB_FONTSIZECOMBOX);
	 if (NULL != pEdit)
	 {
		 int n = m_strFontSize.GetLength();
		 pEdit->SetFocus();
		 pEdit->GetEditCtrl()->SetSel(n,n); 
	 }
	 pEdit = NULL;
	// font.DeleteObject();
 }


 BOOL CWBFunctionAttributeDlg::PreTranslateMessage(MSG* pMsg)
 {
	 // TODO: 在此添加专用代码和/或调用基类
	 if (pMsg->wParam == VK_RETURN)
	 {
		 HWND hWnd = ::GetFocus();
		 int nID = ::GetDlgCtrlID(hWnd);

		 switch (nID)
		 {
		 case IDC_ATRIBUTE_TRANSPAREDIT:
			 {
				 UpdateData(TRUE);
			 }
			 break;

		 case IDC_ATRIBUTE_LINEWIDTHEDIT:
			 {
				 UpdateData(TRUE);
			 }
		 default:
			 break;
		 }
	 }
	 return CDialog::PreTranslateMessage(pMsg);
 }
 
 void CWBFunctionAttributeDlg::SetAllComboxExStatue()
 {
	if (NULL != m_LineTypeCombx.m_hWnd)
	{
		m_LineTypeCombx.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_ArrowCombx.m_hWnd)
	{
		m_ArrowCombx.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_LineWidthCombx.m_hWnd)
	{
		m_LineWidthCombx.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_BrushSizeCombx.m_hWnd)
	{
		m_BrushSizeCombx.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_ArrowCombx.m_hWnd)
	{
		m_ArrowCombx.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_FontFaceComBox.m_hWnd)
	{
		m_FontFaceComBox.SendMessage( CB_GETCURSEL, 0, 0);
	}
	if (NULL != m_FontSizeComBox.m_hWnd)
	{
		m_FontSizeComBox.SendMessage( CB_GETCURSEL, 0, 0);

	}
 }


 //void CWBFunctionAttributeDlg::OnEnKillfocusAttributeEdbox()
 //{
	// SetIsEditingText(false);
	// if (g_cTextstr.IsEmpty())
	// {
	//	 g_pWhiteBoardView->SetIsKillFocus(TRUE);
	// }
	// else
	// {
	//     g_pWhiteBoardView->SetIsKillFocus(FALSE);
	// }
	// g_pWhiteBoardView->UpdateObjTextStr();
 //}

 void CWBFunctionAttributeDlg::OnBnClickedDefaultbackground()
 {

	  CWBObject *pBkgObj = g_pWBPage->GetBkgObj();
	 if (pBkgObj)
	 {
		 //取消背景时，将白板大小设成默认值大小，
		 /*g_nWidth = WB_VIEWHSCROLL;
		 g_nHeight = WB_VIEWVSCROLL;
		 setViewScroll();
		 m_pEagleEyeDlg->GetPreviewSingleView()->SetRatio();
		 m_pEagleEyeDlg->GetPreviewSingleView()->SetRectInPreview();*/
		 g_pWBPage->SetIsExistBackGround(FALSE);
		 m_DefaultBackGround.ShowWindow(SW_HIDE);
		 pBkgObj->SetBkg(FALSE);
		 g_pWhiteBoardView->Invalidate();
		 //发送同步信息
		 XMemIni xmlObj;
		 pBkgObj->SetXmlValue(xmlObj);
		 g_pWhiteBoardView->ChangeObj(xmlObj);
	 }
 }


 void CWBFunctionAttributeDlg::OnSize(UINT nType, int cx, int cy)
 {
	 CDialog::OnSize(nType, cx, cy);

	 // TODO: 在此处添加消息处理程序代码
	 CRect FuncDlgRt;
	 if (g_pWhiteBoardView == NULL || g_pWhiteBoardView->GetWBFunctionDlg() == NULL)
		return;
	 g_pWhiteBoardView->GetWBFunctionDlg()->GetClientRect(FuncDlgRt);
	 CRect  attrDlgRect;
	 GetClientRect(attrDlgRect);
	 //滚动条的信息设置
	 m_si.cbSize = sizeof (SCROLLINFO);
	 m_si.fMask = SIF_ALL;
	 m_si.nMin = 0;
	 m_si.nMax =600;

	 m_si.nPage = 200;   //滚动条一整页的高度
	 m_si.nPos = 0;
	 SetScrollInfo(SB_VERT,&m_si,1);
	 ShowScrollBar(SB_VERT,TRUE);
	 if (FuncDlgRt.Height() > 500)
	 {
		 ShowScrollBar(SB_VERT,FALSE);
	 }
	 if(g_bU7NewStyle)
	 {
		 CString strTranspar;
		 strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
		 if (strTranspar.IsEmpty())
		 {
			 strTranspar = _T("透明度");
		 }
		 m_cReduceTransperBtn.ShowWindow(SW_SHOW); //显示公共的调节透明度
		 m_cAddTransparBtn.ShowWindow(SW_SHOW);
		 m_TransparSlider.ShowWindow(SW_SHOW);
		 m_TransparEdit.ShowWindow(SW_SHOW);
		 m_TransparStatic.ShowWindow(SW_SHOW);
		 m_cReduceTransperBtn.MoveWindow(FUNCTIONATTRIBUTE_X,58,30,30);
		 m_cAddTransparBtn.MoveWindow(145,58,30,30);
		 m_TransparSlider.MoveWindow(53,58,90,30);
		 m_TransparStatic.SetCaption(strTranspar);
		 m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
		 CSize nsize;
		 nsize = m_TransparStatic.GetStringLength();
		 m_TransparEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,31,40,20);
		 m_PerStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 50,35);
		 m_PerStatic.ShowWindow(SW_SHOW);


		 if(DRAW_NORMAL == g_nDrawStatus)
		 {		 
			 WBFunctionAttribute functionAttribute;
			 ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
			 functionAttribute.LineColorStatic = 5;
			 functionAttribute.LineColorButton = 5;
			 functionAttribute.SolidColorStatic = 5;
			 functionAttribute.SolidCheckBtn = 5;
			 functionAttribute.SolidCheckStatic = 5;
			 functionAttribute.SolidColorButton = 5;
			 functionAttribute.LineWidthStatic = 5;
			 functionAttribute.LineWidthEdit = 5;
			 functionAttribute.LineWidthCombx = 5;
			 functionAttribute.LineTypeStatic = 5;
			 functionAttribute.LineTypeCombx = 5;
			 functionAttribute.ArrowStatic = 5;
			 functionAttribute.ArrowCombx = 5;
			 functionAttribute.FontStatic = 5;
			 functionAttribute.FontFaceComBox = 5;
			 functionAttribute.FontSizeComBox = 5;
			 functionAttribute.FontBoldBtn = 5;
			 functionAttribute.FontItalicBtn = 5;
			 functionAttribute.FontUnderlineBtn = 5;
			 functionAttribute.VerticalBtn = 5;
			 WBFunctionAttributeShowWindow(functionAttribute);
			 WBFunctionAttributeMoveWindowUnSelect();
			 m_cAttMTOperate.ShowWindow(SW_HIDE);
			 m_btnMTRotate.ShowWindow(SW_HIDE);
			 m_btnMTZoom.ShowWindow(SW_HIDE);
			 m_BrushSizeCombx.ShowWindow(SW_HIDE);
			 m_BrushSizeStatic.ShowWindow(SW_HIDE);
			 g_bAttriDlginitialize = TRUE;
		 }
	 }
 }

 void CWBFunctionAttributeDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值
	 SCROLLINFO scrollinfo;
	 GetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
	 switch (nSBCode)  
	 {  
	 case SB_BOTTOM: 
		 ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax)); 
		 UpdateWindow();
		 scrollinfo.nPos = scrollinfo.nMax;  
		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		 break;  
	 case SB_TOP:  
		 ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMin)); 
		 UpdateWindow();
		 scrollinfo.nPos = scrollinfo.nMin; 
		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		 break; 
	 case SB_LINEUP:  
		 {	
			 if (scrollinfo.nPos - scrollinfo.nMin  == 0)
			 {
				 break;
			 }
			 else if (scrollinfo.nPos - scrollinfo.nMin < 20)
			 {
				 ScrollWindow(0,(scrollinfo.nPos - scrollinfo.nMin));
				 scrollinfo.nPos = scrollinfo.nMin;
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
				 break;
			 }
			 else if (scrollinfo.nPos - scrollinfo.nMin >= 20)
			 {
				 scrollinfo.nPos -= 20;
				 ScrollWindow(0,20);
				 UpdateWindow();
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
				 break;
			 }
		 }
	 case SB_LINEDOWN:
		 {
			 if ((scrollinfo.nMax - (int)(scrollinfo.nPos + scrollinfo.nPage)) <= 0)
			 {
				 break;
			 }
			 else if ((scrollinfo.nMax - (int)(scrollinfo.nPos   + scrollinfo.nPage)) >= 20)
			 {
				 scrollinfo.nPos += 20;
				 ScrollWindow(0,-20);
				 UpdateWindow();
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
				 break;
			 }
			 else if (scrollinfo.nMax - scrollinfo.nPos  - scrollinfo.nPage < 20)
			 {
				 ScrollWindow(0,-(scrollinfo.nMax - scrollinfo.nPos) );
				 UpdateWindow();
				 scrollinfo.nPos = scrollinfo.nMax;
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
				 break;
			 }
		 }
	 //case SB_PAGEUP: 
		// {	
		//	 m_bLineDown = TRUE;
		//	 UINT a = scrollinfo.nPos;
		//	 scrollinfo.nPos -= scrollinfo.nPage;				
		//	 if (scrollinfo.nPos<scrollinfo.nMin)
		//	 { 
		//		 scrollinfo.nPos = scrollinfo.nMin; 
		//		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
		//		 ScrollWindow(0,a); 
		//		 UpdateWindow();
		//		 break;
		//	 } 
		//	 else
		//	 {
		//		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		//		 ScrollWindow(0,scrollinfo.nPage); 
		//		 UpdateWindow();
		//		 break; 
		//	 }	  
		// }
	 //case SB_PAGEDOWN: 			
		// {	
		//	 m_bLineUp = TRUE;
		//	 UINT a = scrollinfo.nPos;
		//	 scrollinfo.nPos += scrollinfo.nPage ;  
		//	 if (scrollinfo.nPos>(int)(scrollinfo.nMax - scrollinfo.nPage)) 	
		//	 {   
		//		 int offset = 0;
		//		 offset =  scrollinfo.nMax - scrollinfo.nPos ;
		//		 scrollinfo.nPos = scrollinfo.nMax;
		//		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		//		 ScrollWindow(0,-offset); 
		//		 UpdateWindow();
		//		 break;  
		//	 }
		//	 else
		//	 {
		//		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		//		 ScrollWindow(0,-((int)scrollinfo.nPage));
		//		 UpdateWindow();
		//		 break; 
		//	 }	
		// }
	 case SB_ENDSCROLL: 
		 break; 
	 case SB_THUMBPOSITION:  
		 break;  
	 case SB_THUMBTRACK:  
		 {	
			 //校准
			 int offset = (int)nPos -(int)scrollinfo.nPos;
			 if (offset > 0)//向下移动
			 {
				 //if ( (nPos + scrollinfo.nPage) > scrollinfo.nMax )//越过最大值
				 //{
					// offset = (scrollinfo.nMax -scrollinfo.nPage + 1) - scrollinfo.nPos;
				 //}
				 ScrollWindow(0,-offset); 
				 UpdateWindow();
				 scrollinfo.nPos = scrollinfo.nPos + offset; 
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
			 }
			 else if (offset < 0)
			 {
				 //if (nPos < scrollinfo.nMin)
				 //{
					// offset = scrollinfo.nMin - scrollinfo.nPos;
				 //}
				 ScrollWindow(0,-offset); 
				 UpdateWindow();
				 scrollinfo.nPos = scrollinfo.nPos + offset; 
				 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
			 }			    
			 break; 
		 }
	 }
	 CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
 }

 void CWBFunctionAttributeDlg::ResetVScroll( UINT nSBCode)
 {
	 //把滚动条重新置顶
	 SCROLLINFO scrollinfo;
	 GetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
	 switch (nSBCode)  
	 {  
	 case SB_TOP:  
		 ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMin));
		 UpdateWindow();
		 scrollinfo.nPos = scrollinfo.nMin; 
		 SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		 break; 
	 default:
		 break;
	 }
 }


 void CWBFunctionAttributeDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
 {
	 // 取消滚动条的右键消息
 }
