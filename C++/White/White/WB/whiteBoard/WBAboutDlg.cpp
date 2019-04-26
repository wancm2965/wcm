// WBAboutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBAboutDlg.h"
#include "afxdialogex.h"


// CWBAboutDlg 对话框

IMPLEMENT_DYNAMIC(CWBAboutDlg, CDialogEx)

CWBAboutDlg::CWBAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBAboutDlg::IDD, pParent)
	,m_pBitmap(NULL)
{
	
}

CWBAboutDlg::~CWBAboutDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CWBAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBAboutDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWBAboutDlg 消息处理程序


void CWBAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnCancel();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CWBAboutDlg::SetSkinImage( CString strPath )
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
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
}


BOOL CWBAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	CRect ClientRC;
	int   cx,cy;//屏幕大小
	GetClientRect(&ClientRC);
	cx = GetSystemMetrics(SM_CXSCREEN);  
	cy = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow((cx - ClientRC.Width())/2,(cy - ClientRC.Height())/2,ClientRC.Width(),ClientRC.Height());
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	SetSkinImage(strPath + _T("\\AboutDlgBG.png"));

	// 获得窗口的客户区设备上下文句柄
	CPaintDC dc(this); // 更改当前字体
	LOGFONT lf;
	dc.GetCurrentFont()->GetLogFont(&lf);
	CFont font;
	CFont *pOldFont; // 保存设备上下文最初使用的字体对象
	lf.lfCharSet=10;
	lf.lfHeight=-15;
	lf.lfWidth=0;
	_tcscpy_s(lf.lfFaceName, _T("宋体"));
	font.CreateFontIndirect( &lf);
	pOldFont=dc.SelectObject( &font);
	dc.SetBkMode(TRANSPARENT); 
//		dc.BeginPath();    // 开始一个路径
	dc.SetTextColor(RGB(255,255,255));

	dc.TextOut(10, 10, L"Version 1.1.0.4554");


//  	dc.EndPath(); // 绘制路径
//  	dc.StrokePath();
	//可以用dc.StrokeAndFillPath()函数来代替，不过该函数会使用当前刷子填充路径的内部。
 	dc.SelectObject(pOldFont);
	return TRUE;  
}


BOOL CWBAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CDialogEx::OnEraseBkgnd(pDC);
}
