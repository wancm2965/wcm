// WBMeetingListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBMeetingListDlg.h"
#include "WBLoginDlg.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"
#include "WBMainDlg.h"

// CWBMeetingListDlg 对话框

IMPLEMENT_DYNAMIC(CWBMeetingListDlg, CDialog)

CWBMeetingListDlg::CWBMeetingListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWBMeetingListDlg::IDD, pParent)
	, m_pParentWnd(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CWBMeetingListDlg::~CWBMeetingListDlg()
{

}

void CWBMeetingListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEETINGLIST, m_meetingListTree);
}

	
BEGIN_MESSAGE_MAP(CWBMeetingListDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MEETINGLIST, &CWBMeetingListDlg::OnTvnSelchangedMeetinglist)
	ON_NOTIFY(NM_DBLCLK, IDC_MEETINGLIST, &CWBMeetingListDlg::OnNMDblclkMeetinglist)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CWBMeetingListDlg 消息处理程序

BOOL CWBMeetingListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CRect rc;
	GetClientRect(&rc);
	m_meetingListTree.MoveWindow(rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top);
	m_meetingAvatarLst.Create(16, 16, ILC_COLOR32, 3, 3);//创建图像列表
	CBitmap bm;
	bm.LoadBitmap(IDB_WBMEETINGROOMNAME);//载入Bitmap资源
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//添加到图像列表
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBNOMEMBERINMEETING);//载入Bitmap资源
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//添加到图像列表
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGLSTNAME);//载入Bitmap资源
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//添加到图像列表
	bm.DeleteObject();
	m_meetingListTree.SetImageList(&m_meetingAvatarLst,TVSIL_NORMAL);
	ModifyStyleEx(0,WS_EX_APPWINDOW);
	this->CenterWindow(NULL);

	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room *>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room == NULL)
	{
		return FALSE;
	}
	pPlugin_Room->AddObserver(this); //添加观察者

	return TRUE;
}

HTREEITEM *CWBMeetingListDlg::GetCurrentMeeting()
{
	return &m_currentmeeting;
}

void CWBMeetingListDlg::InitMeetingList()
{
	if (m_meetingListTree.GetCount() != 0)
	{
		m_meetingListTree.DeleteAllItems();
	}

	HTREEITEM hItem;
	defaultMeetingLstRoot = m_meetingListTree.InsertItem(CString("默认会议室"),2,2,TVI_ROOT);
	advanceMeetingLstRoot = m_meetingListTree.InsertItem(CString("高级会议室"),2,2,TVI_ROOT,defaultMeetingLstRoot);

	IPlugin_U7_MM_Room* pPlugin = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin == NULL)
	{
		return;
	}
	MMS_ROOM_LST lstRoom = pPlugin->GetRoomList();

	MMS_ROOM_LST::iterator it = lstRoom.begin();
	PMMS_ROOM pRoom;
	CString strm;//会议名称后面的会议在线人数/总人数
	for (; it != lstRoom.end(); it++)
	{
		pRoom = *it;
		if (pRoom->roomtype == 2)//临时会议类型
		{
			strm = A2U(pRoom->roomname) + _T(" ") + _T("(") + _T("0/1") + _T(")");
			hItem = m_meetingListTree.InsertItem(strm,1,1,defaultMeetingLstRoot);
		} 
		else
		{
			CString strOnlineSum;
			CString strMemberSum;
			strOnlineSum.Format(_T("%d"),pRoom->onlinesum);
			strMemberSum.Format(_T("%d"),pRoom->membersum);
			strm = A2U(pRoom->roomname) + _T(" ") + _T("(") + strOnlineSum + _T("/") + strMemberSum + _T(")");
			if (pRoom->onlinesum == 0)
			{
				hItem = m_meetingListTree.InsertItem(strm,1,1,advanceMeetingLstRoot);
			} 
			else
			{
				hItem = m_meetingListTree.InsertItem(strm,0,0,advanceMeetingLstRoot);
			}			
		}		
		m_meetingListTree.SetItemData(hItem, (DWORD)pRoom);
	}
	m_meetingListTree.Expand(defaultMeetingLstRoot, TVE_EXPAND);
	m_meetingListTree.Expand(advanceMeetingLstRoot, TVE_EXPAND);
}

void CWBMeetingListDlg::InsertMember( CString strMemberName )
{
	m_menberlist.InsertItem(strMemberName);
}

CString * CWBMeetingListDlg::GetSelectMeetingName()//获取选中的会议室的名字
{
	return &strSelectMeetingName;
}

PMMS_ROOM CWBMeetingListDlg::GetCurrentMeetingInfo()
{
	return m_pRoom;
}

void CWBMeetingListDlg::OnTvnSelchangedMeetinglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_currentmeeting = pNMTreeView->itemNew.hItem;
	*pResult = 0;
}

void CWBMeetingListDlg::OnNMDblclkMeetinglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	HTREEITEM nhitem = m_meetingListTree.GetParentItem(*GetCurrentMeeting());
	if (nhitem == 0)//如果是父项则展开其所有子项
	{
		*pResult = 0;
		return;
	}
	else//如果是子项则打开白板
	{
//		WB_Close();	//释放白板中的资源

	  	PMMS_ROOM pRoom = (PMMS_ROOM)m_meetingListTree.GetItemData(m_currentmeeting);
		m_pRoom = pRoom;
//		GetDataHandle().SetMeetingMode(MEETING_MODE_NORMAL);// 统一写在datahandle里面保存	 这是没用并没有发送信息

		IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
		if (NULL == pPlugin_Room)
		{
			return;
		}
		pPlugin_Room->EnterRoom((pRoom->domain).c_str(), (pRoom->roomid).c_str(), (pRoom->password).c_str(), "PC");

	  	ShowWindow(SW_HIDE);
		strSelectMeetingName = A2U(pRoom->roomname);//得到选中的会议室的名字

		CWBMainDlg::g_pMainDlg->SetIsOnline(TRUE);
		CWBMainDlg::g_pMainDlg->SetWBStatic();
		CWBMainDlg::g_pMainDlg->ShowWindow(SW_SHOW);

		WB_SetIsLoacalWhiteboard(false);		//add by yang

  		CString strItem;
		CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pParentWnd;
   		strItem =(CString)"AVCONWB电子白板-" + *(pLoginDlg->GetLoginID()) + (CString)" - " + A2U(pRoom->roomname);
    	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//设置白板对话框的标题文字
	}
 	*pResult = 0;
}

void CWBMeetingListDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( (nID & 0xFFF0) == SC_CLOSE )
	{
		IPlugin_U7_IM_Login* pPlugin = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
		if (pPlugin == NULL)
		{
			return ;
		}
		pPlugin->Logout();

		CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pParentWnd;

		pLoginDlg->DeleteMeetingListDlg();
		pLoginDlg->ChangeLoginOrCancel();
		pLoginDlg->ShowWindow(SW_SHOW);

		CMenu * tempSubMenw;
		//文件子菜单	
		tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
		tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}	
}

void CWBMeetingListDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CWBMeetingListDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CWBMeetingListDlg::PreTranslateMessage( MSG* pMsg )
{
	//屏蔽键盘Enter消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	//屏蔽键盘Esc消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CWBMeetingListDlg::OnRoomListEnd(void)			//登入MCU后得到会议列表结束
{
	InitMeetingList();//初始化会议列表
	ShowWindow(SW_SHOW);
	CString strItem;
	IPlugin_U7_IM_Friend* pPlugin_Friend = static_cast<IPlugin_U7_IM_Friend *>(g_System->GetPluginByName("Plugin_U7_IM_Friend"));
	//A2U：ANSI转换为UNICODE (string 类型转化为 CString)
	strItem =(CString)"AVCONWB" + (CString)"-" + (CString)"(" + A2U(pPlugin_Friend->GetUserID()) + (CString)")";
	SetWindowText(strItem);
	m_pParentWnd->ShowWindow(SW_HIDE);

	//设置HPDTS的文件接收临时目录
// 	IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
// 	if (NULL == pMember)
// 		return;
// 	std::string strMemberID =pMember->GetUserIDDomain();
//	CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pParentWnd;
// 	CString cStrLoginId = *(pLoginDlg->GetLoginID());
// 	SetHPDTSSavePath(cStrLoginId);
}

void CWBMeetingListDlg::OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType)
{
	if (ERR_NO != nErrCode)
	{
		return;
	}
	CWBMainDlg::g_pMainDlg->SetMMS_Member_Type(usMemberType);
// 	//当会议成员状态全部接受完毕，就判断会议是不是有人，如果没人的话就发送打开白板广播
// 	int nOnlineCurMeetingSum = CWBMainDlg::g_pMainDlg->GetOnlineMemberCounts();
// 	if (nOnlineCurMeetingSum <= 1)
// 	{
// // 		int nScreenID = 2, nWindowID = 0;
// // 		CUICmdSender::Instance().AVCast(GetDataHandle().GetUserID(), \
// // 			HPWBCHANNEL_INDEX, HPAV_TYPE_VIDEO,nScreenID, nWindowID);		
// 
// 		//发布本地音视频通道
// 		//在这个地方判断会议类型
// 		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
// 
// 		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);
// 	}

}

void CWBMeetingListDlg::OnExitRoom(unsigned int nErrCode)
{
	unsigned long ulErrorCode = nErrCode;
	IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pPlugin_Function == NULL)
	{
		return ;
	}

	//IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
	//if (NULL == pPlugin_Member)
	//{
	//	return ;
	//}
	//std::string strMemberID = pPlugin_Member->GetUserIDDomain();
	//BOOL bPower = CWBMainDlg::g_pMainDlg->LimitsOfPower();
	//-------------------------------------------------------------------------
	//退出会议时，不会进退出会议的回调函数 OnExitRoom
	//在退出会议的时候，判断是会议是不是有人，如果没有的话就发送关闭白板的广播
	//if (m_onLineCurMeetingNumber <= 1)
	//{
	//	if (bPower)
	//	{
	//		//发布本地音视频通道
	//		//在这个地方判断会议类型
	//		//		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function *>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
	//		//if ( GetCurrentMeetingInfo()->roommode  == MEETING_MODE_NORMAL)
	//		pPlugin_Function->CloseBroadcast(strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
	//	}
	//}

	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (NULL == pPlugin_Room)
	{
		return ;
	}
	pPlugin_Room->ExitRoom();
	CWBMainDlg::g_pMainDlg->SetIsReceiveBoardcast(FALSE);
	if (CWBMainDlg::g_pMainDlg->GetIsNeedRealseTouchScreenAuthorize())//释放服务器触摸授权
	{
		pPlugin_Function->ReleaseTouchScreenAuthorize();
		CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(FALSE);
	}
	//放开单机版的白板内授权，如果硬件支持
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
	if (!bHardwareTouch) //硬件不支持触摸
	{
		WB_SetTouchAuthorization(FALSE);
	} 
	else
	{
		WB_SetTouchAuthorization(TRUE);
	}
	IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return ;
	}
	pPlugin_Login->Logout();
	CWBMainDlg::g_pMainDlg->SetIsOnline(FALSE);

	CString strItem;
	strItem =(CString)"AVCONWB电子白板";
	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//设置白板对话框的标题文字

	WB_CloseAllChildBox();//wangxiaobin
	CWBMainDlg::g_pMainDlg->SetHPDTSNotify(NULL);

	CWBMainDlg::g_pMainDlg->SetWBStatic();

	CWBMainDlg::g_pMainDlg->SetisLocalBoardCastWB(false);

	(CWBMainDlg::g_pMainDlg->GetLoginDlg())->ChangeLoginOrCancel();
	(CWBMainDlg::g_pMainDlg->GetLoginDlg())->ShowWindow(SW_SHOW);

	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
}

void CWBMeetingListDlg::OnEnterRoomReady()
{
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
	if (!bHardwareTouch) //硬件不支持触摸
	{
		bool bIsNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(FALSE);
		CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(bIsNeedRealseTouchScreenAuthorize);
	} 
	else
	{
		if ((CWBMainDlg::g_pMainDlg->GetLoginDlg() != NULL) && !((CWBMainDlg::g_pMainDlg->GetLoginDlg())->GetIsNeedToCheckTouch())) //没有勾选多点触摸功能
		{
			bool bIsNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(FALSE);
			CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(bIsNeedRealseTouchScreenAuthorize);
		}
		else //向服务器申请触摸授权
		{
			IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
			if (pPlugin_Function == NULL)
			{
				return ;
			}
			pPlugin_Function->GetTouchScreenAuthorize();
		}
	}
}

BOOL CWBMeetingListDlg::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;

	return CDialog::Create(nIDTemplate, pParentWnd);
}


void CWBMeetingListDlg::OnDestroy()
{
	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (NULL == pPlugin_Room)
	{
		return ;
	}
	pPlugin_Room->DelObserver(this);
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
