#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#include "../Core/UIContainer.h"
#include "UIList.h"

namespace CoUI {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();
 	virtual ~CComboUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

    CStdString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);
	bool IsAutoDropWidth() const;
	void SetAutoDropWidth(bool bAuto);

    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);
	void SetDownSelect(bool bDownSelect);
	bool IsDownSelect() const;

    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
	bool Add(LPCTSTR pstrName);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    bool Activate();

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

    TListInfoUI* GetListInfo();
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
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
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
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

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

	void SetDropAnimate( bool bOpen = true );
	bool IsDropAnimate( void );
    
	SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);
	void PaintButton(HDC hDC);

protected:
    CComboWnd* m_pWindow;

    int m_iCurSel;
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

	CStdString m_sImageModify;

	DWORD m_dwDropBoxBorderColor;
	bool m_bShowButton;
	RECT m_rcButton;

	RECT m_rcText;

    TListInfoUI m_ListInfo;
	bool m_bDropAnimate;
};

} // namespace CoUI

#endif // __UICOMBO_H__
