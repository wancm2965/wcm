//ImageTab.h: interface for the CDirectUI class.
//
// CImageTab				Version 1.0
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

#if !defined(IMAGETAB_H_INCLUDED_)
#define IMAGETAB_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "ButtonObj.h"

class CImageTab;

class DIRECTUI_API CImageTabItem : public CObjectBase
{
public:
	CImageTabItem(HWND hWndOwner = NULL);
	virtual ~CImageTabItem();

public:
	SkinImageBase m_imgItem[IMAGETAB_LAST];
	CImageTab*	  m_pImageTab;

	IMAGETABSTATE GetItemState(){return m_enumState;}

	HRGN&	GetSelectRgn(){return m_rgnSelect;}
	void	SetItemState(IMAGETABSTATE enumState){m_enumState = enumState;}
	void	ResetState();
		
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual BOOL PtInWindow(CPoint point);

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);

private:
	IMAGETABSTATE m_enumState;
	HRGN	m_rgnSelect;
	int		m_nNoSelHeight;

};

typedef std::deque<CImageTabItem*>	DEQUE_IMAGETABITEM;

#define ID_IMAGETAB_SPIN_UPBUTTON		1
#define ID_IMAGETAB_SPIN_LEFTBUTTON		1
#define ID_IMAGETAB_SPIN_DOWNBUTTON		2
#define ID_IMAGETAB_SPIN_RIGHTBUTTON	2

class DIRECTUI_API CImageTab : public CObjectBase
{
public:
	CImageTab(HWND hWndOwner = NULL);
	virtual ~CImageTab();

public:
	SkinImageBase m_imgTab[IMAGETAB_LAST];
	CButtonObj	  m_btnUpObj;
	CButtonObj	  m_btnDownObj;
	HRGN		  m_rgnDraw;

//interface
public:
	void    SetSpinSpace(int nSpinSpace){m_nSpinSpace = nSpinSpace;}
	void	SetTabSpinTooltip(const CString& strUpTooltip,const CString& strDownTooltip);
	void	SetTabSpinImage(UINT uUpBtn[BTN_LAST],UINT uUpArrow[BTN_LAST],UINT uDownBtn[BTN_LAST],UINT uDownArrow[BTN_LAST],LPCTSTR szResourceType);
	void	SetHorzOffset(int nOffsetX){m_nOffsetX = nOffsetX;}
	void	SetVertOffset(int nOffsetY){m_nOffsetY = nOffsetY;}
	int		GetCurrItemIndex(){return m_nCurrIndex;}
	void	SetCurrItem(int nIndex);
	CImageTabItem* GetTabItem(int nIndex);
	CImageTabItem* GetCurrTabItem(){return m_pCurrItem;}
	void	SetSendCommand(BOOL bDowned){m_bDownSendCommand = bDowned;}

	void SetTabType(TABTYPE enumTabType){m_enumTabType = enumTabType;}
	TABTYPE GetTabType(){return m_enumTabType;}

	void SetTabState(int nTabState){m_nTabState = nTabState;}
	int  GetTabState(){return m_nTabState;}

	//Item
	void 	AddItem(UINT uItem[IMAGETAB_LAST],LPCTSTR szResourceType,const CString& strTooltip,UINT uID);
	void	RemoveAt(int nIndex);
	BOOL	RemoveAll();
	void	SetTabItemSize(const CSize& size){m_sizeTabItem = size;}
	const CSize& GetTabItemSize(){return m_sizeTabItem;}
		
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void CalcPosition(const CRect& rcParent);
	virtual void SetOwnerHwnd(HWND hWnd);
	virtual void SetMsgRecvHwnd(HWND hWnd);
//	virtual	BOOL PtInWindow(CPoint point);
	virtual void Refresh(BOOL bImmediate = TRUE );
	
public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnDirectUICommand(WPARAM wParam,LPARAM lParam);
//	virtual LRESULT OnKillFocus(WPARAM wParam,LPARAM lParam);
//	virtual LRESULT OnActivate(UINT nState, HWND hWndOther, BOOL bMinimized);

protected:
	void DrawTabItem(HDC hDC);
	void SetSpinVisible(BOOL bVisible);
	void InitializeTabItemPos(int nHeight);
	BOOL SendCommand(UINT nHitTest, CPoint point);

private:
	TABTYPE m_enumTabType;
	DEQUE_IMAGETABITEM m_dequeItem;
	int		m_nTabState;//0-active,1-disabled
	CSize	m_sizeTabItem;
	int		m_nCurrIndex;
	CImageTabItem* m_pCurrItem;
	CImageTabItem* m_pCurrItemHot;
	int		m_nOffsetX;
	int		m_nOffsetY;
	int		m_nRightSpace;
	int		m_nBottomSpace;
	BOOL	m_bUpdownBtn;
	int		m_nSpinSpace;
	BOOL	m_bDownSendCommand;

	//animate tab show
	int		m_nTopNoVisibleIndex;
	int		m_nBottomNoVisibleIndex;
};

#endif //IMAGETAB_H_INCLUDED_
