// DlgPrintLoadingMsg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "DlgPrintLoadingMsg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CDlgPrintLoadingMsg 对话框

IMPLEMENT_DYNAMIC(CDlgPrintLoadingMsg, CDialog)

CDlgPrintLoadingMsg::CDlgPrintLoadingMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPrintLoadingMsg::IDD, pParent)
	,m_pParentWnd(pParent)
	,m_fPercent(0)
//	,m_nTimerID(0)
{
	 m_bitmapLoadingBK.SetBitmap("SKIN\\WhiteBoard\\loading_bk2.bmp");
}

CDlgPrintLoadingMsg::~CDlgPrintLoadingMsg()
{
}

void CDlgPrintLoadingMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_LONGING_DATA, m_ProgLoading);
}


BEGIN_MESSAGE_MAP(CDlgPrintLoadingMsg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_REDROW_PRINT_LOADING_MSG, &CDlgPrintLoadingMsg::OnRedrowPrintLoadingMsg)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgPrintLoadingMsg 消息处理程序


BOOL CDlgPrintLoadingMsg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HRGN hRgn = BitmapToRegion(m_bitmapLoadingBK.m_hBitMap,RGB(255,255, 255));// RGB(255,0, 255));

	if (hRgn)
	{
		::SetWindowRgn(m_hWnd, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
	RECT rect;
	m_pParentWnd->GetClientRect(&rect);
	SetWindowPos(&wndTop,(rect.right - rect.left)/2,(rect.bottom - rect.top)/2,0,0,SWP_NOSIZE);
	m_ProgLoading.SetRange(0,100);
	m_ProgLoading.SetPos(0);
// 	//显示动态GIF图像
// 	CString str = GetExePath();
// #ifdef _UNICODE
// 	wstring strGifPath = str;
// #else
// 	string strGifPath = str;
// #endif
// 	string strLoadingPicPath = "SKIN\\WhiteBoard\\loading.gif";
// 	strGifPath += strLoadingPicPath;
// 	if (_access(strGifPath.c_str(),0) == 0)
// 	{
// 		TCHAR strLanguageText[255]=_T("白板正在加载文件...");		
// 		SetMsg(strLanguageText);/*_T("白板正在加载文件...")*/
// 		return TRUE;  
// 	}
	return FALSE;
	// 异常: OCX 属性页应返回 FALSE
}


HRGN CDlgPrintLoadingMsg::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor)
{
	HRGN hRgn = NULL;
	HRGN hRgn2 = NULL;

	if (!hBmp) return NULL;

	//显示位图
	HDC		hDC = ::GetDC(NULL);
	if (hDC)
	{
		HDC		hMemDC = ::CreateCompatibleDC(hDC);
		HBITMAP	hOldBitmap;
		BITMAP	bm;	

		GetObject(hBmp, sizeof(bm), &bm);	
		hRgn = ::CreateRectRgn(0, 0, bm.bmWidth, bm.bmHeight);
		if (hMemDC && hRgn)
		{
			hOldBitmap		=	(HBITMAP)::SelectObject(hMemDC, hBmp);

			for (int y = 0; y < bm.bmHeight; ++y)
			{
				// Scan each bitmap pixel from left to right
				for (int x = 0; x < bm.bmWidth; ++x)
				{
					COLORREF clr1 = ::GetPixel(hMemDC, x, y);
					if (::GetPixel(hMemDC, x, y) == cTransparentColor)
					{
						hRgn2 = ::CreateRectRgn(x, y, x+1, y+1);
						CombineRgn(hRgn, hRgn, hRgn2, RGN_DIFF);
						DeleteObject(hRgn2);
					}
				}
			}
			::SelectObject(hMemDC, hOldBitmap);
			::DeleteObject(hMemDC);
		}
		::ReleaseDC(NULL, hDC);
	}

	return hRgn;
}
void CDlgPrintLoadingMsg::SetMsg(CString strMsg)
{
	GetDlgItem(IDC_STATIC_DATALOADING)->SetWindowText(strMsg);
	RedrawWindow();	
}


HBRUSH CDlgPrintLoadingMsg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr;
	if( nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject( NULL_BRUSH );
	}
	else
	{
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	return hbr;
}


BOOL CDlgPrintLoadingMsg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
}

BOOL CDlgPrintLoadingMsg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::DestroyWindow();
}


// BOOL CDlgPrintLoadingMsg::PreTranslateMessage(MSG* pMsg)
// {
// 	// TODO: 在此添加专用代码和/或调用基类
// 	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
// 	{
// 		return TRUE;
// 	}
// 	return CDialog::PreTranslateMessage(pMsg);
// }

// void CALLBACK CDlgPrintLoadingMsg::OnCallbackData( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime )
// {
// 	CDlgPrintLoadingMsg* pthis = (CDlgPrintLoadingMsg*)FromHandle(hwnd);
// 	if (pthis && (3 == idEvent))
// 	{
// 			pthis->_OnCallbackData();
// 	}
// }
// 
// void CDlgPrintLoadingMsg::_OnCallbackData()
// {
// 		UINT nTemp = GetTickCount();
// 		UINT nTime = (nTemp - m_nTimeTotal)/1000;
// 
// 		CString strTime;
// 		strTime.Format(_T("%02d:%02d"),nTime/60,nTime%60);
// 		m_staTime.SetWindowText(strTime);
// 		RedrawWindow();		
// }

 /*bool CDlgPrintLoadingMsg::InitCreateThread()
 {
 	if (m_hThread == NULL)
 	{
 		unsigned int nTID = 0;
		m_bExitThread = true;
 		m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadDrawTime, this, CREATE_SUSPENDED,&nTID);
 		if (!m_hThread)
 		{
 			TRACE(_T("Couldn't start a GIF animation thread\n"));
 			return true;
 		} 
 		else 
 			ResumeThread(m_hThread);
 	}
 	return true;
 }

 UINT WINAPI CDlgPrintLoadingMsg::_ThreadDrawTime( LPVOID pParam )
 {
	 ASSERT(pParam);
	 CDlgPrintLoadingMsg *pThis = reinterpret_cast<CDlgPrintLoadingMsg *> (pParam);
	 pThis->ThreadDrawTime();
	 return 0;
 }

 void CDlgPrintLoadingMsg::ThreadDrawTime()
 {
	 while(m_bExitThread)
	 {
		 Sleep(1000);
		 if (this)
		 {
			 if (IsWindow(this->m_hWnd))
			 {
				 Invalidate();
				 UpdateWindow();
				 continue;
			 }
		 }
		break;
	 }
	 }*/


afx_msg LRESULT CDlgPrintLoadingMsg::OnRedrowPrintLoadingMsg(WPARAM wParam, LPARAM lParam)
{
	m_fPercent = (float)wParam;
	m_fPercent = m_fPercent/100;
	m_ProgLoading.SetPos((int)m_fPercent);
	m_ProgLoading.Invalidate();
	SendMessage(WM_PAINT,0,0);
	return 0;
}


void CDlgPrintLoadingMsg::OnPaint()
{
 	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	m_bitmapLoadingBK.drawOnDcNXY(dc.m_hDC, rc.left, rc.top, rc.right, rc.bottom);
	m_ProgLoading.Invalidate();
	Graphics* graphics = Graphics::FromHDC(dc.m_hDC);
 //	FontFamily fontFamily(L"幼圆");
	FontFamily fontFamily(L"Trebuchet MS");
 	Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPoint);
 	PointF pointF(100,20);
 	LinearGradientBrush linGrBrush(
 		Point(30, 50),
 		Point(100, 50),
 		Color(255, 255, 0, 0), 
 		Color(255, 0, 0, 255)); 
	string strout = "";
	char c[20] ={0};
	sprintf_s(c,"%.2f%%",m_fPercent);
	strout = c;
	WCHAR* string = new WCHAR[strout.size()];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strout.c_str(), -1, string, strout.size() );
	graphics->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
 	graphics->DrawString(string, (INT)strout.size(), &font, pointF, &linGrBrush);

	pointF.X = 40;
	pointF.Y = 40; 
	Gdiplus::Font fontbk(&fontFamily, 10, FontStyleRegular, UnitPoint);

	CString strLoadTip = _T("");
	strLoadTip = ReadConfigText(_T("WhiteBoardDocConverText"), _T("WhiteBoardConving"));
	if (strLoadTip.IsEmpty())
	{
		strLoadTip = _T("白板正在加载文件...");
	}
	int nlen = strLoadTip.GetLength();
	WCHAR* stringbk = new WCHAR[nlen + 1];
	//WCHAR stringbk[]/* = L"白板正在加载文件..."*/;
	wcsncpy(stringbk,T2W((LPTSTR)(LPCTSTR)strLoadTip),strLoadTip.GetLength());
	stringbk[nlen] = '\0'; 
	graphics->DrawString(stringbk, (INT)wcslen(stringbk), &fontbk, pointF, &linGrBrush);
// 			CFont font;
// 			font.CreatePointFont(125,"Trebuchet MS",NULL);
// 			dc.SelectObject(&font);
// 			dc.SetBkColor(TRANSPARENT);
// 			dc.SetBkMode(TRANSPARENT);
// 			string strout = "";
// 			char c[20] ={0};
// 			sprintf(c,"%.2f%%",m_fPercent);
// 			strout = c;
// 			dc.TextOut(100,20,strout.c_str(),strout.size());
// 			strout = "白板正在加载文件...";
// 			dc.TextOut(60,40,strout.c_str(),strout.size());
			Invalidate();
			UpdateWindow();
			delete []string;
			delete graphics;
			delete []stringbk;
			graphics = NULL;
}
