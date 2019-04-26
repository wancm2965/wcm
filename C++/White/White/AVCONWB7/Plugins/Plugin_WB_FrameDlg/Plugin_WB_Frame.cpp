#include "stdafx.h"
#include "Plugin_WB_Frame.h"
#include "WB/WBDefine.h"
#include "AutoLockResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace AVCONPlugin;

AVCONPlugin::CPlugin_WB_FrameDlg::CPlugin_WB_FrameDlg( void )
: m_pWhiteBoardDlg(NULL)
, m_bConnect(false)
,m_strUserID("")
{
}

AVCONPlugin::CPlugin_WB_FrameDlg::~CPlugin_WB_FrameDlg( void )
{

}

std::string AVCONPlugin::CPlugin_WB_FrameDlg::Name()
{
	return "Plugin_WB_FrameDlg";
}

std::string AVCONPlugin::CPlugin_WB_FrameDlg::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_WB_FrameDlg";
}

bool AVCONPlugin::CPlugin_WB_FrameDlg::InitPlugin()
{
	IPlugin_WB_Meeting* pMeetingPlugin = dynamic_cast<IPlugin_WB_Meeting*>(g_System->GetPluginByName("Plugin_WB_Meeting"));
	if(pMeetingPlugin)
		pMeetingPlugin->RegCard(103,this);
	return true;
}

bool AVCONPlugin::CPlugin_WB_FrameDlg::ClosePlugin()
{
	IPlugin_U7_MM_Member* pPlugin = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin)
	{
		pPlugin->DelObserver(this);
	}
	return true;
}

HWND AVCONPlugin::CPlugin_WB_FrameDlg::GetHwnd( void )
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->GetSafeHwnd();
	}
	return NULL;
}

HWND AVCONPlugin::CPlugin_WB_FrameDlg::CreateWhiteBoard( void )
{
	if (NULL ==  m_pWhiteBoardDlg)
	{
		IPlugin_U7_MM_Member* pPlugin_Meeting_Member = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (pPlugin_Meeting_Member == NULL)
		{
			return NULL;
		}
		pPlugin_Meeting_Member->AddObserver(this);
		std::string strUserId = pPlugin_Meeting_Member->GetUserIDDomain();
		AutoLockResource l;
		m_pWhiteBoardDlg = new CWBPersonalDlg(0, strUserId);
		m_pWhiteBoardDlg->Create(CWBPersonalDlg::IDD, NULL);
		if (m_pWhiteBoardDlg->GetSafeHwnd() == NULL)
		{
			delete m_pWhiteBoardDlg;
			m_pWhiteBoardDlg = NULL;
			return NULL;
		}
		m_pWhiteBoardDlg->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
	}
	return m_pWhiteBoardDlg->GetSafeHwnd();
}

void AVCONPlugin::CPlugin_WB_FrameDlg::DestroyWhiteBoard( void )
{
	if (m_pWhiteBoardDlg)
	{
		IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member");
		IPlugin_U7_MM_Member* pU7MMMember = dynamic_cast<IPlugin_U7_MM_Member*>(pIPlugin);
		if (NULL != pU7MMMember)
		{
			pU7MMMember->DelObserver(this);
		}
		m_pWhiteBoardDlg->DestroyWBDlg();
		//m_pWhiteBoardDlg->SendMessage(WM_DESTROY);
		delete m_pWhiteBoardDlg;
		m_pWhiteBoardDlg = NULL;
	}
}

bool AVCONPlugin::CPlugin_WB_FrameDlg::Connect( void )
{
	if (NULL == m_pWhiteBoardDlg)
		return false;
		
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member");
	IPlugin_U7_MM_Member* pU7MMMember = dynamic_cast<IPlugin_U7_MM_Member*>(pIPlugin);
	if (NULL == pU7MMMember)
		return false;
	std::string strLocalUserID = pU7MMMember->GetUserIDDomain();
	GetCardWnd(strLocalUserID.c_str(),0);//0是由于参数暂时没用到
	PMMS_CARD pCardItem = pU7MMMember->GetCardItem(strLocalUserID.c_str(), 103);
	if (NULL == pCardItem)
		return false;

	// 	使用IM中的数据
	// 			IPlugin_U7_IM_Friend* pFriend = dynamic_cast<IPlugin_U7_IM_Friend*>
	// 							(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Friend"));
	// 			if (pFriend == NULL)
	// 				return false;
	// 		
	// 			std::string strLocalUserID = pFriend->GetUserIDWithDomain();
	// 			PIMS_CONTACT_ITEM pItem = pFriend->GetUserItem(strLocalUserID);
	// 			pItem->
	// 			PMMS_CARD pCardItem = pFriend->GetCardItem(strLocalUserID.c_str(), 103);
	// 			if (NULL == pCardItem)
	// 				return false;
	return m_pWhiteBoardDlg->Connect(pCardItem->videochnid, pCardItem->mcuid, pCardItem->mcuaddr, pCardItem->mcuport, "");
}

void AVCONPlugin::CPlugin_WB_FrameDlg::DisConnect( void )
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->ReleaseConnection();
	}
}

void AVCONPlugin::CPlugin_WB_FrameDlg::RequestAllObj( void )
{
	WB_ReqAllObj();
}

void AVCONPlugin::CPlugin_WB_FrameDlg::EnableTouch( bool bTouchScreen )
{
	WB_SetTouchAuthorization(bTouchScreen);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::SetLocalUserID( char *strUserID )
{
	WB_SetLocalUserID(strUserID);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::MemberOffline( char *strUserID )
{
	WB_MemberOffline(strUserID);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::Move( int x,int y,int cx,int cy,UINT nFlag )
{
	WB_MoveBorad(&CWnd::wndTop, CRect(x, y, cx, cy), nFlag);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::ProcessWbData( const char *pData, int nLen )
{
	WB_ProcessDataIn(pData, nLen);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::OnMemberStatus( const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus )
{
	if (ucStatus == 0) //人员下线消息
	{
		WB_MemberOffline(strMemberID);
	}
}

BOOL AVCONPlugin::CPlugin_WB_FrameDlg::SetWindowPos( HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags )
{
	if (m_pWhiteBoardDlg)
	{
		::SetWindowPos(m_pWhiteBoardDlg->m_hWnd,hWndInsertAfter,X,Y,cx,cy,uFlags);
		return TRUE;
	}
	return FALSE;
}

void AVCONPlugin::CPlugin_WB_FrameDlg::AddObserver( IPlugin_WB_FrameDlg_Observer* pObserver )
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->addObserver(pObserver);
	}
}

void AVCONPlugin::CPlugin_WB_FrameDlg::DelObserver( IPlugin_WB_FrameDlg_Observer* pObserver )
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->DelObserver(pObserver);
	}
}

HWND AVCONPlugin::CPlugin_WB_FrameDlg::GetCardWnd( const char* szMemberId,int nCardIndex )
{
	if ( m_pWhiteBoardDlg == NULL &&  CreateWhiteBoard() == NULL)
		return NULL; 
	m_pWhiteBoardDlg->SetLocalUserID(szMemberId);
	m_strUserID = szMemberId;
	//Connect();
	return m_pWhiteBoardDlg->GetSafeHwnd();
}

void AVCONPlugin::CPlugin_WB_FrameDlg::GetUserID( std::string &strUser )
{
	strUser = m_strUserID;
}

void AVCONPlugin::CPlugin_WB_FrameDlg::SetLocalUserName( CString str )
{
	if ( m_pWhiteBoardDlg == NULL &&  CreateWhiteBoard() == NULL)
		return ; 
	m_pWhiteBoardDlg->SetLoaclUserName(str);
}

void AVCONPlugin::CPlugin_WB_FrameDlg::SetWBAVLock( bool block )
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->SetWBLock(block);
	}
}

void AVCONPlugin::CPlugin_WB_FrameDlg::OpenResource()
{
	if (m_pWhiteBoardDlg)
	{
		WB_OpenResource();
	}
	
}

