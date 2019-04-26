// WBMeetingListDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBMeetingListDlg.h"
#include "WBLoginDlg.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"
#include "WBMainDlg.h"

// CWBMeetingListDlg �Ի���

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


// CWBMeetingListDlg ��Ϣ�������

BOOL CWBMeetingListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	CRect rc;
	GetClientRect(&rc);
	m_meetingListTree.MoveWindow(rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top);
	m_meetingAvatarLst.Create(16, 16, ILC_COLOR32, 3, 3);//����ͼ���б�
	CBitmap bm;
	bm.LoadBitmap(IDB_WBMEETINGROOMNAME);//����Bitmap��Դ
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//��ӵ�ͼ���б�
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBNOMEMBERINMEETING);//����Bitmap��Դ
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//��ӵ�ͼ���б�
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGLSTNAME);//����Bitmap��Դ
	m_meetingAvatarLst.Add(&bm, RGB(0, 0, 0));//��ӵ�ͼ���б�
	bm.DeleteObject();
	m_meetingListTree.SetImageList(&m_meetingAvatarLst,TVSIL_NORMAL);
	ModifyStyleEx(0,WS_EX_APPWINDOW);
	this->CenterWindow(NULL);

	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room *>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room == NULL)
	{
		return FALSE;
	}
	pPlugin_Room->AddObserver(this); //��ӹ۲���

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
	defaultMeetingLstRoot = m_meetingListTree.InsertItem(CString("Ĭ�ϻ�����"),2,2,TVI_ROOT);
	advanceMeetingLstRoot = m_meetingListTree.InsertItem(CString("�߼�������"),2,2,TVI_ROOT,defaultMeetingLstRoot);

	IPlugin_U7_MM_Room* pPlugin = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin == NULL)
	{
		return;
	}
	MMS_ROOM_LST lstRoom = pPlugin->GetRoomList();

	MMS_ROOM_LST::iterator it = lstRoom.begin();
	PMMS_ROOM pRoom;
	CString strm;//�������ƺ���Ļ�����������/������
	for (; it != lstRoom.end(); it++)
	{
		pRoom = *it;
		if (pRoom->roomtype == 2)//��ʱ��������
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

CString * CWBMeetingListDlg::GetSelectMeetingName()//��ȡѡ�еĻ����ҵ�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_currentmeeting = pNMTreeView->itemNew.hItem;
	*pResult = 0;
}

void CWBMeetingListDlg::OnNMDblclkMeetinglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	HTREEITEM nhitem = m_meetingListTree.GetParentItem(*GetCurrentMeeting());
	if (nhitem == 0)//����Ǹ�����չ������������
	{
		*pResult = 0;
		return;
	}
	else//�����������򿪰װ�
	{
//		WB_Close();	//�ͷŰװ��е���Դ

	  	PMMS_ROOM pRoom = (PMMS_ROOM)m_meetingListTree.GetItemData(m_currentmeeting);
		m_pRoom = pRoom;
//		GetDataHandle().SetMeetingMode(MEETING_MODE_NORMAL);// ͳһд��datahandle���汣��	 ����û�ò�û�з�����Ϣ

		IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
		if (NULL == pPlugin_Room)
		{
			return;
		}
		pPlugin_Room->EnterRoom((pRoom->domain).c_str(), (pRoom->roomid).c_str(), (pRoom->password).c_str(), "PC");

	  	ShowWindow(SW_HIDE);
		strSelectMeetingName = A2U(pRoom->roomname);//�õ�ѡ�еĻ����ҵ�����

		CWBMainDlg::g_pMainDlg->SetIsOnline(TRUE);
		CWBMainDlg::g_pMainDlg->SetWBStatic();
		CWBMainDlg::g_pMainDlg->ShowWindow(SW_SHOW);

		WB_SetIsLoacalWhiteboard(false);		//add by yang

  		CString strItem;
		CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pParentWnd;
   		strItem =(CString)"AVCONWB���Ӱװ�-" + *(pLoginDlg->GetLoginID()) + (CString)" - " + A2U(pRoom->roomname);
    	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//���ðװ�Ի���ı�������
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
		//�ļ��Ӳ˵�	
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

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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
	//���μ���Enter��Ϣ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	//���μ���Esc��Ϣ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CWBMeetingListDlg::OnRoomListEnd(void)			//����MCU��õ������б����
{
	InitMeetingList();//��ʼ�������б�
	ShowWindow(SW_SHOW);
	CString strItem;
	IPlugin_U7_IM_Friend* pPlugin_Friend = static_cast<IPlugin_U7_IM_Friend *>(g_System->GetPluginByName("Plugin_U7_IM_Friend"));
	//A2U��ANSIת��ΪUNICODE (string ����ת��Ϊ CString)
	strItem =(CString)"AVCONWB" + (CString)"-" + (CString)"(" + A2U(pPlugin_Friend->GetUserID()) + (CString)")";
	SetWindowText(strItem);
	m_pParentWnd->ShowWindow(SW_HIDE);

	//����HPDTS���ļ�������ʱĿ¼
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
// 	//�������Ա״̬ȫ��������ϣ����жϻ����ǲ������ˣ����û�˵Ļ��ͷ��ʹ򿪰װ�㲥
// 	int nOnlineCurMeetingSum = CWBMainDlg::g_pMainDlg->GetOnlineMemberCounts();
// 	if (nOnlineCurMeetingSum <= 1)
// 	{
// // 		int nScreenID = 2, nWindowID = 0;
// // 		CUICmdSender::Instance().AVCast(GetDataHandle().GetUserID(), \
// // 			HPWBCHANNEL_INDEX, HPAV_TYPE_VIDEO,nScreenID, nWindowID);		
// 
// 		//������������Ƶͨ��
// 		//������ط��жϻ�������
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
	//�˳�����ʱ��������˳�����Ļص����� OnExitRoom
	//���˳������ʱ���ж��ǻ����ǲ������ˣ����û�еĻ��ͷ��͹رհװ�Ĺ㲥
	//if (m_onLineCurMeetingNumber <= 1)
	//{
	//	if (bPower)
	//	{
	//		//������������Ƶͨ��
	//		//������ط��жϻ�������
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
	if (CWBMainDlg::g_pMainDlg->GetIsNeedRealseTouchScreenAuthorize())//�ͷŷ�����������Ȩ
	{
		pPlugin_Function->ReleaseTouchScreenAuthorize();
		CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(FALSE);
	}
	//�ſ�������İװ�����Ȩ�����Ӳ��֧��
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
	if (!bHardwareTouch) //Ӳ����֧�ִ���
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
	strItem =(CString)"AVCONWB���Ӱװ�";
	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//���ðװ�Ի���ı�������

	WB_CloseAllChildBox();//wangxiaobin
	CWBMainDlg::g_pMainDlg->SetHPDTSNotify(NULL);

	CWBMainDlg::g_pMainDlg->SetWBStatic();

	CWBMainDlg::g_pMainDlg->SetisLocalBoardCastWB(false);

	(CWBMainDlg::g_pMainDlg->GetLoginDlg())->ChangeLoginOrCancel();
	(CWBMainDlg::g_pMainDlg->GetLoginDlg())->ShowWindow(SW_SHOW);

	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
}

void CWBMeetingListDlg::OnEnterRoomReady()
{
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
	if (!bHardwareTouch) //Ӳ����֧�ִ���
	{
		bool bIsNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(FALSE);
		CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(bIsNeedRealseTouchScreenAuthorize);
	} 
	else
	{
		if ((CWBMainDlg::g_pMainDlg->GetLoginDlg() != NULL) && !((CWBMainDlg::g_pMainDlg->GetLoginDlg())->GetIsNeedToCheckTouch())) //û�й�ѡ��㴥������
		{
			bool bIsNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(FALSE);
			CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(bIsNeedRealseTouchScreenAuthorize);
		}
		else //����������봥����Ȩ
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

	// TODO: �ڴ˴������Ϣ����������
}
