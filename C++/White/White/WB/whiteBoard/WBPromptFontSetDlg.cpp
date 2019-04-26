// WBPromptFontSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBPromptFontSetDlg.h"
#include "afxdialogex.h"
#include "ScreenDrawMgr.h"


// CWBPromptFontSetDlg 对话框

IMPLEMENT_DYNAMIC(CWBPromptFontSetDlg, CDialogEx)

	CWBPromptFontSetDlg::CWBPromptFontSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBPromptFontSetDlg::IDD, pParent)
{

}

CWBPromptFontSetDlg::~CWBPromptFontSetDlg()
{
}

void CWBPromptFontSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PROMPTFONTBOLDBTN,m_FontBoldBtn);
	DDX_Control(pDX,IDC_PROMPTFONTITALICBTN,m_FontItalicBtn);
	DDX_Control(pDX,IDC_PROMPTFONTUNDERLINEBTN,m_FontUnderlineBtn);
	DDX_Control(pDX,IDC_PROMPTFONTVERTICALBTN,m_FontVerticalBtn);
	DDX_Control(pDX,IDC_PROMPTFONTFACECOMBOX,m_FontFaceComBox);
	DDX_Control(pDX,IDC_PROMPTFONTSIZECOMBOX,m_FontSizeComBox);
	DDX_Text(pDX,IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize);
}


BEGIN_MESSAGE_MAP(CWBPromptFontSetDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_PROMPTFONTFACECOMBOX, &CWBPromptFontSetDlg::OnCbnSelchangePromptfontfacecombox)
	ON_CBN_SELCHANGE(IDC_PROMPTFONTSIZECOMBOX, &CWBPromptFontSetDlg::OnCbnSelchangePromptfontsizecombox)
	ON_BN_CLICKED(IDC_PROMPTFONTBOLDBTN, &CWBPromptFontSetDlg::OnBnClickedPromptfontboldbtn)
	ON_BN_CLICKED(IDC_PROMPTFONTITALICBTN, &CWBPromptFontSetDlg::OnBnClickedPromptfontitalicbtn)
	ON_BN_CLICKED(IDC_PROMPTFONTUNDERLINEBTN, &CWBPromptFontSetDlg::OnBnClickedPromptfontunderlinebtn)
	ON_BN_CLICKED(IDC_PROMPTFONTVERTICALBTN, &CWBPromptFontSetDlg::OnBnClickedPromptfontVerticalbtn)
	ON_CBN_EDITCHANGE(IDC_PROMPTFONTSIZECOMBOX, &CWBPromptFontSetDlg::OnCbnEditchangeWbFontsizecombox)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_TEXTARRIBUTE,&CWBPromptFontSetDlg::OnWM_TEXTARRIBUTE)
END_MESSAGE_MAP()


// CWBPromptFontSetDlg 消息处理程序
BOOL CWBPromptFontSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	//设置字号
	InsertStr(&m_FontSizeComBox,_T("字号"));
	//设置字体
	InsertStr(&m_FontFaceComBox,_T("字体"));
	m_FontSizeComBox.SetCurSel(12);
	m_FontFaceComBox.SetCurSel(16);
	m_FontBoldBtn.SetIsSelect(FALSE);
	m_FontItalicBtn.SetIsSelect(FALSE);
	m_FontUnderlineBtn.SetIsSelect(FALSE);
	m_FontVerticalBtn.SetIsSelect(FALSE);
	m_FontFaceComBox.MoveWindow(2,4,160,300);
	m_FontSizeComBox.MoveWindow(164,4,60,400);
	m_FontBoldBtn.MoveWindow(226,1,48,28);
	m_FontItalicBtn.MoveWindow(276,1,48,28);
	m_FontUnderlineBtn.MoveWindow(326,1,48,28);
	m_FontVerticalBtn.MoveWindow(376,1,48,28);
	return TRUE;
}

//装载字号下拉框列表
void CWBPromptFontSetDlg::InsertStr(CComboBoxEx *comBoxData,CString str)
{
	CString FontSizeString[37]={_T("8"),_T("9"),_T("10"),_T("11"),_T("12"),_T("14"),_T("16"),_T("18"),_T("20"),_T("22"),_T("24"),
		_T("26"),_T("28"),_T("36"),_T("48"),_T("72"),_T("100")};
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
			//to item position

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
			//to item position

			nItem = comBoxData->InsertItem(&cbi);
			//comBoxData->AddString(FontFaceString[i]);
		}
	}
}
void CWBPromptFontSetDlg::OnCbnSelchangePromptfontfacecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CFont font;
	m_FontSizeComBox.GetWindowText(str);
	if (str.IsEmpty())
	{
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		UpdateData(FALSE);
	}

	int i = m_FontFaceComBox.GetCurSel();
	m_FontFaceComBox.GetLBText(i,g_nWBCurObjFontFaceName);

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
	}

	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_FontFaceComBox.SetCurSel(i);
	}
	font.DeleteObject();
}


void CWBPromptFontSetDlg::OnCbnSelchangePromptfontsizecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	switch (int i = m_FontSizeComBox.GetCurSel())
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

	CFont font;
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
	}

	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
	}
	font.DeleteObject();
}


void CWBPromptFontSetDlg::OnBnClickedPromptfontboldbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CFont font;
	m_FontSizeComBox.GetWindowText(str);
	if (_tcscmp(str, _T("")) == 0)
	{
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		UpdateData(FALSE);
	}

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

	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontBoldBtn()->SetIsSelect(g_isBold);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontBoldBtn()->Invalidate();
		if (_tcscmp(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize, _T("")) == 0)
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
		}
	}

	m_FontBoldBtn.SetIsSelect(g_isBold);
	m_FontBoldBtn.Invalidate();
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
		{
			iFontWeight = FW_NORMAL;
		}

		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		g_pWhiteBoardView->SetCurTextFont();
	}
	g_pWhiteBoardView->GetCurEdit()->SetFocus();
	font.DeleteObject();
}


void CWBPromptFontSetDlg::OnBnClickedPromptfontitalicbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CFont font;
	m_FontSizeComBox.GetWindowText(str);
	if (_tcscmp(str, _T("")) == 0)
	{
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		UpdateData(FALSE);
	}

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
	
	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontItalicBtn()->SetIsSelect(g_isItalic);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontItalicBtn()->Invalidate();
		if (_tcscmp(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize, _T("")) == 0)
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
		}
	}

	m_FontItalicBtn.SetIsSelect(g_isItalic);
	m_FontItalicBtn.Invalidate();
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
		{
			iFontWeight = FW_NORMAL;
		}
		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		g_pWhiteBoardView->SetCurTextFont();
	}
	g_pWhiteBoardView->GetCurEdit()->SetFocus();
	font.DeleteObject();
}


void CWBPromptFontSetDlg::OnBnClickedPromptfontunderlinebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CFont font;
	m_FontSizeComBox.GetWindowText(str);
	if (_tcscmp(str, _T("")) == 0)
	{
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		UpdateData(FALSE);
	}

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
	
	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontUnderlineBtn()->SetIsSelect(g_isUnderline);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->GetFontUnderlineBtn()->Invalidate();
		if (_tcscmp(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize, _T("")) == 0)
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
		}
	}

	m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
	m_FontUnderlineBtn.Invalidate();
	if (NULL != g_pWhiteBoardView->GetCurEdit())
	{
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
		{
			iFontWeight = FW_NORMAL;
		}
		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		g_pWhiteBoardView->GetCurEdit()->SetFont(&font);

		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		g_pWhiteBoardView->SetCurTextFont();
	}
	g_pWhiteBoardView->GetCurEdit()->SetFocus();
	font.DeleteObject();
}

void CWBPromptFontSetDlg::OnBnClickedPromptfontVerticalbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	point = GetFontVerticalBtn()->GetMouseUpPoint();
	//m_FontVerticalBtn.GetIsSelect();
	if (point.x <= 0 || point.x >= 48 || point.y <= 0 || point.y >= 28)
	{
		return;
	}
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg() && NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
	{
		CString str;
		CComboBoxEx *pTempCom = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox();
		pTempCom->GetWindowText(str);
		if (_tcscmp(str, L"") == 0)
		{
			pTempCom->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
			g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
		}
		pTempCom = NULL;
	}
	if(!m_FontVerticalBtn.GetIsSelect())
	{
		//m_isSelect = TRUE;
		m_FontVerticalBtn.SetIsSelect(TRUE);
		g_bIsVertical = TRUE;
	}
	else
	{
		//m_isSelect = FALSE;
		m_FontVerticalBtn.SetIsSelect(FALSE);
		g_bIsVertical = FALSE;
	}

	//按恪?了?竖酣?排?输?出?按恪?钮￥后ó改?变?编括?辑-框ò
	if (g_pWhiteBoardView->GetCurEdit())
	{
		CWBRichEdit *pEdit = g_pWhiteBoardView->GetCurEdit();
		if (pEdit == NULL)
		{
			return;
		}
		CString str;
		pEdit->GetWindowText(str);

		CRect rectText = g_pWhiteBoardView->GetEditRect();

		CPoint pt;
		if (g_bIsVertical)//从洙?横á变?竖酣?
		{
			pt.x = rectText.right;
			pt.y = rectText.top;
		} 
		else//从洙?竖酣?变?横á
		{
			pt = rectText.TopLeft();
		}
		g_pWhiteBoardView->SetEditTopLeft(pt);

		if (g_pWhiteBoardView->GetIsEditing())//如?果?是?编括?辑-状痢?态?
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
			pText->SetIsVertical(g_bIsVertical);
			pText->SetCalRectText();
			delete pTextItem;
		}

		CWBRichEditMgr *ptempM = CWBRichEditMgr::GetInst_();
		ptempM->SetRichEditShow(false);
		ptempM->SetBeginPoint(rectText.left, rectText.top, MOUSE_ID);
		ptempM->SetEndPoint(rectText.right, rectText.bottom, MOUSE_ID);
		ptempM->SetRichEditShow(true);

		CWnd *pParent = NULL;
		if(NULL != CScreenDrawMgr::GetIns()) //如?果?是?在ú桌哩?面?绘?图?，?则ò将?文?本?框ò加ó载?CMouseDlg上?，?否?则ò加ó在úCWhiteBoardView上?
		{
			///设Θ?置?文?本?框ò的?父?窗洹?口ú,如?果?是?win7 vista以?上?则ò是?mousedlg,否?则ò为ascreendrawdlg
			CScreenDrawMgr::GetIns()->SetRichEditParent(&pParent);
		}
		else
		{
			pParent = g_pWhiteBoardView;
		}
		ptempM->SetIsSaveText(false);
		ptempM->ShowWBRichEdit(MOUSE_ID, g_bIsVertical, pParent, true);
		ptempM->SetIsSaveText(true);

		pEdit = g_pWhiteBoardView->GetCurEdit();
		//				pEdit->MoveWindow(&rectText);
		pEdit->SetWindowText(str);
		pEdit->SetFocus();
		(pEdit->GetRichEditCtrl()).LimitText(1024);
		(pEdit->GetRichEditCtrl()).SetSel(-1, -1);//光a标括?到?末?尾2

		//设Θ?置?文?字?字?体?样ù式?
		//CreateFontAndSetFont();
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
		{
			iFontWeight = FW_NORMAL;
		}
		CFont font;
		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);		
		pEdit->SetFont(&font);

		g_pWhiteBoardView->SetEditCharFormat(pEdit);
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
		{
			CWBFunctionAttributeDlg *pTemFunADlg = g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg();
			pTemFunADlg->GetFontVerticalBtn()->SetIsSelect(g_bIsVertical);
			pTemFunADlg->GetFontVerticalBtn()->Invalidate();
			if (_tcscmp(pTemFunADlg->m_strFontSize, L"") == 0)
			{
				pTemFunADlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
				pTemFunADlg->UpdateData(FALSE);
			}
			pTemFunADlg = NULL;
		}
		font.DeleteObject();
	}
}

void CWBPromptFontSetDlg::OnCbnEditchangeWbFontsizecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	int nWBCurObjFontSize = g_nWBCurObjFontSize;
	g_nWBCurObjFontSize = GetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX);

	CString str;
	CFont font1;
	CFont font2;
	CFont font3;
	m_FontSizeComBox.GetWindowText(str);
	if (_tcscmp(str, _T("")) == 0)
	{
		g_nWBCurObjFontSize = nWBCurObjFontSize;
		return;
	}

	if (g_nWBCurObjFontSize < 1)
	{
		m_FontSizeComBox.SetWindowText(_T("1"));
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,1,FALSE);
		g_nWBCurObjFontSize = 1;
		if (NULL != g_pWhiteBoardView->GetCurEdit())
		{
			int iFontWeight;
			if (g_isBold)
			{
				iFontWeight = FW_BOLD;
			}
			else
				iFontWeight = FW_NORMAL;
			
			font1.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
				, OUT_DEFAULT_PRECIS
				, CLIP_DEFAULT_PRECIS
				, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
			g_pWhiteBoardView->GetCurEdit()->SetFont(&font1);

			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

			g_pWhiteBoardView->SetCurTextFont();
		}
	}
	if (g_nWBCurObjFontSize > 100)
	{
		m_FontSizeComBox.SetWindowText(_T("100"));
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,100,FALSE);
		g_nWBCurObjFontSize = 100;
		if (NULL != g_pWhiteBoardView->GetCurEdit())
		{
			int iFontWeight;
			if (g_isBold)
			{
				iFontWeight = FW_BOLD;
			}
			else
				iFontWeight = FW_NORMAL;

			font2.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
				, OUT_DEFAULT_PRECIS
				, CLIP_DEFAULT_PRECIS
				, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
			g_pWhiteBoardView->GetCurEdit()->SetFont(&font2);

			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

			g_pWhiteBoardView->SetCurTextFont();
		}
	}
	if ((g_nWBCurObjFontSize >= 1) && (g_nWBCurObjFontSize <= 100))
	{
		m_FontSizeComBox.SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		if (NULL != g_pWhiteBoardView->GetCurEdit())
		{
			int iFontWeight;
			if (g_isBold)
			{
				iFontWeight = FW_BOLD;
			}
			else
				iFontWeight = FW_NORMAL;
			
			font3.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
				, OUT_DEFAULT_PRECIS
				, CLIP_DEFAULT_PRECIS
				, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
			g_pWhiteBoardView->GetCurEdit()->SetFont(&font3);

			g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

			g_pWhiteBoardView->SetCurTextFont();
		}
	}
	UpdateData(FALSE);
	
	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
	}

	font1.DeleteObject();
	font2.DeleteObject();
	font3.DeleteObject();

}

void CWBPromptFontSetDlg::OnOK()
{
	/*g_pWhiteBoardView->GetCurEdit()->*/
	::SetFocus(g_pWhiteBoardView->GetCurEdit()->m_hWnd);
	return;
}

void CWBPromptFontSetDlg::OnCancle()
{
	return;
}

BOOL CWBPromptFontSetDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CString CurPicPath;	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\promptFontSetDlgbg.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

LRESULT CWBPromptFontSetDlg::OnWM_TEXTARRIBUTE(WPARAM wParam, LPARAM lParam)
{
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
		m_FontVerticalBtn.SetIsSelect(TRUE);
	} 
	else
	{
		m_FontVerticalBtn.SetIsSelect(FALSE);
	}
	CString nFontSizeText;
	nFontSizeText.Format(_T("%d"),g_nWBCurObjFontSize);
	//m_FontSizeComBox.SetWindowText(nFontSizeText);
	int i = m_FontSizeComBox.FindStringExact(0,nFontSizeText);
	m_FontSizeComBox.SetCurSel(i);
	int j = m_FontFaceComBox.FindStringExact(0,g_nWBCurObjFontFaceName);	
	m_FontFaceComBox.SetCurSel(j);	
	return 1;
}

CComboBoxEx *CWBPromptFontSetDlg::GetFontFaceComBox()
{
	return &m_FontFaceComBox;
}

CComboBoxEx *CWBPromptFontSetDlg::GetFontSizeComBox()
{
	return &m_FontSizeComBox;
}

CWBFunctionBtn *CWBPromptFontSetDlg::GetFontBoldBtn()
{
	return &m_FontBoldBtn;
}
CWBFunctionBtn *CWBPromptFontSetDlg::GetFontItalicBtn()
{
	return &m_FontItalicBtn;
}
CWBFunctionBtn *CWBPromptFontSetDlg::GetFontUnderlineBtn()
{
	return &m_FontUnderlineBtn;
}
CWBFunctionBtn *CWBPromptFontSetDlg::GetFontVerticalBtn()
{
	return &m_FontVerticalBtn;
}
void CWBPromptFontSetDlg::CreateFontAndSetFont()
{
	CFont font;
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
	}
	font.DeleteObject();
}
