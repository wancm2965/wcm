#pragma once
#include "afxwin.h"

//CWBEdit
class CWBEdit : public CEdit
{
	DECLARE_DYNAMIC(CWBEdit)

	COLORREF		m_clrBackground;	//背景颜色
	COLORREF		m_clrFrame;			//边框颜色
//	COLORREF		m_clrText;			//文字颜色

// 	COLORREF		m_clrDisableBkg;	//禁用背景色
// 	COLORREF		m_clrDisableTxt;	//禁用文字色

	CBrush			m_brushBkg;			//背景画刷

public:
	CWBEdit();
	virtual ~CWBEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnNcPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	SetIsWritePageCount(bool bflag) {m_bIsWritePageCount = bflag;}
private:
	bool	m_bIsWritePageCount;
	bool	m_bIsHave;
};

