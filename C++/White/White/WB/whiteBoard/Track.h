#pragma once
#include "afxext.h"
class CTrack :
	public CRectTracker
{
public:
	CTrack(void);
	~CTrack(void);
	virtual void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,
		CDC* pDC, CWnd* pWnd );
};

