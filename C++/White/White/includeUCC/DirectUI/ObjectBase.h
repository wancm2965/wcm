//ObjectBase.h: interface for the CObjectBase class.
//
// CObjectBase				Version 1.0
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

#if !defined(OBJECTBASE_H_INCLUDED_)
#define OBJECTBASE_H_INCLUDED_

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
#include "SkinBase.h"
#include "YJObserver.h"
#include "Subclass.h"

class CUIForm;

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
	#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))
#endif

#define MSNMAINFRAME_REGISTERED 0X00200000
#define MSNMAINFRAME_SHADOW_CLASSNAME_		_T("DirectUIHWNDS")
#define MSNMAINFRAME_NOSHADOW_CLASSNAME_	_T("DirectUIHWND")


#define WM_SETTOOPTIP (WM_USER + 222)
#define WM_DIRECTUI_COMMAND (WM_USER + 223)
#define WM_SPLITTERBAR_MOUSEDOWN (WM_USER + 224)
#define WM_SPLITTERBAR_MOUSEUP (WM_USER + 225)

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED           0x00080000

	// win 2000 layered windows support
	#define LWA_COLORKEY            0x00000001
	#define LWA_ALPHA               0x00000002

	#define ULW_COLORKEY            0x00000001
	#define ULW_ALPHA               0x00000002
	#define ULW_OPAQUE              0x00000004
#endif

#pragma warning(disable : 4244)

enum _ObjType
{
	OBJTYPE_ANIMATE = 100,
	OBJTYPE_BUTTON,
	OBJTYPE_BUTTONEX,
	OBJTYPE_COMBOBOX,
	OBJTYPE_IMAGETAB,
	OBJTYPE_LOGOOBJ,
	OBJTYPE_PICTURECARD,
	OBJTYPE_PICTURE,
	OBJTYPE_SCROLLDEAD,
	OBJTYPE_SPLITTERBAR,
	OBJTYPE_STATIC,
	OBJTYPE_TREEVIEW
};

//类申明
class CObjectBase;

//////////////////////////////////////////////////////////////////////////
//类  名 : class CMsgHooker
//功  能 : 把指定句柄的消息发送到指定的CObjectBase
//////////////////////////////////////////////////////////////////////////
class CMsgHooker : public CSubclassWnd
{
public:
	CMsgHooker();
	~CMsgHooker();

public:
	void SetMsgRecv(CObjectBase* pObjBase){ m_MsgRecvObj = pObjBase; }
	BOOL HasMsgRecv() { return m_MsgRecvObj!=NULL; }

public:
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);

protected:
	CObjectBase* m_MsgRecvObj; //消息接受者容器
};


//给观察者发送的消息定义
const UINT ID_DUI_EVENT_BASE = 4000;
const UINT ID_DUI_EVNET_CALCPOS = ID_DUI_EVENT_BASE + 1; //表示重新计算了位置
const UINT ID_DUI_EVENT_SETFOCUS = ID_DUI_EVENT_BASE + 2; //表示本对象被设置了焦点

//////////////////////////////////////////////////////////////////////////
//类  名 : class CObjectBase
//
//////////////////////////////////////////////////////////////////////////

class DIRECTUI_API CObjectBase
{
public:
	CObjectBase(); 
	virtual ~CObjectBase();
	
public:
	static COLORREF s_ClrFrameItem;
	static UINT s_uPenStyle;

public:
	CUIForm* GetUIForm(){return m_pUIForm;}
	void	SetUIForm(CUIForm* pUIForm){m_pUIForm = pUIForm;}
	void	SetCursor(LPTSTR szCursor);
	void	SetCustomCursor(HINSTANCE hModule,UINT uResID);
	BOOL	GetVisible();
	virtual void	SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);
	void	SetObjectID(int nID){m_nObjID = nID;}
	int		GetObjectID(){return m_nObjID;}
	virtual void	Refresh(BOOL bVisible);
	void	SetTooltip(const CString& strTooltip){m_strTooltip = strTooltip;}
	CImage* LoadImageFromResource(UINT uID,LPCTSTR szResourceType,SkinImageBase* pImageBase);
	void	SetNoStretchRgn(SkinImageBase* pImageBase,int nLeftWidth,int nRightWidth,int nTopHeight,int nBottomHeight);
	BOOL	FadeIn(HBITMAP hBitmapStart,HBITMAP hBitmapEnd,int nTimeout);

	void	SetRect(const CRect& rect);
	const CRect& GetRect() const {return m_rect;}
	virtual void	SetPosition(const ObjPosition& ObjPos){m_objpos = ObjPos;}
	ObjPosition& GetPosition(){return m_objpos;}

	virtual BOOL ShowRealWindow(BOOL bShow){return FALSE;}
	virtual void RecalcPosition();
	virtual void CalcPosition(const CRect& rcParent);
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void SetOwnerHwnd(HWND hWnd){m_hWndOwner = hWnd;}
	virtual void SetMsgRecvHwnd(HWND hWnd){m_hWndMsgRecv = hWnd;}
	virtual	BOOL PtInWindow(CPoint point);
	void DeleteBackDC();
	
	//注册观察者 通知
	void AddObserver(YJObserver* pObserver);
	BOOL HasObserver(YJObserver* pObserver);
	void EraserObserver(YJObserver* pObserver);
	void PostNotify( int nEvent ); //给注册的观察者发送消息
	
	//uCode值修改
	void AddCode(UINT uAdd){ m_uCodeAdd = uAdd; }
	UINT GetAddCode() { return m_uCodeAdd; }
	void DelCode(UINT uDel){ m_uCodeDel = uDel; }
	UINT GetDelCode() { return m_uCodeDel; }
	UINT GetCodeDef() { return m_uCodeDef; }

	//添加设置焦点功能
	virtual void SetFocus(BOOL bFocus = TRUE);
	BOOL IsHoldFocus(){ return m_bFocus; }
	
	//设置Tab功能
	void SetTabStop(BOOL bFlag)	;
	BOOL HasTabStopFlag(){ return m_bTabStop; }
	
	//获得焦点时,按下 回车 或者 空格键 响应的事件
	virtual void OnOK(){}

	//在外部画选中框
	virtual void DrawFrame();
	
	//快捷键功能
	virtual BOOL HasHotKeyFunc() { return m_bHasHotKey; }
	
public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam){return 0;}
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point){return 0;}
	virtual LRESULT OnRButtonDown(UINT nHitTest, CPoint point){return 0;}
	virtual LRESULT OnRButtonUp(UINT nHitTest, CPoint point){return 0;}
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnKillFocus(WPARAM wParam,LPARAM lParam){return 0;}
	virtual LRESULT OnActivate(UINT nState, HWND hWndOther, BOOL bMinimized){return 0;}
	virtual LRESULT OnDUICommand(WPARAM wParam,LPARAM lParam){return 0;}
	virtual LRESULT OnTimer(UINT nIDEvent){return 0;}
	virtual LRESULT OnKeyDown(int nVirtKey,long lKeyData){return 0;}
	virtual LRESULT OnReflectCtlClr(WPARAM wParam, LPARAM lParam){return 0;}
	virtual BOOL WindowProc(UINT msg, WPARAM wp, LPARAM lp, LRESULT& lResult);
	
public:
	CRect	m_rect;
	HWND	m_hWndOwner;
	HWND	m_hWndMsgRecv;
	BOOL	m_bVisible;
	int		m_nObjID;
	
protected:
	CMsgHooker m_MsgHooker; //如果Obj内带HWnd
	UINT m_uCodeAdd; //WM_GETDLGCODE消息的返回值
	UINT m_uCodeDel; 
	UINT m_uCodeDef; //记录 WM_GETDLGCODE 的默认返回值
	BOOL m_bFocus; //是否被设置了焦点
	BOOL m_bTabStop; //此ObjBase是否是TabStop

protected:
	//position infomation
	ObjPosition m_objpos;
	int x,y,m_nWidth,m_nHeight;
	HCURSOR		m_hCursor;
	CString		m_strTooltip;
	CUIForm*	m_pUIForm;
	
protected:
	HDC		m_hDC;
	HBITMAP	m_hBitmap;
	
	std::deque<YJObserver*> m_dequeObservers;

public:
	BOOL m_bHasHotKey;
	BOOL m_bHotKeyNeedCtrl;
	BOOL m_bHotKeyNeedShift;
	BOOL m_bHotKeyNeedAtl;
	UINT m_uHotKeyCode;
};

#endif //OBJECTBASE_H_INCLUDED_

