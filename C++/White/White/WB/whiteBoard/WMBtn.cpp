// WMBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "WMBtn.h"
#include "whiteBoard.h"

#define BTN_UP    4
#define BTN_DOWN  2
#define BTN_LEFT  1
#define	BTN_RIGHT 3
#define BTN_CLOSE 5
#define BTN_NONE  6
#define RECT_WIDTH  300  //缩放后的矩形宽度
#define RECT_HIGHT  200  //缩放后的矩形高度

// CWMBtn

IMPLEMENT_DYNAMIC(CWMBtn, CBitmapButton)

CWMBtn::CWMBtn()
{

}
// CWMBtn::CWMBtn(CWBRectMaqn* WBRectMaqn,int nBtnCount)
// {
// 	m_WBRectMaqn = WBRectMaqn;
// 	m_nBtnCount  = nBtnCount;
// 	m_pImg = NULL;
// }

CWMBtn::CWMBtn(CSpotlightDlg *pSpotlight,int nBtnCount)
{

	m_pSpotlight = pSpotlight;
	m_nBtnCount = nBtnCount;
	m_pImg = NULL;
}

CWMBtn::~CWMBtn()
{
	if (NULL != m_pImg )
	{
		delete m_pImg;
	}
}

BEGIN_MESSAGE_MAP(CWMBtn, CBitmapButton)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
// CWMBtn 消息处理程序
void CWMBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	CButton::OnLButtonUp(nFlags, point);
	m_bMoveFlag = BTN_NONE;
	m_pCurPoint = CPoint(0,0);

}


void CWMBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	CButton::OnLButtonDown(nFlags, point);
	CPoint pt = point;
	m_pCurPoint = pt;
	ClientToScreen(&m_pCurPoint);
	switch (m_nBtnCount)
	{
	case BTN_UP:
		{
			m_bMoveFlag  = BTN_UP;

			break;
		}
	case BTN_DOWN:
		{
			m_bMoveFlag = BTN_DOWN;
			break;
		}
	case  BTN_LEFT:
		{
			m_bMoveFlag = BTN_LEFT;
			break;
		}
	case BTN_RIGHT:
		{
			m_bMoveFlag = BTN_RIGHT;
			break;
		}

	case BTN_RECTMAQ_CLOSE://关闭放大镜
		{
			//g_pWhiteBoardView->SetMaqnState(FALSE);
			//g_pWhiteBoardView->SendMagn();
			g_pWhiteBoardView->CloseRectMagn();
			break;
		}
	case BTN_CLOSE_ID:
		{
			//g_pWhiteBoardView->SetSpotlightState(FALSE);
			//g_pWhiteBoardView->SendSpotlight();
			g_pWhiteBoardView->CloseSpotlight();
			break;
		}
	default:
		break;
	}
	return;
}
void CWMBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nTemp = 0;
	int nAbs = 0;
	CPoint pt;
	pt = point;
	ClientToScreen(&pt);

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
}
void CWMBtn::SetButtonImage(CString filename)
{
	//CString strPath = GetCurrentPath();
	CString strPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\";
	filename = strPath + filename;
	if(m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}
	wchar_t strFilename[MAX_PATH];
	CString picturePath = filename;
	MultiByteToWideChar(CP_ACP, 0, ToStr(picturePath.GetBuffer()).c_str(), -1, strFilename, MAX_PATH);
	m_pImg = new Bitmap(strFilename);
	ASSERT(m_pImg);
	picturePath.ReleaseBuffer();
}

BOOL CWMBtn::OnEraseBkgnd(CDC* pDC)
{

	//CRect rect;
	//GetClientRect(&rect);
	//Graphics graph(pDC->GetSafeHdc());
	//if (m_pImg)
	//{
	//	Rect Rt(0, 0, rect.Width(), rect.Height());
	//	
	//	graph.DrawImage(m_pImg, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
	//				
	//}
	return TRUE;
	//	return CBitmapButton::OnEraseBkgnd(pDC);
}


void CWMBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CRect rect;
	GetClientRect(&rect);
	Graphics graph(lpDrawItemStruct->hDC);
	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (m_pImg) //按钮背景图
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());

		graph.DrawImage(m_pImg, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片

	}
}
