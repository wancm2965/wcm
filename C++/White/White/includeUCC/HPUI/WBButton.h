
#if !defined(AFX_WBBUTTON_H__518122CF_358F_11D4_8F4F_00402656D980__INCLUDED_)
#define AFX_WBBUTTON_H__518122CF_358F_11D4_8F4F_00402656D980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAutoFont;
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif

#define ID_LEFT		1
#define ID_RIGHT	2
#define ID_UP		3
#define ID_BOTTOM	4
#define ID_CENTER	5
#define ID_FOCFAR	6
#define ID_FOCNEAR	7
#define ID_FAR		8
#define ID_NEAR		9
#define ID_IRISNEAR 10
#define ID_IRISFAR	11

#define WM_USER_BUTTON_DOWN		WM_USER+151

// This class implements a Window Blinds like button
class AFX_EXT_CLASS CWBButton : public CButton
{

public:
	CWBButton();
	CWBButton(int nID);
	BOOL m_bOver;	//鼠标位于按钮之上时该值为true，反之为flase
	BOOL m_bTracking;	//在鼠标按下没有释放时该值为true
	int m_height,m_width; //载入位图的高度与宽度
	bool m_bRgn;
	int		m_nBtnID;
	CToolTipCtrl m_toolTip;
	CString m_strToolTip ;
// Methods
public:
    // Bitmap Loding and Initialize
	   void DrawBitmap( CDC * pDC, int mode );
    bool LoadBitmaps
    ( 
         UINT id,          // ResourceID
         int count,        // Num Of Pics
         int TopHeight,    // Top Merigin for Resizing
         int BottomHeight, // Bottom Merigin for Resizing
         int LeftWidth,    // Left Merigin for Resizing
         int RightWidth,   // Right Merigin for Resizing
		 int height,
		 int width
    );
	bool LoadBitmaps
    ( 
         const char * pBitMapFile,          // Resource file
         int count,        // Num Of Pics
         int TopHeight,    // Top Merigin for Resizing
         int BottomHeight, // Bottom Merigin for Resizing
         int LeftWidth,    // Left Merigin for Resizing
         int RightWidth,   // Right Merigin for Resizing
		 int height,
		 int width
    );

	void OnOK(void);
	void OnCancel(void);
    // Set Merigins
    void SetButtonDef( int TopHeight, int BottomHeight, int LeftWidth, int RightWidth );
	void SetToolTip(CString strToolTip);

protected:

    enum mode
    {
        normal  = 0,
        select  = 1,
        focus   = 2,
        disable = 3,
		move = 4,
		flgnormal=5,
		flgselect=6,
		flgmove=7
    };

    enum state
    {
        notInited    = 0,
        FileLoaded   = 1,
        BitmapInited = 2
    };

    int m_State;

    // Bitmaps
    bool InitBitmap( CBitmap & src, CBitmap & dist, int index, int count);
	HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent=true);

    UINT m_RcId;       // Resource ID
    int  m_NumofPics;  

    CBitmap NormalBitmap;
    CBitmap SelectBitmap;
    CBitmap FocusBitmap;
    CBitmap DisableBitmap;
	CBitmap MoveBitmap;
	CBitmap FlgNormalBitmap;
	CBitmap FlgSelectBitmap;
	CBitmap FlgMove;
	CBitmap MaskBitmap;

    int m_TopHeight; //= 8;
    int m_BottomHeight; //= 8;
    int m_LeftWidth; //= 8;
    int m_RightWidth;// = 17;

    int m_dwWidth;
    int m_dwHeight;

    // Transpararent BackColor
protected:
    COLORREF m_BkColor;
public:
    void SetBackColor( COLORREF color ) { m_BkColor = color; }
    COLORREF GetBackColor() { return m_BkColor; }

//Fonts
protected:
  //  CAutoFont * m_pFnt;
	HRGN m_rgn;
	UINT m_ID;
public:
//    CFont * GetTextFont() { return (CFont *)m_pFnt; }
//    CAutoFont * GetTextAutoFont() { return m_pFnt; }
	
	//{{AFX_VIRTUAL(CWBButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
	void ChangMod(void);
	void SetCheck(void);
	void SetCheck(bool flg);
	bool GetCheck(void);
	bool m_mode;	bool testflg;
	void SetMod(bool mode);
	virtual ~CWBButton();

	
protected:
	//{{AFX_MSG(CWBButton)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnMouseuP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SetButtonRgn(UINT ID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void SetButtonID(int nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/*
#ifndef _MEMDC_H_
#define _MEMDC_H_

//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// This class implements a memory Device Context


class CMemDC : public CDC {
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CMemDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		if (CreateCompatibleDC(pDC))
        {
            m_bitmap = new CBitmap;
            m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
            m_oldBitmap = SelectObject(m_bitmap);
            m_pDC = pDC;
            m_rcBounds = rcBounds;
            //For some reason the background color is not correct,
            //so we use the button face color.
            DWORD	color = ::GetSysColor( COLOR_BTNFACE );
            CBrush bkg(color);
            FillRect(rcBounds, &bkg);        
        }
        else
        {
            throw exception("CMemDC() error: CreateCompatibleDC()");
        }
	}
	~CMemDC() 
	{
		m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
					this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		SelectObject(m_oldBitmap);
		if (m_bitmap != NULL) delete m_bitmap;

        DeleteDC();
	}
	CMemDC* operator->() {
		return this;
	}
};

#endif

*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++

#endif // !defined(AFX_WBBUTTON_H__518122CF_358F_11D4_8F4F_00402656D980__INCLUDED_)
