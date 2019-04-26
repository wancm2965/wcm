#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#include "../Core/UIContainer.h"

namespace CoUI
{
	class UILIB_API CTabLayoutUI : public CContainerUI
	{
	public:
		CTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool Add(HWND hWnd);
		bool AddAt(HWND hWnd, int iIndex);
		bool Remove(HWND hWnd);
		void RemoveAll();
		int GetCurSel() const;
		bool SelectItem(int iIndex);

		void SetPos(RECT rc);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		int m_iCurSel;
	};
}
#endif // __UITABLAYOUT_H__
