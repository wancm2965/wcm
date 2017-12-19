#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#include "UIButton.h"

namespace CoUI
/////////////////////////////////////////////////////////////////////////////////////
//
{
class UILIB_API COptionUI : public CButtonUI
{
public:
    COptionUI();
    ~COptionUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

    bool Activate();
    void SetEnabled(bool bEnable = true);

    LPCTSTR GetSelectedImage();
    void SetSelectedImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedHotImage();
	void SetSelectedHotImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedPushImage();
	void SetSelectedPushImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedFocusedImage();
	void SetSelectedFocusedImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedDisabledImage();
	void SetSelectedDisabledImage(LPCTSTR pStrImage);
	
	void SetSelectedTextColor(DWORD dwTextColor);
	DWORD GetSelectedTextColor();

	LPCTSTR GetSelectedForeImage();
	void SetSelectedForeImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);
    bool IsSelected() const;
    void Selected(bool bSelected);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);

protected:
    bool m_bSelected;
    CStdString m_sGroupName;

	DWORD m_dwSelectedTextColor;

    CStdString m_sSelectedNormalImage;
	CStdString m_sSelectedHotImage;
	CStdString m_sSelectedPushImage;
	CStdString m_sSelectedFocusedImage;
	CStdString m_sSelectedDisabledImage;
	CStdString m_sSelectedForeImage;
};

} // namespace CoUI

#endif // __UIOPTION_H__

