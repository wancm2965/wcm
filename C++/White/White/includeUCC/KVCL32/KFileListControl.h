#if !defined(AFX_FILELISTCTRL_H__AC7C4049_9583_405D_9CBD_9F89E5C909BB__INCLUDED_)
#define AFX_FILELISTCTRL_H__AC7C4049_9583_405D_9CBD_9F89E5C909BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileListCtrl.h : header file

#include <AfxTempl.h>

#define LIST_GRIDLINES				1
#define INVALID_ITEM				-1
#define NONE_SELECTED				-1
#define STACKSIZE					8

#define LIST_SELECTIONCHANGED		1
#define LIST_LBUTTONDOWN			2
#define LIST_RBUTTONDOWN			3
#define LIST_LBUTTONDBLCLICK		4
#define LIST_ITEMCHECKED			5

#define WM_USER_DROPFILES			WM_USER + 119

struct NM_FILELISTCTRL
{
	NMHDR hdr;
	int nItemNo;
	CString sItemText;
	LPARAM lItemData;
	BOOL bStart;
	BOOL bStop;
};

struct FILELIST_ITEM
{
	FILELIST_ITEM()
	{
		rcItem.SetRectEmpty();		
		nItemNo			= INVALID_ITEM;
		nHeight			= 0;	
		lItemData		= 0;
		uImage			= 0;
		strFileName		= _T("");
		strUserName		= _T("");
		strStatus		= _T("");
		strSize			= _T("0 KB");
		strResidualTime	= _T("00:00:00");
		strElapsedTime	= _T("00:00:00");
		strSpeed		= _T("0 KB/S");
		bStart			= FALSE;
		bStop			= FALSE;
		fPercent		= 0.0;
	}

	CRect		rcItem;
	int			nItemNo;
	int			nHeight;
	LPARAM		lItemData;
	UINT		uImage;					//图标
	CString		strFileName;			//文件名
	CString		strUserName;			//用户名
	CString		strStatus;				//状态
	CString		strSize;				//文件大小
	CString		strResidualTime;		//剩余时间
	CString		strElapsedTime;			//持续时间
	CString		strSpeed;				//速度
	BOOL		bStart;					//开始
	BOOL		bStop;					//停止
	float		fPercent;				//百分之...
};


/////////////////////////////////////////////////////////////////////////////
// KFileListControl window

class AFX_EXT_CLASS KFileListControl : public CWnd
{
// Construction
public:
	KFileListControl();
	virtual ~KFileListControl();

// Operations
public:
	CString		m_strDefault;

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);

	BOOL DeleteItem(int nPos);
	void DeleteAllItems();

	CString GetFileName(int nPos);
	void SetFileName(int nPos, CString strFileName);
	void SetUserName(int nPos, CString strUserName);
	void SetProgress(int nPos, float fPercent);
	void SetFileSize(int nPos, CString strFileSize);
	void SetResidualTime(int nPos, CString strResidualTime);
	void SetElapsedTime(int nPos, CString strElapsedTime);
	void SetSpeed(int nPos, CString strSpeed);
	void SetStatus(int nPos, CString strStatus);
	void SetStartCheck(int nPos, BOOL bChecked = TRUE);
	void SetStopCheck(int nPos, BOOL bChecked = TRUE);
	void SetListTextColor(int nPos, int nType = 0, DWORD r = 255, DWORD g = 0, DWORD b = 0);

	void SetItemData(int nPos,LPARAM lItemData);
	LPARAM GetItemData(int nPos);
	void SetImage(int nPos, UINT uiImage);
	UINT GetImage(int nPos);

	FILELIST_ITEM* GetItem(int nPos);
	int	GetItemCount(){return m_listItems.GetCount();}

	void SetImageList20(CImageList *pImageList){m_pImageList20 = pImageList;}
	void SetImageList16(CImageList *pImageList){m_pImageList16 = pImageList;}
	DWORD GetExtendedStyle();
	void SetExtendedStyle(DWORD dwExStyle);
	int GetSelectedItem();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KFileListControl)
	//}}AFX_VIRTUAL

private:
	CList<FILELIST_ITEM*, FILELIST_ITEM*> m_listItems;
	CMap<int,int,FILELIST_ITEM*, FILELIST_ITEM*> m_mapItems;
	
	int					m_nTotalItems;
	COLORREF			stack[STACKSIZE];
	int					stacktop;
	
	UINT				m_nControlID;			//Controls id	
	int					m_nListHeight;			//List Height (Not the actual window height)	
	int					m_nWndWidth;			//Actual window width
	int					m_nWndHeight;

	CFont				m_font;				//GDI stuff
	COLORREF			m_clrBkSelectedItem;
	
	int					m_nSelectedItem;				//selected item

	BOOL				m_bHasFocus;

	DWORD				m_dwExtendedStyles;

	CImageList*			m_pImageList16;
	CImageList*			m_pImageList20;

	int HexDigit(TCHAR ch);
	int GetToken(LPCTSTR *String, int *Size, int *TokenLength, UINT *WhiteSpace);
	COLORREF ParseColor(LPCTSTR String);
	BOOL PushColor(HDC hdc, COLORREF clr);
	BOOL PopColor(HDC hdc);
	HFONT GetFontVariant(HDC hdc, HFONT hfontSource, int Styles);

	CRect GetItemRect(int nPos);
	int CalculateItemHeight(CString sText, UINT uImage, int nWidth);
	BOOL IsRectVisible(CRect rcClipBox,CRect rcItem);

	void ReArrangeWholeLayout();
	void ReArrangeItems();
	void EnsureVisible(int nPos);

	void SendSelectedNotify(int nPos);

	// Generated message map functions
protected:
	virtual void DrawItem(CDC *pDC,CRect rcItem, FILELIST_ITEM *pItem, BOOL bSelected);
	void DrawProgressBar(CDC *pDC, CRect rc, float fPercent);
	int DrawHTML(HDC hdc, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, BOOL bSelected = FALSE);

	//{{AFX_MSG(KFileListControl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILELISTCTRL_H__AC7C4049_9583_405D_9CBD_9F89E5C909BB__INCLUDED_)
