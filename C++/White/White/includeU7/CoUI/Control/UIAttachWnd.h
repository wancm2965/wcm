#ifndef __UIATTACHCONTROLS_H__
#define __UIATTACHCONTROLS_H__

#include "../Core/UIControl.h"

namespace CoUI {

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CHWndUI : public CControlUI
{
public:
	CHWndUI();
	~CHWndUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);
	void SetPos(RECT rc);

	RECT GetInset() const;
	void SetInset(RECT rcInset);

	bool Attach(HWND hWnd, bool bChild = true);
	// Detach后会隐藏绑定窗口
	HWND Detach(void);
	HWND GetHWND() const;

protected:
	HWND		m_hWnd;
	HWND		m_hOldParent;
	RECT		m_rcInset;
	bool		m_bChild;
};

} // namespace CoUI

#endif // __UIATTACHCONTROLS_H__

