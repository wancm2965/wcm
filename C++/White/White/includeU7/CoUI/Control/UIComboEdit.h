#ifndef __UICOMBOEDIT_H__
#define __UICOMBOEDIT_H__
#include <CoUI/Control/UICombo.h>
#include <CoUI/Control/UIEdit.h>

namespace CoUI
{
	class CComboEditWnd;
	class CDropListWnd;
	class UILIB_API CComboEditUI : public CComboUI
	{
		friend class CComboEditWnd;
		friend class CDropListWnd;
	public:
		struct DropBoxPos
		{
			LONG	left;
			LONG	right;
			LONG	top;
		};
	public:
		CComboEditUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Activate();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoEvent(TEventUI& event);
		void PaintText(HDC hDC);

		void SetHintText(LPCTSTR lpszHint);
		LPCTSTR GetHintText(void) const;

		void SetHintFont(int iHintFont);
		int GetHintFont(void) const;

		void SetHintTextColor(DWORD dwHintColor);
		DWORD GetHintTextColor(void) const;

		void SetDisabledHintTextColor(DWORD dwDisabledHintColor);
		DWORD GetDisabledHintTextColor(void) const;

		void SetDropBoxPos(const DropBoxPos& dbPos);
		const DropBoxPos& GetDropBoxPos() const;

		void SetMaxChar(UINT uMax);
		UINT GetMaxChar() const;

		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		void SetNorTextColor(DWORD dwTextColor);
		DWORD GetNorTextColor() const;

		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;

		void ShowEditWnd();

		LPCTSTR GetEditText(void) const;
	protected:
		CComboEditWnd* m_pEditWnd;
		CDropListWnd* m_pComboWnd;
		BOOL m_bShowDropBox;

		CStdString m_strHint;
		int m_iHintFont;
		DWORD m_dwHintColor;
		DWORD m_dwDisabledHintColor;

		DropBoxPos m_DropBoxPos;//�����б�������combo��λ�ã�ʹ�б����combo��ߡ��ұߡ�������룬Ĭ��Ϊ(0,0,0)

		UINT m_uMaxChar;//�༭���е��ַ���

		int m_iEditStyls; //�༭����

		DWORD	m_dwNorTextColor;//����״̬�µ�������ɫ
		DWORD	m_dwTextColor; //������״̬�µ�������ɫ
	};
}

#endif // __UICOMBOEDIT_H__