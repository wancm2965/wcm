#pragma once
#include "win_impl_base.hpp"
using namespace AVCONPlugin;

namespace AVCONPlugin
{
	class CMeetingTip
		: public WindowImplBase
	{
	public:
		CMeetingTip(void);
		~CMeetingTip(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		// �̳�WindowImplBase�ӿ�
		virtual void Init();
		virtual LPCTSTR GetWindowClassName() const;
		virtual CString GetSkinFile();
		 virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		 virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	protected:
		 virtual void Notify(TNotifyUI& msg);
	private:
		CButtonUI*	m_pBtnShowMeetingList;
		
	};
}