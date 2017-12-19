#ifndef __LOGIC_WIN_IMPL_BASE_H_
#define __LOGIC_WIN_IMPL_BASE_H_

#include "../Core/UIDlgBuilder.h"
#include "../Utils/WinImplBase.h"

namespace CoUI
{
	class CPaintManagerUI;
	class UILIB_API CLogicWinImplBaseBase
		: public IDialogBuilderCallback
		, public IMessageFilterUI
	{
	public:
		CLogicWinImplBaseBase();
		virtual ~CLogicWinImplBaseBase(void);

		///////////////////////////////////////////////////////////////////////////
	public:
		virtual bool Create();
		virtual CControlUI* GetRoot(void);
		virtual void Release();
		virtual void Init();
		virtual void CoUI_SetTimer(UINT nIDEvent, UINT uElapse);
		virtual void CoUI_KillTimer(UINT nIDEvent);
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
		CPaintManagerUI* m_pParentManager; // 保存父容器的manager
		CControlUI*		m_pRoot;
	};
}//end of CoUI
#endif // __LOGIC_WIN_IMPL_BASE_H_