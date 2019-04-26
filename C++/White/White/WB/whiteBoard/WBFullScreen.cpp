// WBFullScreen.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFullScreen.h"
#include "afxdialogex.h"


// CWBFullScreen �Ի���

IMPLEMENT_DYNAMIC(CWBFullScreen, CDialog)

CWBFullScreen::CWBFullScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CWBFullScreen::IDD, pParent)
{
//	EnableAutomation();

	//��ȡ��Ļ�ֱ���
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

	//��ȡ��Ļ��λͼ��
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
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CDialog::OnFinalRelease();
}

void CWBFullScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CWBFullScreen::OnInitDialog()
{
	CDialog::OnInitDialog();

	//�ѶԻ������ó�ȫ�����㴰��
	SetWindowPos(&wndTopMost, m_nScreenLeft, m_nScreenTop, m_xScreen-m_nScreenLeft, m_yScreen-m_nScreenTop, SWP_SHOWWINDOW);
    
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CWBFullScreen, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWBFullScreen, CDialog)
END_DISPATCH_MAP()

// ע��: ������� IID_IWBFullScreen ֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {11D0F020-1360-4EF3-8C68-6984240CFF1E}
static const IID IID_IWBFullScreen =
{ 0x11D0F020, 0x1360, 0x4EF3, { 0x8C, 0x68, 0x69, 0x84, 0x24, 0xC, 0xFF, 0x1E } };

BEGIN_INTERFACE_MAP(CWBFullScreen, CDialog)
	INTERFACE_PART(CWBFullScreen, IID_IWBFullScreen, Dispatch)
END_INTERFACE_MAP()


// CWBFullScreen ��Ϣ�������

HBITMAP CWBFullScreen::CopyScreenToBitmap(LPRECT lpRect)
{
	if (IsRectEmpty(lpRect))
	{
		return NULL;
	}

	HDC hScrDC, hMemDC; // ��Ļ���ڴ��豸������
	HBITMAP hBitmap, hOldBitmap; // λͼ���
	int nX, nY, nX2, nY2; // ѡ����������
	int nWidth, nHeight;

	//Ϊ��Ļ�����豸������
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hMemDC = CreateCompatibleDC(hScrDC);

	// ���ѡ����������
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//ȷ��ѡ�������ǿɼ���
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

	// ����һ������Ļ�豸��������ݵ�λͼ
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

	// ����λͼѡ���ڴ��豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// ����Ļ�豸�����������ڴ��豸��������
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

	//�õ���Ļλͼ�ľ��
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	
	//��� 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	return hBitmap;
}

BOOL CWBFullScreen::OnEraseBkgnd(CDC* pDC)
{
	//�������������ȫ���Ի��򱳾�
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
