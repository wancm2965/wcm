#pragma once


/********************************
*								*
*		背景控制按钮			*
*		caoyuqin2012.3.19		*
*								*
********************************/

class CWBFillPadBtn : public CButton
{
	DECLARE_DYNAMIC(CWBFillPadBtn)

public:
	CWBFillPadBtn();
	virtual ~CWBFillPadBtn();
private:
	BOOL m_isMouseIn;//鼠标是否处于按钮上
	WB_MOUSE_STATE  m_mouseState;//按钮状态
	BOOL m_isSelect;//按钮是否被选中
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetIsSelectBtn(BOOL isSelect);
};


