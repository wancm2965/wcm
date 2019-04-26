#pragma once


/********************************
*								*
*		�������ư�ť			*
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
	BOOL m_isMouseIn;//����Ƿ��ڰ�ť��
	WB_MOUSE_STATE  m_mouseState;//��ť״̬
	BOOL m_isSelect;//��ť�Ƿ�ѡ��
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


