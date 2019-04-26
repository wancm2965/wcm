#pragma once


// CWBColorPatetteBtn

class CWBColorPatetteBtn : public CButton
{
	DECLARE_DYNAMIC(CWBColorPatetteBtn)

public:
	CWBColorPatetteBtn(COLORREF bkColor = RGB(255,255,255));
	virtual ~CWBColorPatetteBtn();
	void SetBackgroundColor(COLORREF bkColor);
	COLORREF GetBackgroundColor();
protected:
	DECLARE_MESSAGE_MAP()
private:
	COLORREF  m_bkColor;
	UINT m_btnID;
	CString			 m_strText;
public:
	void SetShowText(CString strText);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct/*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetBtnStyle();
};


