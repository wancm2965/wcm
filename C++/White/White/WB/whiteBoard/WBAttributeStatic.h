#pragma once
#include "WBToolTip.h"
// CWBAttributeStatic

class CWBAttributeStatic : public CStatic
{
	DECLARE_DYNAMIC(CWBAttributeStatic)

public:
	CWBAttributeStatic();
	virtual ~CWBAttributeStatic();
protected:
	DECLARE_MESSAGE_MAP()
private:
	CString		m_strCaption;
	BOOL		m_bText;
	bool			m_bRrow;
	int			m_nFontSize;
	CSize		m_size;
	CFont		 *m_pFont;
	bool         m_bIsTextShowTip;//主要是提示文本上面那一排不加TOOLTIP
public:
	void SetCaption( CString strCaption );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	void SetTextPosition(BOOL bText);
	void SetRrowBK(bool bRrow = true);
	void SetFontSize(int nFontSize = 13);
	//wangxiaobin动态分配字符串长度
	CSize GetStringLength();
	void MoveStaticPos(int x,int y);

	void SetWindowTextBool(bool bflag) {m_bIsTextShowTip = bflag;}

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};


