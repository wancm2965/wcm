// CShockwaveFlash.cpp : �� Microsoft Visual C++ ������ ActiveX �ؼ���װ��Ķ���


#include "stdafx.h"
#include "CShockwaveFlash.h"

#include "WhiteBoard.h"

/////////////////////////////////////////////////////////////////////////////
// CShockwaveFlash

IMPLEMENT_DYNCREATE(CShockwaveFlash, CWnd)

// CShockwaveFlash ����

// CShockwaveFlash ����
BEGIN_MESSAGE_MAP(CShockwaveFlash, CWnd)

//	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()

	ON_WM_TIMER()
END_MESSAGE_MAP()



/*
void CShockwaveFlash::OnMouseMove(UINT nFlags, CPoint point)
{
	 CRect rc;
	((CWBFlash *)GetParent())->GetClientRect(&rc);
	CPoint pHit;
	pHit.x = point.x + rc.left;
	pHit.y = point.y + rc.top;

	((CWBFlash *)GetParent())->OnNcHitTest(pHit);
	//CWnd::OnMouseMove(nFlags, point);
}
*/

LRESULT CShockwaveFlash::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CWnd::OnNcHitTest(point);
}




void CShockwaveFlash::OnTimer(UINT_PTR nIDEvent)
{
	if(ID_TIMER_FLASH == nIDEvent)
		Invalidate();

	CWnd::OnTimer(nIDEvent);
}
