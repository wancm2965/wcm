rgbColors
// WBTextAttributeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBTextAttributeDlg.h"
#include "afxdialogex.h"


// CWBTextAttributeDlg 对话框

IMPLEMENT_DYNAMIC(CWBTextAttributeDlg, CDialogEx)

CWBTextAttributeDlg::CWBTextAttributeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBTextAttributeDlg::IDD, pParent)
{
	m_logFont.lfItalic=0;
	m_logFont.lfStrikeOut=0;
	m_logFont.lfUnderline=0;
	m_logFont.lfWeight=400;
	m_logFont.lfWidth=0;
	m_logFont.lfHeight=-13;	
	m_logFont.lfCharSet=134;
	m_logFont.lfEscapement = 0;
	m_logFont.lfOrientation = 0;
	_tcscpy_s(m_logFont.lfFaceName, _T(""));

	m_clrFont = RGB(0, 0, 0);

	m_strText = _T("");
}

CWBTextAttributeDlg::~CWBTextAttributeDlg()
{
}

void CWBTextAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);
	DDV_MaxChars(pDX, m_strText, 256);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_editFont);
}


BEGIN_MESSAGE_MAP(CWBTextAttributeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_TEXTFONT, &CWBTextAttributeDlg::OnBnClickedTextfont)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CWBTextAttributeDlg 消息处理程序

BOOL CWBTextAttributeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFont font;
	font.CreateFont(m_logFont.lfHeight
		, m_logFont.lfWidth, 0,0
		, m_logFont.lfWeight
		, m_logFont.lfItalic
		, m_logFont.lfUnderline
		, m_logFont.lfStrikeOut, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,m_logFont.lfFaceName);
	m_editFont.SetFont(&font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//修改字体
void CWBTextAttributeDlg::OnBnClickedTextfont()
{
	CFontDialog dlg(&m_logFont);
	dlg.m_cf.rgbColors = m_clrFont;

	if(IDOK == dlg.DoModal())
	{
		//m_pWhiteboard_view->SetTextFontColor(*(dlg.m_cf.lpLogFont), dlg.GetColor());
		memcpy((char *)&m_logFont, (char *)dlg.m_cf.lpLogFont, sizeof(LOGFONT));
		m_clrFont = dlg.GetColor();

		CFont font;
		font.CreateFont(m_logFont.lfHeight
			, m_logFont.lfWidth, 0,0
			, m_logFont.lfWeight
			, m_logFont.lfItalic
			, m_logFont.lfUnderline
			, m_logFont.lfStrikeOut, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,m_logFont.lfFaceName);
		m_editFont.SetFont(&font);

		font.DeleteObject();
	}
}


HBRUSH CWBTextAttributeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ 
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//改变编辑框颜色
	if(pWnd->GetDlgCtrlID()== IDC_EDIT_TEXT && nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetTextColor(m_clrFont);		
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
