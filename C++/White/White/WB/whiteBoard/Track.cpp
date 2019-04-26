#include "StdAfx.h"

#ifndef _TRACK_H_
#define _TRACK_H_
#include "Track.h"
#include "WVedio.h"
class CWVedio;

#endif /*_TRACK_H_*/
CTrack::CTrack(void)
{
}


CTrack::~CTrack(void)
{
}
void CTrack::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,
	CDC* pDC, CWnd* pWnd )
{
	//更新窗口,消除TRACK时的虚线框
	((CWVedio *)pWnd)->PaintWindow();
	((CWVedio *)pWnd)->SendMessage(WM_MOUSEMOVE);
}