#ifndef __CHILD_WIN_IMPL_BASE_H__
#define __CHILD_WIN_IMPL_BASE_H__

#include "../Core/UIBase.h"
#include "../Core/UIManager.h"
#include "../Core/UIDlgBuilder.h"

namespace CoUI
{
	class UILIB_API CChildWinImplBaseBase
		: public IDialogBuilderCallback
		, public IMessageFilterUI
	{
	public:
		CChildWinImplBaseBase(void);
		virtual ~CChildWinImplBaseBase(void);

		///////////////////////////////////////////////////////////////////////////
	public:
		virtual void Create(HWND hParent);
		virtual CControlUI* GetRoot(void);
		virtual void Release();
		virtual void Init();
		virtual void CoUI_SetTimer(UINT_PTR nIDEvent, UINT uElapse);
		virtual void CoUI_KillTimer(UINT_PTR nIDEvent);
		//////////////////////////////////////////////////////////////////////////
		// 继承IMessageFilterUI接口
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
		//////////////////////////////////////////////////////////////////////////
		// 继承IDialogBuilderCallback接口
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		//////////////////////////////////////////////////////////////////////////


	protected:
		virtual bool OnRootInit(void* param);
		virtual bool OnRootDestroy(void* param);
		virtual CStdString GetSkinFile();
		virtual CStdString GetSkinFolder();
		virtual EDialogResType GetResType()const{return DRT_FOLDER_FILE;};
		virtual unsigned int GetResourceID() const{return DEFAULT_ZIP_RESOURCE_ID;};
		virtual HINSTANCE GetResouceInstance(){return NULL;}

	protected:
		CPaintManagerUI paint_manager_;
		CPaintManagerUI* m_pParentManager; // 保存父容器的manager
		CControlUI*		m_pRoot;
		HWND			m_hParent;
		bool			m_bInitFilter;
	};
}//end of CoUI
#endif // __CHILD_WIN_IMPL_BASE_H__