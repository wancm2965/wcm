#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#include "../Core/UIContainer.h"

namespace CoUI
{
	class UILIB_API CChildWindowUI : public CContainerUI
	{
	public:
		CChildWindowUI();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildWindowXML(CStdString pXML);
		CStdString GetChildWindowXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;

	private:
		CStdString m_pstrXMLFile;
	};
} // namespace CoUI
#endif // __UICHILDLAYOUT_H__
