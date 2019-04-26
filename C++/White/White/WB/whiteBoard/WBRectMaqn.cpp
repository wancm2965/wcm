#include "StdAfx.h"
#include "WBRectMaqn.h"
#include "resource.h"
#include "whiteBoard.h"
#include "Windows.h"
#define  BTNWIDTH       35
#define  BTNHIGHT       25
#define  BTN_MAQ_CLOSE  7
#define  BTN_MAQ_ZOOM   8
CWBRectMaqn::CWBRectMaqn(void)
{

}
CWBRectMaqn::CWBRectMaqn(CRect rc,CWZoomCtrl*& ViewZoomCtrl)
{
	m_rect = rc;
	m_prePoint = CPoint(0, 0);
	m_nZoom = 150;
	m_count = 0;
	m_CloseRectBtn = NULL;
	m_WZoomCtrl    = ViewZoomCtrl;
 
	m_CloseRectBtn = new CWMBtn(this,BTN_RECTMAQ_CLOSE);
	m_CloseRectBtn->Create(_T("¹Ø±Õ"),WS_VISIBLE|TBS_BOTH|TBS_NOTICKS|WS_TABSTOP | BS_PUSHBUTTON,CRect(m_rect.right - BTNWIDTH, m_rect.bottom + 2, m_rect.right + 2, m_rect.bottom + BTNHIGHT + 10), g_pWhiteBoardView,ID_BtnCloseMaq);//

	if (NULL == m_WZoomCtrl)
	{
		m_WZoomCtrl = new CWZoomCtrl();
		m_WZoomCtrl->Create(WS_VISIBLE|TBS_BOTH|TBS_NOTICKS|WS_TABSTOP | BS_PUSHBUTTON,
			CRect(m_rect.left-2, m_rect.bottom + 2, m_rect.left + 121, m_rect.bottom + BTNHIGHT + 10), g_pWhiteBoardView,ID_WZOOMCTRL);
		m_WZoomCtrl->SetRange(-150,-30);
		m_WZoomCtrl->SetPos(-150);
	}
}
void  CWBRectMaqn::GetViewWZoomCtrl(CWZoomCtrl*& ZWoomCtrl)
{
	if (m_WZoomCtrl == NULL)
	{
		return;
	}
	ZWoomCtrl = m_WZoomCtrl;
}
CWBRectMaqn::~CWBRectMaqn(void)
{
	if (NULL != m_CloseRectBtn)
	{
		delete m_CloseRectBtn;
		m_CloseRectBtn = NULL;
	}

	if (NULL != m_WZoomCtrl)
	{
		delete m_WZoomCtrl;
		m_WZoomCtrl = NULL;
	}
}
 
void  CWBRectMaqn::MoveRect(CPoint ptMovePoint)
{	
	CRect BtnRect;
	m_rect += ptMovePoint;

	if (NULL != m_CloseRectBtn)
	{
		m_CloseRectBtn -> GetClientRect(&BtnRect);
		m_CloseRectBtn->ClientToScreen(&BtnRect);
		g_pWhiteBoardView->ScreenToClient(&BtnRect);
		BtnRect += ptMovePoint;
		m_CloseRectBtn -> MoveWindow(BtnRect);
		m_CloseRectBtn->UpdateWindow();
	}

	if (NULL != m_WZoomCtrl)
	{
		m_WZoomCtrl->GetClientRect(&BtnRect);
		m_WZoomCtrl->ClientToScreen(&BtnRect);
		g_pWhiteBoardView->ScreenToClient(&BtnRect);
		BtnRect += ptMovePoint;
		m_WZoomCtrl->MoveWindow(BtnRect);
		m_WZoomCtrl->UpdateWindow();
	}
}
void  CWBRectMaqn::MoveZoom(CPoint ptMovePoint)
{
	CRect BtnRect;

	if (NULL != m_WZoomCtrl)
	{
		m_WZoomCtrl->GetClientRect(&BtnRect);
		m_WZoomCtrl->ClientToScreen(&BtnRect);
		g_pWhiteBoardView->ScreenToClient(&BtnRect);
		BtnRect += ptMovePoint;
		m_WZoomCtrl->MoveWindow(BtnRect);
		m_WZoomCtrl->UpdateWindow();
	}
}
void  CWBRectMaqn::MoveCloseBtn(CPoint ptMovePoint)
{
	CRect BtnRect;

	if (NULL != m_CloseRectBtn)
	{
		m_CloseRectBtn -> GetClientRect(&BtnRect);
		m_CloseRectBtn->ClientToScreen(&BtnRect);
		g_pWhiteBoardView->ScreenToClient(&BtnRect);
		BtnRect += ptMovePoint;
		m_CloseRectBtn -> MoveWindow(BtnRect);
		m_CloseRectBtn->UpdateWindow();
	}
}
void CWBRectMaqn::DrawRectMaqn(CDC*pDC, CPoint point)
{
	m_Point = point;
	CPoint movepoint = m_Point - m_prePoint;
	m_prePoint = m_Point;

	if (0 != m_count)
	{
		MoveRect(movepoint);
	}
	if (0 == m_count)
	{
		m_rect += movepoint;
	}
	m_count++;
	HPEN m_RectMaqnPen = ::CreatePen(PS_SOLID,5,RGB(120,0,0));
	pDC->SelectObject(m_RectMaqnPen);
 
		m_nZoom = -g_pWhiteBoardView->GetZoomCtrl();
		pDC->StretchBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),pDC,
			m_rect.CenterPoint().x - m_nZoom,
			m_rect.CenterPoint().y - m_nZoom,
			m_nZoom*2,m_nZoom*2,
			SRCCOPY
			);
 
	pDC->SelectStockObject(NULL_BRUSH);	
	pDC->Rectangle(m_rect);	
}

int  CWBRectMaqn::PointIn(CPoint ptPoint)
{
	CRect rect;
	rect.left   = m_rect.left + INSIDERECT;
	rect.top    = m_rect.top + INSIDERECT;
	rect.right  = m_rect.right - INSIDERECT;
	rect.bottom = m_rect.bottom - INSIDERECT;
	if (rect.PtInRect(ptPoint))
	{
		return 0;
	} 
	else if( m_rect.PtInRect(ptPoint))
	{
		if (!(rect.PtInRect(ptPoint)))
		{
			int x= PointIsInSide(ptPoint);
			return x;
		}

	}
	else
	{
		return 1;
	}
}
int CWBRectMaqn::PointIsInSide(CPoint ptPoint)
{
	CRect northwestrect(m_rect.left, m_rect.top, m_rect.left+INSIDERECT, m_rect.top+INSIDERECT);
	CRect southeastrect(m_rect.right-INSIDERECT, m_rect.bottom-INSIDERECT, m_rect.right, m_rect.bottom);
	CRect southwestrect(m_rect.left, m_rect.bottom-INSIDERECT, m_rect.left+INSIDERECT, m_rect.bottom);
	CRect northeastrect(m_rect.right-INSIDERECT, m_rect.top, m_rect.right, m_rect.top+INSIDERECT);

	if (ptPoint.x>(m_rect.left+INSIDERECT) && ptPoint.x<(m_rect.right-INSIDERECT))
	{
		return 2;
	}
	if (ptPoint.y>(m_rect.top+INSIDERECT) && ptPoint.y<(m_rect.bottom-INSIDERECT))
	{
		return 3;
	}
	if (northwestrect.PtInRect(ptPoint) || southeastrect.PtInRect(ptPoint))
	{
		return 4;
	}
	if(southwestrect.PtInRect(ptPoint) || northeastrect.PtInRect(ptPoint))
	{
		return 5;
	}
	return  FALSE;
}
 
  