//TabCtrlObj.h: interface for the CTabCtrlObj class.
//
// CTabCtrlObj  				Version 1.0
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

#if !defined(TABCTRLOBJ_H_INCLUDED_)
#define TABCTRLOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"
#include "ButtonObj.h"

#define ID_TABCTRL_SPIN_UPBUTTON		1
#define ID_TABCTRL_SPIN_LEFTBUTTON		2
#define ID_TABCTRL_SPIN_DOWNBUTTON		3
#define ID_TABCTRL_SPIN_RIGHTBUTTON		4

class CTabCtrlObj;

class DIRECTUI_API CTabCtrlItem : public CObjectBase , public YJObserver
{
public:
	CTabCtrlItem(HWND hWndOwner,CTabCtrlObj* pTabCtrlObj);
	virtual ~CTabCtrlItem();
	
	SkinImageBase m_image[BTN_LAST];//Tab页面上按钮的图标
	
public:
	CSize	GetItemSize();
	void	SetItemText(LPCTSTR strText);

	TABCTRLSTATE GetItemState(){return m_enumState;}
	void	SetItemState(TABCTRLSTATE enumState){m_enumState = enumState;}
	
	void	ResetState();
	
	void SetRect(const CRect& rect);

	//设置按钮的布局信息
	void	SetPositionBtnClose(ObjPosition& ObjPos){ m_PosBtnClose = ObjPos; } 
	ObjPosition& GetPositionBtnClose() { return m_PosBtnClose; }
	const ObjPosition& GetPositionBtnClose() const { return m_PosBtnClose; }
	
	//设置按钮的状态
	void SetBtnState(DIRECTUI_BUTTONSTATE eBtnState){ m_eBtnState = eBtnState; }
	DIRECTUI_BUTTONSTATE GetBtnState() { return m_eBtnState; }
	
	//设置闪动动画
	void SetAnimateObj( CAnimateObj* pAnimateObj ); 
	BOOL HasAnimateObj() { return m_pAnimateObj!=NULL; }
	CAnimateObj* GetAnimateObj() { return m_pAnimateObj; }
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void Notify( CObjectBase* pObj, int nEvent);
	virtual void SetVisible(BOOL bVisible /* = TRUE */,BOOL bRefresh /* = TRUE */);
	
public:
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnTimer(UINT nIDEvent);
	
private:
	CRect GetBtnRect();
	void  CalcPositionOnly(const CRect& rcParent);
	
private:
	CString		m_strText;
	CTabCtrlObj* m_pTabCtrlObj;
	TABCTRLSTATE m_enumState;
	int			m_nIconOffsetX;
	int			m_nIconOffsetY;
	CSize		m_sizeText;
	
	ObjPosition m_PosBtnClose;	//关闭按钮矩形位置
	DIRECTUI_BUTTONSTATE m_eBtnState;	//关闭按钮状态
	BOOL		m_bBtnDown;	//记录鼠标是否在Btn内被按下
	
	CAnimateObj* m_pAnimateObj; //闪动的动画
};

typedef std::deque<CTabCtrlItem*>	DEQUE_TABCTRLITEM;

class DIRECTUI_API CTabCtrlObj : public CObjectBase, public YJObserver
{
	//begin add by wyf 5.18
	//类型定义
	enum COMMANDMSGTYPE
	{
		CMD_MOUSEMOVE = 0,
		CMD_LBUTTONDOWN,
		CMD_LBUTTONUP,
		CMD_RBUTTONDOWN
	};
	//end add
	
public:
	CTabCtrlObj(HWND hWndOwner = NULL); 
	virtual ~CTabCtrlObj();
	
	void GetTabItemNoStretch(int& nLeftWidth,int& nRightWidth,int& nTopHeight,int& nBottomHeight);
	
	void SetSpinSpace(int nSpinSpace){ m_nSpinSpace = nSpinSpace; }
	void SetTabSpinTooltip(const CString& strUpTooltip,const CString& strDownTooltip);
	
	void SetTabType(TABTYPE enumTabType){ m_enumTabType = enumTabType; }
	TABTYPE GetTabType(){ return m_enumTabType; }

	int GetCurrItemIndex();	//{return m_nCurrIndex;}
	void SetCurrItem(int nIndex);
	
	//Item
	int GetItemCount(){ return (int)m_dequeTabItem.size(); }
	CTabCtrlItem* GetTabItem(int nIndex);
	CTabCtrlItem* GetTabItemByID(int nID);
	BOOL HasTabItemID(int nID);
	CTabCtrlItem* GetCurrTabItem(){ return GetItem(TABCTRL_SELECTED); }
	CTabCtrlItem* AddItem(const CString& strText,const CString& strTooltip,int nID);
	CTabCtrlItem* InsertItem(int nIndex,const CString& strText,const CString& strTooltip,UINT uID);
	BOOL RemoveAt(int nIndex);
	BOOL RemoveItem(UINT uID);
	BOOL RemoveAll();
	//nStartShow 显示的起点Item的索引值　nEndShow　可以显示的终点的索引值,如果为-1表示没有Item被显示
	void GetShowItemRange( int& nStartShow, int& nEndShow);
	
	//SolidItem
	int GetSolidItemCount(){ return (int)m_dequeSolidItem.size(); }
	CTabCtrlItem* GetSolidItem(TABCTRLSTATE eTabItemState);
	CTabCtrlItem* GetSolidItemByIndex( int nIndex ); //这个nIndex指的是SolidItem容器内的索引值
	CTabCtrlItem* GetSolidItemByID( int nID ); 
	CTabCtrlItem* AddSolidItem(const CString& strText, const CString& strTooltip, int nID);
	CTabCtrlItem* InsertSolidItem(int nIndex, const CString& strText, const CString& strTooltip, UINT uID);
	BOOL RemoveSolidItemAt(int nIndex);
	BOOL RemoveSolidItem(UINT uID);
	void RemoveAllSolidItem();
	int CalcSolidItemVisibleCount();
	void CalcSolidItemPos();

	//闪动(在自动大小的确时候不能不用)
	void SetItemFlash(int nID, CAnimateObj* pAnimateObj);
	void SetSolidItemFlash(int nID, CAnimateObj* pAnimateObj);
	void SetSpinFlash(BOOL bLeft, CAnimateObj* pAnimateObj);
	
	//自动大小
	void	SetAutosize(BOOL bAutosize){m_bAutosize = bAutosize;}
	BOOL	GetAutosize(){return m_bAutosize;}
	void	SetItemSize(const CSize& sizeItem){m_sizeItem = sizeItem;}
	const CSize& GetItemSize(){return m_sizeItem;}
	
	//add by wyf
	void NoteCloseItemID(int nCloseItemID){ m_nCloseItemID = nCloseItemID; }	//记录被按下关闭按钮的Item的ID
	void SetUIForm(CUIForm* pUIForm);
	void Notify(CObjectBase* pObj, int nEvent);

public:
	SkinImageBase m_imgTab;		//Tab页 0-NORMAL,1-PRESS,2-DISABLE,3-HOT,4-SELECTED
	SkinImageBase m_imgBack;	//Tab背景
	CButtonObj	  m_btnLeftObj;	
	CButtonObj	  m_btnRightObj;
	
	SkinFont      m_font[TABCTRL_LAST];
	TextStyle	  m_textStyle[TABCTRL_LAST];
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void CalcPosition(const CRect& rcParent);
	virtual void SetOwnerHwnd(HWND hWnd);
	virtual void SetMsgRecvHwnd(HWND hWnd);
	
public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnDUICommand(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnTimer(UINT nIDEvent);
	
private:
	void	DrawBk(HDC hDC,CPoint ptOffset = CPoint(0,0));
	void	DrawArrowBtn(HDC hDC, CPoint ptOffset = CPoint(0,0));
	void	DrawTabItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	void	DrawTabItem(HDC hDC,CTabCtrlItem* pItem,CPoint ptOffset = CPoint(0,0));
	int		GetItemLength();
	int		GetTabRightArea();
	int		GetItemLength(int nStart,int nEnd);
	void	GetItemPosition(int& nStart,int& nEnd,int nWorkArea);
	void	CalcItemPosition(int nStart,int nEnd,int nWorkArea);
	BOOL	SendCommand(UINT nHitTest, CPoint point, COMMANDMSGTYPE eCmdMsgType);
	void	SetSpinVisible(BOOL bVisible);
	
	//add by wyf 5.18
	BOOL	HasItem(TABCTRLSTATE eTabItemState);
	CTabCtrlItem*	GetItem(TABCTRLSTATE eTabItemState);
	void	EraseItem(DEQUE_TABCTRLITEM& ItemContainer , int nItemID);
	void	AutoSetSelectItem(); //选中m_dequeItemsOrder中最后的Item
	int 	GetSolidItemLength(); //得到SolidItem所需要的长度
	void	DrawSolidItem(HDC hDC,CPoint ptOffset = CPoint(0,0)); //画出所有SolidItem
	
private:
	BOOL	m_bAutosize;
	CSize   m_sizeItem;
	
	DEQUE_TABCTRLITEM m_dequeTabItem;
	TABTYPE m_enumTabType;
	
	int		m_nCurrIndex;
	CTabCtrlItem* m_pCurrItem;
	CTabCtrlItem* m_pCurrItemHot;
	
	int		m_nLeftHide;
	int		m_nRightHide;
	
	int		m_nSpinSpace;
	BOOL	m_bDownSendCommand;
	BOOL	m_bSpinBtnVisible;
	
	//add by wyf 5-18
	BOOL	m_bOldMouseIn;
	DEQUE_TABCTRLITEM m_dequeItemsOrder;	//记录TabItem选中的顺序
	int		m_nCloseItemID;					//记录哪个TabItem的关闭按钮被按下了;
	
	//add by wyf 6-17
	DEQUE_TABCTRLITEM m_dequeSolidItem; //固定在前面的Items
	int m_nSolidVisibleCount; //可见的SolidItem的数目
	
	//add by wyf 6-18
	CString m_strAnimateSeg; //动画资源字段
	
};



#endif //TABCTRLOBJ_H_INCLUDED_