

#include "stdafx.h"

#include "CatchScreenDlg.h"
//#include "common/XYIniReader.h"
#include <atlimage.h>
#include "whiteBoard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCatchScreenDlg dialog

CCatchScreenDlg::CCatchScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCatchScreenDlg::IDD, pParent)
{
    //初始化像皮筋类,新增的resizeMiddle 类型
	m_rectTracker.m_nStyle=CMyTracker::resizeMiddle|CMyTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
	//设置矩形颜色
	m_rectTracker.SetRectColor(RGB(10,100,130));
	//设置矩形调整时光标,默认的太小了,弄了个大点的
	m_rectTracker.SetResizeCursor(IDC_CURSOR6,IDC_CURSOR5,IDC_CURSOR2,IDC_CURSOR3,IDC_CURSOR4);

	//光标
    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);  
    	
	m_bDraw=FALSE;
	m_bFirstDraw=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
    m_startPt=0;
    m_hIcon = NULL;
	m_xScreen = 0;
	m_yScreen = 0;
	m_nscreenLeft = 0;
	m_nscreenTop = 0;
	m_ptEditLT = CPoint(0,0);	
	//获取屏幕分辩率
// 	POINT pt;
// 	::GetCursorPos(&pt);
// 	MONITORINFO oMonitor = {}; 
// 	oMonitor.cbSize = sizeof(oMonitor);
// 	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
// 	CRect rcWork = oMonitor.rcWork;
	
	
	//m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	//m_yScreen = GetSystemMetrics(SM_CYSCREEN);
// 	m_xScreen = rcWork.right;
// 	m_yScreen = rcWork.bottom;
	GetDisplayInfo();
	m_ptEditLT = CPoint(m_nscreenLeft,m_nscreenTop);
	//截取屏幕到位图中
	CRect rect(m_nscreenLeft, m_nscreenTop,m_xScreen,m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));
    
	//初始化刷新窗口区域 m_rgn
    m_rgn.CreateRectRgn(m_nscreenLeft,m_nscreenTop,50,50);	
}

CCatchScreenDlg::~CCatchScreenDlg()
{
	if(m_pBitmap)
	{
		m_pBitmap->DeleteObject();
	}
	m_rgn.DeleteObject();
}

void CCatchScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tipEdit);
}

BEGIN_MESSAGE_MAP(CCatchScreenDlg, CDialog)
	//{{AFX_MSG_MAP(CCatchScreenDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
	ON_STN_CLICKED(IDC_EDIT1, &CCatchScreenDlg::OnStnClickedEdit1)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCatchScreenDlg message handlers

BOOL CCatchScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//**************************************************************************
	//把对化框设置成全屏顶层窗口
	SetWindowPos(&wndTopMost,m_nscreenLeft,m_nscreenTop,m_xScreen-m_nscreenLeft,m_yScreen-m_nscreenTop,SWP_SHOWWINDOW);
    
	//移动操作提示窗口
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
	m_tipEdit.SetWindowLT(m_ptEditLT);
	int width = GetSystemMetrics(SM_CXSCREEN);
	m_tipEdit.SetScreenWidth(width);
    //m_tipEdit.MoveWindow(m_ptEditLT.x+10,m_ptEditLT.y+10,rect.Width(),rect.Height());

	//显示操作提示窗口文字
	DrawTip();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TOOLWINDOW);
	//捕获按键消息窗口,将对话框的句柄传递到CCatchScreenApp中
//	((CCatchScreenApp *)AfxGetApp())->m_hwndDlg=m_hWnd;
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//this->SetFocus();
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


void CCatchScreenDlg::OnPaint() 
{
	if (IsIconic())//假如窗口是最小化的
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
//**************************************************************************
		CPaintDC dc(this);
         
		//显示截取矩形大小信息
		if(m_bShowMsg&&m_bFirstDraw)
		{
			//得到当前矩形大小
			CRect rect;
			m_rectTracker.GetTrueRect(&rect);
			rect.left -= m_ptEditLT.x;
			rect.top -= m_ptEditLT.y;
			rect.right -= m_ptEditLT.x;
			rect.bottom -= m_ptEditLT.y;
			//传递CPaintDC 是为了不在了窗口上画信息
			DrawMessage(rect,&dc);
		}

		//画出像皮筋矩形
		if(m_bFirstDraw)
		{
			m_rectTracker.Draw(&dc);
		}

//*************************************************************************
		CDialog::OnPaint();
	}
}

HCURSOR CCatchScreenDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCatchScreenDlg::OnOK() 
{
	//将光标形状恢复为箭头
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
	CDialog::OnOK();
}

void CCatchScreenDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
//***************************************************************
	//将光标形状恢复为箭头
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
	if(m_bFirstDraw)
	{
		//取消已画矩形变量
		m_bFirstDraw=FALSE;
		m_bDraw=FALSE;
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
//*******************************************************************
}

void CCatchScreenDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	   if(m_bDraw)
	   {
			//动态调整矩形大小,并刷新画出
		    point = point - CPoint(18,9)/* + m_ptEditLT*/;
		    m_rectTracker.m_rect.SetRect(m_startPt.x+4,m_startPt.y+4,point.x,point.y);
			PaintWindow();
	   }
	   
	   //弥补调整大小和位置时,接收不到MouseMove消息
	   CRect rect;
	   m_tipEdit.GetWindowRect(&rect);
	   if(rect.PtInRect(point))
	   m_tipEdit.SendMessage(WM_MOUSEMOVE);
       
	   ChangeRGB();
	   CDialog::OnMouseMove(nFlags, point);
}

void CCatchScreenDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //判断击中位置
	if(nHitTest<0)
	{
		if(!m_bFirstDraw)
		{
			//第一次画矩形
			point = point - CPoint(18,9)/* + m_ptEditLT*/;
			m_startPt = point;
			m_bDraw=TRUE;
			m_bFirstDraw=TRUE;
			//设置当鼠标按下时最小的矩形大小
			m_rectTracker.m_rect.SetRect(point.x,point.y,point.x + 4,point.y + 4);	
			
			//保证当鼠标当下时立刻显示信息
			m_bShowMsg=TRUE;		
			DrawTip();
			PaintWindow();
		}
	}
	else
	{
		//保证当鼠标当下时立刻显示信息
		m_bShowMsg=TRUE;		
		PaintWindow();
		
		if(m_bFirstDraw)
		{
			//调束大小时,Track会自动调整矩形大小,在些期间,消息归CRectTracker内部处理
			m_rectTracker.Track(this,point + m_ptEditLT,TRUE);
			//SendMessage(WM_LBUTTONUP,NULL,NULL);
			PaintWindow();

		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CCatchScreenDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
    
	m_bShowMsg=FALSE;
	m_bDraw=FALSE;
	DrawTip();
	PaintWindow();

	//调整选择矩形，使其为从左上到右下的坐标
	int nTemp;
	if (m_rectTracker.m_rect.left > m_rectTracker.m_rect.right)
	{
		nTemp = m_rectTracker.m_rect.left;
		m_rectTracker.m_rect.left = m_rectTracker.m_rect.right;
		m_rectTracker.m_rect.right = nTemp;
	}
	if (m_rectTracker.m_rect.bottom < m_rectTracker.m_rect.top)
	{
		nTemp = m_rectTracker.m_rect.top;
		m_rectTracker.m_rect.top = m_rectTracker.m_rect.bottom;
		m_rectTracker.m_rect.bottom = nTemp;
	}
	CRect rectDraw ;
	rectDraw = m_rectTracker.m_rect + m_ptEditLT;
	
	CopyScreenToBitmap(rectDraw,TRUE);
	OnOK();
	CDialog::OnLButtonUp(nFlags, point);
}

void CCatchScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //如果在是矩形内部双击
	if(nHitTest==8)      
	{
		//保存位图到粘贴板中,bSave 为TRUE,
		CopyScreenToBitmap(m_rectTracker.m_rect,TRUE);
	    OnOK();
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CCatchScreenDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	if(m_bFirstDraw)
	{
		//如果已经截取矩则清除截取矩形
		m_bFirstDraw=FALSE;
		//清除矩形大小
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		DrawTip();
		PaintWindow();
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
		EndDialog(100);//wangxiaobin先关闭模态框
	}
	else
	{
	   OnCancel();
	   EndDialog(100);
	}

	CDialog::OnRButtonUp(nFlags, point);
}

HBRUSH CCatchScreenDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//设置操作提示窗口文本颜色
	/*if(pWnd->GetDlgCtrlID()==IDC_EDIT1)
	{
	pDC->SetTextColor(RGB(255,255,255));
	}*/
	return hbr;
}

BOOL CCatchScreenDlg::OnEraseBkgnd(CDC* pDC) 
{
	//用整个桌面填充全屏对话框背景
	BITMAP bmp;
	m_pBitmap->GetBitmap(&bmp);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	dcCompatible.SelectObject(m_pBitmap);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);

	return TRUE;
}

BOOL CCatchScreenDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	
	//设置改变截取矩形大小时光标
	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
		             &&!m_bDraw &&m_bFirstDraw) //此处判断保截取时当标始中为彩色光标
    {
		return TRUE; 
	}

	//设置彩色光标
	SetCursor(m_hCursor);
	return TRUE;
}

//*********************增加的函数**********************************************************
//考贝屏幕,这段代码是拿来主义 呵呵
HBITMAP CCatchScreenDlg::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)
{                                                                          //lpRect 代表选定区域
	HDC       hScrDC, hMemDC;       // 屏幕和内存设备描述表
	
	HBITMAP    hBitmap, hOldBitmap;    // 位图句柄
	
	int       nX, nY, nX2, nY2;       // 选定区域坐标
	
	int       nWidth, nHeight;
	
	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top ;
	nX2 = lpRect->right ;
	nY2 = lpRect->bottom ;

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
		//创建军兼容DC,当bSave为中时把开始保存的全屏位图,按截取矩形大小保存

		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);


		CImage img;
		
		img.Create(nWidth,nHeight,32);

		HDC hdc = img.GetDC();

		SetStretchBltMode(hdc,HALFTONE);

		BitBlt(hdc, 0, 0, nWidth, nHeight,
			dcCompatible, nX -m_ptEditLT.x, nY - m_ptEditLT.y, SRCCOPY);				
				
		CTime time = CTime::GetCurrentTime();			

		m_filePath = g_strAvconTempPath + /*"截图"*/_T("catch");	


		if(!PathIsDirectory(g_strAvconTempPath))	
		{		
			if(!CreateDirectory(g_strAvconTempPath, NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("提示");
				}
				CString strInterimFolderCreateFail;
				strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
				if (strInterimFolderCreateFail.IsEmpty())
				{
					strInterimFolderCreateFail = _T("临时文件夹创建失败");
				} 
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strInterimFolderCreateFail);
				}
				return NULL;	
			}		
		}	
		//判断资源库文件夹是否存在，没有则创建
		if(!PathIsDirectory(m_filePath.Left(m_filePath.GetLength() - 3)))
		{
			if(!CreateDirectory(m_filePath.Left(m_filePath.GetLength() - 3), NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("提示");
				}
				CString strInterimFolderCreateFail;
				strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
				if (strInterimFolderCreateFail.IsEmpty())
				{
					strInterimFolderCreateFail = _T("临时文件夹创建失败");
				} 
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strInterimFolderCreateFail);
				}
				return NULL;	
			}
		}
		
		//判断截图文件夹是否存在，没有则创建
		if(!PathIsDirectory(m_filePath))	
		{		
			if(!CreateDirectory(m_filePath, NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("提示");
				}
				CString strScrshotCreateFail;
				strScrshotCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScrshotCreateFail"));
				if (strScrshotCreateFail.IsEmpty())
				{
					strScrshotCreateFail = _T("截图文件夹创建失败");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strScrshotCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strScrshotCreateFail);
				}
				else
				{
					AfxMessageBox(strScrshotCreateFail);
				}
				return NULL;

			}		
		}	
		if (g_pWhiteBoardView == NULL)
		{
			return NULL;
		}
		m_filePath +=  _T("\\")+ time.Format(_T("%Y%m%d %H-%M-%S")) + _T(".jpg");
		
		img.Save(m_filePath);
		img.ReleaseDC();	
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
	// 返回位图句柄

	return hBitmap;
}

//显示操作提示信息
void CCatchScreenDlg::DrawTip()
{
    //得当前坐标像素,
	CPoint pt;
	GetCursorPos(&pt);
	pt  = pt - m_ptEditLT;
	//当到当前R,G,B,各像素值
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	CSize size;
	CFont *pFont = NULL;
	pFont = m_tipEdit.GetFont();
	dc.SelectObject(pFont);
	
	//按格式排放字符串
	CString string;
	CString strTemp;
	CString strCurrentPixel;
	strCurrentPixel = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("CurrentPixel"));
	if (strCurrentPixel.IsEmpty())
	{
		strCurrentPixel = _T("当前像素RGB");
	}
	string.Format(_T("\r\n\r\n\r\n·")+strCurrentPixel+"(%d,%d,%d)\r\n",rValue,gValue,bValue);
   	
	if(!m_bDraw&&!m_bFirstDraw)
	{
		CString strLbuttonDown;
		strLbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonDown"));
		if (strLbuttonDown.IsEmpty())
		{
			strLbuttonDown = _T(" ·按下鼠标左键确定截取范围");
		}
		CString strLbuttonDownNext = _T("");
		strLbuttonDownNext = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonDownNext"));
		if (strLbuttonDownNext.IsEmpty())
		{
			strLbuttonDownNext = _T("");
		}
		CString strRbuttonDown;
		strRbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RbuttonDown"));
		if (strRbuttonDown.IsEmpty())
		{
			strRbuttonDown = _T("·按下鼠标右键退出");
		}
		strTemp.Format(_T("\r\n")+ strLbuttonDown+"\r\n\r\n"+strRbuttonDown);

		size=dc.GetTextExtent(strLbuttonDown,_tcslen(strLbuttonDown));
		
	}
	else if(m_bDraw&&m_bFirstDraw)
	{
		CString strLbuttonUp;
		strLbuttonUp = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonUp"));
		if (strLbuttonUp.IsEmpty())
		{
			strLbuttonUp = _T(" ·松开鼠标左键确定截取范围");
		}
		CString strLbuttonUpNext = _T("");
		strLbuttonUpNext = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonUpNext"));
		if (strLbuttonUpNext.IsEmpty())
		{
			strLbuttonUpNext = _T("");
		}
		
		CString strRbuttonDown;
		strRbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RbuttonDown"));
		if (strRbuttonDown.IsEmpty())
		{
			strRbuttonDown = _T("·按下鼠标右键退出");
		}
		//strTemp="\r\n ·松开鼠标左键确定截取范围\r\n\r\n ·按ESC键退出";
		strTemp.Format(_T("\r\n")+ strLbuttonUp+"\r\n\r\n"+strRbuttonDown);
		size=dc.GetTextExtent(strLbuttonUp,_tcslen(strLbuttonUp));
		
	}
	else if(m_bFirstDraw)
	{
		//strTemp="\r\n ·用鼠标左键调整截取范围的\r\n 大小和位置\r\n\r\n ·截取范围内双击鼠标左键保\r\n 存图像，结束操作\r\n\r\n ·点击鼠标右键重新选择";
		strTemp=_T("\r\n ·用鼠标左键调整截取范围的\r\n 位置\r\n\r\n ·截取范围内双击鼠标左键保\r\n 存图像，结束操作\r\n\r\n ·点击鼠标右键重新选择");
	}
	string+=strTemp;
	//显示到编缉框中,操作提示窗口
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
	//m_tipEdit.MoveWindow(m_ptEditLT.x+10,m_ptEditLT.y+10,size.cx + 28,rect.Height());
	m_tipEdit.SetWindowText(string);
	//m_tipEdit.UpdateWindow();
}

//显示截取矩形信息
void CCatchScreenDlg::DrawMessage(CRect &inRect,CDC * pDC)
{
	//截取矩形大小信息离鼠标间隔
	const int space=3;
    
	//设置字体颜色大小
	
	CPoint pt;
	
	//CPen pen(PS_SOLID,1,RGB(147,147,147));

	//dc.SetTextColor(RGB(147,147,147));
	CFont font;
	CFont * pOldFont;
	CString strFont;
	strFont = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("Font"));
	if (strFont.IsEmpty())
	{
		strFont = _T("宋体");
	}
	CString strTopPosition;
	strTopPosition = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("TopPosition"));
	if (strTopPosition.IsEmpty())
	{
		strTopPosition = _T("顶点位置");
	}
	CString strRectSize;
	strRectSize = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RectSize"));
	if (strRectSize.IsEmpty())
	{
		strRectSize = _T("矩形大小");
	}
	CString strCoordinate;
	strCoordinate = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("Coordinate"));
	if (strCoordinate.IsEmpty())
	{
		strCoordinate = _T("光标坐标");
	}
	font.CreatePointFont(90,strFont);
	pOldFont=pDC->SelectObject(&font);

	//得到字体宽度和高度
	GetCursorPos(&pt);
	pt -= m_ptEditLT;
	int OldBkMode;
	OldBkMode=pDC->SetBkMode(TRANSPARENT);

	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	int	lineLength;
	pDC->GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;
	size=pDC->GetTextExtent(strCoordinate,_tcslen(strCoordinate));
	lineLength=size.cx + 10;
    
	//初始化矩形, 以保证写下六行文字
	CRect rect(pt.x+space,pt.y-charHeight*6-space,pt.x+lineLength+space,pt.y-space);
    
    //创建临时矩形
    CRect rectTemp;
	//当矩形到达桌面边缘时调整方向和大小
	CPoint ptTempt;
	GetCursorPos(&ptTempt);
	if((ptTempt.x+rect.Width())>=m_xScreen)
	{
		//桌面上方显示不下矩形
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((ptTempt.y-rect.Height())<=0)
	{
		//桌面右方显示不下矩形
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
		
	}

	//创建空画刷画矩形
	CBrush * pOldBrush;
    pOldBrush=pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	
	pDC->Rectangle(rect);
   	rect.top+=2;
	//在矩形中显示文字
	CRect outRect(rect.left,rect.top,rect.left+lineLength,rect.top+charHeight);
	CString string(strTopPosition);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	outRect.SetRect(rect.left,rect.top+charHeight,rect.left+lineLength,charHeight+rect.top+charHeight);
	string.Format(L"(%d,%d)",inRect.left,inRect.top);
	pDC->DrawText(string,outRect,DT_CENTER);
	

	outRect.SetRect(rect.left,rect.top+charHeight*2,rect.left+lineLength,charHeight+rect.top+charHeight*2);
	string=strRectSize;
	pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*3,rect.left+lineLength,charHeight+rect.top+charHeight*3);
	string.Format(L"(%d,%d)",inRect.Width(),inRect.Height());
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*4,rect.left+lineLength,charHeight+rect.top+charHeight*4);
	string=strCoordinate;
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*5,rect.left+lineLength,charHeight+rect.top+charHeight*5);
	string.Format(L"(%d,%d)",pt.x,pt.y);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	pDC->SetBkMode(OldBkMode);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);

	font.DeleteObject();
	
}
//重画窗口
void CCatchScreenDlg::PaintWindow()
{
	//获取当全屏对话框窗口大小
	CRect rect1;
	//GetWindowRect(rect1);
	GetClientRect(&rect1);

	//获取编辑框窗口大小
	CRect rect2;
	//m_tipEdit.GetWindowRect(rect2);
	m_tipEdit.GetClientRect(&rect2);

	CRgn rgn1,rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rect2);

	//获取更新区域,就是除了编辑框窗口不更新
	m_rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	
	InvalidateRgn(&m_rgn);

	rgn1.DeleteObject();
	rgn2.DeleteObject();
}
//改变操作提示窗口当RGB值
void CCatchScreenDlg::ChangeRGB()
{
	//保存旧的RGB值字符串
	static CString strOld("");

	CPoint pt;
	GetCursorPos(&pt);
	pt += m_ptEditLT;
	//当到当前R,G,B,各像素值
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//按格式排放字符串
	CString string;
	string.Format(L"(%d,%d,%d)",rValue,gValue,bValue);
	//如果当前颜色没变则不刷新RGB值,以免窗口有更多闪烁
    if(strOld!=string)
	{
	    //得到RGB文本那一行的文本长度
		int LineLength=m_tipEdit.LineLength(6);
		//复选RGB值文本,也就是(255,255,255)形式
	    m_tipEdit.SetSel(20,LineLength+6);
        
		//替换RGB内容
		m_tipEdit.ReplaceSel(string);
    }
	
	strOld=string;

}

void CCatchScreenDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//MessageBox(_T("进入ONCHAR"));
	if (nChar == VK_ESCAPE)
	{
		if(m_bFirstDraw)
		{
			//如果已经截取矩则清除截取矩形
			m_bFirstDraw=FALSE;
			//清除矩形大小
			m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
			DrawTip();
			PaintWindow();
		}
		else
		{
			OnCancel();
		}
	}
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CCatchScreenDlg::OnStnClickedEdit1()
{
	// TODO: 在此添加控件通知处理程序代码
}


//BOOL CCatchScreenDlg::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if   (pMsg->message == WM_KEYDOWN) 
//	{ 
//		switch(pMsg->wParam)   
//		{ 
//		case   VK_ESCAPE: 
//			{
//				if(m_bFirstDraw)
//				{
//					//如果已经截取矩则清除截取矩形
//					m_bFirstDraw=FALSE;
//					//清除矩形大小
//					m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
//					DrawTip();
//					PaintWindow();
//				}
//				else
//				{
//					OnCancel();
//				}
//			}
//			return   true; 
//		default: 
//			return   true; 
//		} 
//	} 
//
//	return CDialog::PreTranslateMessage(pMsg);
//}
void CCatchScreenDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnRButtonDown(nFlags, point);
}

void CCatchScreenDlg::GetDisplayInfo()
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

		BOOL BTRUE =::EnumDisplaySettings(dev.DeviceName,ENUM_CURRENT_SETTINGS, &devmode);
		/*if(BTRUE )
		{
			CString stroutput2;
		
			stroutput2.Format(_T("ssssssssssssssssss\n"));		
			OutputDebugString(stroutput2);
		}
		else
		{
			CString stroutput2;
		
			stroutput2.Format(_T("ppppppppppppppppppppppppp\n"));		
			OutputDebugString(stroutput2);
		}*/
		int x = devmode.dmPosition.x;
		int y = devmode.dmPosition.y;
		DWORD cx = devmode.dmPelsWidth +x;
		DWORD cy = devmode.dmPelsHeight +y;
		/*CString stroutput;
		
		stroutput.Format(_T("%d\n"),y);		
		OutputDebugString(stroutput);
		stroutput.Format(_T("%d\n"),cx);		
		OutputDebugString(stroutput);
		stroutput.Format(_T("%d\n"),cy);		
		OutputDebugString(stroutput);*/
		if (m_nscreenLeft >= x && x > -10000)
		{
			m_nscreenLeft = x;
			/*stroutput.Format(_T("%d\n"),m_nscreenLeft);		
			OutputDebugString(stroutput);*/
		}
		if (m_nscreenTop >= y && y> -10000)
		{
			m_nscreenTop = y;
			/*stroutput.Format(_T("%d\n"),m_nscreenTop);		
			OutputDebugString(stroutput);*/
		}
// 		m_xScreen += cx;
// 		m_yScreen += cy;
		if (m_xScreen <= cx && cx > 0 && cx < 10000)
		{
			m_xScreen =cx;
			/*stroutput.Format(_T("%d\n"),m_xScreen);		
			OutputDebugString(stroutput);*/
		}
		if (m_yScreen <= cy && cy >0 && cy < 10000)
		{
			m_yScreen = cy;
			/*stroutput.Format(_T("%d\n"),m_yScreen);		
			OutputDebugString(stroutput);*/
		}
		index++;

	}
	/*CString stroutputw;
	stroutputw.Format(_T("%d\n"),m_nscreenLeft);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_nscreenTop);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_xScreen);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_yScreen);		
	OutputDebugString(stroutputw);*/
}
