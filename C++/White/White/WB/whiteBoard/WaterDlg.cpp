// WaterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WaterDlg.h"
#include "afxdialogex.h"
#define BMP_WIDTH   60          //位图宽度
#define BMP_HEIGHT  60          //位图高度
#define TIMER_START 1989
// CWaterDlg 对话框

IMPLEMENT_DYNAMIC(CWaterDlg, CDialogEx)

CWaterDlg::CWaterDlg(CBitmap *bitmap,CPoint point,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaterDlg::IDD, pParent)
	,m_bitmap(NULL)	
	,m_waterpoint(0,0)
{
	m_bitmap = bitmap;
	m_number = 0;	
	m_waterpoint = point; 
}

CWaterDlg::~CWaterDlg()
{
	if (m_bitmap && m_bitmap->m_hObject)
	{
		m_bitmap->DeleteObject();
	}
}

void CWaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CWaterDlg 消息处理程序
BOOL CWaterDlg::OnInitDialog()
{
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	SetLayeredWindowAttributes(0,140,LWA_ALPHA);


	
	if (m_bitmap)
	{
		CPictureHolder myTmpPicture;
		myTmpPicture.CreateFromBitmap(m_bitmap);
		m_bmpRenderSource.Create32BitFromPicture(&myTmpPicture,BMP_WIDTH,BMP_HEIGHT);
		m_bmpRenderTarget.Create32BitFromPicture(&myTmpPicture,BMP_WIDTH,BMP_HEIGHT);

		//创建水纹对象
		m_myWater.Create(BMP_WIDTH,BMP_HEIGHT);
		m_myWater.HeightBlob(BMP_WIDTH/2,BMP_HEIGHT/2,13,900,m_myWater.m_iHpage);
		SetTimer(TIMER_START,30,NULL);
	}
	//SetLayeredWindowAttributes
	ShowWindow(SW_HIDE);
	MoveWindow(m_waterpoint.x-BMP_WIDTH/2,m_waterpoint.y-BMP_HEIGHT/2,BMP_WIDTH,BMP_HEIGHT,TRUE);
	return true;
}

void CWaterDlg::OnPaint()
{
	if (m_bitmap)
	{
		CPaintDC dc(this);
		CPoint ptOrigin(0,0);
		m_bmpRenderTarget.Draw(&dc,ptOrigin);
		CDialog::OnPaint();
	}
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void CWaterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_START)
	{		
		m_myWater.Render((DWORD*)m_bmpRenderSource.GetDIBits(),(DWORD*)m_bmpRenderTarget.GetDIBits());
		CClientDC dc(this);
		CPoint ptOrigin(0,0);
		m_bmpRenderTarget.Draw(&dc,ptOrigin);
		ShowWindow(SW_SHOW);
		m_number++;
		if (m_number == 25)
		{
			ShowWindow(SW_HIDE);
			KillTimer(TIMER_START);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CWaterDlg::SetBackgroundBitmap( CBitmap *bitmap,CPoint point)
{
	
	m_waterpoint = point;
	ShowWindow(SW_HIDE);
	m_bitmap = bitmap;
	m_number = 0;
	if (m_bitmap)
	{
		KillTimer(TIMER_START);
		CPictureHolder myTmpPicture;
		myTmpPicture.CreateFromBitmap(m_bitmap);
		m_bmpRenderSource.Create32BitFromPicture(&myTmpPicture,BMP_WIDTH,BMP_HEIGHT);
		m_bmpRenderTarget.Create32BitFromPicture(&myTmpPicture,BMP_WIDTH,BMP_HEIGHT);

		//创建水纹对象
		m_myWater.Create(BMP_WIDTH,BMP_HEIGHT);
		m_myWater.HeightBlob(BMP_WIDTH/2,BMP_HEIGHT/2,13,900,m_myWater.m_iHpage);
		SetTimer(TIMER_START,30,NULL);
		MoveWindow(m_waterpoint.x-BMP_WIDTH/2,m_waterpoint.y-BMP_HEIGHT/2,BMP_WIDTH,BMP_HEIGHT,TRUE);		
	}
}



void CWaterDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
	delete this;
}


