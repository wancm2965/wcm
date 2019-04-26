#pragma once
#include "SubClass.h"
#include "objectbase.h"

class DIRECTUI_API CHwndObj : public CObjectBase
{
//	DECLARE_DYNAMIC(CHwndObj)
public:
	CHwndObj(void);
	~CHwndObj(void);
	
public:
	HWND GetSafeHwnd();

	void Attach(HWND hWnd, BOOL bNeedDestroyHWnd = FALSE);
	void Detach();
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void CalcPosition(const CRect& rcParent);
	virtual void SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);
	virtual void SetFocus(BOOL bFocus  = TRUE );

protected:
	BOOL m_bNeedToDestoryWnd;
	HWND m_hWnd;
};
