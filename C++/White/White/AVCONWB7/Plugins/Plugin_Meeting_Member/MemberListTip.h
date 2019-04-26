#pragma once
#include "win_impl_base.hpp"
using namespace AVCONPlugin;

namespace AVCONPlugin
{
	class CMemberTip
		: public WindowImplBase
	{
	public:
		CMemberTip(void);
		~CMemberTip(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		// ¼Ì³ÐWindowImplBase½Ó¿Ú
		virtual void Init();
		virtual LPCTSTR GetWindowClassName() const;
		virtual CString GetSkinFile();
		 virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		 virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	protected:
		 virtual void Notify(TNotifyUI& msg);
	private:
		CButtonUI*	m_pBtnShowMemberList;
		
	};
}