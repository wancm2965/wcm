#include "StdAfx.h"
#include "MeetingTip.h"
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
using namespace AVCONPlugin;
AVCONPlugin::CMeetingTip::CMeetingTip( void )
{
	m_pBtnShowMeetingList = NULL;
}

AVCONPlugin::CMeetingTip::~CMeetingTip( void )
{

}

void AVCONPlugin::CMeetingTip::Init()
{
	m_pBtnShowMeetingList = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("btnShowMeetingList")));
	if (m_pBtnShowMeetingList == NULL)
	{
		return;
	}
	CString str ;
	str = ReadConfigText(_T("PluginText"), _T("SHOWMEETINGLIST"));
	if (str.IsEmpty())
	{
		str = _T("显示会议列表");
	}
	m_pBtnShowMeetingList->SetToolTip(str);
	//paint_manager_.EnableSysMTParser(MOUSE_GESTURE_PARSER);
}


LPCTSTR AVCONPlugin::CMeetingTip::GetWindowClassName() const
{
		return _T("CMeetingTip");
}

CString AVCONPlugin::CMeetingTip::GetSkinFile()
{
	return _T("MeetingTip.xml");
}

LRESULT AVCONPlugin::CMeetingTip::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return WindowImplBase::OnLButtonDown(uMsg, wParam,lParam,bHandled );
}

LRESULT AVCONPlugin::CMeetingTip::OnMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return WindowImplBase::OnMove(uMsg, wParam,lParam,bHandled );
}

void AVCONPlugin::CMeetingTip::Notify( TNotifyUI& msg )
{
	if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if (_tcscmp(msg.pSender->GetName(),_T("btnShowMeetingList")) == 0)
		{
			IPlugin_WB_Meeting* pPluginWBMeeting = dynamic_cast<IPlugin_WB_Meeting*>(g_System->GetPluginByName("Plugin_WB_Meeting"));
			if (pPluginWBMeeting)
			{
				pPluginWBMeeting->ProcessTipMessage();
				//pPluginWBMeeting->ShowUIMeetingList(true);
			}
			ShowWindow(false);
		}
	}
}
