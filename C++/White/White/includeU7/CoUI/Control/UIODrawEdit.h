#ifndef __UIODRAWEDIT_H__
#define __UIODRAWEDIT_H__

#include <CoUI/Control/UILabel.h>
namespace CoUI
{

	//部分功能未实现，暂时注释
	class UILIB_API CODrawEditUI : public CLabelUI //Owner-Draw Edit
	{
	public:
		CODrawEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool IsWantTab();
		void SetWantTab(bool bWantTab = true);
		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar() const;
		//void SetReadOnly(bool bReadOnly);
		//bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		//void SetPasswordChar(TCHAR cPasswordChar);
		//TCHAR GetPasswordChar() const;
		//void SetNumberOnly(bool bNumberOnly);
		//bool IsNumberOnly() const;

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;

		//void SetSel(long nStartChar, long nEndChar);
		//void GetSel(long& nStartChar, long& nEndChar);
		//void SetSelAll();
		//void SetReplaceSel(LPCTSTR lpszReplace);

		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	public:
		void SetCursorColor(DWORD dwColor);
		DWORD GetCursorColor() const;
		void SetShowCursor(bool bShowCursor);//是否使用光标
		void SetCursorPos(int curPos);//设置光标位置
		virtual bool IsEnableChar( TCHAR chKey );//判断字符是否支持

		void SetHintColor(DWORD dwColor);
		void SetHintText(LPCTSTR pstrText);
		void SetHintFont(int index);

	protected:
		//bool m_bReadOnly;
		bool m_bWantTab;
		UINT m_uButtonState;
		CStdString m_sNormalImage;
		CStdString m_sHotImage;
		CStdString m_sDisabledImage;
		DWORD m_dwEditbkColor;

	protected:
		bool m_bShowcCursor;
		bool m_bEnabledCursor;
		UINT m_uMaxChar;//最多字符个数
		DWORD m_dwCursorColor;
		TCHAR m_cPasswordChar;

	protected:
		//光标定位用变量
		UINT m_uCurIndex;			//光标所在位置
		bool m_bCurChange;			//绘制信息是否有效
		UINT m_uLeftMove;			//左移距离
		UINT m_uCurRightMove;		//光标辅助坐标
		UINT m_uTextHeight;			//文本高度
		RECT m_rPreTextRect;		//文本上次位置

	protected:

		CStdString m_sHintText;		//提示文本（编辑框没有内容时，显示提示字体）
		DWORD	m_dwHintColor;		//
		int		m_iHintFont;		//
	};

} // namespace CoUI

#endif // __UIODRAWEDIT_H__