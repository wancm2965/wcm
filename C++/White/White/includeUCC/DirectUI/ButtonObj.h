//ButtonObj.h: interface for the CButtonObj class.
//
// CButtonObj				Version 1.0
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

#if !defined(BUTTONOBJ_H_INCLUDED_)
#define BUTTONOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "IDUISound.h"
#include <map>
using namespace std;

#define ID_BUTTON_FADEIN  1201
#define ID_BUTTON_FADEOUT 1202

class DIRECTUI_API CButtonObj : public CObjectBase, public YJObserver
{
public:
	CButtonObj(HWND hWndOwner = NULL);
	virtual ~CButtonObj();

public:
	SkinImageBase BtnImage[BTN_LAST];
	SkinImageBase m_btnImage;
	SkinImageBase m_btnArrowImage;
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void DrawItemText(HDC hDC,const CRect& rect);
	void ResetState(); 
	
	void	SetDropdown(BOOL bDropdown){m_bDropdown = bDropdown;}
	void	SetCheck(BOOL bCheck){m_bChecked = bCheck;Refresh(TRUE);}
	BOOL	GetCheck(){return m_bChecked;}
	
	void	SetState(DIRECTUI_BUTTONSTATE enumState){ m_enumBtnState = enumState; }
	DIRECTUI_BUTTONSTATE GetState(){return m_enumBtnState;}
	
	void	SetArrowAlign(int nHAlign,int nVAlign = -1);
	void	SetBackNoStretchRgn(DIRECTUI_BUTTONSTATE enumState,int nLeftWidth,int nRightWidth,int nTopHeight,int nBottomHeight);
	
	void	SetText(const CString& strText);
	const CString& GetText(){return m_strText;}
	
	void	SetTextFont(SkinFont* pSkinFont);
	SkinFont& GetTextFont(DIRECTUI_BUTTONSTATE enumState){return m_fontText[enumState];}
	
	void	SetTextStyle(TextStyle* pTextStyle);
	TextStyle& GetTextStyle(DIRECTUI_BUTTONSTATE enumState){return m_textStyle[enumState];}
	
	void	SetTextStyle(DIRECTUI_BUTTONSTATE enumState,int nHAlign = -1,int nVAlign = -1,COLORREF clrText = -1,
					     int nLeftOffset = -1,int nRightOffset = -1,int nTopOffset = -1,int nBottomOffset = -1);
	
	void	SetTextColor(COLORREF clrNormal,COLORREF clrHot = -1,COLORREF clrPress = -1,COLORREF clrDisabled = -1,COLORREF clrFocus = -1);
	void	SetOffset(int x,int y){m_nOffsetX = x;m_nOffsetY = y;}
	void	SetArrowOffsetPress(int nOffsetX,int nOffsetY){m_nOffsetXPress = nOffsetX;m_nOffsetYPress = nOffsetY;}
	
	void	SetBtnBkImage(const CString& strPicPath, DIRECTUI_BUTTONSTATE eBtnState);
	void	SetBtnBkImage(CImage* pImage, DIRECTUI_BUTTONSTATE eBtnState);

	void	SetSound(IDUISound*	pDUISound){m_pDUISound = pDUISound;}
	BOOL	HasSound(){ return m_pDUISound!=NULL;}
	IDUISound*	GetSound(){return m_pDUISound;}
	
	HBITMAP GetHBitmap();
	
	void	SetBtnBkImage(const CString& strPicPath);
	void	SetBtnBkImage(CImage* pImage);

	//add by wyf 06-21-2006
	//设置闪动动画
	void SetAnimateObj( CAnimateObj* pAnimateObj ); 
	BOOL HasAnimateObj() { return m_pAnimateObj!=NULL; }
	CAnimateObj* GetAnimateObj() { return m_pAnimateObj; }
	void SetRect(const CRect& rect);

	virtual void Notify( CObjectBase* pObj, int nEvent);
	virtual void SetVisible(BOOL bVisible = TRUE ,BOOL bRefresh = TRUE );
	virtual void CalcPosition(const CRect& rcParent);

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual BOOL    OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnTimer(UINT nIDEvent);
	
protected:
	DIRECTUI_BUTTONSTATE	  m_enumBtnState;
	BOOL		  m_bMouseIn;
	BOOL		  m_bLMouseDown;
	
	SkinFont	m_fontText[BTN_LAST];
	TextStyle	m_textStyle[BTN_LAST];
	CString		m_strText;
	
	int			m_nHorzAlign;//0-left,1-center,2-right
	int			m_nVertAlign;//0-top,1-vcenter,2-bottom
	int			m_nOffsetX;
	int			m_nOffsetY;
	int			m_nOffsetXPress;
	int			m_nOffsetYPress;
	BOOL		m_bDropdown;
	BOOL		m_bChecked;
	
	IDUISound*	m_pDUISound;
	CAnimateObj* m_pAnimateObj; 
};

class DIRECTUI_API CButtonObjEx : public CButtonObj 
{
public:
	CButtonObjEx(HWND hWndOwner = NULL);
	virtual ~CButtonObjEx();

public:
	virtual void DrawItemText(HDC hDC,const CRect& rect);

	void SetRightSpace(int nSpace){m_nRightSpace = nSpace;}
	int  GetRightSpace(){return m_nRightSpace;}
	
	void	SetRightText(const CString& strText);
	const CString& GetRightText(){return m_strTextRight;}

	void	SetTextFontRight(SkinFont* pSkinFont);
	SkinFont& GetTextFontRight(DIRECTUI_BUTTONSTATE enumState){return m_fontTextRight[enumState];}
	
	void	SetTextStyleRight(TextStyle* pTextStyle);
	TextStyle& GetTextStyleRight(DIRECTUI_BUTTONSTATE enumState){return m_textStyleRight[enumState];}

protected:
	SkinFont	m_fontTextRight[BTN_LAST];
	TextStyle	m_textStyleRight[BTN_LAST];
	CString		m_strTextRight;
	int			m_nRightSpace;
};

class DIRECTUI_API CCheckBoxObj : public CObjectBase
{
public:
	CCheckBoxObj(HWND hWndOwner = NULL);
	virtual ~CCheckBoxObj();

public:
	SkinImageBase m_Image;

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	
	//当本ObjectBase获得焦点后,按下空格或者回车键响应的事件
	virtual void OnOK();

	//获得焦点后的虚框
	virtual void DrawFrame();

	void	SetText(LPCTSTR lpszText){m_strText = lpszText;}
	const CString& GetText(){return m_strText;}

	void	SetThreeState(BOOL bThreeState){m_bThreeState = bThreeState;}
	BOOL	SetValue(CHECKBOX_VALUE enumValue);
	CHECKBOX_VALUE	GetValue(){return m_enumValue;}

	void	SetTextFont(SkinFont* pSkinFont);
	SkinFont& GetTextFont(DIRECTUI_CHECKBOXSTATE enumState){return m_fontText[enumState];}
	void	SetTextStyle(TextStyle* pTextStyle);
	TextStyle& GetTextStyle(DIRECTUI_CHECKBOXSTATE enumState){return m_textStyle[enumState];}
	void	SetTextStyle(DIRECTUI_CHECKBOXSTATE enumState,int nHAlign = -1,int nVAlign = -1,COLORREF clrText = -1,
					     int nLeftOffset = -1,int nRightOffset = -1,int nTopOffset = -1,int nBottomOffset = -1);
	void	SetTextColor(COLORREF clrNormal,COLORREF clrHot = -1,COLORREF clrPress = -1,COLORREF clrDisabled = -1);
	void	ResetState();

	//设置Check框与文字之间的间隔距离
	void	SetSpacePicText(int nSpace){ m_nSpacePicText = nSpace; }
	int		GetSpacePicText(){ return m_nSpacePicText; }

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);


private:
	SkinFont	m_fontText[CHECK_LAST];
	TextStyle	m_textStyle[CHECK_LAST];
	CString		m_strText;

	CHECKBOX_VALUE	m_enumValue;//0-unchecked,1-checked,2-half checked
	BOOL		m_bThreeState;
	DIRECTUI_CHECKBOXSTATE m_enumState;
	BOOL		m_bLMouseDown;
	int m_nSpacePicText;
};

struct _TabItemInfo
{
	UINT uID;
	TCHAR szTooltip[255];
	_TabItemInfo()
	{
		uID = -1;
		_tcscpy_s(szTooltip,255,_T(""));
	}
};

class DIRECTUI_API CTabButton : public CObjectBase
{
public:
	CTabButton(HWND hWndOwner = NULL);
	virtual ~CTabButton();

public:
	void SetButtonCount(int nCount){m_nButtons = nCount;}
	int  GetButtonCount(){return m_nButtons;}
	
	void AddTabItemInfo(UINT uIndex,UINT uID,LPCTSTR szTooltip);
	
	void SetSmallWidth(int nWidth){m_nSmallWidth = nWidth;}
	
	void		SetType(TABTYPE nType){m_nType = nType;}
	TABTYPE		GetType(){return m_nType;}
	
	void SetSelectedItem(int nItemIndex);
	int	 GetSelectedItem(){return m_nCurrItem;}

	void ResetItems(){m_nCurrItem = -1;m_nItemHot = -1;Refresh(TRUE);}
	
	BOOL GetExpandType(){return m_bExpand;}
	void SetExpandType(BOOL bExpand){m_bExpand = bExpand;}

public:
	SkinImageBase m_Image;

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);

protected:
	int GetHotButton(CPoint point);

private:
	int			m_nButtons;
	TABTYPE		m_nType;		//0-left,1-top,2-right,3-bottom
	int			m_nCurrItem;

	int			m_nItemHot;
	IMAGETABSTATE	m_enumState;

	BOOL		m_bExpand;
	
	int			m_nSmallWidth;

	map<UINT,_TabItemInfo*> m_mapTabItems;
};

#endif //BUTTONOBJ_H_INCLUDED_