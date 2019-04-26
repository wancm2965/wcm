#if !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
#define AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_CXSHADE_RADIO WM_USER+0x100
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton window
class CxSkinButton : public CButton
{
// Construction
public:
	CxSkinButton();

// Attributes
private:
	BOOL m_bDispText;//按钮上显示的文字

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:
	void AddTool(LPCTSTR strTip);
	void SetDispText(BOOL btext);//设置按钮上显示的文字
	void SetToolTipText(CString s);//设置帮助气泡上显示的文字
	COLORREF SetTextColor(COLORREF new_color);//设置字体颜色
	//设置按钮各种状态下的位图（使用资源id）
	void SetSkin(UINT normal,UINT down, UINT over=0, UINT disabled=0, UINT focus=0,UINT mask=0,
				short drawmode=1,short border=1,short margin=4);
	//设置按钮各种状态下的位图（使用位图）
	void SetSkin(HBITMAP normal,HBITMAP down, HBITMAP mask,HBITMAP over=0,  HBITMAP focus=0,HBITMAP disabled=0,
				short drawmode=0,short border=0,short margin=0);
	virtual ~CxSkinButton();

	void LoadBmpFromFile(CString bmpnormal,CString bmpfocus,CString bmpmask);
	// Generated message map functions
	void    SetButtonFocus();
	void    SetButtonNormal();

	HBITMAP  m_hBitmapNormal;
	HBITMAP  m_hBitmapFocus;
	HBITMAP  m_hBitmapMask;

	CString  m_strbmpNormal;
	CString  m_strbmpFocus;
	CString  m_strbmpMask;
	
protected:
	CString strTipText;
	bool	m_Checked;	//按钮是否位check被状态	//radio & check buttons
	DWORD	m_Style;	//按钮的风格			//radio & check buttons
    bool m_tracking;	//追踪鼠标是否在按钮区域内
    bool m_button_down;	//按钮是否正在被按下
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);//追踪鼠标显示提示文字 无用
	CToolTipCtrl m_tooltip;	//显示提示气泡的窗口类
	//按钮的几种状态位图：正常、压下、失效、掩码、鼠标移到上面、获得焦点
	CBitmap m_bNormal,m_bDown,m_bDisabled,m_bMask,m_bOver,m_bFocus; //skin bitmaps
	short	m_FocusRectMargin;		//焦点框的margin offset//dotted margin offset
	COLORREF m_TextColor;			//按钮上显示的字体颜色//button text color
	HRGN	hClipRgn;				//clipping region
	BOOL	m_Border;				//0=flat; 1=3D;
	short	m_DrawMode;				//绘制模式 0=normal; 1=stretch; 2=tiled;
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);//从掩码位图形状转化为按钮形状
	void	FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r);//第2种绘制模式用来填充位图
	void	DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode);//绘制位图
	int		GetBitmapWidth (HBITMAP hBitmap);//获得位图宽度
	int		GetBitmapHeight (HBITMAP hBitmap);//获得位图高度
	//{{AFX_MSG(CxSkinButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);//鼠标离开按钮区的消息响应
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM); //radio button 使用
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);//设置状态check
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);//获得是否在check 状态
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
