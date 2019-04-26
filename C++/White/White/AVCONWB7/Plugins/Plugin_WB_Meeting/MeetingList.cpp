#include "StdAfx.h"
#include "MeetingList.h"
#include "IPlugin/IU7T.Comm/IPlugin_U7_Common.h"
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"
#include "Conference.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;

CMeetingList* g_pMeetingList = NULL;
CMeetingList* GetMeetingList(void)
{
	return g_pMeetingList;
}

AVCONPlugin::CMeetingList::CMeetingList( void )
:m_pMemberTree(NULL)
,m_bLogout(false)
, m_nHiddenDir(DIR_TOP)
, m_bFloat(true)
, m_bTrack(false)
,m_pMeetingTip(NULL)
{
}



AVCONPlugin::CMeetingList::~CMeetingList( void )
{

}

#pragma region 继承WindowImplBase接口
//////////////////////////////////////////////////////////////////////////
// 继承WindowImplBase接口
void AVCONPlugin::CMeetingList::Init()
{
	paint_manager_.SetTransparent(200);//设置透明度
	m_pMemberTree= static_cast<CTreeUI*>(paint_manager_.FindControl(_T("meetings")));
	//m_pMemberTree= new CTreeUI();
	if(NULL == m_pMemberTree)
		return;
// 	m_pMemberTree->EnableDBClickExpand(false);
// 	m_pMemberTree->EnableUpdate(false);
	CString strTemp;
	strTemp.Format(_T("file='%s' corner='2,2,2,2'"), _T("..\\CommRes\\ListItemHot.png"));
	m_pMemberTree->SetHotItemImage(strTemp);
	strTemp.Format(_T("file='%s' corner='2,2,2,2'"),  _T("..\\CommRes\\ListItemSel.png"));
	m_pMemberTree->SetSelectedItemImage(strTemp);

	InitLocalLang();
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room");
	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pIPlugin);
	if (pU7MMRoom )
	{	
		pU7MMRoom->AddObserver(this);
		pU7MMRoom->LoadRoomList();
	}
	
	if (m_pMeetingTip == NULL)
	{
		m_pMeetingTip = new CMeetingTip();
		m_pMeetingTip->Create(GetHWND(), _T(""), UI_WNDSTYLE_FRAME & ~WS_MAXIMIZEBOX, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 74, 32);
		::SetWindowPos(m_pMeetingTip->GetHWND(),NULL,GetSystemMetrics(SM_CXSCREEN)*3/4,GetSystemMetrics(SM_CYSCREEN)/4,74,32,SWP_SHOWWINDOW);
		m_pMeetingTip->ShowWindow(false);
	}
	//开启手势支持
	//paint_manager_.EnableSysMTParser(MOUSE_GESTURE_PARSER);
}

LRESULT AVCONPlugin::CMeetingList::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room");
	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pIPlugin);
	if (NULL != pU7MMRoom)
	{
		pU7MMRoom->DelObserver(this);
	}
	CoUI_KillTimer(TIMER_REFRESHMEETINGLIST);
	if (m_pMeetingTip)
	{
		::DestroyWindow(m_pMeetingTip->GetHWND());
		delete m_pMeetingTip;
		m_pMeetingTip = NULL;
	}
	return WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
}



LRESULT AVCONPlugin::CMeetingList::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LPCTSTR AVCONPlugin::CMeetingList::GetWindowClassName() const
{
	return _T("CMeetingList");
}

CString AVCONPlugin::CMeetingList::GetSkinFile()
{
	return _T("MeetingList.xml");
}

LRESULT AVCONPlugin::CMeetingList::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	return WindowImplBase::OnSysCommand(uMsg, wParam, lParam, bHandled);
}

LRESULT AVCONPlugin::CMeetingList::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}



CControlUI* CMeetingList::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(pstrClass, _T("MeetingList")) == 0)
	{
		return new CTreeUI();
	}
	return NULL;
}

void AVCONPlugin::CMeetingList::AddMeetingList()
{
	RemoveAllMemList();
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room");
	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pIPlugin);
	if (pU7MMRoom == NULL)
		return;

	std::map<std::string,bool> mapFilter; //临时map用于筛选重复的会议
	MMS_ROOM_LST& lstRoom = pU7MMRoom->GetRoomList();
	MMS_ROOM_LST lstTmp;
	MMS_ROOM_LST::iterator it = lstRoom.begin();
	for (; it != lstRoom.end(); it++)
	{
		PMMS_ROOM pRoomListItem = *it;
		if (MRT_ADVANCE == pRoomListItem->roomtype)
		{
			lstTmp.push_back(*it);
		}
		else
		{
			lstTmp.push_front(*it);
		}
	}
	MMS_ROOM_LST::iterator itTmp = lstTmp.begin();
	for (; itTmp != lstTmp.end(); itTmp++)
	{
		PMMS_ROOM pRoomListItem = *itTmp;
		if (NULL == pRoomListItem)
			continue;
		std::map<std::string,bool>::iterator itFilter = mapFilter.find(pRoomListItem->roomid);
		if (itFilter == mapFilter.end())
		{
			AddMeetItem(pRoomListItem);
			mapFilter[pRoomListItem->roomid] = true;
		}
	}
}

void AVCONPlugin::CMeetingList::OnRoomListEnd( void )
{
	AddMeetingList();
}

void AVCONPlugin::CMeetingList::RemoveAllMemList()
{
	if (m_pMemberTree)
	{
		m_pMemberTree->RemoveAll();
	}
}

#pragma endregion 

void AVCONPlugin::CMeetingList::OnRoomListItem( const std::string& strRoomID,const std::string& strRoomName,const std::string& strRoomPwd, unsigned short usRoomType,const std::string& strDesc,int nMemberSum,int nOnlineSum )
{

}

void AVCONPlugin::CMeetingList::OnEnterRoom( unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType )
{
//  	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_Meeting");
//  	IPlugin_U7_Meeting* pU7MMRoom = dynamic_cast<IPlugin_U7_Meeting*>(pIPlugin);
//  	if (pU7MMRoom == NULL)
//  		return;
//  	pU7MMRoom->AVConf_Enter(usMemberType);
 	/*ShowWindow(SW_HIDE);*/
	if (nErrCode != 0)
	{
		return;
	}
// 	IPlugin_Meeting_Member* pPlugin_Meeting_Member = dynamic_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
// 	if (pPlugin_Meeting_Member)
// 	{
// 		pPlugin_Meeting_Member->ShowUI(true);
// 	}
	CoUI_KillTimer(TIMER_REFRESHMEETINGLIST);
	if (m_pMeetingTip)
	{
		m_pMeetingTip->ShowWindow(false);
	}
	HiddenWindow();
	ShowWindow(false,false);
}

void AVCONPlugin::CMeetingList::OnEnterRoomReady()
{
// 	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_Meeting");
// 	IPlugin_U7_Meeting* pU7MMRoom = dynamic_cast<IPlugin_U7_Meeting*>(pIPlugin);
// 	if (pU7MMRoom == NULL)
// 		return;
// 	pU7MMRoom->AVConf_Enter();
}

void AVCONPlugin::CMeetingList::EnterMeetingRoom(PMMS_ROOM pMMSRoom)
{
	if (pMMSRoom == NULL)
	{
		return;
	}
	GetConference().EnterRoom(pMMSRoom->domain, pMMSRoom->roomid, pMMSRoom->password);
//  	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room");
//  	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pIPlugin);
//  	if (pU7MMRoom == NULL)
//  		return;
//  	pU7MMRoom->EnterRoom(, "Online");
// 	IPlugin_Meeting_Member* plugin = dynamic_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
// 	if (plugin)
// 	{
// 		plugin->SetRoomName(pMMSRoom->roomname);
// 		plugin->MainFrameHWnd(this->GetHWND());
// 	}

	//ShowWindow(false,false);

// 	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_Meeting");
// 	IPlugin_U7_Meeting* pU7Meeting = dynamic_cast<IPlugin_U7_Meeting*>(pIPlugin);
// 	if (pU7Meeting == NULL)
// 		return;
// 
// 	CString strUserID;
// 	pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Friend");
// 	IPlugin_U7_IM_Friend* p7IMFriend= dynamic_cast<IPlugin_U7_IM_Friend*>(pIPlugin);
// 	if (p7IMFriend != NULL)
// 	{
// 		strUserID = A2U(p7IMFriend->GetUserIDWithDomain());
// 	}

// 	std::string strDomain   = pMMSRoom->domain;
// 	std::string strRoomID   = pMMSRoom->roomid;
// 	std::string strPassWord = pMMSRoom->password;
// 
// 
// 	//IPlugin_U7_Cfg_Comm* pIPlugin_U7_Cfg_Comm = dynamic_cast<IPlugin_U7_Cfg_Comm*>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
// 	//if (pIPlugin_U7_Cfg_Comm)
// 	//{
// 	//	pIPlugin_U7_Cfg_Comm->SetCfgString(strUserID, _T("Domain"), A2U(strDomain.c_str()));
// 	//	pIPlugin_U7_Cfg_Comm->SetCfgString(strUserID, _T("RoomID"), A2U(strRoomID.c_str()));
// 	//	pIPlugin_U7_Cfg_Comm->SetCfgString(strUserID, _T("PassWord"), A2U(strPassWord.c_str()));
// 	//	pIPlugin_U7_Cfg_Comm->SetCfgBool(strUserID, _T("NormalExitMeeting"), FALSE);
// 	//}
// 
// 
// 	pU7Meeting->AVConf_Enter(pMMSRoom->domain, pMMSRoom->roomid, pMMSRoom->password);
// 	Restore();//清空搜索内容
// 	ShowWindow(false, false);
// 	IPlugin_U7T_Main* pluginMain = dynamic_cast<IPlugin_U7T_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_Main"));
// 	if (pluginMain)
// 	{
// 		pluginMain->RemoveExistWnd();
// 	}
}

bool AVCONPlugin::CMeetingList::ProcessListItemBtnEvent(void* p)
{
	TEventUI *pEvent = static_cast<TEventUI*>(p); //把消息交给父窗口来处理
	if (pEvent->Type == UIEVENT_MOUSEENTER || pEvent->Type == UIEVENT_MOUSELEAVE && pEvent->pSender)
	{
		CControlUI *pControl = pEvent->pSender->GetParent();
		while (pControl)
		{
			pControl->Event(*pEvent);
			pControl = pControl->GetParent();
		}
	}
	return true;
}

void AVCONPlugin::CMeetingList::OnExitRoom( unsigned int nErrCode )
{
	//退会后刷新会议列表
	IPlugin* pPlugin = g_System->GetPluginByName("Plugin_U7_MM_Room"); 
	IPlugin_U7_MM_Room* pPluginMMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pPlugin);
	if (pPluginMMRoom)
	{
		pPluginMMRoom->LoadRoomList();
	}
	//设置皮肤路径
	paint_manager_.SetResourcePath(GetFolderPath());
	IPlugin_Meeting_Member *pPlugin_Meeting_Member = dynamic_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
	if (pPlugin_Meeting_Member)
	{
		pPlugin_Meeting_Member->DestroyUI();
	}
	if (m_pMeetingTip == NULL)
	{
		m_pMeetingTip = new CMeetingTip();
		m_pMeetingTip->Create(GetHWND(), _T("PromptLog"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0, 500, 350);
		::SetWindowPos(m_pMeetingTip->GetHWND(),NULL,500,600,100,25,SWP_SHOWWINDOW);
		m_pMeetingTip->ShowWindow(false);
	}
	ReShowWindow();
	this->ShowWindow(true,true);
}

LRESULT AVCONPlugin::CMeetingList::OnShowWindow( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	CoUI_KillTimer(TIMER_REFRESHMEETINGLIST);
	if (wParam == 1)
	{
		CoUI_SetTimer(TIMER_REFRESHMEETINGLIST,10000);
	}
	else
	{
		
	}
	return 0;
}

void AVCONPlugin::CMeetingList::AddMeetItem( PMMS_ROOM pRoomListItem )
{
	std::string name = pRoomListItem->roomname;
	CString strUnKnowType;
	strUnKnowType = ReadConfigText(_T("Meeting"), _T("UnKnowType"));
	if (strUnKnowType.IsEmpty())
	{
		strUnKnowType = _T("未知类型");
	}
 	CString strType = strUnKnowType;
	CString strSeniorConference;
	strSeniorConference = ReadConfigText(_T("Meeting"), _T("SeniorConference"));
	if (strSeniorConference.IsEmpty())
	{
		strSeniorConference = _T("高级会议");
	}
	CString strStandardConference;
	strStandardConference = ReadConfigText(_T("Meeting"), _T("StandardConference"));
	if (strStandardConference.IsEmpty())
	{
		strStandardConference = _T("标准会议");
	}
	CString strMonitorConference;
	strMonitorConference = ReadConfigText(_T("Meeting"), _T("MonitorConference"));
	if (strMonitorConference.IsEmpty())
	{
		strMonitorConference = _T("监控会议");
	}
	CString strPersonalConference;
	strPersonalConference = ReadConfigText(_T("Meeting"), _T("PersonalConference"));
	if (strPersonalConference.IsEmpty())
	{
		strPersonalConference = _T("个人会议");
	}
 	switch (pRoomListItem->roomtype)
 	{
 	case MRT_ADVANCE:
 		{
 			strType = strSeniorConference;
 		}
 		break;
 	case MRT_TEMPORARY:
 		{
 			strType = strStandardConference;
 		}
 		break;
 	case MRT_MONITOR:
 		{
 			strType = strMonitorConference;
 			return;
 		}
 		break;
 	case MRT_PERSONAL:
 		{
 			strType = strPersonalConference;
 		}
 	default:
 		break;
	}

	CTreeItemUI* pListElement = NULL;
	if( !m_dlgBuilder.GetMarkup()->IsValid() )
	{
		paint_manager_.SetResourcePath(GetSkinFolder());
		pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create(_T("MeetingListItem.xml"), (UINT)0, NULL, &paint_manager_));
		if(NULL == pListElement)
			return;
	}
	else
	{		
		pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
		if(NULL == pListElement)
			return;
		
	}

	pListElement->SetTag((UINT_PTR)pRoomListItem);
	pRoomListItem = (PMMS_ROOM)pListElement->GetTag();
	CLabelUI *pLbMemberTip = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("LblName")));
	if (pLbMemberTip)
	{
		if (pRoomListItem)
		{
			CString strMemSumTip;
			strMemSumTip.Format(_T(" %s (%d / %d)"), A2U(pRoomListItem->roomname.c_str()), pRoomListItem->onlinesum, pRoomListItem->membersum);
			pLbMemberTip->SetText(strMemSumTip);
			//pLbMemberTip->SetText(A2U(pRoomListItem->roomname.c_str()));
			//SetControlFontSize(pLbMemberTip, 20);
		}
	}
	CButtonUI *pButton = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, _T("optSet")));
	if (pButton)
	{
		pButton->OnEvent += MakeDelegate<CMeetingList, AVCONPlugin::CMeetingList>(this, &AVCONPlugin::CMeetingList::ProcessListItemBtnEvent);
	}
	
	CString strPath = GetFolderPath();
	CString strImage = _T("");
	if (pRoomListItem->onlinesum > 0)
	{
		strImage.Format(_T("%sMeetingIcon.png"), strPath);
	}
	else
	{
		strImage.Format(_T("%sMeetIcon.png"), strPath);
	}
	//m_pMemberList->SetItemPic(pListElement, strImage.GetBuffer());
	CLabelUI* lbLogo =static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement,_T("lblLogo")));
	if (lbLogo != NULL)
	{
		lbLogo->SetBkImage(strImage);
	}
	//m_pMemberList->Add(pListElement);
	m_pMemberTree->Insert(pListElement,NULL);
	
}

void AVCONPlugin::CMeetingList::InitLocalLang()
{
	CString strPluginName = _T("Plugin_U7T_Meeting");
	CLabelUI* lbLabel = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("lblTitle")));
	if (lbLabel)
	{
		CString strConferenceListe;
		strConferenceListe = ReadConfigText(_T("Meeting"), _T("ConferenceList"));
		if (strConferenceListe.IsEmpty())
		{
			strConferenceListe = _T("会议列表");
		}
		CString strText = strConferenceListe;
// 		if (GetPluginU7Lang())
// 			strText = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("ML_MEETINGLIST"),_T("会议列表"));
		lbLabel->SetText(strText);
	}

	CButtonUI* btnButton = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("btnLogout")));
	if (btnButton)
	{
		CString strCancel;
		strCancel = ReadConfigText(_T("Meeting"), _T("Cancel"));
		if (strCancel.IsEmpty())
		{
			strCancel = _T("注销");
		}
		CString strText = strCancel;
// 		if (GetPluginU7Lang())
// 			strText = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("CLOSE"),_T("关闭"));
		btnButton->SetText(strText);
	}
}
void AVCONPlugin::CMeetingList::HiddenWindow()
{
	m_bTrack = true;
	RECT rectWindow;     //当前窗口的位置
	GetWindowRect(m_hWnd, &rectWindow);
// 	if (m_nHiddenDir == DIR_LEFT)
// 	{
// 		if (rectWindow.right>= 40)
// 		{
// 			SetWindowPos(m_hWnd, NULL, rectWindow.left - 35, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
// 		}
// 		else
// 		{
// 			SetWindowPos(m_hWnd, NULL, 4 - (rectWindow.right-rectWindow.left), rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
// 			SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );//设置置顶
// 			CoUI_KillTimer(TIMER_MOVEINWINDOW);
// 			m_bTrack = true;
// 		}
// 	}
	if (m_nHiddenDir == DIR_TOP)
	{
		if (rectWindow.bottom>= 40)
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, rectWindow.top - 35, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, 0-(rectWindow.bottom-rectWindow.top)+5, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
			SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );//设置置顶
			CoUI_KillTimer(TIMER_MOVEINWINDOW);
			if (m_pMeetingTip)
			{
				m_pMeetingTip->ShowWindow(true);
			}
			m_bTrack = true;
		}
	}
 	else if (m_nHiddenDir == DIR_RIGHT)
 	{
 		if (rectWindow.left<= (GetSystemMetrics(SM_CXSCREEN) - 40))
 		{
 			SetWindowPos(m_hWnd, NULL, rectWindow.left + 34, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
 		}
 		else
 		{
 			SetWindowPos(m_hWnd,NULL, GetSystemMetrics(SM_CXSCREEN)-8, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
 			SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );//设置置顶
 			CoUI_KillTimer(TIMER_MOVEINWINDOW);
			if (m_pMeetingTip)
			{
				m_pMeetingTip->ShowWindow(true);
			}
 			m_bTrack = true;
 		}
 	}
	
}

void AVCONPlugin::CMeetingList::ReShowWindow()
{
	m_bTrack = false;
	RECT rectWindow;     //当前窗口的位置
	GetWindowRect(m_hWnd, &rectWindow);
// 	if (m_nHiddenDir == DIR_LEFT)
// 	{
// 		if(rectWindow.left<=(-40))
// 		{
// 			SetWindowPos(m_hWnd, NULL, rectWindow.left + 35, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
// 		}
// 		else
// 		{
// 			SetWindowPos(m_hWnd, NULL, 0, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
// 			CoUI_KillTimer(TIMER_MOVEOUTWINDOW);
// 			m_bTrack = false;
// 		}
// 	}
	if (m_nHiddenDir == DIR_TOP)
	{
		if (rectWindow.top<= -40)
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, rectWindow.top + 35, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, 0, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
			CoUI_KillTimer(TIMER_MOVEOUTWINDOW);
			if (m_pMeetingTip)
			{
				m_pMeetingTip->ShowWindow(false);
			}
			m_bTrack = false;
		}
	}
 	else if (m_nHiddenDir == DIR_RIGHT)
 	{
 		if (rectWindow.right>= (GetSystemMetrics(SM_CXSCREEN))+40)
 		{
 			SetWindowPos(m_hWnd, NULL, rectWindow.left - 35, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
 		}
 		else
 		{
 			SetWindowPos(m_hWnd, NULL, GetSystemMetrics(SM_CXSCREEN)-(rectWindow.right - rectWindow.left), rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
 			CoUI_KillTimer(TIMER_MOVEOUTWINDOW);
			if (m_pMeetingTip)
			{
				m_pMeetingTip->ShowWindow(false);
			}
 			m_bTrack = false;
 		}
 	}
}

LRESULT AVCONPlugin::CMeetingList::OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
 	RECT rectWindow;     //当前窗口的位置
 	POINT pointCursor;   //当前鼠标的位置
 	GetWindowRect(m_hWnd, &rectWindow);
 	GetCursorPos(&pointCursor);
 	if (!PtInRect(&rectWindow, pointCursor) &&!m_bFloat && !m_bTrack)
 	{
 		CoUI_SetTimer(TIMER_MOVEINWINDOW, 20);	
		CoUI_KillTimer(TIMER_MOVEOUTWINDOW);
 	}
 	if (m_bTrack)
 	{
          CoUI_SetTimer(TIMER_MOVEOUTWINDOW, 20);	
		  CoUI_KillTimer(TIMER_MOVEINWINDOW);
 	}
	return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

LRESULT AVCONPlugin::CMeetingList::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rectWindow;     //当前窗口的位置
	POINT pointCursor;   //当前鼠标的位置
	GetWindowRect(m_hWnd, &rectWindow);
	GetCursorPos(&pointCursor);
	if (rectWindow.top<=0 && rectWindow.right < GetSystemMetrics(SM_CXSCREEN))//本窗口移到了顶部位置
	{
		if ((pointCursor.y <= 50) && m_bFloat)
		{
			m_nHiddenDir = DIR_TOP;
			m_bFloat = false;
		}

	}
 	else if (rectWindow.right >= GetSystemMetrics(SM_CXSCREEN) && rectWindow.top > 0)//本窗口移到了右边位置
 	{
 		if ((pointCursor.x >= (GetSystemMetrics(SM_CXSCREEN) - 340)) && m_bFloat)
 		{
 			m_nHiddenDir = DIR_RIGHT;
 			m_bFloat = false;
 		}
 	}
// 	else if (rectWindow.left <=0)//本窗口移到了左边位置
// 	{
// 		if ((pointCursor.x <= 10) && m_bFloat)
// 		{
// 			//SetWindowPos(m_hWnd, NULL, 0, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
// 			//CoUI_SetTimer(TIMER_MOVEINWINDOW, 10);
// 			m_nHiddenDir = DIR_LEFT;
// 			m_bFloat = false;
// 		}
// 	}
	else
	{
		m_bFloat = true;
	}

	return WindowImplBase::OnMove(uMsg, wParam, lParam, bHandled);
}
void AVCONPlugin::CMeetingList::Notify(TNotifyUI& msg)
{
	if (COUI_MSGTYPE_TIMER == msg.nType)
	{	
		if (msg.wParam == TIMER_MOVEINWINDOW)
		{
			HiddenWindow();
		}
		else if (msg.wParam == TIMER_MOVEOUTWINDOW)
		{
			ReShowWindow();
		}
		else if (msg.wParam == TIMER_REFRESHMEETINGLIST)
		{
			IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room");
			IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(pIPlugin);
			if (pU7MMRoom == NULL)
				return;
			pU7MMRoom->LoadRoomList();
			paint_manager_.SetResourcePath(GetFolderPath());			
			this->ShowWindow(true,true);
		}

	}
	else if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		CString strName = msg.pSender->GetName();
		if (_tcsicmp(msg.pSender->GetName(), _T("btnLogout")) == 0 )
		{
			//注销
			IPlugin_U7T_MessageBox *pMessage = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
			if (pMessage == NULL) 
				return;
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}
			CString strIsCancel;
			strIsCancel = ReadConfigText(_T("Meeting"), _T("IsCancel"));
			if (strIsCancel.IsEmpty())
			{
				strIsCancel = _T("是否注销？");
			}
			int nResult = pMessage->MessageBox(GetHWND(),strPrompt,strIsCancel,MB_OKCANCEL,false);
			if (nResult == IDOK)
			{
				IPlugin_U7_IM_Login* pPlugin = static_cast<IPlugin_U7_IM_Login*>(g_System->GetPluginByName("Plugin_U7_IM_Login"));
				if (pPlugin != NULL)
					pPlugin->Logout();
				m_bLogout = true;
			}
			return;
		}
		else if ( _tcsicmp(msg.pSender->GetName(), _T("optSet") ) == 0)
		{
			EnterMeetingRoom((PMMS_ROOM)msg.pSender->GetParent()->GetParent()->GetParent()->GetTag());
		}
	}
	else if (COUI_MSGTYPE_ITEMACTIVATE == msg.nType)
	{
		CString strname = msg.pSender->GetClass();
		if (_tcsicmp(msg.pSender->GetClass(), _T("TreeItemUI")) == 0)
		{
			if (NULL != (PMMS_ROOM)msg.pSender->GetTag())
			{
				EnterMeetingRoom((PMMS_ROOM)msg.pSender->GetTag());
				::ReleaseCapture();
			}
		}
	}
	else if(COUI_MSGTYPE_WINDOWINIT == msg.nType)
	{
	}
	else if (COUI_MSGTYPE_KILLFOCUS == msg.nType)
	{
	}
	else if (COUI_MSGTYPE_ITEMSELECT == msg.nType)
	{
		int i = 0;
	}
}

void AVCONPlugin::CMeetingList::ProcessTipMessage()
{
	CoUI_SetTimer(TIMER_MOVEOUTWINDOW, 20);	
	CoUI_KillTimer(TIMER_MOVEINWINDOW);
}


