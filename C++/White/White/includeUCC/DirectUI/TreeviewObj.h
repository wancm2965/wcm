//TreeviewObj.h: interface for the CTreeviewObj class.
//
// CTreeviewObj				Version 1.0
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

#if !defined(TREEVIEWOBJ_H_INCLUDED_)
#define TREEVIEWOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "ButtonObj.h"
#include "FriendTreeCtrl.h"

class DIRECTUI_API CTreeviewObj : public CObjectBase
{
public:
	CTreeviewObj(HWND hWndOwner = NULL);
	virtual ~CTreeviewObj();

	BOOL Create(DWORD dwStyle,const CRect& rect,CWnd* pWndParent,UINT uID);
	virtual void CalcPosition(const CRect& rcParent);
	CFriendTreeCtrl* GetTreeview(){return &m_wndTreeCtrl;}

	virtual BOOL ShowRealWindow(BOOL bShow);
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);

private:
	CFriendTreeCtrl m_wndTreeCtrl;
};


#endif //TREEVIEWOBJ_H_INCLUDED_