#include "stdafx.h"
#include "ClickTarget.h"
#include "ResGallery.h"
#include "resource.h"
#include "Define.h"

CWBTouchClick::CWBTouchClick(HWND hWnd, ResMapArray *pResData)
	: m_hWnd(hWnd)
	, m_pResData(pResData)
	,m_bDown(0)
	,m_State(CLICKSTATE_MOUSEHOVE)
	,m_pBitmap(NULL)
	,m_pGdiPlusBmp(NULL)
{
}

CWBTouchClick::~CWBTouchClick()
{
	if (m_pGdiPlusBmp != NULL)
	{
		delete m_pGdiPlusBmp;
		m_pGdiPlusBmp = NULL;
	}
	if (m_pBitmap != NULL)
	{
		m_pBitmap->Release();
		m_pBitmap = NULL;
	}
}

void CWBTouchClick::OnCallbackGesture( const unsigned int &nMessage, 
	const unsigned int &nAppendMessage /*= 0*/ )
{
	WORD lWord = LOWORD(nMessage);
	if (TGT_ACLICK == lWord)
	{
		WORD hWord = HIWORD(nMessage);
		if( (TGC_TAP == hWord) || (TGC_CLICK == hWord) )
		{
			OutputDebugString(TEXT("\nCWBTouchClick::OnCallbackGesture"));
			//PostMessage(m_hWnd, UWM_CHANGE_NUME, nAppendMessage, 0);
		}
	}
}

bool  CWBTouchClick::BInRegion( const POINT &pt ) const
{
	POINT ptTemp;
	ptTemp.x = pt.x;
	ptTemp.y = pt.y;
	::ScreenToClient(m_hWnd, &ptTemp);

	RECT rctWnd;
	GetRectRegion(&rctWnd);

	return (PtInRect(&rctWnd, ptTemp))?(true):(false);
}

void CWBTouchClick::GetRectRegion( RECT *rct ) const
{
	GetClientRect(m_hWnd, rct);
	rct->left = rct->right - CLICK_ZONE_WIDTH;
	rct->bottom = rct->top + CLICK_ZONE_HEIGHT;
}

void CWBTouchClick::touchDownHandler( const TouchDown& te )
{
	if (m_State != CLICKSTATE_LBUTTONDOWN)
	{
		m_State = CLICKSTATE_LBUTTONDOWN;
		::InvalidateRect(m_hWnd,NULL,FALSE);
	}

	POINT pt = {te.xpos, te.ypos};
	RECT rt;
	::GetClientRect(m_hWnd,&rt);
	ScreenToClient(m_hWnd,&pt);
	if(pt.y < CLICK_ZONE_HEIGHT)
	{
		m_bDown = 1;
	}
	else if(pt.y < 2*CLICK_ZONE_HEIGHT)
	{
		m_bDown = 2;
	}
	else
	{
		m_bDown = 3;
	}
}

void CWBTouchClick::touchUpHandler( const TouchUp& te )
{
	if (m_State != CLICKSTATE_MOUSEHOVE)
	{
		m_State = CLICKSTATE_MOUSEHOVE;
		::InvalidateRect(m_hWnd,NULL,FALSE);
	}

	POINT pt = {te.xpos, te.ypos};
	RECT rt;
	::GetClientRect(m_hWnd,&rt);
	ScreenToClient(m_hWnd,&pt);
	if(pt.y < CLICK_ZONE_HEIGHT)
	{
		if(m_bDown == 1)
		{
			PostMessage(m_hWnd, UWM_CHANGE_NUME, MAKEWPARAM(te.xpos,te.ypos), 0);
		}
	}
	else if(pt.y < 2*CLICK_ZONE_HEIGHT)
	{
		if(m_bDown == 2)
		{
			PostMessage(m_hWnd, UWM_CHANGE_NUME, MAKEWPARAM(te.xpos,te.ypos), 0);

		}
	}
	else
	{
		if(m_bDown == 3)
		{
			PostMessage(m_hWnd, UWM_CHANGE_NUME, MAKEWPARAM(te.xpos,te.ypos), 0);
		}
	}
	m_bDown = 0;
}