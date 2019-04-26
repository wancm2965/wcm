// WBFullScreen.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFullScreen.h"
#include "afxdialogex.h"


// CWBFullScreen 对话框

IMPLEMENT_DYNAMIC(CWBFullScreen, CDialog)

CWBFullScreen::CWBFullScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CWBFullScreen::IDD, pParent)
{
//	EnableAutomation();

	//获取屏幕分辩率
// 	POINT pt;
// 	::GetCursorPos(&pt);
// 	MONITORINFO oMonitor = {}; 
// 	oMonitor.cbSize = sizeof(oMonitor);
// 	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
// 	CRect rcWork = oMonitor.rcWork;
	m_nScreenLeft = 0;
	m_nScreenTop = 0;
	m_xScreen = 0;
	m_yScreen = 0;
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
// 	m_xScreen = rcWork.right;
// 	m_yScreen = rcWork.bottom;

	//截取屏幕到位图中
	CRect rect(m_nScreenLeft, m_nScreenTop, m_xScreen, m_yScreen);
	m_pBitmap = CBitmap::FromHandle(CopyScreenToBitmap(&rect));
}

CWBFullScreen::~CWBFullScreen()
{
	if(m_pBitmap != NULL)
	{
		m_pBitmap->DeleteObject();
	}
}

void CWBFullScreen::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CDialog::OnFinalRelease();
}

void CWBFullScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CWBFullScreen::OnInitDialog()
{
	CDialog::OnInitDialog();

	//把对化框设置成全屏顶层窗口
	SetWindowPos(&wndTopMost, m_nScreenLeft, m_nScreenTop, m_xScreen-m_nScreenLeft, m_yScreen-m_nScreenTop, SWP_SHOWWINDOW);
    
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CWBFullScreen, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWBFullScreen, CDialog)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IWBFullScreen 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {11D0F020-1360-4EF3-8C68-6984240CFF1E}
static const IID IID_IWBFullScreen =
{ 0x11D0F020, 0x1360, 0x4EF3, { 0x8C, 0x68, 0x69, 0x84, 0x24, 0xC, 0xFF, 0x1E } };

BEGIN_INTERFACE_MAP(CWBFullScreen, CDialog)
	INTERFACE_PART(CWBFullScreen, IID_IWBFullScreen, Dispatch)
END_INTERFACE_MAP()


// CWBFullScreen 消息处理程序

HBITMAP CWBFullScreen::CopyScreenToBitmap(LPRECT lpRect)
{
	if (IsRectEmpty(lpRect))
	{
		return NULL;
	}

	HDC hScrDC, hMemDC; // 屏幕和内存设备描述表
	HBITMAP hBitmap, hOldBitmap; // 位图句柄
	int nX, nY, nX2, nY2; // 选定区域坐标
	int nWidth, nHeight;

	//为屏幕创建设备描述表
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);

	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//确保选定区域是可见的
// 	if (nX < 0)
// 	{
// 		nX = 0;
// 	}
// 	if (nY < 0)
// 	{
// 		nY = 0;
// 	}
	if (nX2 > m_xScreen)
	{
		nX2 = m_xScreen;
	}
	if (nY2 > m_yScreen)
	{
		nY2 = m_yScreen;
	}
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

	//得到屏幕位图的句柄
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	return hBitmap;
}

BOOL CWBFullScreen::OnEraseBkgnd(CDC* pDC)
{
	//用整个桌面填充全屏对话框背景
	BITMAP bmp;
	m_pBitmap->GetBitmap(&bmp);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	CBitmap* hOldBitmap;
	hOldBitmap = dcCompatible.SelectObject(m_pBitmap);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);

	dcCompatible.SelectObject(hOldBitmap);
	return TRUE;
}
