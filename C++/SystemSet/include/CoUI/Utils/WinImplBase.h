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
	// ����:AnimateWindow
	// ����:ʹ�ô�����ʾ�����ز���������Ч��
	// ����˵����InitPos ��ʼ���ؼ�λ�ã���һ����ʾ����ʱ�������Ϊtrue�Գ�ʼ���ؼ�λ��
	// ����˵����dwTime ����������ʱ������λ��ms
	// ����˵����dwFlags ����Ч��������
	// dwFlags��⣺Ĭ����ʾ���ڣ�����Ҫ����ڿ����AW_ACTIVATE;��Ҫ���ش������AW_HIDE;
	// ����Ч�������֣�AW_BLEND(���뵭��)��AW_CENTER(չ������)��AW_SLIDE(ƽ��)
	// ƽ�ƶ�����Ҫָ������ˮƽ����AW_HOR_POSITIVE��AW_HOR_NEGATIVE����ֱ����AW_VER_POSITIVE��AW_VER_NEGATIVE��
	// ˮƽ����ֱ����ָֻ��һ������Ҳ������ϳ�б�Ƿ���
	// ��ע��ʹ��ǰ��ȷ��CoUI���ڿؼ�λ���ѳ�ʼ�����������Ʋ����ɳ�ʼ���ؼ�λ�ã�
	// �ο���������ϸ���������MSDN����AnimateWindow()����ϸ˵��
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
