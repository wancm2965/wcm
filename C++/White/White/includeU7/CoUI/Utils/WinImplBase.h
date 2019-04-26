#ifndef __WIN_IMPL_BASE_H__
#define __WIN_IMPL_BASE_H__

#include "../Core/UIBase.h"
#include "../Core/UIManager.h"
#include "../Core/UIDlgBuilder.h"

namespace CoUI
{
class UILIB_API WindowImplBaseBase
	: public CWindowWnd
	, public INotifyUI
	, public IMessageFilterUI
	, public IDialogBuilderCallback
{
public:
	WindowImplBaseBase();
	virtual ~WindowImplBaseBase();

	virtual void OnFinalMessage(HWND hWnd);

	
	virtual UINT GetClassStyle() const;

	virtual CControlUI* GetRoot() const;

	virtual void Init();
	virtual void Notify(TNotifyUI& msg) {};

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif

	virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual void CoUI_SetTimer(UINT_PTR nIDEvent, UINT uElapse);
	virtual void CoUI_KillTimer(UINT_PTR nIDEvent);
	virtual CStdString GetSkinFolder(){return TEXT("");};
	virtual CStdString GetSkinFile(){return TEXT("");};
	virtual LPCTSTR GetWindowClassName(void) const = 0 ;

	virtual EDialogResType GetResType()const{return DRT_FOLDER_FILE;};
	virtual unsigned int GetResourceID() const{return DEFAULT_ZIP_RESOURCE_ID;};
	virtual HINSTANCE GetResouceInstance(){return NULL;}

	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	CPaintManagerUI paint_manager_;	
};
}//end of CoUI
#endif // WIN_IMPL_BASE_HPP
