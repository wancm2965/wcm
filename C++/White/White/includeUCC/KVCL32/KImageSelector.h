#pragma once

#define ID_EMOTIONS_IMAGE	10000

//-----------------------------------------------------------------------------------------
// KImageSelector ¿‡∂®“Â
class AFX_EXT_CLASS KImageSelector : public CWnd
{
public:
	KImageSelector(CImageList *il, LPCTSTR tips[], int x, int y, int cols, CWnd *parent);
	virtual ~KImageSelector();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void PostNcDestroy();
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	void DrawButton(CDC *pDC, int row, int col, BOOL erase);

	CImageList *imageList;
	CToolTipCtrl toolTip;
	int nrColumns;
	CSize buttonSize;
	int lastSelectRow, lastSelectCol;
};
