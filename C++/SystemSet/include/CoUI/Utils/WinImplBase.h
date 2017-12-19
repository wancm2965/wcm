#ifndef __WIN_IMPL_BASE_H__
#define __WIN_IMPL_BASE_H__

#include "../Core/UIBase.h"
#include "../Core/UIManager.h"
#include "../Core/UIDlgBuilder.h"

namespace CoUI
{
#define TIME_DEFALT_ANIMATE		(200)

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

	//************************************
	// 函数:AnimateWindow
	// 功能:使得窗口显示和隐藏产生动画的效果
	// 参数说明：InitPos 初始化控件位置，第一次显示窗口时最好设置为true以初始化控件位置
	// 参数说明：dwTime 动画经历的时长，单位是ms
	// 参数说明：dwFlags 动画效果控制量
	// dwFlags详解：默认显示窗口，若需要激活窗口可添加AW_ACTIVATE;若要隐藏窗口添加AW_HIDE;
	// 动画效果有三种：AW_BLEND(淡入淡出)，AW_CENTER(展开收缩)，AW_SLIDE(平移)
	// 平移动画需要指明方向：水平方向（AW_HOR_POSITIVE，AW_HOR_NEGATIVE）竖直方向（AW_VER_POSITIVE，AW_VER_NEGATIVE）
	// 水平和竖直可以只指定一个方向，也可以组合成斜角方向。
	// 备注：使用前请确定CoUI窗口控件位置已初始化（正常绘制操作可初始化控件位置）
	// 参考：更多详细资料请查阅MSDN关于AnimateWindow()的详细说明
	//************************************
	virtual bool AnimateWindow(DWORD dwFlags, DWORD dwTime = TIME_DEFALT_ANIMATE, bool InitPos = false ) const;

	virtual void Init();
	virtual void Notify(TNotifyUI& /*msg*/) {};

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
	virtual void CoUI_SetTimer(UINT nIDEvent, UINT uElapse);
	virtual void CoUI_KillTimer(UINT nIDEvent);
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
