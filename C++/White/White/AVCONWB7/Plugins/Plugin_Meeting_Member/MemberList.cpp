#include "stdafx.h"
#include "MemberList.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"


#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;

#define WM_LOGOUT_MEETING		WM_USER + 101
#define	 WM_SORT_TREE					WM_USER + 102
#pragma region 实现通用的对话框创建销毁方法
CDlgMemberList* g_pMemberList = NULL;
CDlgMemberList* CreateMemberList(HWND hParent)
{
	if (NULL == g_pMemberList)
	{
		g_pMemberList = new CDlgMemberList();
		g_pMemberList->Create(hParent, _T("MemberList"), UI_WNDSTYLE_FRAME & ~WS_MAXIMIZEBOX, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 600, 570);
		g_pMemberList->CenterWindow(hParent);
	}
	return g_pMemberList;
}

CDlgMemberList* GetMemberList(void)
{
	return g_pMemberList;
}
void HandleInitMemberInfo()
{
	if (g_pMemberList)
	{
		g_pMemberList->ThreadInitMemberInfo();
	}
}

void DestroyMemberList(void)
{
	if (g_pMemberList)
	{
		::DestroyWindow(g_pMemberList->GetHWND());
		delete g_pMemberList;
		g_pMemberList = NULL;
	}
}

#pragma endregion


CDlgMemberList::CDlgMemberList(void)
: m_pMemberTree(NULL)
, m_strMemberID("")
, m_pTreeElement(NULL)
, m_nHiddenDir(DIR_TOP)
, m_bFloat(true)
, m_bTrack(false)
,m_pMemberListTip(NULL)
{
}
CDlgMemberList::~CDlgMemberList(void)
{
}

void CDlgMemberList::Init()
{
	paint_manager_.SetTransparent(200);//设置透明度
	CString strLang = _T("");
	paint_manager_.GetDefaultFontColor();
	m_pMemberTree = static_cast<CTreeUI*>(paint_manager_.FindControl(kMemberListControlName));
	if(NULL == m_pMemberTree)
		return;
	m_pMemberTree->EnableUpdate(false);
	m_pMemberTree->EnableDBClickExpand(false);

	CString strTemp;
	strTemp.Format(_T("file='%s' corner='2,2,2,2'"), _T("..\\CommRes\\ListItemHot.png"));
	m_pMemberTree->SetHotItemImage(strTemp);
	strTemp.Format(_T("file='%s' corner='2,2,2,2'"),  _T("..\\CommRes\\ListItemSel.png"));
	m_pMemberTree->SetSelectedItemImage(strTemp);

	CButtonUI* pBtnClose = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("btnLogoutMeeting")));
	if (NULL != pBtnClose)
	{
		CString strExit;
		strExit = ReadConfigText(_T("PluginText"), _T("MEMBER_Exit"));
		if (strExit.IsEmpty())
		{
			strExit = _T("退会");
		}
		strLang = strExit;
		pBtnClose->SetText(strLang);
	}

	CLabelUI* pLblTitle = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("lblTitle")));
	if (NULL != pLblTitle)
	{
		CString strList;
		strList = ReadConfigText(_T("PluginText"), _T("MEMBER_List"));
		if (strList.IsEmpty())
		{
			strList = _T("人员列表");
		}
		strLang = strList;
		pLblTitle->SetText(strLang);
	}
	if (m_pMemberListTip == NULL)
	{
		m_pMemberListTip = new CMemberTip();
		m_pMemberListTip->Create(GetHWND(), _T(""), UI_WNDSTYLE_FRAME & ~WS_MAXIMIZEBOX, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0,74, 32);
		::SetWindowPos(m_pMemberListTip->GetHWND(),NULL,GetSystemMetrics(SM_CXSCREEN)*3/4,GetSystemMetrics(SM_CYSCREEN)/4,74,32,SWP_SHOWWINDOW);
		m_pMemberListTip->ShowWindow(false);
	}
	GetDataRoomMember().AddObserver(this);
	//开启手势支持
	//paint_manager_.EnableSysMTParser(MOUSE_GESTURE_PARSER);
}

LRESULT CDlgMemberList::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//释放节点内存
	//释放节点内存

	GetDataRoomMember().RemoveObserver(this);
	MAP_NODE_IT itNode = m_mapNodeGroup.begin();
	for (; itNode != m_mapNodeGroup.end(); itNode++)
	{
		delete itNode->second;
	}
	m_mapNodeGroup.clear();

	itNode = m_mapNodeMember.begin();
	for (; itNode != m_mapNodeMember.end(); itNode++)
	{
		delete itNode->second;
	}
	m_mapNodeMember.clear();
	if (m_pMemberListTip)
	{
		::DestroyWindow(m_pMemberListTip->GetHWND());
		delete m_pMemberListTip;
		m_pMemberListTip = NULL;
	}
	return WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
}

LRESULT CDlgMemberList::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rectWindow;     //当前窗口的位置
	POINT pointCursor;   //当前鼠标的位置
	GetWindowRect(m_hWnd, &rectWindow);
	GetCursorPos(&pointCursor);
	if (!PtInRect(&rectWindow, pointCursor) &&!m_bFloat && !m_bTrack)
	{
		//CoUI_KillTimer(TIMER_MOVEOUTWINDOW_MEMBER);
		CoUI_SetTimer(TIMER_MOVEINWINDOW_MEMBER, 20);	
		CoUI_KillTimer(TIMER_MOVEOUTWINDOW_MEMBER);

	}
	if (m_bTrack)
	{
		CoUI_SetTimer(TIMER_MOVEOUTWINDOW_MEMBER, 20);	
		CoUI_KillTimer(TIMER_MOVEINWINDOW_MEMBER);
	}
	return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

LRESULT CDlgMemberList::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (uMsg == WM_LOGOUT_MEETING)
	{
		bHandled = TRUE;
		IPlugin_U7T_MessageBox *pMessage = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
		if (pMessage == NULL) 
			return 0;
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		CString strIsExit;
		strIsExit = ReadConfigText(_T("PluginText"), _T("MEMBER_IsExit"));
		if (strIsExit.IsEmpty())
		{
			strIsExit = _T("是否确定退会？");
		}
		int nResult = pMessage->MessageBox(GetHWND(),strPrompt,strIsExit,MB_OKCANCEL,false);

		if (nResult == IDOK)
		{
			IPlugin_U7_MM_Room* pPlugin = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
			if (pPlugin == NULL)
			{
				return 0 ;
			}
			pPlugin->ExitRoom(false);
		}
	}
	if (uMsg == WM_SORT_TREE)
	{
		ASSERT(m_pMemberTree);
		m_pMemberTree->SortChildrenByKey(NULL,true); //启动排序
		m_pMemberTree->EnableUpdate(true);
		m_pMemberTree->NeedUpdate();
		ShowWindow();
		
	}
	return 0;
}

LPCTSTR CDlgMemberList::GetWindowClassName() const
{
	return _T("MemberList");
}

CString CDlgMemberList::GetSkinFile()
{
	return _T("MemberList.xml");
}

void CDlgMemberList::Notify(TNotifyUI& msg)
{
	// 列表item单击事件
	if (COUI_MSGTYPE_TIMER == msg.nType)
	{	
		if (msg.wParam == TIMER_MOVEINWINDOW_MEMBER)
		{
			
			HiddenWindow();
		}
		else if (msg.wParam == TIMER_MOVEOUTWINDOW_MEMBER)
		{
			ReShowWindow();
		}
	}
	else if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if (m_pMemberTree == NULL) return;
		if (_tcsicmp(msg.pSender->GetName(), kLogoutRoomName) == 0)
		{
			PostMessage(WM_LOGOUT_MEETING);
		}
	}
	else if ((COUI_MSGTYPE_ITEMCLICK == msg.nType) && _tcsicmp(msg.pSender->GetName(), _T("ListContainerElement")) == 0)
	{
		CTreeItemUI* pItem = (CTreeItemUI*)msg.pSender;
		if (NULL == pItem)
			return;

		if (pItem->IsHasChildren())
		{
			pItem->Expand(!pItem->IsExpanded());
		}
	}
}

void CDlgMemberList::OnlyShowOnlineUser()
{
	//设置结构列表
	m_pMemberTree->EnableUpdate(false);
	MAP_NODE_IT itNode = m_mapNodeMember.begin();
	for (; itNode != m_mapNodeMember.end(); itNode++)
	{
		ItemData* pUserData = itNode->second;
		if (NULL == pUserData)
		{
			return;
		}
		pUserData->bVisible = !(pUserData->bVisible);
		if (pUserData->bVisible == true)//显示
		{
			pUserData->pControlUI->SetVisible(true);
		}
		else//不显示
		{
			if (!IsUserInMeeting(pUserData))
			{
				pUserData->pControlUI->SetVisible(false);
			}
		}
	}
	m_pMemberTree->EnableUpdate(true);
	m_pMemberTree->NeedUpdate();
}


BOOL CDlgMemberList::SetItemStatusPic(CControlUI* pControlUI, CString strSubCtrlName, CString strLogo)
{	
	if(NULL == pControlUI)
		return FALSE;

	CLabelUI* pLabel = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pControlUI, strSubCtrlName));
	if (NULL == pLabel)
		return FALSE;
	CString strImage = pLabel->GetBkImage();
	CString strImageNew =_T("");
	if (strLogo != _T(""))
	{
		strImageNew = GetFolderPath() + strLogo;
	}
	if (strImage != strImageNew)
	{
		pLabel->SetBkImage(strImageNew);
		return TRUE;
	}
	return FALSE;
}

void CDlgMemberList::SetMemberItemLableText(CControlUI* pListElement, CString strSubCtrlName,CString strLogo)
{
	if(NULL == pListElement)
		return;
	CLabelUI* pLabel = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, strSubCtrlName));
	if (NULL == pLabel)
		return;
	pLabel->SetVisible(true);
	pLabel->SetShowHtml(true);

	UINT nTextStyle = pLabel->GetTextStyle();
	nTextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT);
	nTextStyle |= DT_BOTTOM| DT_RIGHT;//文字设置在右下角
	pLabel->SetTextStyle(nTextStyle);
	CString strText = pLabel->GetText();
	int nCount = atoi(U2A(strText).c_str());

	if (strLogo == _T(""))
	{
		if (nCount)
		{		
			strText.Format(_T("%d"),--nCount);
		}
	}
	else
	{
		strText.Format(_T("%d"),++nCount);
	}
	if (nCount<=0)
	{
		pLabel->SetText(_T(""));
		pLabel->SetBkImage(_T(""));
	}
	else
	{
		if (strLogo != _T(""))
		{
			CString csImgPath = GetFolderPath() + strLogo;
			pLabel->SetBkImage(csImgPath);
		}
		pLabel->SetText(strText);
	}
};

void CDlgMemberList::LoadRootTree()
{
	if (m_pTreeElement == NULL && m_pMemberTree != NULL)
	{
		CTreeItemUI* pListElement = NULL;
		if( !m_dlgBuilder.GetMarkup()->IsValid() )
		{
			paint_manager_.SetResourcePath(GetSkinFolder());
			pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create(_T("MemberItem.xml"), (UINT)0, NULL, &paint_manager_));
			if(NULL == pListElement)
				return;
		}
		else
		{
			pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
			if(NULL == pListElement)
				return;
		}
		CLabelUI *pLbMemberTip = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("lblName")));
		if (pLbMemberTip)
		{
			CString strDefault;
			strDefault = ReadConfigText(_T("PluginText"), _T("MEMBER_Default"));
			if (strDefault.IsEmpty())
			{
				strDefault = _T("默认会议");
			}
			CString strRoom = strDefault;
			IPlugin_WB_Meeting* pPlugin = dynamic_cast<IPlugin_WB_Meeting*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Meeting"));
			if (pPlugin)
			{
				strRoom = A2U(pPlugin->GetRoomName().c_str());
			}
			pLbMemberTip->SetText(strRoom);
		}
		CLabelUI* lbLogo =static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement,_T("lblLogo")));
		if (lbLogo != NULL)
		{
			//CString img = GetSkinFolder();
			//img += _T("IconMeeting.png");
			//lbLogo->SetBkImage(img);
		}
		pListElement->SetSortKey(0,_T("0"),false);
		m_pMemberTree->Insert(pListElement,NULL);
		m_pTreeElement = pListElement;
	}
}

void CDlgMemberList::ThreadInitMemberInfo()
{
	m_bStartThread = false;
	// 	unsigned int nThreadId = 0;
	// 	HANDLE hThread = NULL;
	// 	hThread = (HANDLE)_beginthreadex(NULL,0,ThreadFunc,this,0,&nThreadId);
	// 	if (hThread)
	// 	{
	// 		m_bStartThread = true;
	// 	}
	// 	CloseHandle(hThread);
	CString strIniBegin;
	strIniBegin = ReadConfigText(_T("PluginText"), _T("MEMBER_IniBegin"));
	if (strIniBegin.IsEmpty())
	{
		strIniBegin = _T("初始化人员列表开始");
	}
	CString strIniEnd;
	strIniEnd = ReadConfigText(_T("PluginText"), _T("MEMBER_IniEnd"));
	if (strIniEnd.IsEmpty())
	{
		strIniEnd = _T("初始化人员列表结束");
	}
	OutputSystemTime(strIniBegin);
	InitMemberInfo();
	OutputSystemTime(strIniEnd);
}

unsigned int __stdcall CDlgMemberList::ThreadFunc( void* param )
{
	CDlgMemberList* pDlgMl = static_cast<CDlgMemberList*>(param);
	if (pDlgMl)
	{
		pDlgMl->InitMemberInfo(); //仅初始化
	}
	return 0;
}

void CDlgMemberList::InitMemberInfo()
{
	LoadRootTree();
	// 	MAP_MEMEBER_INFO mapMemberInfo = GetDataRoomMember().GetMapMemberInfo();
	// 	if (0 == mapMemberInfo.size())
	// 		return;
	// 	MAP_MEMEBER_INFO_IT itMemberMap = mapMemberInfo.begin();
	// 	for (; itMemberMap != mapMemberInfo.end(); itMemberMap++)
	// 	{
	// 		MEMEBER_INFO MemberInfo = itMemberMap->second;
	// 		AddOneMemberInfo(MemberInfo);
	// 	}
	IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin_Member == NULL)
		return ;
	MMS_MEMBER_MAP MapGroup = pPlugin_Member->GetMemberList();
	MMS_MEMBER_MAP ::iterator it = MapGroup.begin();

	for (; it != MapGroup.end(); ++it)
	{
		PMMS_MEMBER_EX item  = it->second;
		if(item == NULL) 
			continue;
		MEMEBER_INFO MemberInfo;
		MemberInfo.strMemberID = item->memberid;
		MemberInfo.strMemberName = item->membername;
		MemberInfo.strParentID = item->groupid;
		MemberInfo.ucStatus = item->status;
		MemberInfo.nMemberType = item->membertype;
		MAP_NODE::iterator __it = m_mapNodeMember.find(MemberInfo.strMemberID);
		if (__it != m_mapNodeMember.end())
			continue;
		AddOneMemberInfo(MemberInfo);
	}
	//排序
	PostMessage(WM_SORT_TREE);
	//SendMessage(WM_SORT_TREE);
	if (m_pTreeElement)
	{
		m_pTreeElement->Expand(true);
		IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
		if (pPlugin_Wb_Dlg == NULL)
		{
			return ;
		}
		std::string strUser ;
		IPlugin_U7_MM_Member* pPluginMm = (IPlugin_U7_MM_Member*)g_System->GetPluginByName("Plugin_U7_MM_Member");
		if (pPluginMm == NULL)
		{
			return ;
		}
		strUser = pPluginMm->GetUserIDDomain();
		PMMS_MEMBER_EX pMemberEX = pPluginMm->GetMemberItem(strUser.c_str());
		if (pMemberEX)
		{		

			std::string strmenbername = pMemberEX->membername;
			CString str = A2U(strmenbername);	
			if (!str.IsEmpty())
			{
				pPlugin_Wb_Dlg->SetLocalUserName(str);
			}

		}
	}
	return;
}

bool CDlgMemberList::AddOneMemberInfo(MEMEBER_INFO&MemberInfo)
{
	//初始化用户节点数据
	ItemData* pUserNode = new ItemData;
	if (pUserNode == NULL)
	{
		return NULL;
	}
	pUserNode->bFolder = true;
	pUserNode->csID = MemberInfo.strMemberID;
	pUserNode->csNodeName = MemberInfo.strMemberName;
	if (pUserNode->csNodeName == "")
	{
		pUserNode->csNodeName = pUserNode->csID;
	}

	pUserNode->strItemType = ITEM_STYLE_MEETING_MEMBER;
	pUserNode->gStyle = STYLE_MEMBER;
	pUserNode->ucMeetingStatus = MemberInfo.ucStatus;
	pUserNode->nMemberType = MemberInfo.nMemberType;
	//添加节点
	CTreeItemUI* pListElement = NULL;
	if( !m_dlgBuilder.GetMarkup()->IsValid() )
	{
		paint_manager_.SetResourcePath(GetSkinFolder());
		pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create(_T("MemberItem.xml"), (UINT)0, NULL, &paint_manager_));
		if(NULL == pListElement)
			return false;
	}
	else
	{
		pListElement = static_cast<CTreeItemUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
		if(NULL == pListElement)
			return false;
	}
	pListElement->SetText(A2U(pUserNode->csNodeName));
	pListElement->SetTag((UINT_PTR)pUserNode);
	pUserNode->pControlUI = (CControlUI*)pListElement;

	CString img = GetSkinFolder();
	if (pUserNode->ucMeetingStatus)
	{
		pListElement->SetSortKey(1,_T("0"),false);
		img += _T("online.png"); 
	}
	else
	{
		pListElement->SetSortKey(1,_T("1"),false);
		img += _T("outline.png"); 
	}
	CLabelUI* lbLogo =static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement,_T("lblLogo")));
	if (lbLogo != NULL)
	{
		lbLogo->SetBkImage(img);
	}
	bool isok = m_pMemberTree->Insert((CControlUI*)pListElement, (CControlUI*)m_pTreeElement, UITREE_INSERT_LAST, NULL);

	if (isok)
	{
		m_mapNodeMember[ MemberInfo.strMemberID] = pUserNode;
	}
	return false;
}

void CDlgMemberList::OnMeetMemStatus( string strMemberID, unsigned char ucStatus, unsigned short usMemberType )
{
	ASSERT(m_pMemberTree);
	//非临时人员
	//修改父节点人数
	ItemData* pMember = NULL;

	MAP_NODE_IT itMember = m_mapNodeMember.find(strMemberID);
	if (itMember == m_mapNodeMember.end())
	{
		return;
	}
	pMember = itMember->second;
	ASSERT(pMember);
	if (pMember->ucMeetingStatus == ucStatus)
	{
		return;
	}
	pMember->ucMeetingStatus = ucStatus;
	pMember->nMemberType = usMemberType;//应付人员权限动态修改的情况
	CTreeItemUI* pUserItem = (CTreeItemUI*)pMember->pControlUI;
	if (pUserItem == NULL)
		return;
	if (ucStatus)//在会
	{
		SetItemPic(pMember,strOnlineHeadPicture, paint_manager_);
		pUserItem->SetSortKey(1,_T("0"),false);
	}
	else//不在会
	{
		pUserItem->SetSortKey(1,_T("1"),false);
		SetItemPic(pMember, strOfflineHeadPicture, paint_manager_);
	}
	//排序
	m_pMemberTree->SortChildrenByKey(m_pTreeElement,false);
	m_pMemberTree->NeedUpdate();
}


void CDlgMemberList::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	//delete this;
}

void CDlgMemberList::HiddenWindow()
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
// 			CoUI_KillTimer(TIMER_MOVEINWINDOW_MEMBER);
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
			CoUI_KillTimer(TIMER_MOVEINWINDOW_MEMBER);
			if (m_pMemberListTip)
			{
				m_pMemberListTip->ShowWindow(true);
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
 			CoUI_KillTimer(TIMER_MOVEINWINDOW_MEMBER);
 			if (m_pMemberListTip)
 			{
 				m_pMemberListTip->ShowWindow(true);
 			}
 			m_bTrack = true;
 		}
 	}

}

void CDlgMemberList::ReShowWindow()
{
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
// 			CoUI_KillTimer(TIMER_MOVEOUTWINDOW_MEMBER);
// 			m_bTrack = false;
// 		}
// 	}
	m_bTrack = false;
	
	if (m_nHiddenDir == DIR_TOP)
	{
		if (rectWindow.top<= -40)
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, rectWindow.top + 35, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(m_hWnd, NULL, rectWindow.left, 0, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, SWP_NOSIZE);
			CoUI_KillTimer(TIMER_MOVEOUTWINDOW_MEMBER);
			if (m_pMemberListTip)
			{
				m_pMemberListTip->ShowWindow(false);
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
 			CoUI_KillTimer(TIMER_MOVEOUTWINDOW_MEMBER);
 			if (m_pMemberListTip)
 			{
 				m_pMemberListTip->ShowWindow(false);
 			}
 			m_bTrack = false;
 		}
 	}

}

LRESULT CDlgMemberList::OnMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	RECT rectWindow;     //当前窗口的位置
	POINT pointCursor;   //当前鼠标的位置
	GetWindowRect(m_hWnd, &rectWindow);
	GetCursorPos(&pointCursor);
	if (rectWindow.top<=0 && rectWindow.right < GetSystemMetrics(SM_CXSCREEN))//本窗口移到了顶部位置
	{
		if ((pointCursor.y <= 10) && m_bFloat)
		{
			m_nHiddenDir = DIR_TOP;
			m_bFloat = false;
		}
	}
 	else if (rectWindow.right >= GetSystemMetrics(SM_CXSCREEN) && rectWindow.top > 0)//本窗口移到了右边位置
 	{
 		if ((pointCursor.x >= (GetSystemMetrics(SM_CXSCREEN) - 340)) && m_bFloat && (rectWindow.top > 10))
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
// 			//CoUI_SetTimer(TIMER_MOVEINWINDOW_MEMBER, 10);
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

void CDlgMemberList::ProcessTipMessage()
{
	CoUI_SetTimer(TIMER_MOVEOUTWINDOW_MEMBER, 20);	
	CoUI_KillTimer(TIMER_MOVEINWINDOW_MEMBER);
}


