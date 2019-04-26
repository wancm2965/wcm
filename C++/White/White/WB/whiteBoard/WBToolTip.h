#pragma once
#include "resource.h"


class CWBToolTipTemplate : public CDialogEx
{
public:
	CWBToolTipTemplate(void);
	~CWBToolTipTemplate(void);

	void SetColor(COLORREF bgColor);
	void SetText(CString str);
	CString GetText(){return m_strText;};
	LOGFONT GetFont(){return m_fontlog;};
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	LOGFONT m_fontlog;
	CString m_strText;
	COLORREF m_bgColor;
};

class CWBToolTip
{
public:
	CWBToolTip();
	~CWBToolTip(void);

	void SetColor(COLORREF toolTipBgColor);
	void SetText(CString toolTipStr);
	void SetPosition(CPoint toolTipPos);
	void SetSize(CSize toolTipSize);
	void SetTextString(CString toolTipStr,CSize size = 0);

	void Display();//��ʾ��ʾ�Ի���
	void Restore();//������ʾ�Ի���
	static CWBToolTip *	    GetIns();											///���ʵ��
	static bool				CreateIns();										///����ʵ��
	static void				DestroyIns();										///����ʵ��
	void					InitCToolTipTemplateDlg(CWnd* pParentWnd);	

private:
	static CWBToolTip	*s_pWBToolTipIns;	

	CPoint m_position;
	CSize  m_size;
	CWBToolTipTemplate *m_pToolTip;
};


