//ListviewObj.h: interface for the CListviewObj class.
//
// CListviewObj				Version 1.0
//
// Created: Jan 14, 2006        Last Modified: Jan 14, 2006
//
// See the official site at www.uipower.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 2004-2006 by Haizhong Que. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// support@uipower.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#if !defined(LISTVIEWOBJ_H_INCLUDED_)
#define LISTVIEWOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "ButtonObj.h"


class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();
	
	void SetUIForm(CUIForm* pForm) {m_pUIForm = pForm;}
	CUIForm* GetUIForm(){return m_pUIForm;}
	BOOL HasUIForm(){return m_pUIForm!=NULL;}
	
protected:
	CUIForm* m_pUIForm;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMyKeyDown(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

class DIRECTUI_API CListviewObj : public CObjectBase
{
public:
	CListviewObj(HWND hWndOwner = NULL);
	virtual ~CListviewObj();

	BOOL Create(DWORD dwStyle,const CRect& rect,CWnd* pWndParent,UINT uID);
	virtual void CalcPosition(const CRect& rcParent);
	CListCtrl* GetListview(){return &m_wndListCtrl;}
	
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);
	virtual BOOL ShowRealWindow(BOOL bShow);
	
private:
	CMyListCtrl m_wndListCtrl;
};


#endif //LISTVIEWOBJ_H_INCLUDED_