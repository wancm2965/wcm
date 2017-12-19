#ifndef __UILABEL_H__
#define __UILABEL_H__

#include "../Core/UIControl.h"

namespace CoUI
{
class UILIB_API CLabelUI : public CControlUI
{
public:
    CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
	DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
	DWORD GetDisabledTextColor() const;
    void SetFont(int index);
	int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);

	void SetPos(RECT rc);

	void SetAutoFont( bool bAutoFont = true );
	bool IsAutoFont() const;

	void SetTextOutGlow( bool bOuterGlow = true );
	bool IsTextOutGlow();

protected:
	int GetFitFont();

protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
	bool m_bAutoFont;
	bool m_bOuterGlow;//是否开启毛玻璃外发光
};

} // namespace CoUI

#endif // __UILABEL_H__

