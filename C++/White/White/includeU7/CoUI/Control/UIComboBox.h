#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#include "UICombo.h"

namespace CoUI
{
/////////////////////////////////////////////////////////////////////////////////////
//

/// ��չ�����б��
/// ����arrowimage����,һ��ͼƬƽ���ֳ�5��,Normal/Hot/Pushed/Focused/Disabled(������source����)
/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
class UILIB_API CComboBoxUI : public CComboUI
{
public:
	CComboBoxUI();
	LPCTSTR GetClass() const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
	void PaintStatusImage(HDC hDC);

protected:
	CStdString m_sArrowImage;
	int        m_nArrowWidth;
};

} // namespace CoUI

#endif // __UICOMBOBOX_H__

