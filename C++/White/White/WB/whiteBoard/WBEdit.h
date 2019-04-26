#pragma once
#include "afxwin.h"

//CWBEdit
class CWBEdit : public CEdit
{
	DECLARE_DYNAMIC(CWBEdit)

	COLORREF		m_clrBackground;	//������ɫ
	COLORREF		m_clrFrame;			//�߿���ɫ
//	COLORREF		m_clrText;			//������ɫ

// 	COLORREF		m_clrDisableBkg;	//���ñ���ɫ
// 	COLORREF		m_clrDisableTxt;	//��������ɫ

	CBrush			m_brushBkg;			//������ˢ

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

