#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#include "../Core/UIContainer.h"

namespace CoUI
{
	class UILIB_API CTileLayoutUI : public CContainerUI
	{
	public:
		CTileLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetPos(RECT rc);

		SIZE GetItemSize() const;
		void SetItemSize(SIZE szItem);
		int GetColumns() const;
		void SetColumns(int nCols);
		int GetVerticalPadding() const;
		void SetVerticalPadding(int nPadding);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		SIZE m_szItem;
		int m_nColumns;
		int m_nVerticalPadding;
	};
}
#endif // __UITILELAYOUT_H__
