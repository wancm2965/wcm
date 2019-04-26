#pragma once
#define  MOUSE_ID		(100000000)
#include "MyDropTarget.h"
// WBRichEdit 视图

class CWBRichEdit : public CRichEditView
{
	DECLARE_DYNCREATE(CWBRichEdit)

public:
	CWBRichEdit();           // 动态创建所使用的受保护的构造函数
	virtual ~CWBRichEdit();


public:
	virtual HRESULT QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat, DWORD reco, BOOL bReally, HGLOBAL hMetaPict);

	BOOL CreateEdit(BOOL bVertical, const CRect &rct, CWnd *pParent);

	//设置当前字体
	void SetCurTextFont();
	LOGFONT GetCurFont();
	void SetCurFont(const LOGFONT& lfFont);

	//得到当前字体颜色
	COLORREF GetFontColor();
	//设置当前文本颜色
	void SetFontColor(COLORREF clrText);
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd );
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	//文本颜色
	COLORREF m_clrCurTextColor;	
	//CFont m_Font;
	//文字
	LOGFONT m_lfCurFont;
	//CMyDropTarget m_myDropTarget;
};

typedef enum RichEditRange
{
	RICHEDIT_VERTICAL_IN_VIEW = 400,
	RICHEDIT_HORIZENTAL_IN_VIEW,
	RICHEDIT_VERTICAL_IN_MOUSEDLG,
	RICHEDIT_HORIZENTAL_IN_MOUSEDLG,
	RICHEDIT_VERTICAL_IN_SCREENDRAW,
	RICHEDIT_HORIZENTAL_IN_SCREENDRAW,
}ERichEditRange;

class CWBRichEditMgr
{
protected:
	CWBRichEditMgr();
	~CWBRichEditMgr();

public:
	static CWBRichEditMgr *GetInst_();
private:
	static CWBRichEditMgr m_inst;

public:
	BOOL ShowWBRichEdit(const unsigned long &nTouchID, BOOL bVertical, 
		CWnd *pParent, BOOL bShowPrompt = true);
	void DestroyRichWindow();

public:
	CWBRichEdit *GetRichEdit() {return m_pRichEdit;}
	CWBPromptFontSetDlg *GetRichEditPrompt() {return m_pPromptFontSetDlg;}
	bool IsRichEditShow()const {return m_bRichEditShow;}

public:
	void SetBeginPoint(const unsigned int &nX, const unsigned int &nY, const unsigned long nTouchID = MOUSE_ID);
	void SetEndPoint(const unsigned int &nX, const unsigned int &nY, const unsigned long nTouchID = MOUSE_ID);
	bool GetRect(const unsigned long &nID, CRect &rct);
	void SetRichEditText(CString str);
	void SetRichEdit(CWBRichEdit *pEdit);
	void SetRichEditShow(bool bRichEditShow);
	void SetIsSaveText(bool bIsSaveText);

private:
	//std::map<const unsigned int, CWBRichEdit*> m_map;
	CWBRichEdit* m_pRichEdit;
	CWBPromptFontSetDlg* m_pPromptFontSetDlg;
	bool m_bRichEditShow;
	bool m_bIsSaveText; //删除编辑框时是否保存文字

// 	CRect m_rctRichEdit;
// 	unsigned long m_nCurTouchID;
	std::map<unsigned long, CRect*> m_mapRect;

	XCritSec m_csMapOperation;
};