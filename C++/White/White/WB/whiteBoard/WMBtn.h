#pragma once


// CWMBtn
#include "SpotlightDlg.h"

//class CWBRectMaqn;
class CSpotlightDlg;

//modify by zhuyuyuan
class CWMBtn : public CBitmapButton
{
	DECLARE_DYNAMIC(CWMBtn)

public:
	//CWMBtn(CWBRectMaqn* WBRectMaqn,int nBtnCount);
	CWMBtn(CSpotlightDlg *const pSpotlight,int nBtnCount);
	CWMBtn();
	virtual ~CWMBtn();
public:
	int  m_nBtnCount;
	BOOL m_bMoveFlag;
private:
	//CWBRectMaqn*  m_WBRectMaqn;
	CSpotlightDlg* m_pSpotlight;
	Bitmap			 *m_pImg;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void SetButtonImage(CString filename);//…Ë÷√∞¥≈•Õº∆¨
public:
	CPoint m_pCurPoint;
	BOOL   m_IsUpOrDown;
	BOOL   m_IsLeftOrRight;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};