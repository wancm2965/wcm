#pragma once
#include "common\WBData.h"
#include "WBToolTip.h"
// CWBFunctionBtn

class CWBFunctionBtn : public CBitmapButton
{
	DECLARE_DYNAMIC(CWBFunctionBtn)

public:
	CWBFunctionBtn();
	virtual ~CWBFunctionBtn();

protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bMouseIn;
	BOOL m_isSelect;
	WB_MOUSE_STATE  m_mouseState;//°´Å¥×´Ì¬
	CSize m_size;
	CPoint m_UpPoint; //获取"V"按钮ONMOUSEUP弹起的点，传给CWBPromptFontSetDlg文件中的垂直"V"按钮调用
public:
	void SetIsSelect(BOOL IsSelect);
	BOOL GetIsSelect();
	void SetMouseState(WB_MOUSE_STATE MouseState);
	WB_MOUSE_STATE GetMouseState();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS/*lpDrawItemStruct*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void MoveWindowBtn(int x,int y);//¸ù¾Ý×Ö·û¶àÉÙ·ÖÅä´óÐ¡
	CSize GetBtnSizeHeight();
	void ShowToolText();//ÏÔÊ¾ÌáÊ¾ÎÄ±¾
	CPoint GetMouseUpPoint();
};


