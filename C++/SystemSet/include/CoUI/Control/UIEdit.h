#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#include "UILabel.h"

namespace CoUI
{
/////////////////////////////////////////////////////////////////////////////////////
//

class CEditWnd;

class UILIB_API CEditUI : public CLabelUI
{
    friend class CEditWnd;
public:
    CEditUI();
	~CEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

	bool IsWantTab();
	void SetWantTab(bool bWantTab = true);
    void SetEnabled(bool bEnable = true);
    void SetText(LPCTSTR pstrText);
    void SetMaxChar(UINT uMax);
    UINT GetMaxChar();
    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
    void SetPasswordMode(bool bPasswordMode);
    bool IsPasswordMode() const;
    void SetPasswordChar(TCHAR cPasswordChar);
    TCHAR GetPasswordChar() const;
	void SetNumberOnly(bool bNumberOnly);
	bool IsNumberOnly() const;
	void SetWindowStyles(int nStyles);
	int GetWindowStyles() const;
	//set for showing or hiding internal balloontip.
	static void ShowBalloontip(bool bShow = true);
	static bool IsShowBalloonTip();

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
    void SetNativeEditBkColor(DWORD dwBkColor);
    DWORD GetNativeEditBkColor() const;

    void SetSel(long nStartChar, long nEndChar);
	void GetSel(long& nStartChar, long& nEndChar);
    void SetSelAll();
    void SetReplaceSel(LPCTSTR lpszReplace);

    void SetPos(RECT rc);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    SIZE EstimateSize(SIZE szAvailable);
	void SetFocus();
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
    void PaintText(HDC hDC);

	const HWND GetAssociatedHwnd()const;

protected:
	int GetFitFont();

protected:
    CEditWnd* m_pWindow;

    UINT m_uMaxChar;
    bool m_bReadOnly;
    bool m_bPasswordMode;
	bool m_bWantTab;
    TCHAR m_cPasswordChar;
    UINT m_uButtonState;
    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;
    DWORD m_dwEditbkColor;
	int m_iWindowStyles;
	//internal balltip, default true
	static bool m_bShowBalltip;
};

} // namespace CoUI

#endif // __UIEDIT_H__

