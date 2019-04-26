// WVedio.cpp : 实现文件
//

#include "stdafx.h"
#include "WVedio.h"
#include "whiteBoard.h"
#include "afxdialogex.h"

#define MINWIDTHTRACK 200       //选区的最小宽度
#define MINHEIGHTTRACK 200      //选区的最小高度
#define MINANGLERECTWIDTH 60    //选区每个角上使光标变形的矩形宽度
#define OUTSIDEWIDTH 20
enum _TrackerHit
{
	hitNothing = -1,
	hitTopLeft = 0,
	hitTopRight = 1, 
	hitBottomRight = 2, 
	hitBottomLeft = 3,
	hitTop = 4, 
	hitRight = 5, 
	hitBottom = 6, 
	hitLeft = 7, 
	hitMiddle = 8
};
AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0,};
// CWVedio 对话框

IMPLEMENT_DYNAMIC(CWVedio, CDialog)

CWVedio::CWVedio(CWnd* pParent /*=NULL*/)
	: CDialog(CWVedio::IDD, pParent)
{
	/*--------------------------------------------------------------*/
	//初始化像皮筋类
	m_rectTracker.m_nStyle=CRectTracker::resizeInside|CRectTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
	/*--------------------------------------------------------------*/
	//m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR10); 
	 m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);  
	m_bDraw=FALSE;
	m_bFirstDraw=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
	g_hVedioHwnd = NULL;
	m_pBitmap = NULL;
	m_xScreen = 0;
	m_yScreen = 0;
	m_nScreenLeft = 0;
	m_nScreenTop = 0;
	//m_IsUp = TRUE;
	/*--------------------------------------------------------------*/
	//获取屏幕分辩率
// 	POINT pt;
// 	::GetCursorPos(&pt);
// 	MONITORINFO oMonitor = {}; 
// 	oMonitor.cbSize = sizeof(oMonitor);
// 	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
// 	CRect rcWork = oMonitor.rcWork;
// 	m_xScreen = rcWork.right;
// 	m_yScreen = rcWork.bottom;

	GetDisplayInfo();
	//截取屏幕到位图中
	CRect rect(m_nScreenLeft, m_nScreenTop,m_xScreen,m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));

	//一定要初始化m_rgn，
	m_rgn.CreateRectRgn(0,0,50,50);
	m_prePoint = CPoint(-1,-1);
	//m_bmpButton = NULL;
	m_tipRect = CRect(0,0,0,0);
	/*--------------------------------------------------------------*/
	//m_bIsClick = TRUE;

}

CWVedio::~CWVedio()
{
	m_rgn.DeleteObject();
}

void CWVedio::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWVedio, CDialog)
	//ON_BN_CLICKED(IDOK, &CWVedio::OnBnClickedOk)
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	//ON_WM_RBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


// CWVedio 消息处理程序

BOOL CWVedio::OnInitDialog()
{
	HINSTANCE hInst = AfxFindResourceHandle(
		MAKEINTRESOURCE(AFX_IDC_TRACK4WAY), RT_GROUP_CURSOR);
	_afxCursors[0] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACKNWSE));
	_afxCursors[1] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACKNESW));
	_afxCursors[2] = _afxCursors[0];
	_afxCursors[3] = _afxCursors[1];
	_afxCursors[4] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACKNS));
	_afxCursors[5] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACKWE));
	_afxCursors[6] = _afxCursors[4];
	_afxCursors[7] = _afxCursors[5];
	_afxCursors[8] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACK4WAY));
	_afxCursors[9] = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_MOVE4WAY));

	//CRect RightRectBtn(m_xScreen-54, m_yScreen-54, m_xScreen, m_yScreen);
	//m_bmpButton = new CAreaMedioBtn(this);
	//m_bmpButton->Create(_T("取消"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, RightRectBtn, this, BTN_CLOSE_ID);
	//m_bmpButton->SetButtonImage(_T("SpotlighyClose.png"));
	// 	把对化框设置成全屏顶层窗口
	/*------------------------------------------------------------------*/
	SetWindowPos(&wndTopMost,m_nScreenLeft,m_nScreenTop,m_xScreen-m_nScreenLeft,m_yScreen-m_nScreenTop,SWP_SHOWWINDOW);
	/*------------------------------------------------------------------*/  
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TOOLWINDOW);
	SetCapture();
	::SetCursor(m_hCursor);

	if (this->m_hWnd)
	{
		if (g_hVedioHwnd)
		{
			g_hVedioHwnd = NULL;
		}
		g_hVedioHwnd = this->m_hWnd;
	}
	return TRUE;
}

BOOL CWVedio::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int hitPos = GetHitPos();
	if (hitPos != hitNothing)
	{
		::SetCursor(_afxCursors[hitPos]);
		return TRUE;
	}
	else if(m_bFirstDraw == TRUE || m_bDraw == TRUE)//设置选择矩形存在时和正在画矩形时的光标形状
	{
		//设置彩色光标
		//SetCursor(m_hCursor);
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR4));
		return TRUE;
	}
	else//如果没有画矩形，并且没有矩形存在时，将光标设置为箭头
	{
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	}	
}

BOOL CWVedio::OnEraseBkgnd(CDC* pDC)
{
	if (m_pBitmap == NULL)
	{
		 return FALSE;
	}
	ASSERT(m_pBitmap);

	//创建兼容DC，选择位图
	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);
	dcCompatible.SelectObject(m_pBitmap);

	//将兼容DC中的位图复制到pDC中
	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);

	dcCompatible.DeleteDC();
	return TRUE;
}


void CWVedio::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	/*if(m_bShowMsg)
	{
		CRect rect;
		m_rectTracker.GetTrueRect(&rect);
		DrawMessage(rect);
	}*/
	
	//画提示(1)初始状态，没有开始画矩形；（2）开始画矩形（3）画完矩形
	if (FALSE == m_bFirstDraw && FALSE == m_bDraw )
	{
		CString strSelectRECRegion;
		strSelectRECRegion = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SelectRECRegion"));
		if (strSelectRECRegion.IsEmpty())
		{
			strSelectRECRegion = _T("请选择录像区域：在区域左上角按下鼠标左键拖动至右下角；单击右键退出录像");
		}
		DrawTip(strSelectRECRegion);
	}
	else if (TRUE == m_bDraw)
	{
		CRect rect;
		m_rectTracker.GetTrueRect(&rect);
		rect.left -=m_nScreenLeft;
		rect.top -= m_nScreenTop;
		rect.right -= m_nScreenLeft;
		rect.bottom -= m_nScreenTop;
		DrawMessage(rect);
	}
	else if (TRUE == m_bFirstDraw)
	{
		CString strDoubleClick;
		strDoubleClick = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("DoubleClick"));
		if (strDoubleClick.IsEmpty())
		{
			strDoubleClick = _T("双击区域开始录像；拖动区域调整大小位置；单击右键取消区域");
		}
		DrawTip(strDoubleClick);
	}

	////画出像皮筋矩形
	if(m_bFirstDraw == TRUE || m_bDraw == TRUE)
	{
		m_rectTracker.Draw(&dc);
	}

	CDialog::OnPaint();
}

/*
当左键按下时有几种状态
1、开始选择区域
2、移动已有区域
*/
void CWVedio::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
// 	TCHAR szname[512];
// 	::GetClassName(m_hWnd,szname,512);
// 	CString STR;
// 	STR.Format(_T("%s",szname));
// 	OutputDebugString(STR);

	m_bShowMsg=TRUE;
	m_prePoint = point;
	if (FALSE == m_bFirstDraw)//没有选择区域
	{
		m_startPt = point;
		m_bDraw = TRUE;     //开始选择区域
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CWVedio::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SetCapture();
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR4));	
	if(m_bDraw && !m_bFirstDraw)//当前还没有存在矩形区域，正在画区域
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
		m_rectTracker.m_rect.SetRect(m_startPt.x+1,m_startPt.y+1,point.x,point.y);
		PaintWindow();
		return;
	}
	if (m_bFirstDraw && m_bShowMsg)//区域已经存在,并且鼠标左键按下
	{
		CPoint offset;
		offset = point - m_prePoint;
		m_prePoint = point;

		
		CPoint tmpStartPt(m_startPt);
		CPoint tmpEndPt(m_endPt);

		int hitPos = GetHitPos();
		switch(hitPos)
		{
		case hitTopLeft:
			{
				tmpStartPt += offset;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
				}
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpStartPt.y = tmpEndPt.y - MINHEIGHTTRACK;
				}
				m_startPt = tmpStartPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitTop:
			{
				tmpStartPt.y += offset.y;
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpStartPt.y = tmpEndPt.y - MINHEIGHTTRACK;
				}
				m_startPt = tmpStartPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitTopRight:
			{
				tmpStartPt.y += offset.y;
				tmpEndPt.x += offset.x;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					//tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
					tmpEndPt.x = tmpStartPt.x + MINWIDTHTRACK;
				}
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpStartPt.y = tmpEndPt.y - MINHEIGHTTRACK;
				}
				m_startPt = tmpStartPt;
				m_endPt = tmpEndPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitRight:
			{
				//m_endPt.x += offset.x;
				tmpEndPt.x += offset.x;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					tmpEndPt.x = tmpStartPt.x + MINWIDTHTRACK;
				}
				m_endPt = tmpEndPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitBottomRight:
			{
				//m_endPt += offset;
				tmpEndPt += offset;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					tmpEndPt.x = tmpStartPt.x + MINWIDTHTRACK;
				}
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpEndPt.y = tmpStartPt.y + MINHEIGHTTRACK;
				}
				m_endPt = tmpEndPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitBottom:
			{
				//m_endPt.y += offset.y;
				tmpEndPt.y += offset.y;
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpEndPt.y = tmpStartPt.y + MINHEIGHTTRACK;
				}
				m_endPt = tmpEndPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitBottomLeft:
			{
				//m_startPt.x += offset.x;
				//m_endPt.y += offset.y;
				tmpStartPt.x += offset.x;
				tmpEndPt.y += offset.y;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					//tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
					tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
				}
				if ((tmpEndPt.y - tmpStartPt.y) < MINHEIGHTTRACK)
				{
					tmpEndPt.y = tmpStartPt.y + MINHEIGHTTRACK;
				}
				m_startPt = tmpStartPt;
				m_endPt = tmpEndPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitLeft:
			{
				//m_startPt.x += offset.x;
				tmpStartPt.x += offset.x;
				if ((tmpEndPt.x - tmpStartPt.x) < MINWIDTHTRACK)
				{
					//tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
					tmpStartPt.x = tmpEndPt.x - MINWIDTHTRACK;
				}
				m_startPt = tmpStartPt;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
			}
			break;
		case hitMiddle:
			{
				m_startPt += offset;
				m_endPt += offset;
				m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);

				//msg.Format(_T("hitMiddle: m_startPt(%d,%d) m_endPt(%d,%d)\r\n"), m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
				//OutputDebugString(msg);
			}
			break;
		}
		PaintWindow();
		return;
	}
	//刷新提示区域
	m_tipRect.InflateRect(+50,+50);
	if (::IsWindow(m_hWnd))
	{
		InvalidateRect(m_tipRect);
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CWVedio::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bShowMsg=FALSE;
	if (TRUE == m_bDraw)
	{
		m_bDraw = FALSE;
		m_endPt = point;
		//判断选择区域的大小，最小要为200*200
		//获取选择区域坐标
		CRect rectPos;
		m_rectTracker.GetTrueRect(&rectPos);

		if (m_startPt == m_endPt)//假如只是单击窗口
		{
			CPoint cursorPos;
			GetCursorPos(&cursorPos);
			rectPos.left = cursorPos.x;
			rectPos.top = cursorPos.y;
			rectPos.right = rectPos.left + MINWIDTHTRACK;
			rectPos.bottom = rectPos.top + MINHEIGHTTRACK;
		}
		else//拖动鼠标选择了区域
		{
			if (rectPos.Width() < MINWIDTHTRACK)
			{
				rectPos.right = rectPos.left + MINWIDTHTRACK;
			}
			if (rectPos.Height() < MINHEIGHTTRACK)
			{
				rectPos.bottom = rectPos.top + MINHEIGHTTRACK;
			}
		}
		
		m_startPt = rectPos.TopLeft();
		m_endPt = rectPos.BottomRight();
		m_rectTracker.m_rect.SetRect(m_startPt,m_endPt);
		m_bFirstDraw = TRUE;//表示选择框已经存在
	
		//EndDialog(100); 
		PaintWindow();
	}
	if (g_bMedioScreenWndEnd == FALSE)
	{
		
		EndDialog(100);
	}
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

void CWVedio::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ( FALSE == m_bFirstDraw)//假如没有选择区域
	{
		return;
	}

	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);
	if(nHitTest==8)
	{
		//CopyScreenToBitmap(m_rectTracker.m_rect,TRUE);
		//CString str;
		//m_pStart = m_startPt;
		//m_pEnd  = m_endPt;
		//判断选择区域的最终大小
		CRect trueRect;
		m_rectTracker.GetTrueRect(&trueRect);//后去矫正后的坐标
		if (trueRect.left < 0)
		{
			trueRect.left = 0;
		}
		if (trueRect.top < 0)
		{
			trueRect.top = 0;
		}
		if (trueRect.right > m_xScreen)
		{
			trueRect.right = m_xScreen;
		}
		if (trueRect.bottom > m_yScreen)
		{
			trueRect.bottom = m_yScreen;
		}
		m_startPt = trueRect.TopLeft();
		m_endPt = trueRect.BottomRight();

		//关闭模态对话框，100为DoModal()函数的返回值
		ReleaseCapture();
		EndDialog(100); 
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CWVedio::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CancelFunction();
	CDialog::OnRButtonDown(nFlags, point);
}

void CWVedio::CancelFunction()
{
	if(m_bFirstDraw)
	{
		//如果已经截取矩则清除截取矩形
		m_bFirstDraw=FALSE;
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	//else
	//{
		//ReleaseCapture();
		//关闭程序
		EndDialog(2);
	//}
}

HBITMAP CWVedio::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)  //考贝桌面到位图 
{
	HDC hScrDC, hMemDC;      
	// 屏幕和内存设备描述表
	HBITMAP    hBitmap, hOldBitmap;   
	// 位图句柄
	int       nX, nY, nX2, nY2;      
	// 选定区域坐标
	int       nWidth, nHeight;

	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//确保选定区域是可见的
// 	if (nX < 0)
// 		nX = 0;
// 	if (nY < 0)
// 		nY = 0;
	if (nX2 > m_xScreen)
		nX2 = m_xScreen;
	if (nY2 > m_yScreen)
		nY2 = m_yScreen;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap
		(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	if(bSave)
	{
		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);

		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			dcCompatible, nX-m_nScreenLeft, nY-m_nScreenTop, SRCCOPY);

	}
	else
	{
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			hScrDC, nX, nY, SRCCOPY);
	}

	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//得到屏幕位图的句柄
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	if(bSave)
	{

		if (OpenClipboard()) 
		{
			//清空剪贴板
			EmptyClipboard();
			//把屏幕内容粘贴到剪贴板上,
			//hBitmap 为刚才的屏幕位图句柄
			SetClipboardData(CF_BITMAP, hBitmap);
			//关闭剪贴板
			CloseClipboard();
		}
	}
	return hBitmap;
}

void CWVedio::DrawMessage(CRect &inRect)      //显示截取矩形信息
{
	//截取矩形大小信息离鼠标间隔
	const int space = 3;
	//设置字体颜色大小
	CClientDC dc(this);
	CPoint pt;
	//CPen pen(PS_SOLID,1,RGB(147,147,147));
	dc.SetTextColor(RGB(0,0,0));
	CFont font;
	font.CreatePointFont(150,_T("楷体"));
	dc.SelectObject(&font);

	//得到字体宽度和高度
	GetCursorPos(&pt);
	pt -=CPoint(m_nScreenLeft,m_nScreenTop);
	dc.SetBkMode(TRANSPARENT);
	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	CSize sizePos;
	CSize sizeCursor;
	int	lineLength;
	dc.GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;
	CString strTopPosition;
	strTopPosition = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("TopPosition"));
	if (strTopPosition.IsEmpty())
	{
		strTopPosition = _T("顶点位置");
	}
	sizePos = dc.GetTextExtent(strTopPosition,lstrlen(strTopPosition));
	CString strCursor;
	strCursor = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Coordinate"));
	if (strCursor.IsEmpty())
	{
		strCursor = _T("光标坐标");
	}
	sizeCursor = dc.GetTextExtent(strCursor,lstrlen(strCursor));
	//取一行中字符最多的作为大小判定
	if (sizePos.cx > sizeCursor.cx)
	{
		size = sizePos;
	}
	else
	{
		size = sizeCursor;
	}
	lineLength=size.cx + 25;
	//初始化矩形, 以保证写下六行文字
	CRect rect(pt.x+space,pt.y-charHeight*6-space,pt.x+lineLength+space,pt.y-space);

	int x = GetDeviceCaps(dc, HORZRES);
	int y = GetDeviceCaps(dc, VERTRES);

	//创建临时矩形
	CRect rectTemp;
	//当矩形到达桌面边缘时调整方向和大小
	if((pt.x+rect.Width())>=x)
	{
		//桌面上方显示不下矩形
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((pt.y-rect.Height())<=0)
	{
		//桌面右方显示不下矩形
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
	}

	//创建空画刷画矩形
	dc.SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	dc.Rectangle(rect);
	rect.top+=2;
	//在矩形中显示文字
	CString string(strTopPosition);
	dc.TextOut(rect.left,rect.top,string);

	string.Format(_T("(%d,%d)"),inRect.left,inRect.top);
	dc.TextOut(rect.left,rect.top+charHeight,string);

	CString strRectSize;
	strRectSize = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("RectSize"));
	if (strRectSize.IsEmpty())
	{
		strRectSize = _T("矩形大小");
	}
	string=strRectSize;
	dc.TextOut(rect.left,rect.top+charHeight*2,string);

	string.Format(_T("(%d,%d)"),inRect.Width(),inRect.Height());
	dc.TextOut(rect.left,rect.top+charHeight*3,string);

	CString strCoordinate;
	strCoordinate = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Coordinate"));
	if (strCoordinate.IsEmpty())
	{
		strCoordinate = _T("光标坐标");
	}
	string=strCoordinate;
	dc.TextOut(rect.left,rect.top+charHeight*4,string);

	m_endPt = pt;
	string.Format(_T("(%d,%d)"),m_endPt.x,m_endPt.y);
	dc.TextOut(rect.left,rect.top+charHeight*5,string);

	font.DeleteObject();
}

void CWVedio::DrawTip(CString tipStr)
{
	//截取矩形大小信息离鼠标间隔
	const int space = 3;
	//设置字体颜色大小
	CClientDC dc(this);
	CPoint pt;
	//CPen pen(PS_SOLID,1,RGB(147,147,147));
	dc.SetTextColor(RGB(0,0,0));
	CFont font;
	font.CreatePointFont(150,_T("楷体"));
	dc.SelectObject(&font);

	//得到字体宽度和高度
	GetCursorPos(&pt);
	pt -=CPoint(m_nScreenLeft,m_nScreenTop);
	dc.SetBkMode(TRANSPARENT);
	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	CSize strSize;
	int	lineLength = 0;
	dc.GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;

	int nRow = 6;
	//一行显示6个汉子
	strSize=dc.GetTextExtent(tipStr);
	size=dc.GetTextExtent(_T("顶顶顶顶顶顶"),lstrlen(_T("顶顶顶顶顶顶")));
	CString strTopPosition = _T("");
	strTopPosition = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("DrawTipEn"));
	if (strTopPosition.IsEmpty())
	{
		lineLength=size.cx;
		nRow = strSize.cx/size.cx + 1;
	}
	else
	{
		int a = strSize.cx;
		nRow = a/128;
		lineLength=size.cx + 30;
	}
	

	//初始化矩形, 以保证写下六行文字
	CRect rect(pt.x+space,pt.y-charHeight*nRow-space,pt.x+lineLength+space,pt.y-space);
	
	int x = GetDeviceCaps(dc, HORZRES);
	int y = GetDeviceCaps(dc, VERTRES);

	//创建临时矩形
	CRect rectTemp;
	//当矩形到达桌面边缘时调整方向和大小
	if((pt.x+rect.Width())>=x)
	{
		//桌面上方显示不下矩形
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((pt.y-rect.Height())<=0)
	{
		//桌面右方显示不下矩形
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
	}
	m_tipRect = rect;

	//创建空画刷画矩形
	dc.SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	dc.Rectangle(rect);
	
	//在目标矩形内画字，左对齐并且自动断字
	dc.DrawText(tipStr,rect, DT_LEFT|DT_WORDBREAK);

	font.DeleteObject();

}
void CWVedio::PaintWindow()              //重画窗口 
{
	if (::IsWindow(m_hWnd))
	{
		Invalidate();
	}
	
}

CPoint CWVedio::GetPoint(CPoint& Point1,CPoint& Point2)
{
	//Point1 = m_startPt;
	//Point2 = m_endPt;
	CRect trueRect;
	m_rectTracker.GetTrueRect(&trueRect);
	Point1 = trueRect.TopLeft() + CPoint(m_nScreenLeft,m_nScreenTop);
	Point2 = trueRect.BottomRight() +CPoint(m_nScreenLeft,m_nScreenTop);
	return Point2,Point1;
}

//获取鼠标点击位置
BOOL CWVedio::GetHitPos()
{
	//假如没有画矩形框，那么直接返回
	if (!m_bFirstDraw)
	{
		return hitNothing;
	}

	//首先计算矩形区域8个区域的坐标,选区最小为200*200，每个角矩形的大小设定为60*60
	CRect trackRect;
	m_rectTracker.GetTrueRect(&trackRect);
	trackRect.InflateRect(+OUTSIDEWIDTH,+OUTSIDEWIDTH);//超出边框的距离

	CRect topLeft,top,topRight,right,bottomRight, bottom, bottomLeft,left;
	topLeft.left = trackRect.left;
	topLeft.top = trackRect.top;
	topLeft.right = topLeft.left + MINANGLERECTWIDTH;
	topLeft.bottom = topLeft.top + MINANGLERECTWIDTH;

	topRight.left = trackRect.right - MINANGLERECTWIDTH;
	topRight.top = topLeft.top;
	topRight.right = trackRect.right;
	topRight.bottom = topRight.top + MINANGLERECTWIDTH;

	top.left = topLeft.right;
	top.top = topLeft.top;
	top.right = topRight.left;
	top.bottom = topRight.bottom;

	bottomRight.left = trackRect.right - MINANGLERECTWIDTH;
	bottomRight.top = trackRect.bottom - MINANGLERECTWIDTH;
	bottomRight.right = trackRect.right;
	bottomRight.bottom = trackRect.bottom;

	right.left = topRight.left;
	right.right = trackRect.right;
	right.top = topRight.bottom;
	right.bottom = bottomRight.top;

	bottomLeft.left = trackRect.left;
	bottomLeft.top = trackRect.bottom - MINANGLERECTWIDTH;
	bottomLeft.right = trackRect.left + MINANGLERECTWIDTH;
	bottomLeft.bottom = trackRect.bottom;

	bottom.left = bottomLeft.right;
	bottom.top = bottomLeft.top;
	bottom.right = bottomRight.left;
	bottom.bottom = bottomLeft.bottom;

	left.left = trackRect.left;
	left.top = topLeft.bottom;
	left.right = topLeft.right;
	left.bottom = bottomLeft.top;

	CPoint point;
	GetCursorPos(&point);
	this->ScreenToClient(&point);

	if (!trackRect.PtInRect(point))
	{
		return hitNothing; 
	}
	if (topLeft.PtInRect(point))
	{
		return hitTopLeft;
	}
	if (top.PtInRect(point))
	{
		return hitTop;
	}
	if (topRight.PtInRect(point))
	{
		return hitTopRight;
	}
	if (right.PtInRect(point))
	{
		return hitRight;
	}
	if (bottomRight.PtInRect(point))
	{
		return hitBottomRight;
	}
	if (bottom.PtInRect(point))
	{
		return hitBottom;
	}
	if (bottomLeft.PtInRect(point))
	{
		return hitBottomLeft;
	}
	if (left.PtInRect(point))
	{
		return hitLeft;
	}
	return hitMiddle;
}





void CWVedio::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnMouseHover(nFlags, point);
}


void CWVedio::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnMouseLeave();
}

void CWVedio::GetDisplayInfo()
{
	int nDispDivIndex = 0;
	int index = 0;
	DEVMODE devmode;
	DISPLAY_DEVICE dev = {sizeof(DISPLAY_DEVICE)};
	BOOL bNext = FALSE;

	while (true)
	{
		bNext = EnumDisplayDevices(NULL,index,&dev,0);
		if (bNext == FALSE)
		{
			break;
		}

		EnumDisplaySettings(dev.DeviceName,ENUM_CURRENT_SETTINGS, &devmode);
		int x = devmode.dmPosition.x;
		int y = devmode.dmPosition.y;
		DWORD cx = devmode.dmPelsWidth + x;
		DWORD cy = devmode.dmPelsHeight + y;
		if (m_nScreenLeft >= x && x > -10000)
		{
			m_nScreenLeft = x;
		}
		if (m_nScreenTop >= y && y> -10000)
		{
			m_nScreenTop = y;
		}
		if (m_xScreen <= cx  && cx > 0 && cx < 10000)
		{
			m_xScreen =cx;
		}
		if (m_yScreen <= cy && cy >0 && cy < 10000)
		{
			m_yScreen = cy;
		}
		index++;

	}
}
