//FriendTreeCtrl.h: interface for the CFriendTreeCtrl class.
//
// CFriendTreeCtrl				Version 1.0
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

#if !defined(FRIENDTREECTRL_H_INCLUDED_)
#define FRIENDTREECTRL_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "UIForm.h"

enum Friend_Status 
{
	STATUS_ONLINE	= 1,
	STATUS_OFFLINE	= 2,
	STATUS_LEAVE	= 3,
	STATUS_MOBILE	= 4,
	STATUS_PREVENT	= 5,
	STATUS_BUSY		= 6,
	STATUS_ERROR	= 100
};

struct ItemInfo 
{
	CString				m_strID;
	Friend_Status		m_nState;
	BOOL				m_bGroup;
	
	ItemInfo(const CString& strID,Friend_Status nState,BOOL bGroup)
	{
		m_strID = strID;
		m_nState = nState;
		m_bGroup = bGroup;
	}
	
	explicit ItemInfo(const ItemInfo& rhItemInfo)
	{
		m_strID = rhItemInfo.m_strID;
		m_nState = rhItemInfo.m_nState;
		m_bGroup = rhItemInfo.m_bGroup;
	}
	
	~ItemInfo(){};
};

class DIRECTUI_API CFriendTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CFriendTreeCtrl)

public: 
	CFriendTreeCtrl();
	virtual ~CFriendTreeCtrl();

// Attributes
	enum BkMode { BK_MODE_BMP = 0, BK_MODE_GRADIENT, BK_MODE_FILL };

	BkMode GetBkMode( ) { return m_bkgd_mode; }
	void SetBkMode( BkMode bkgd_mode ) { m_bkgd_mode = bkgd_mode; }
	
// Operations
	bool ItemIsVisible( HTREEITEM item );

	HTREEITEM AddGroup(LPCTSTR lpszGroup);
	HTREEITEM AddItem(HTREEITEM hGroup,LPCTSTR lpszItemName,LPCTSTR strID);

	HTREEITEM InsertGroup(LPCTSTR lpszGroup, HTREEITEM hParent, HTREEITEM hInsertAfter);
	HTREEITEM InsertItemEx(HTREEITEM hGroup, HTREEITEM hInsertAfter, LPCTSTR lpszItemName, const ItemInfo* pItemInfo = NULL);

	void SetNodeTextColor(COLORREF clrNormal,COLORREF clrSelected){m_clrNodeText[0] = clrNormal;m_clrNodeText[1] = clrSelected;}
	void SetFriendTextColor(COLORREF clrNormal,COLORREF clrSelected){m_clrFriendText[0] = clrNormal;m_clrFriendText[1] = clrSelected;}

	BOOL SetNodeSelectedBK(UINT uSelected,LPCTSTR szResourceType);
	BOOL SetNodeImage(UINT uClosed,UINT uOpen,LPCTSTR szResourceType);
	BOOL SetBackground(UINT	uBackgroundID,LPCTSTR szResourceType);
	BOOL SetStatusImage(UINT uStatusBase,UINT uOffline,UINT uOnline,UINT uBusy,
						UINT uLeave,UINT uMobile,UINT uPrevent,LPCTSTR szResourceType);

	void SetNoStretchRgn(SkinImageBase* pImageBase,int nLeftWidth,int nRightWidth,int nTopHeight,int nBottomHeight);
	BOOL SetFriendStatus(const CString& strID,Friend_Status enumFriendStatus);
	Friend_Status GetFriendStatus(const CString& strID);
	BOOL SetFriendStatus(Friend_Status enumFriendStatus);
	BOOL SetFriendStatus(HTREEITEM hTreeItem,Friend_Status enumFriendStatus);
	CString GetFriendID(HTREEITEM hTreeItem);
	Friend_Status GetFriendStatus(HTREEITEM hTreeItem);
	ItemInfo*	GetItemInfoFromID(const CString& strID,HTREEITEM& hItem);
	
	HTREEITEM GetCurrentItem(){return m_hCurrentItem;}
	
	void ClearItemInfo();

public:
	SkinImageBase m_imageBackground;

	SkinImageBase m_nodeClosed;
	SkinImageBase m_nodeOpen;
	SkinImageBase m_nodeSelected;

	SkinImageBase m_imageStatusBase;
	SkinImageBase m_imageOffline;
	SkinImageBase m_imageMobile;

	SkinImageBase m_imageOnline;
	SkinImageBase m_imageBusy;
	SkinImageBase m_imageLeave;
	SkinImageBase m_imagePrevent;

	CUIForm*	  m_pDirectUI;
	
private:
	//////////////////////////////////////////////////////////////////////////
	//Cursors
	HCURSOR		m_hCursorNo;
	HCURSOR		m_hCursorArrow;

	//////////////////////////////////////////////////////////////////////////
	//Drag & Drop
	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;

	HTREEITEM	m_hCurrentItem;

	//////////////////////////////////////////////////////////////////////////
	BkMode  m_bkgd_mode;				// Current Background mode
	CRect m_rect;						// The client rect when drawing
	int   m_h_offset;					// 0... -x (scroll offset)
	int   m_h_size;						// width of unclipped window
	int   m_v_offset;					// 0... -y (scroll offset)
	int   m_v_size;						// height of unclipped window

	COLORREF	m_clrNodeText[2];		//0-normal,1-selected
	COLORREF	m_clrFriendText[2];		//0-normal,1-selected

	COLORREF    m_clrDescText;

	void DrawBackGround( CDC* pDC );	// Background Paint code
	void DrawItems( CDC* pDC );			// Item Paint Code
	CImage* LoadImageFromResource(UINT uID,LPCTSTR szResourceType,SkinImageBase* pImageBase);
	void DeleteAllItemData();
	HTREEITEM	MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);

	//////////////////
	std::deque<ItemInfo*> m_dequeItemInfo;
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDblclkTreeFriend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeFriend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTreeFriend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditTreeFriend(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#endif //FRIENDTREECTRL_H_INCLUDED_


