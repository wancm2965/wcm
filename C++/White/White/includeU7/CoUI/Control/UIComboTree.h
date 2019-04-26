#ifndef __UICOMBOTREE_H__
#define __UICOMBOTREE_H__

#include "../Core/UIContainer.h"
#include "UITree.h"

namespace CoUI {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboTreeWnd;

class UILIB_API CComboTreeUI : public CControlUI, public IContainerUI, public ITreeUI
{
    friend class CComboTreeWnd;
public:
    CComboTreeUI();
 	virtual ~CComboTreeUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

	bool Activate();

    CStdString GetText() const;

	SIZE EstimateSize(SIZE szAvailable);
	void SetPos(RECT rc);
	void DoEvent(TEventUI& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetEnabled(bool bEnable = true);

	void DoPaint(HDC hDC, const RECT& rcPaint);
	void PaintText(HDC hDC);
	void PaintStatusImage(HDC hDC);

    CStdString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);
	bool IsAutoDropWidth() const;
	void SetAutoDropSize(bool bAuto);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	CControlUI* FindSubControl(LPCTSTR pstrSubControlName);

	//////////////////////////////////////////////////////////////////////////
	// 继承自IContainerUI
	CControlUI* GetItemAt(int iIndex) const { return NULL; }
	int GetItemIndex(CControlUI* pControl) const { return -1; }
	bool SetItemIndex(CControlUI* pControl, int iIndex) { return false; }
	int GetCount() const { return -1; }
	bool Add(CControlUI* pControl);	
	bool AddAt(CControlUI* pControl, int iIndex) { return false; }
	bool Remove(CControlUI* pControl);
	bool RemoveAt(int iIndex) { return true ;}
	void RemoveAll();
  
	//////////////////////////////////////////////////////////////////////////
	// 继承自ITreeUI
    CControlUI* GetCurSel() const; 
    bool SelectItem(CControlUI* pItem, bool bTakeFocus = false);
	bool ExpandItem(CControlUI* pItem, bool bExpand);
	CControlUI* GetExpandedItem() const;
	virtual void EnableDBClickExpand(bool bEnable);
	virtual bool IsEnableDBClickExpand() const;
	virtual void EnableUpdate(bool bUpdate);
	virtual CControlUI* GetRoot();
	
	void SetItemFont(int index);
	void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
	void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
	void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
	void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
	void SetItemBkImage(LPCTSTR pStrImage);
	DWORD GetSelectedItemTextColor() const;
	void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
	void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
	void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
	void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
	void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
	void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
	void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
	void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
	void SetDisabledItemImage(LPCTSTR pStrImage);
	bool IsItemShowHtml();
	void SetItemShowHtml(bool bShowHtml = true);
	bool GetMultiExpandable() const;
	void SetMultiExpandable(bool bMultiExpand);
	void SetGroupCollapseImage(LPCTSTR pStrImage);
	LPCTSTR GetGroupCollapseImage() const;
	void SetGroupExpandImage(LPCTSTR pStrImage);
	LPCTSTR GetGroupExpandImage() const;
	void SetIndent(int nIndent);
	int GetIndent() const;
	TTreeInfoUI* GetTreeInfo();

	DWORD GetTreeBkColor() const;
	void SetDropboxBkColor(DWORD dwTreeBkColor);
	LPCTSTR GetTreeBkImage();
	void SetDropboxBkImage(LPCTSTR pStrImage);

	// nCode只可能是以下四个值中的一个
	// UITREE_INSERT_ROOT		插入一个根节点，此时将忽略pParent参数
	// UITREE_INSERT_FIRST		插入到所有子节点之前
	// UITREE_INSERT_LAST		插入到所有子节点之后
	// UITREE_INSERT_SORT		插入后立即排序,应将排序函数指针传入，否则无法排序
	bool Insert(CControlUI* pControl, CControlUI* pParent = NULL, int nCode = UITREE_INSERT_ROOT, LPTREESORT lpTreeSort = NULL);
	bool Add(LPCTSTR pstrName,CControlUI* pParent);

    
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

	bool IsShowButton() const;
	void SetShowButton(bool bShow);
	LPCTSTR GetButtonNormalImage() const;
	void SetButtonNormalImage(LPCTSTR pStrImage);
	LPCTSTR GetButtonHotImage() const;
	void SetButtonHotImage(LPCTSTR pStrImage);
	LPCTSTR GetButtonPushedImage() const;
	void SetButtonPushedImage(LPCTSTR pStrImage);
	LPCTSTR GetButtonFocusedImage() const;
	void SetButtonFocusedImage(LPCTSTR pStrImage);
	LPCTSTR GetButtonDisabledImage() const;
	void SetButtonDisabledImage(LPCTSTR pStrImage);

	DWORD GetDropBoxBorderColor() const;
	void SetDropBoxBorderColor(DWORD dwColor);

	void PaintButton(HDC hDC);

protected:
	CControlUI* FindSelectable(CControlUI* pControl, UINT uFlags) const;
    CComboTreeWnd* m_pWindow;

    RECT m_rcTextPadding;
    CStdString m_sDropBoxAttributes;
	bool m_bAutoDropWidth;
    SIZE m_szDropBox;
    UINT m_uButtonState;

    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sPushedImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;

	CStdString m_sButtonNormalImage;
	CStdString m_sButtonHotImage;
	CStdString m_sButtonPushedImage;
	CStdString m_sButtonFocusedImage;
	CStdString m_sButtonDisabledImage;

	DWORD m_dwDropboxBkColor;
	CStdString m_sDropboxBkImage;

	CStdString m_sImageModify;

	DWORD m_dwDropBoxBorderColor;
	bool m_bShowButton;
	RECT m_rcButton;

	RECT m_rcText;

	CControlUI* m_pCurSel;
    TTreeInfoUI m_TreeInfo;
	bool			m_bDBClickExpand;
	bool			m_bEnableUpdate;
	CControlUI*		m_pExpandedItem;
	CTreeItemUI* m_pRoot;
};

} // namespace CoUI

#endif // __UICOMBOTREE_H__
