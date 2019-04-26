#include "stdafx.h"
#include "Plugin_WB_Meeting.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
#include "Conference.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;
CPlugin_WB_Meeting* g_pPluginWBMeeting = NULL;
CPlugin_WB_Meeting* GetPluginWBMeeting()
{
	return g_pPluginWBMeeting;
}
AVCONPlugin::CPlugin_WB_Meeting::CPlugin_WB_Meeting()
: m_pMeetingList(NULL)
{
	g_pPluginWBMeeting = this;
}

AVCONPlugin::CPlugin_WB_Meeting::~CPlugin_WB_Meeting()
{
}

std::string AVCONPlugin::CPlugin_WB_Meeting::Name()
{
	return "Plugin_WB_Meeting";
}

std::string AVCONPlugin::CPlugin_WB_Meeting::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_WB_Meeting";
}

bool AVCONPlugin::CPlugin_WB_Meeting::InitPlugin()
{
	return true;
}

bool AVCONPlugin::CPlugin_WB_Meeting::ClosePlugin()
{
	if (m_pMeetingList)
	{
		::DestroyWindow(m_pMeetingList->GetHWND());
		delete m_pMeetingList;
		m_pMeetingList = NULL;
	}
	return true;
}

void AVCONPlugin::CPlugin_WB_Meeting:: ShowUIMeetingList(bool bShow)
{
	GetLangPath();
	if (NULL == m_pMeetingList)
	{
		CPaintManagerUI::SetResourcePath(GetFolderPath());
		m_pMeetingList = new CMeetingList();
		IPlugin_WB_Main* pPlugin_main = dynamic_cast<IPlugin_WB_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Main"));
		HWND hWnd = NULL;
		if (pPlugin_main)
		{
			hWnd = pPlugin_main->GetExeParentWnd();
		}
		UINT style = UI_WNDSTYLE_FRAME;
		style&=~WS_MAXIMIZEBOX;
		m_pMeetingList->Create(hWnd, _T("AVCON_WB _MEETING"), style, WS_EX_TOOLWINDOW, 0, 0, 300, 800);
		m_pMeetingList->CenterWindow();
		m_pMeetingList->ShowWindow(bShow);
		::SetWindowPos(m_pMeetingList->GetHWND(),HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		CPaintManagerUI::SetResourcePath(GetFolderPath());
		m_pMeetingList->ShowWindow(bShow);
	}
	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pU7MMRoom )
	{	
		pU7MMRoom->LoadRoomList();
	}
}

void AVCONPlugin::CPlugin_WB_Meeting::AddObserver( IPlugin_WB_Meeting_Observer* pObserver )
{
	if (pObserver == NULL) return;
	m_lstObserver.remove(pObserver);
	m_lstObserver.push_back(pObserver);
}

void AVCONPlugin::CPlugin_WB_Meeting::DelObserver( IPlugin_WB_Meeting_Observer* pObserver )
{
	if(NULL == pObserver) return;
	m_lstObserver.remove(pObserver);
}

void AVCONPlugin::CPlugin_WB_Meeting::RegCard( int nCardIndex,IPlugin_WB_Meeting_Card* pCard )
{
	if(NULL == pCard) return;
	m_mapCard.insert(std::pair<int,IPlugin_WB_Meeting_Card*>(nCardIndex,pCard));
}

void AVCONPlugin::CPlugin_WB_Meeting::UnRegCard( int nCardIndex )
{
	m_mapCard.erase(nCardIndex);
}

std::string AVCONPlugin::CPlugin_WB_Meeting::GetRoomName()
{
	return GetConference().GetRoomName();
}

void AVCONPlugin::CPlugin_WB_Meeting::AVConf_Enter( const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword )
{
	GetConference().EnterRoom(strDomain,strRoomID,strPassword);
}

void AVCONPlugin::CPlugin_WB_Meeting::ExitMeeting( void )
{

}

void AVCONPlugin::CPlugin_WB_Meeting::OnExitMeeting( void )
{

}

void AVCONPlugin::CPlugin_WB_Meeting::Broadcast( int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType )
{

}

void AVCONPlugin::CPlugin_WB_Meeting::CloseBroadcast( const std::string& strMemberID,int nCardIndex,int nChannelType, bool bDestroy /*= true*/ )
{

}

void AVCONPlugin::CPlugin_WB_Meeting::ProcessTipMessage()
{
	if (m_pMeetingList)
	{
		m_pMeetingList->ProcessTipMessage();
	}
}



