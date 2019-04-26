#include "StdAfx.h"
#include "MemberListTip.h"
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
using namespace AVCONPlugin;
AVCONPlugin::CMemberTip::CMemberTip( void )
{
	m_pBtnShowMemberList = NULL;
}

AVCONPlugin::CMemberTip::~CMemberTip( void )
{

}

void AVCONPlugin::CMemberTip::Init()
{
	m_pBtnShowMemberList = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("btnShowMemberList")));
	if (m_pBtnShowMemberList == NULL)
	{
		return;
	}
	CString str ;
	str = ReadConfigText(_T("PluginText"), _T("SHOWMEMBERLIST"));
	if (str.IsEmpty())
	{
		str = _T("显示人员列表");
	}
	m_pBtnShowMemberList->SetToolTip(str);
	//paint_manager_.EnableSysMTParser(MOUSE_GESTURE_PARSER);
}


LPCTSTR AVCONPlugin::CMemberTip::GetWindowClassName() const
{
		return _T("CMemberTip");
}

CString AVCONPlugin::CMemberTip::GetSkinFile()
{
	return _T("MemberListTip.xml");
}

LRESULT AVCONPlugin::CMemberTip::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return WindowImplBase::OnLButtonDown(uMsg, wParam,lParam,bHandled );
}

LRESULT AVCONPlugin::CMemberTip::OnMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return WindowImplBase::OnMove(uMsg, wParam,lParam,bHandled );
}

void AVCONPlugin::CMemberTip::Notify( TNotifyUI& msg )
{
	if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if (_tcscmp(msg.pSender->GetName(),_T("btnShowMemberList")) == 0)
		{
			IPlugin_Meeting_Member* pPluginWBMember = dynamic_cast<IPlugin_Meeting_Member*>(g_System->GetPluginByName("Plugin_Meeting_Member"));
			if (pPluginWBMember)
			{
				pPluginWBMember->ProcessTipMessage();
				//pPluginWBMeeting->ShowUIMeetingList(true);
			}
			ShowWindow(false);
		}
	}
}
