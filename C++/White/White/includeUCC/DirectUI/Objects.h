//Objects.h: interface for the Objects class.
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

#if !defined(OBJECTS_H_INCLUDED_)
#define OBJECTS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
#include "ObjectBase.h"
#include "Subclass.h"

enum DIRECTUI_BUTTONSTATE 
{
	BTN_NORMAL	= 0,
	BTN_PRESS	= 1,
	BTN_DISABLE	= 2,
	BTN_HOT		= 3,
	BTN_FOCUS   = 4,
	BTN_LAST	= 5
};

enum DIRECTUI_CHECKBOXSTATE
{
	CHECK_NORMAL	= 0,
	CHECK_HOT		= 1,
	CHECK_PRESS		= 2,
	CHECK_DISABLE	= 3,
	CHECK_LAST		= 4
};

enum CHECKBOX_VALUE
{
	CHECKBOX_UNCHECKED	 = 0,
	CHECKBOX_CHECKED	 = 1,
	CHECKBOX_HALFCHECKED = 2
};

enum IMAGETABSTATE 
{
	IMAGETAB_NORMAL	  = 0,
	IMAGETAB_PRESS	  = 1,
	IMAGETAB_DISABLE  = 2,
	IMAGETAB_HOT	  = 3,
	IMAGETAB_SELECTED = 4,
	IMAGETAB_LAST	  = 5
};

enum TABCTRLSTATE
{
	TABCTRL_NORMAL		= 0,
	TABCTRL_PRESS		= 1,
	TABCTRL_DISABLE		= 2,
	TABCTRL_HOT			= 3,
	TABCTRL_SELECTED	= 4,
	TABCTRL_LAST		= 5
};

enum TABTYPE
{
	TABTYPE_LEFT	= 0,
	TABTYPE_TOP		= 1,
	TABTYPE_RIGHT	= 2,
	TABTYPE_BOTTOM	= 3
};

class CButtonObj;

typedef std::deque<CObjectBase*>	DEQUE_OBJECT;

class DIRECTUI_API CPictureObj : public CObjectBase
{
public:
enum PICSTATE 
{
	PIC_ACTIVE	 = 0,
	PIC_INACTIVE = 1,
	PIC_LAST	 = 2
};
	CPictureObj(HWND hWndOwner);
	virtual ~CPictureObj();

public:
	SkinImageBase m_PicImage[PIC_LAST];//0-Active,1-Inactive
	void	SetTransparent(COLORREF clrTrans,BOOL bTrans);
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	
private:
	PICSTATE m_enumPicState;
};

class DIRECTUI_API CScrollDead : public CPictureObj
{
public:
	CScrollDead(HWND hWndOwner);
	virtual ~CScrollDead();
};

class DIRECTUI_API CStaticObj : public CObjectBase
{
public:
enum STATICSTATE
{
	STATIC_ACTIVE   = 0,
	STATIC_INACTIVE = 1,
	STATIC_LAST     = 2
};
	CStaticObj(HWND hWndOwner);
	virtual ~CStaticObj();

public:
	void	SetText(const CString& strText){m_strText = strText;Refresh(TRUE);}
	const CString& GetText(){return m_strText;}

	void	SetTextFont(SkinFont* pSkinFont);
	SkinFont& GetTextFont(STATICSTATE enumState){return m_fontText[enumState];}
	
	void	SetTextStyle(TextStyle* pTextStyle);
	TextStyle& GetTextStyle(STATICSTATE enumState){return m_textStyle[enumState];}

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	
private:
	STATICSTATE m_enumStaticState;
	SkinFont	m_fontText[STATIC_LAST];
	TextStyle	m_textStyle[STATIC_LAST];
	CString		m_strText;
};

//Edit��֪ͨ�¼�
#define ID_EVENT_RETURN_DOWN 1040

class DIRECTUI_API CEditObj : public CObjectBase
{
public:
enum EDITSTATE
{
	EDIT_ACTIVE   = 0,
	EDIT_INACTIVE = 1,
	EDIT_LAST     = 2
};
enum EDITTYPE
{
	NORMAL		= 0,
	RICHEDIT	= 1,
	RICHEDIT2	= 2
};

enum BKBRUSHTYPE
{
	DEFAULT_TYPE = 0,
	COLOR_TYPE = 1,
	IMAGE_TYPE = 2
};

	CEditObj(HWND hWndOwner, DWORD dStyle = ES_AUTOHSCROLL|WS_CHILDWINDOW|WS_TABSTOP|ES_LEFT, 
		     EDITTYPE eEditType = EDITTYPE::NORMAL, DWORD dwExStyle = 0,BOOL bAutoCreate = FALSE,BOOL bPassword = FALSE);
	
	virtual ~CEditObj();
	
public:
	SkinImageBase m_EditBkImage[EDIT_LAST];

public:
	DWORD	GetEditCtrlStyle();
	BOOL	SetEditCtrlStyle(DWORD dwStyle);
	void	SetAutoCreate(BOOL bAutoCreate){m_bAutoCreate = bAutoCreate;}
	void	SetText(const CString& strText);
	void	SetBKText(CString strText = L""){m_strBKText = strText;}
	const CString& GetText();
	void	SetStatus(EDITSTATE enumState){m_enumEditState = enumState;}
	EDITSTATE GetStatus(){return m_enumEditState;}
	void	SetReadOnly(BOOL bReadOnly);
	BOOL	GetReadOnly(){return m_enumEditState == EDIT_INACTIVE ? TRUE : FALSE;}
	SkinFont& GetTextFont(EDITSTATE enumState){return m_fontText[enumState];}
	TextStyle& GetTextStyle(EDITSTATE enumState){return m_textStyle[enumState];}
	BOOL SetTextFont(EDITSTATE enumState,int nHeight, BOOL bBold, BOOL bItalic, const CString& strFaceName);
	void SetTextFont(EDITSTATE enumState, SkinFont& Font);
	void SetTextColor(COLORREF clrActive,COLORREF clrInactive);
	void SetBKTextColor(COLORREF clr){m_clrBKText = clr;}
	HWND	GetEditHWnd();

	void	SetEditType(EDITTYPE enumType){ m_eEditType = enumType; }	//ֻ��CreateEditCtrl����֮ǰʹ����Ч
	EDITTYPE GetEditType(){return m_eEditType; };

	void	SetEditTypeEx(DWORD dwStyleEx){m_dwExStyle = dwStyleEx; }	//ֻ��CreateEditCtrl����֮ǰʹ����Ч
	DWORD	GetEditTypeEx(){ return m_dwExStyle; }
	void	CreateEditCtrl();
	virtual BOOL ShowRealWindow(BOOL bShow);

	//���ñ�����ˢ����
	const BKBRUSHTYPE GetBkBrushType()const {return m_eBkBrushType;}
	const COLORREF GetBkBrushColor()const {return m_clrBkBrush;}
	const HBRUSH GetBkBrush()const {return m_hBrush_Bk;}

	void SetBkBrushType(BKBRUSHTYPE eBrushType) {m_eBkBrushType =  eBrushType;}
	void SetBkBrushColor(COLORREF bkClr){m_clrBkBrush = bkClr;}
	void SetBkBrush(HBRUSH hBrush_Bk) {m_hBrush_Bk = hBrush_Bk;}

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void CalcPosition(const CRect& rcParent);
	virtual void SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);
	virtual void SetFocus(BOOL bFocus);
	virtual void DrawFrame(){}


public:
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnKeyDown(int nVirtKey,long lKeyData);
	virtual LRESULT OnReflectCtlClr(WPARAM wParam, LPARAM lParam);
	
	//�Լ�����Ϣ
	virtual BOOL WindowProc(UINT msg, WPARAM wp, LPARAM lp, LRESULT& lResult);
	
protected:
	void	UpdateData();
	void	DrawBk(HDC hdc, CPoint ptOffset = CPoint(0,0));
	HBRUSH CreateBkBrushByImage( HDC hDC, HBITMAP hBitmap );
	HBRUSH CreateBkBrushByColor( COLORREF bkClr );
	
private:
	EDITSTATE	m_enumEditState;
	SkinFont	m_fontText[EDIT_LAST];
	TextStyle	m_textStyle[EDIT_LAST];
	CString		m_strText;
	CString		m_strBKText;
	COLORREF	m_clrBKText;
	HWND		m_hWndEdit;
	DWORD		m_dwStyle;
	BOOL		m_bPassword;
	BOOL		m_bAutoCreate;
	EDITTYPE	m_eEditType;
	DWORD		m_dwExStyle;

	BKBRUSHTYPE	m_eBkBrushType;
	BOOL		m_bCreateBrush;
	HBRUSH		m_hBrush_Bk;
	COLORREF	m_clrBkBrush;
};

class DIRECTUI_API CEditObjEx : public CObjectBase
{
public:
enum EDITEXSTATE 
{
	EDITEX_NORMAL	  = 0, 
	EDITEX_PRESS	  = 1,
	EDITEX_DISABLE  = 2,
	EDITEX_HOT	  = 3,
	EDITEX_LAST	  = 4
};

enum BKBRUSHTYPE
{
	DEFAULT_TYPE = 0,
	COLOR_TYPE = 1,
	IMAGE_TYPE = 2
};
	CEditObjEx(HWND hWndOwner = NULL,BOOL bPassword = FALSE);
	virtual ~CEditObjEx();

public:
	SkinImageBase	m_imageEditBK[EDITEX_LAST];

public:
	void	SetTextFont(const SkinFont& fontText);
	SkinFont& GetTextFont();
	const SkinFont& GetTextFont() const ;
	void	SetEditTooltip(const CString& strTooltip){m_strEditTip = strTooltip;}
	
	void	SetAutoResize(BOOL bAutoResize){m_bAutoResize = bAutoResize;} 
	const CString& GetText();
	void	SetText(const CString& strText);
	const CString& GetDefText(){return m_strDefText;}
	void	SetDefText(const CString& strDefText){m_strDefText = strDefText;}
	void	SetTextColor(EDITEXSTATE enumState,COLORREF clrText){m_clrText[enumState] = clrText;}
	void	SetDefTextColor(COLORREF clrDefText){m_clrDefText = clrDefText;}
	void	SetItemImage(CImage* pImage){m_pItemImage = pImage;}
	
	void	SetPassword(BOOL bPassword){m_bPassword = bPassword;}
	void	SetItemText(const CString& strText,UINT uID);
	TextStyle& GetTextStyle(EDITEXSTATE enumState){return m_textStyle[enumState];}
	void	SetEditImage(SkinImageBase* pImgEdit);
	
	HIMAGELIST GetImageList(){return m_hImageList;}
	void	SetImageList(HIMAGELIST hImageList){m_hImageList = hImageList;}
	void	SetEditBackNoStretchRgn(EDITEXSTATE enumState,int nLeftWidth,int nRightWidth,
								   int nTopHeight,int nBottomHeight);
	
	//����Disable״̬
	void SetDisable(BOOL bDisable = TRUE, BOOL bImmediate = TRUE);
	BOOL IsDisable() { return m_bDisable; }
	
	//�ı�Edit�ķ��
	DWORD GetEditStyle();
	void SetEditStyle(DWORD dwEStyle);
	void ModifyEditStyle(DWORD dwAddStyle, DWORD dwDelStyle);
	
	//�õ��༭����
	HWND GetEditHWnd();
	BOOL IsAutoDestory(){return m_bAutoDestory; }
	void SetAutoDestory(BOOL bAutoDestory) { m_bAutoDestory = bAutoDestory; }

	//���ñ�����ˢ����
	const BKBRUSHTYPE GetBkBrushType()const {return m_eBkBrushType;}
	const COLORREF GetBkBrushColor()const {return m_clrBkBrush;}
	const HBRUSH GetBkBrush()const {return m_hBrush_Bk;}

	void SetBkBrushType(BKBRUSHTYPE eBrushType) {m_eBkBrushType =  eBrushType;}
	void SetBkBrushColor(COLORREF bkClr){m_clrBkBrush = bkClr;}
	void SetBkBrush(HBRUSH hBrush_Bk) {m_hBrush_Bk = hBrush_Bk;}

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void DrawItemText(HDC hDC,const CRect& rect);
	virtual void CalcPosition(const CRect& rcParent);
	virtual	BOOL PtInWindow(CPoint point);
	virtual	void SetFocus(BOOL bFocus = TRUE);
	virtual void SetVisible(BOOL bVisible = TRUE ,BOOL bRefresh = TRUE );
	virtual void DrawFrame(){}

public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnKillFocus(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnActivate(UINT nState, HWND hWndOther, BOOL bMinimized);
	virtual LRESULT OnKeyDown(int nVirtKey,long lKeyData );
	virtual LRESULT OnReflectCtlClr(WPARAM wParam, LPARAM lParam);
	virtual BOOL WindowProc(UINT msg, WPARAM wp, LPARAM lp, LRESULT& lResult);

protected:
	void Refresh(BOOL bImmediate = TRUE);
	void ResetState();
	void RecalcPos();
	void UpdateData();
	void CreateEdit();
	void OnEditLButtonDown(WPARAM wp, LPARAM lp);
	HBRUSH CreateBkBrushByImage( HDC hDC, HBITMAP hBitmap );
	HBRUSH CreateBkBrushByColor( COLORREF bkClr );
	
private:
	EDITEXSTATE m_enumEditState;
	CString m_strCurrText;
	UINT m_uCurrentID;
	CString m_strDefText;
	int m_nCurrIndex;
	COLORREF m_clrText[EDITEX_LAST];
	COLORREF m_clrDefText;
	TextStyle m_textStyle[EDITEX_LAST];
	CImage* m_pItemImage;
	SkinFont m_fontText;
	BOOL m_bAutoResize;
	CRect m_rcEdit;
	BOOL	m_bPassword;

	HIMAGELIST m_hImageList;

	CString m_strEditTip;

	BOOL m_bLMouseDown;
	HWND m_hWndEdit;
	BOOL m_bDisable;
	
	DWORD m_dwEditStyle; //�༭��ķ��
	BOOL m_bAutoDestory; //UpData���Ƿ��Զ�Destory �༭��
	
	BOOL m_bFirstKnockEdit; //�Ƿ��ǵ�1���û��༭������

	BKBRUSHTYPE	m_eBkBrushType;
	BOOL m_bCreateBrush;
	HBRUSH m_hBrush_Bk;
	COLORREF m_clrBkBrush;
};

class DIRECTUI_API CLogoObj : public CObjectBase
{
public:
	CLogoObj(HWND hWndOwner);
	virtual ~CLogoObj();

	void SetTransColor(COLORREF clrBorder,COLORREF clrInner);
	void SetBottomHeight(int nHeight){m_nBottomHeight = nHeight;}
	void SetBackNoStretchRgn(int nLeftWidth,int nRightWidth,int nTopHeight,int nBottomHeight);
	void SetLogoImage( LPCTSTR szFileName );
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	
public:
	SkinImageBase m_LogoImage;
	SkinImageBase m_BorderImage;
	SkinImageBase m_BottomImage;
	
private:
	COLORREF	  m_clrTransBorder;
	COLORREF	  m_clrTransInner;
	int			  m_nBottomHeight;
};

typedef std::deque<CObjectBase*>	DEQUE_OBJECT;

class DIRECTUI_API CPictureCard : public CObjectBase
{
public:
	CPictureCard(HWND hWndOwner);
	virtual ~CPictureCard();

public:
	SkinImageBase m_LogoImage;
	SkinImageBase m_BorderImage;

public:
	CButtonObj* AddButtonObj(UINT uIDNORMALEDIT,UINT uIDPress,UINT uIDHot,UINT uIDDisabled,
							 UINT uIDFocus,LPCTSTR szResourceType,ObjPosition& ObjPos);
	void SetInnerRect(CRect rcInner){m_rectInner = rcInner;}
	CRect& GetInnerRect(){return m_rectInner;}

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual BOOL	OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);

protected:
	BOOL AddButtonObj(UINT uIDNORMALEDIT,UINT uIDPress,UINT uIDHot,UINT uIDDisabled,
		              UINT uIDFocus,LPCTSTR szResourceType,ObjPosition& ObjPos,CButtonObj* pBtnObj);
	
private:
	COLORREF	  m_clrTransBorder;
	COLORREF	  m_clrTransInner;

	CRect		  m_rectInner;
	DEQUE_OBJECT  m_dequeObject;
	
};

class DIRECTUI_API CSplitterBar : public CObjectBase
{
public:
	CSplitterBar(HWND hWndOwner);
	virtual ~CSplitterBar();

public:	
	SkinImageBase m_imageBar[BTN_LAST-1];//0-normal,1-press,2-disable,3-hot
	SkinImageBase m_imageArrow[BTN_LAST-1];//0-normal,1-press,2-disable,3-hot

public:
	void SetBarPressColor(COLORREF clrPress){m_clrBarPress = clrPress;}
	void SetSplitterType(BOOL bHorz){m_bHorz = bHorz;}
	void SetSplitterHorzMin(int nTopMin,int nBottomMin){m_nTopMin = nTopMin;m_nBottomMin = nBottomMin;}
	void SetSplitterVertMin(int nLeftMin,int nRightMin){m_nTopMin = nLeftMin;m_nBottomMin = nRightMin;}
	void SetUpDownControls(DEQUE_OBJECT* UpControls,DEQUE_OBJECT* DownControls){m_dequeUpControls = UpControls;m_dequeDownControls = DownControls;}

	int GetTopLeftMin(){ return m_nTopMin; }
	int GetBottomRightMin(){ return m_nBottomMin; }
	BOOL AdjustObjPos();
	
public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));

public:
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	
private:
	void	MoveObjects();
	void	ResetState();
	void	SetClipStyle(BOOL bSet);
	void	DrawBar();

private:
	BOOL	m_bHorz;
	COLORREF m_clrBarPress;
	DIRECTUI_BUTTONSTATE	m_enumBtnState;
	BOOL m_bLMouseDown;
	CPoint m_ptOld,m_pt;
	CRect m_rcOld;
	int	 m_nTopMin,m_nBottomMin;
	DEQUE_OBJECT* m_dequeUpControls;
	DEQUE_OBJECT* m_dequeDownControls;
};

//////////////////////////////////////////////////////////////////////////
//��  �� : CTimerIDAlloc
//��  �� : ������ɶ�ʱ��ID(����ID�ظ�)
//////////////////////////////////////////////////////////////////////////
class DIRECTUI_API CTimerIDAlloc
{
public:
	static CTimerIDAlloc* GetInstance();
	int GetID();
	
	~CTimerIDAlloc(){}

private:
	CTimerIDAlloc();
			
private:
	int m_IDBase;
};

//define Event Id
#define ID_EVENT_PLAY_FRAME 1030
#define ID_EVENT_STOP_FRAME 1031

class DIRECTUI_API CAnimateObj : public CObjectBase 
{
public:
	CAnimateObj(HWND hWndOwner); 
	virtual ~CAnimateObj(); 

public:
	void	SetFramesCount(int nCount){m_nCount = nCount;}
	int		GetFramesCount(){return m_nCount;}

	void	SetSpeed(int nSpeed){m_nSpeed = nSpeed;}
	int		GetSpeed(){return m_nSpeed;}
	
	void	StartAnimate();
	void	StopAnimate();
	
	void	SetAutoStop(BOOL bAutoStop = TRUE, UINT uElapse = 2000 );
	int GetPlayEventID(){ return m_nPlayEventID; }
	int GetStopEventID(){ return m_nStopEventID; }
	
public:
	SkinImageBase m_imageBar;

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual void CalcPosition(const CRect& rcParent);

public:
	virtual LRESULT OnTimer(UINT nIDEvent);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	
private:
	void	DrawItemAnimate(HDC hDC);

private:
	int		m_nCount;
	int		m_nSpeed;
	int		m_nCurrFrame;
	BOOL	m_bAnimating;
	BOOL	m_bAutoStop;
	UINT_PTR m_nPlayEventID;
	UINT	m_uAutoStopElapse;
	UINT_PTR m_nStopEventID;
};

class DIRECTUI_API CSliderBarObj : public CObjectBase  
{
	friend class CUIForm;
//��������:
public:
	static	const	double	EPS;
//���Ͷ���
public:
	enum SLIDERTYPE
	{
		SLIDER_HORZ_L = 0, //ˮƽ����,���Ϊ��ֵ���
		SLIDER_HORZ_R, //ˮƽ����,�Ҳ�Ϊ��ֵ���
		SLIDER_VERT_T,	//��ֱ����,����Ϊ��ֵ���
		SLIDER_VERT_B //��ֱ����, �ײ�Ϊ��ֵ���
	};
		
	enum SLIDERSTATE 
	{
		SLIDER_NORMAL	 = 0,
		SLIDER_DISABLE = 1,
		SLIDER_LAST	 = 2
	};
	
public:
	CSliderBarObj(HWND hWndOwner = NULL);
	virtual ~CSliderBarObj();
	
public:
	SkinImageBase	m_imageBK[SLIDER_LAST];		//0-normal,1-disable
	SkinImageBase	m_imageState[SLIDER_LAST];	//0-normal,1-disable	//��ʾ״̬�ı���,��������״̬����ʾ����˵����������С
	SkinImageBase	m_imageFront[SLIDER_LAST];	//0-normal,1-disable
	SkinImageBase	m_imageBtn[SLIDER_LAST];	//0-normal,1-disable
	
public:
	//������ָʾ��ֵ�ķ�Χ
	void	GetRangeVal(int& nMin, int& nMax) const;
	void	SetRangeVal(int nMin, int nMax, BOOL bRedraw = FALSE);
	void	SetRangeMinVal(int nMin, BOOL bRedraw = FALSE);
	void	SetRangeMaxVal(int nMax, BOOL bRedraw = FALSE);
	//�õ�����������ָʾ��ֵ
	int		GetVal();
	//ָ����������λ��ͨ����������ֵ(Val�ķ�Χ��(nMin, nMax), ��Pos�ķ�Χ��(0, m_nLen))
	void	SetPosByVal(int nVal, BOOL bRefresh = TRUE);
	//���������ƶ���Χ�趨
	int		GetPosRange();	//�õ�����ƶ���Pos����Ч��Χ
	void	SetSpace(int nSpace_Start, int nSpace_End);	//ָ����㡢�յ�հ׿��ֵ
	void	SetSpaceL(int nSpace){ m_nSpaceStart = nSpace;} //ˮƽʱ�����߿հ׾��� == ��ֱ����ʱ��Ķ�������
	void	SetSpaceR(int nSpace){ m_nSpaceEnd = nSpace;} //ˮƽʱ����ұ߿հ׾��� == ��ֱ����ʱ��ĵײ�����
	void	SetSpaceStart(int nSpace){ m_nSpaceStart = nSpace; }
	void	SetSpaceEnd(int nSpace){ m_nSpaceEnd = nSpace; }
	double	GetPos() const{ return m_nPos; }
	void	SetPos(double dPos);
	//�趨SliderBtn��ͼƬ
	void	SetSliderBtnPic(UINT uResID, LPCTSTR szResourceType, int nCount = 2);
	//�趨SliderBar��״̬
	void	SetState(SLIDERSTATE enumState){ m_enumSliderBarState = enumState; }
	//״̬��ʾ��ֵ�ķ�Χ
	void	GetStateRangeVal(int& nMin, int& nMax) const;
	void	SetStateRangeVal(int nMin, int nMax, BOOL bReDraw = FALSE);
	void	SetStateRangeMinVal(int nMin, BOOL bReDraw = FALSE);
	void	SetStateRangeMaxVal(int nMax, BOOL bReDraw = FALSE);
	//�õ�״̬��ʾ��ֵ
	int		GetStateVal();
	void	SetStatePosByStateVal(int nVal);
	//״̬ͼƬ��ʾ��λ��
	int		GetStatePosRange();	//�õ�m_nStatePos����Ч��Χ
	void	SetStateSpace(int nStateSpace_L, int nStateSpace_R);	//ָ����߿հ׿��ֵ���ұ߿հ׿��ֵ
	void	SetStateSpaceL(int nStateSpace){ m_nStateSpaceL = nStateSpace;}
	void	SetStateSpaceR(int nStateSpace){ m_nStateSpaceR = nStateSpace;}
	double	GetStatePos() const{ return m_nStatePos; }
	void	SetStatePos(double dStatePos);
	//�趨State��ͼƬ
	void	SetStatePic(UINT uResID, LPCTSTR szResourceType, int nCount = 2);
	//����StateͼƬ��״̬
	void	SetStatePicState(SLIDERSTATE enumState){ m_enumStatePicState = enumState; }
	//����������
	void	SetSliderType(SLIDERTYPE eSliderType) { m_eSliderType = eSliderType; }
	SLIDERTYPE GetSliderType(){ return m_eSliderType; }

public:
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
//	virtual void SetCursor(LPTSTR szCursor);
//	virtual void SetCustomCursor(HINSTANCE hModule,UINT uResID);

	//��Ϣ��Ӧ
protected:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);	

	//���û����鰴ť,����CDirectUI::AddSliderBarObj()�����������õ�
private:	
	//����m_pPos
	void	CalcSliderPos(CPoint point);
	//����ťʱ�����㰴ť����λ��
	void	CalcBtnRect(CRect& rc);
	//��ǰ��, ����ǰ������
	void	CalcFrontRect(CRect& rc);

protected:
	int		m_nSpaceStart, m_nSpaceEnd;		//���Ҳ�հ�(Ĭ��Ϊ���m_pBtnSliderBar��)
	int		m_nMinVal, m_nMaxVal;		//�趨ȡֵ��Χ(Pos��0λ�ö�ӦnMinֵ, ��nLenλ�ö�ӦnMaxֵ)
	int		m_nPos;						//ָ��ָʾ��λ��(0, nLen)
	SLIDERSTATE	m_enumSliderBarState;	
	BOOL		m_bLBtnDown;			//�Ƿ񱻰���

	//״̬
	int		m_nStateSpaceL, m_nStateSpaceR;
	int		m_nStateMinVal, m_nStateMaxVal;	//״̬����Сֵ�����ֵ
	int		m_nStatePos;					//״ָ̬ʾ��λ��
	SLIDERSTATE	m_enumStatePicState;
	
	SLIDERTYPE m_eSliderType; //�������������� (ˮƽ���ã����Ǵ�ֱ����)
};

class DIRECTUI_API CObjectNULL : public CObjectBase
{
public:
	CObjectNULL(HWND hWndOwner = NULL);
	virtual ~CObjectNULL(){};
public:
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point){return 0;}
};

#endif //OBJECTS_H_INCLUDED_
