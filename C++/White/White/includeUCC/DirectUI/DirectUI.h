//DirectUI.h: interface for the CDirectUI class.
//
// CDirectUI				Version 1.0
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
 
#include "Objects.h"
#include "ImageTab.h"
#include "ComboBoxObj.h"  
#include "EditObjEx.h" 
#include "ButtonObj.h"
#include "TreeviewObj.h"
#include "ListviewObj.h"
#include "UIForm.h"
#include "TabCtrlObj.h"
#include "HwndObj.h"

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW       0x00020000
#endif

//////////////////////////////////////////////////////////////////////////
//class CDirectUI
typedef std::deque<CUIForm*>	DEQUE_UIFORMS;
class DIRECTUI_API CDirectUI
{
public: 
	CDirectUI();
	virtual ~CDirectUI();
	
//静态成员
public:
	static CDirectUI* s_CurDirectUI;
	
public:
	BOOL	Create(const CRect& rect,HWND hWndParent,HINSTANCE hInstance,BOOL bPopup = FALSE,BOOL bShadow = TRUE);
	void	Close();
	void	ShowAllForm(BOOL bShow);
	HWND	GetSafeHwnd(){ return m_hWnd; }
	void	SetClassStyle(UINT uStyle){ m_uClassStyle = uStyle; }
	BOOL	SetBackImage(const SkinImageBase& pImageBase);
	BOOL	SetHeaderImage(UINT uIDRes,const SkinImageBase& pImageBase);
	void	SetTransparentColor(COLORREF clrTrans){ m_clrTrans = clrTrans; }
	void	FadeOut(int nSpeed);
	void	FadeIn(int nSpeed);
	void	CalcWindowRgn();
	void	SetPicHitTestTrans(BOOL bHitTrans){ m_bPicHitTestTrans = bHitTrans; }
	BOOL	GetPicHitTestTrans(){ return m_bPicHitTestTrans; }
	SkinImageBase& GetBackImageBase();
	void	OnOK();
	CRect   GetInfoCardPos(HWND hWnd,const CPoint& pt,int nWidth,int nHeight);
	void    SetCurrentMouseObject(CObjectBase* pObjBase){ m_pCurrentMouseObject = pObjBase; }
	
	CObjectBase*  GetObjectByID(int nID);

	//////////////////////////////////////////////////////////////////////////
	//CUIForm
	void	AddUIForm(CUIForm* pUIForm);
	CUIForm* AddUIForm(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//
	CHwndObj* AddHandleObj(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//
	CTabCtrlObj* AddTabCtrlObj(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//CImageTab
	CImageTab*	AddImageTab(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//CObjectNULL
	CObjectNULL* AddObjectNULL(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//
	CListviewObj* AddListviewObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CTreeviewObj
	CTreeviewObj* AddTreeviewObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CCheckBoxObj
	CCheckBoxObj* AddCheckBoxObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CSliderBarObj
	CSliderBarObj* AddSliderBarObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CEditObj
	CEditObj*	  AddEditObj(ObjPosition& ObjPos,CEditObj::EDITTYPE eEditType = CEditObj::EDITTYPE::NORMAL,BOOL bReadOnly = FALSE,BOOL bAutoCreate = FALSE,BOOL bPassword = FALSE);
	CEditObj*	  AddEditObj(const CString& strText,ObjPosition& ObjPos, DWORD dStyle =0, CEditObj::EDITTYPE eEditType = CEditObj::EDITTYPE::NORMAL, BOOL bReadOnly = FALSE, DWORD dwExStyle = 0);
	
	//////////////////////////////////////////////////////////////////////////
	//CEditObjEx
	CEditObjEx* AddEditObjEx(ObjPosition& ObjPos,BOOL bPassword = FALSE);
	
	//////////////////////////////////////////////////////////////////////////
	//CTabButton
	CTabButton*	  AddTabButton(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CAnimateObj
	CAnimateObj*  AddAnimateObj(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//CSplitterBar
	CSplitterBar* AddSplitterBar(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//CComboBoxObj
	CComboBoxObj* AddComboBoxObj(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//CLogoObj
	CLogoObj* AddLogoObj(ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//CStaticObj
	CStaticObj* AddStaticObj(ObjPosition& ObjPos);
	CStaticObj* AddStaticObj(SkinFont* pFont,TextStyle* pTextStyle,LPCTSTR lpszText,ObjPosition& ObjPos);

	//////////////////////////////////////////////////////////////////////////
	//Picture
	CPictureObj* AddPictureObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//Buttons   
	CButtonObjEx* AddButtonObjEx(ObjPosition& ObjPos);
	CButtonObj*	  AddButtonObj(ObjPosition& ObjPos);
	
	//////////////////////////////////////////////////////////////////////////
	//All Objects
	void	AddObject(CObjectBase* pObjBase);
	BOOL	RemoveObject(CObjectBase* pObjBase);
	void    DeleteAllObject();
	
	DWORD	GetStyle();
	DWORD	SetStyle(DWORD dwStyle);
	
	DWORD	GetStyleEx();
	DWORD	SetStyleEx(DWORD dwStyle);
	
	void	SetDragabled(BOOL bDragabled);
	
	UINT	NcHitTest(CPoint point);
	
	CObjectBase* GetObjectFromPoint(const CPoint& point);
	
	void	ResizeAllObjects();

	//响应SetFocus 和 KillFocus 的对象的队列
	void	AddObjCtlClr(HWND hWnd, CObjectBase* pObj);
	BOOL	HasObjCtlClr(CObjectBase* pObj);
	void	EraseObjCtlClr(CObjectBase* pObj);
	void	ClearObjsCtlClr();

	//tooltip
	void	SetTooltip(const CString& strTooltip);
	void	SetTooltipMaxWidth(int nTooltipMaxWidth){m_nTooltipMaxWidth = nTooltipMaxWidth;}
	
	void	SetAutoDelete(BOOL bAutoDelete){m_bAutoDelete = bAutoDelete;}
	BOOL    GetAutoDelete(){return m_bAutoDelete;}
	HBITMAP CreateDIBSection(HDC hDC,int nWidth,int nHeight);
	
	//uCode值修改
	void AddCode(UINT uAdd){ m_uCodeAdd = uAdd; }
	UINT GetAddCode() { return m_uCodeAdd; }
	void DelCode(UINT uDel){ m_uCodeDel = uDel; }
	UINT GetDelCode() { return m_uCodeDel; }
	
	//设置TabUIForm
	void SetTabUIForm(CUIForm* pTabUIForm);
	CUIForm* GetTabUIForm(){ return m_pTabUIForm; }
	BOOL HasTabUIForm(){ return m_pTabUIForm!=NULL; }
	BOOL IsTabReverse(){ return m_bReverse; } //反向Tab标志
	void SetTabReverse(BOOL bReverse){ m_bReverse = bReverse; }
	
//消息处理
public:
	virtual LRESULT OnPaint(HDC hDC);
	virtual BOOL	OnEraseBkgnd(HDC hdc);
	virtual LRESULT OnNcMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnTimer(UINT nIDEvent);
	virtual void	OnDestroy();
	virtual LRESULT OnGetItemInfo(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnSetLayoutInfo(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnNcCreate(WPARAM wParam,LPARAM lParam);
	virtual BOOL	OnSize(WPARAM fwSizeType,int cx, int cy);
	virtual UINT	OnNcHitTest(CPoint point);
	
	virtual LRESULT OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual LRESULT OnSetTooltipText(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnDirectUICommand(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnCtlEditColors(HDC hdcSB,HWND hwnd,UINT nCtlColor);
	
	virtual LRESULT OnKeyDown(int nVirtKey,long lKeyData);
	virtual LRESULT OnKeyUp(int nVirtKey,long lKeyData);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnRButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnRButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnActivate(UINT nState, HWND hWndOther, BOOL bMinimized);
	virtual LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	virtual BOOL    OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnKillFocus(WPARAM wParam,LPARAM lParam);
	virtual BOOL	OnClose();
	virtual void	OnShowWindow(WPARAM wParam, LPARAM lParam);
	virtual	LRESULT GetDlgCode(WPARAM wParam, LPARAM lParam);
	virtual BOOL	OnNcActivate(BOOL bActive);
	
private:
	static	LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	HRGN	CreateTabDrawRgn(SkinImageBase* pImageBase);
	void	DrawForm(HDC hDC);
	void	RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL	RegisterDirectUIClass( void );
	void	DrawBackground(HDC hDC);
	void	DrawAllObjects(HDC hDC);
	CImage*	LoadImageFromResource(UINT uID,LPCTSTR szResourceType,SkinImageBase* pImageBase);
	void	CreateAllForm();
	
	//键盘消息钩子
	void	InstallKBHook();
	void	UnInstallKBHook();
	static LRESULT __stdcall KbHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static BOOL CheckHotKey(int nCode, WPARAM wParam, LPARAM lParam);
	void	DisposalHotKey(int nCode, WPARAM wParam, LPARAM lParam);
	
	//From的集合
public:
	DEQUE_UIFORMS 	m_UIFormContainer;
	CRect		m_rcUpdate;
	
private:
	DWORD	m_dwFlag;
	HWND	m_hWnd;
	BOOL	m_bSized;
	BOOL	m_bDragabled;
	UINT	m_nHitTest;
	UINT	m_uClassStyle;
	BLENDFUNCTION m_Blend;
	
	CObjectBase* m_pCurrObj;
	
	//resize border;
	int m_nLeftWith,m_nRightWidth,m_nTopHeight,m_nBottomHeight;
	
	//Background DC
	HBITMAP	m_hBmpBack,m_hBmpTemp;
	HDC		m_dcMem,m_dcTemp;
	int		m_nWidth,m_nHeight;
	
	//Header
	SkinImageBase* m_pImgBaseHeader;
	
	COLORREF	m_clrTrans;

	//tooltip
	CToolTipCtrl m_tooltip;
	CString		 m_strTooltip;
	int			 m_nTooltipMaxWidth;
	
	BOOL m_bSupportLayered;

	BOOL	m_bPicHitTestTrans;

	BOOL		m_bAutoDelete;

	CObjectBase* m_pCurrentMouseObject;

	//需要响应SetFocus 和 KillFocus 的对象容器
	std::map<HWND , CObjectBase*>	m_MapCtlClrObjs;

	UINT m_uCodeAdd; //WM_GETDLGCODE消息的返回值
	UINT m_uCodeDel; 
	UINT m_uDefCode; //默认Code值
	
	//当前拥有Tab控制控制权的Form //(具有Tab控制权的Form 和 具有响应 )
	CUIForm* m_pTabUIForm;
	BOOL m_bReverse; //Tab是否反向
	
	//HHOOK 
	HHOOK m_hKBHook; //键盘Hook句柄
};
