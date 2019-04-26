#if !defined(AFX_MYEDIT_H__A34EEA6D_E8FC_4D15_B03C_BAA42FDF6FCB__INCLUDED_)
#define AFX_MYEDIT_H__A34EEA6D_E8FC_4D15_B03C_BAA42FDF6FCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

//modify by zhuyuyuan 2011-11-26
class CMyEdit : public CEdit
{
public:
	CMyEdit();
	virtual ~CMyEdit();
	
protected:
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bMove;
	COLORREF m_crText;
	COLORREF m_crBackGnd;
	CBrush m_brBackGnd;
	CPoint m_ptWindowLT; //Edit显示位置的左上点
	int m_ScreenWidth;   //Edit显示位置屏幕的宽度
public:
	void SetTextColor(COLORREF rgb);
	void SetBackColor(COLORREF rgb);
	void SetWindowLT(CPoint pt);
	void SetScreenWidth(int width);
};


#endif 
