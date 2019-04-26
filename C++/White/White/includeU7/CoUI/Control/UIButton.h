#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#include "UILabel.h"

namespace CoUI
{
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CButtonUI : public CLabelUI
{
public:
    CButtonUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool Activate();
    void SetEnabled(bool bEnable = true);
    void DoEvent(TEventUI& event);

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage();
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);

    void SetHotTextColor(DWORD dwColor);
	DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
	DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
	DWORD GetFocusedTextColor() const;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    UINT m_uButtonState;

	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;

    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sPushedImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;
	CStdString m_sForeImage;

public:
	//扩展方法
	void SetTextRow( int RowNum);
	int GetTextRow()const{return m_nTextRow;}
	void SetNeedParse(){m_bNeedUpdateParse = true;}
	//重写方法
	virtual void SetText(LPCTSTR pstrText);
	virtual void SetPos(RECT rc);
private:
	//扩展属性
	int ParseStr(HDC,RECT&);//解析字符串
	int DrawMuLineText(HDC,RECT&,DWORD);//绘制多行字符串

	int m_nLastTextWidth;//上一次绘制的行宽
	bool m_bNeedUpdateParse;//是否需要重新解析
	int m_nTextRow;//指定显示行数
	int m_TextRowNumBackUp;//多行显示时，字符串解析后的实际行数。
	//如果多行显示，则将源字符串分解成两部分
	CStdString m_strTexHead;//字符串头备份
	CStdString m_strTextTail;//字符串尾备份
};
} // namespace CoUI

#endif // __UIBUTTON_H__

