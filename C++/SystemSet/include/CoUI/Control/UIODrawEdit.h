#ifndef __UIODRAWEDIT_H__
#define __UIODRAWEDIT_H__

#include <CoUI/Control/UILabel.h>
namespace CoUI
{

	//���ֹ���δʵ�֣���ʱע��
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
		void SetShowCursor(bool bShowCursor);//�Ƿ�ʹ�ù��
		void SetCursorPos(int curPos);//���ù��λ��
		virtual bool IsEnableChar( TCHAR chKey );//�ж��ַ��Ƿ�֧��

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
		UINT m_uMaxChar;//����ַ�����
		DWORD m_dwCursorColor;
		TCHAR m_cPasswordChar;

	protected:
		//��궨λ�ñ���
		UINT m_uCurIndex;			//�������λ��
		bool m_bCurChange;			//������Ϣ�Ƿ���Ч
		UINT m_uLeftMove;			//���ƾ���
		UINT m_uCurRightMove;		//��긨������
		UINT m_uTextHeight;			//�ı��߶�
		RECT m_rPreTextRect;		//�ı��ϴ�λ��

	protected:

		CStdString m_sHintText;		//��ʾ�ı����༭��û������ʱ����ʾ��ʾ���壩
		DWORD	m_dwHintColor;		//
		int		m_iHintFont;		//
	};

} // namespace CoUI

#endif // __UIODRAWEDIT_H__