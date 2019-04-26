#pragma once
#include <atlimage.h>

// CBitmapTabCtr

class CBitmapTabCtr : public CTabCtrl
{
	DECLARE_DYNAMIC(CBitmapTabCtr)

public:
	CBitmapTabCtr();
	virtual ~CBitmapTabCtr();

	virtual void OnFinalRelease();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT  lpDS/*lpDrawItemStruct*/);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


