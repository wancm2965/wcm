#pragma once


// CWBImageSliderCtrl

class CWBImageSliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CWBImageSliderCtrl)

public:
	CWBImageSliderCtrl();
	CWBImageSliderCtrl(COLORREF cr);
	virtual ~CWBImageSliderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	COLORREF m_crPrimary;
	COLORREF m_crShadow;
	COLORREF m_crHilite;
	COLORREF m_crMidShadow;
	COLORREF m_crDarkerShadow;

	CBrush m_normalBrush;
	CBrush m_focusBrush;
public:
	COLORREF GetShadowColor() const;
	COLORREF GetHiliteColor() const;
	COLORREF GetPrimaryColor() const;
	void SetPrimaryColor( COLORREF cr );
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


