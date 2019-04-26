//EditObjEx.h: interface for the CEditObjEx class.
//

#if !defined(COMBOBOXOBJ_H_INCLUDED_)
#define COMBOBOXOBJ_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"

class DIRECTUI_API CEditObjEx : public CObjectBase
{
public:
enum COMBOBOXSTATE 
{
	COMBOBOX_NORMAL	  = 0, 
	COMBOBOX_PRESS	  = 1,
	COMBOBOX_DISABLE  = 2,
	COMBOBOX_HOT	  = 3,
	COMBOBOX_LAST	  = 4
};

enum BKBRUSHTYPE
{
	DEFAULT_TYPE = 0,
	COLOR_TYPE = 1,
	IMAGE_TYPE = 2
};
	CEditObjEx(HWND hWndOwner = NULL);
	virtual ~CEditObjEx();

public:
	SkinImageBase	m_imageEditBK[COMBOBOX_LAST];

public:
	void	SetTextFont(const SkinFont& fontText);
	SkinFont& GetTextFont();
	const SkinFont& GetTextFont() const ;
	void	SetEditTooltip(const CString& strTooltip){m_strEditTip = strTooltip;}
	
	void	SetCurrentID(UINT uID);
	void	SetAutoResize(BOOL bAutoResize){m_bAutoResize = bAutoResize;} 
	const CString& GetCurrText();
	void	SetCurrText(const CString& strText);
	const CString& GetDefText(){return m_strDefText;}
	void	SetDefText(const CString& strDefText){m_strDefText = strDefText;}
	void	SetTextColor(COMBOBOXSTATE enumState,COLORREF clrText){m_clrText[enumState] = clrText;}
	void	SetDefTextColor(COLORREF clrDefText){m_clrDefText = clrDefText;}
	void	SetItemImage(CImage* pImage){m_pItemImage = pImage;}
	
	void	SetItemText(const CString& strText,UINT uID);
	
	void	SetEditImage(SkinImageBase* pImgEdit);
	
	HIMAGELIST GetImageList(){return m_hImageList;}
	void	SetImageList(HIMAGELIST hImageList){m_hImageList = hImageList;}
	void	SetEditBackNoStretchRgn(COMBOBOXSTATE enumState,int nLeftWidth,int nRightWidth,
								   int nTopHeight,int nBottomHeight);
	
	//设置Disable状态
	void SetDisable(BOOL bDisable = TRUE, BOOL bImmediate = TRUE);
	BOOL IsDisable() { return m_bDisable; }
	
	//改变Edit的风格
	DWORD GetEditStyle();
	void SetEditStyle(DWORD dwEStyle);
	void ModifyEditStyle(DWORD dwAddStyle, DWORD dwDelStyle);
	
	//得到编辑框句柄
	HWND GetEditHWnd();
	BOOL IsAutoDestory(){return m_bAutoDestory; }
	void SetAutoDestory(BOOL bAutoDestory) { m_bAutoDestory = bAutoDestory; }

	//设置背景画刷类型
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
	COMBOBOXSTATE m_enumEditState;
	CString m_strCurrText;
	UINT m_uCurrentID;
	CString m_strDefText;
	int m_nCurrIndex;
	COLORREF m_clrText[COMBOBOX_LAST];
	COLORREF m_clrDefText;
	CImage* m_pItemImage;
	SkinFont m_fontText;
	BOOL m_bAutoResize;
	CRect m_rcEdit;

	HIMAGELIST m_hImageList;

	CString m_strEditTip;

	BOOL m_bLMouseDown;
	HWND m_hWndEdit;
	BOOL m_bDisable;
	
	DWORD m_dwEditStyle; //编辑框的风格
	BOOL m_bAutoDestory; //UpData的是否自动Destory 编辑框
	
	BOOL m_bFirstKnockEdit; //是否是第1次敲击编辑框区域

	BKBRUSHTYPE	m_eBkBrushType;
	BOOL m_bCreateBrush;
	HBRUSH m_hBrush_Bk;
	COLORREF m_clrBkBrush;
};

#endif //COMBOBOXOBJ_H_INCLUDED_

