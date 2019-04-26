#pragma once
#include "win_impl_base.hpp"

class CMainParentWnd : public WindowImplBase
{
public:
	CMainParentWnd(void);
	~CMainParentWnd(void);

	LPCTSTR GetWindowClassName() const;
	CString GetSkinFile();
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void Notify(TNotifyUI& msg){};
};
