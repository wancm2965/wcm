#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#include "UILabel.h"

namespace CoUI
{
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CProgressUI : public CLabelUI
{
public:
    CProgressUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);

	bool IsStretchForeImage();
	void SetStretchForeImage(bool bStretchForeImage = true);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

protected:
    bool m_bHorizontal;
	bool m_bStretchForeImage;
    int m_nMax;
    int m_nMin;
    int m_nValue;

    CStdString m_sForeImage;
    CStdString m_sForeImageModify;
};

} // namespace CoUI

#endif // __UIPROGRESS_H__

