#include "stdafx.h"
#include "Conference.h"
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;
CConference g_Conference;
CConference& GetConference(void)
{
	return g_Conference;
} 

CConference::CConference( void )
:m_strRoomName("Default meeting ")
,m_usMemberType(5)
{
}

CConference::~CConference()
{
}

void CConference::EnterRoom( const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword )
{
	IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if(pPlugin_Room == NULL)
		return;
		
	pPlugin_Room->AddObserver(this);
	
	pPlugin_Room->EnterRoom(strDomain.c_str(), strRoomID.c_str(), strPassword.c_str(), "Online");
}

void CConference::OnRoomMemberItemEnd( void )
{

}

void CConference::OnEnterRoom( unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType )
{
	if (0 == nErrCode)
	{
		IPlugin_U7_MM_Function* pPlugin_Func = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if(pPlugin_Func)
			pPlugin_Func->AddObserver(this);
		IPlugin_U7_MM_Member* pPlugin_MM = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if(pPlugin_Func)
			pPlugin_Func->AddObserver(this);
		//在这里就把成员类表显示出来
		m_strRoomName = strRoomName;
		m_usMemberType = usMemberType;
		IPlugin_Meeting_Member* pPlugin = dynamic_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
		if (pPlugin)
		{
			pPlugin->ShowUI(true);
		}
	}
	else
	{
	//登入会议不成功就删除观察者，要登入会议的时候在重新添加观察者
		IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
		if (pPlugin_Room)
		{
			pPlugin_Room->DelObserver(this);
		}
	}
}

void CConference::OnEnterRoomReady()
{
	//添加会议相应的观察者，然后根据实际情况转发各自的方法
	

}

void CConference::OnExitRoom( unsigned int nErrCode )
{
	//添加会议相应的观察者，然后根据实际情况转发各自的方法
	IPlugin_U7_MM_Function* pPlugin_Func = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if(pPlugin_Func)
		pPlugin_Func->DelObserver(this);

	IPlugin_U7_MM_Member* pPlugin_MM = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if(pPlugin_Func)
		pPlugin_Func->DelObserver(this);

	IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if(pPlugin_Room == NULL)
		return;
	pPlugin_Room->DelObserver(this);
}

void CConference::OnBroadcast( int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType )
{
//在这个地方把白板联通网络

}

void CConference::OnCloseBroadcast( const char* strMemberID,int nCardIndex,int nChannelType )
{
//关掉网络

}

std::string CConference::GetRoomName()
{
	return m_strRoomName;
}

void CConference::OnSetRight( int nRight, bool bEnable, const std::string& strMemberID )
{
	if (nRight == 5)
	{
		IPlugin_WB_Meeting* pPluginWBMeeting = dynamic_cast<IPlugin_WB_Meeting*>(g_System->GetPluginByName("Plugin_WB_Meeting"));
		if (pPluginWBMeeting == NULL)
		{
			return;
		}
		IPlugin_WB_FrameDlg* pPluginWBFrameDlg = dynamic_cast<IPlugin_WB_FrameDlg*>(g_System->GetPluginByName("Plugin_WB_FrameDlg"));
		if (pPluginWBFrameDlg != NULL)
		{
			if (m_usMemberType >4)
			{
				pPluginWBFrameDlg->SetWBAVLock(!bEnable);
			}
			
		}
	}
}


