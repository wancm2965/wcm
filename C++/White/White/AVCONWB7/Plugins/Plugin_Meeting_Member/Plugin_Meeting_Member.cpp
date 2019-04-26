// Plugin_Meeting_Member.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Plugin_Meeting_Member.h"
#include "DataRoomMember.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"


CDlgMemberList* CreateMemberList(HWND hParent);
CDlgMemberList* GetMemberList(void);
void HandleInitMemberInfo();
void DestroyMemberList(void);

AVCONPlugin::CPlugin_Meeting_Member::CPlugin_Meeting_Member()
: m_pMemberList(NULL)
{
}

AVCONPlugin::CPlugin_Meeting_Member::~CPlugin_Meeting_Member()
{
}
std::string AVCONPlugin::CPlugin_Meeting_Member::Name()
{
	return "Plugin_Meeting_Member";
}

std::string AVCONPlugin::CPlugin_Meeting_Member::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_Meeting_Member";
}

bool AVCONPlugin::CPlugin_Meeting_Member::InitPlugin()
{
  	IPlugin_U7_MM_Member* pU7MMMember = (IPlugin_U7_MM_Member*)g_System->GetPluginByName("Plugin_U7_MM_Member");
  	if (NULL != pU7MMMember)
  	{
  		pU7MMMember->AddObserver(this);
  	}
	return true;
}

bool AVCONPlugin::CPlugin_Meeting_Member::ClosePlugin()
{
  	IPlugin_U7_MM_Member* pU7MMMember = (IPlugin_U7_MM_Member*)g_System->GetPluginByName("Plugin_U7_MM_Member");
  	if (NULL != pU7MMMember)
  	{
  		pU7MMMember->DelObserver(this);
  	}
/*	DestroyUI();*/
	return true;
}

void AVCONPlugin::CPlugin_Meeting_Member::ResleseResource()
{
	//释放数据类中的数据
	GetDataRoomMember().ReleaseData();
	//销毁三个列表
	DestroyMemberList();
}


void AVCONPlugin::CPlugin_Meeting_Member::DestroyUI()
{
	if (GetMemberList())
	{
		ResleseResource();
	}
}
void AVCONPlugin::CPlugin_Meeting_Member::ShowUI( bool bShow)
{
	if (bShow)
	{
		GetLangPath();
		if (NULL == GetMemberList())
		{
			HWND hParentWnd = NULL;
			IPlugin_WB_Main* pPlugin_main = dynamic_cast<IPlugin_WB_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Main"));
			if (pPlugin_main)
			{
				hParentWnd = pPlugin_main->GetExeParentWnd();
			}
			CreateMemberList(hParentWnd);
		}
	}
}

void AVCONPlugin::CPlugin_Meeting_Member::OnRoomGroupItem( const MMS_GROUP_EX& item )
{
	MMS_GROUP_INFO GroupInfo;
	GroupInfo.strGroupID		= item.groupid.c_str();
	GroupInfo.strGroupName		= item.groupname.c_str();
	GroupInfo.strParentID		= item.parentid.c_str();
	GroupInfo.levelid = item.levelid;

	GetDataRoomMember().SetMapGroupInfo(GroupInfo.strGroupID, GroupInfo);
}

void AVCONPlugin::CPlugin_Meeting_Member::OnRoomGroupItemEnd( void )
{

}

void AVCONPlugin::CPlugin_Meeting_Member::OnRoomMemberItem( const MMS_MEMBER_EX& item )
{
 	if (NULL != GetDataRoomMember().GetMemberInfo(item.memberid.c_str()) || item.memberid == "")
 	{
 		return;
 	}
	MEMEBER_INFO MemberInfo;
	MemberInfo.strMemberID		= item.memberid.c_str();
	MemberInfo.strMemberName	= item.membername.c_str();
	MemberInfo.strParentID		= item.groupid.c_str();
	MemberInfo.ucStatus			= item.status;
	MemberInfo.nMemberType      = item.membertype;
	GetDataRoomMember().SetMapMemberInfo(MemberInfo.strMemberID, MemberInfo);
		//添加到人员列表
	if (NULL != GetMemberList())
	{
		//GetMemberList()->AddGroupMember(MemberInfo.strMemberID);
		//发送状态改变（促使邀请、请出列表响应状态）
		OnMemberStatus(item.groupid.c_str(), item.memberid.c_str(), item.membertype, item.status);
	}
}

void AVCONPlugin::CPlugin_Meeting_Member::OnRoomMemberItemEnd( void )
{
	CString strBegin;
	strBegin = ReadConfigText(_T("PluginText"), _T("MEMBER_Begin"));
	if (strBegin.IsEmpty())
	{
		strBegin = _T("开始");
	}
	CString strThe;
	strThe = ReadConfigText(_T("PluginText"), _T("MEMBER_The"));
	if (strThe.IsEmpty())
	{
		strThe = _T("第");
	}
	CString strTimeToEnter;
	strTimeToEnter = ReadConfigText(_T("PluginText"), _T("MEMBER_TimeToEnter"));
	if (strTimeToEnter.IsEmpty())
	{
		strTimeToEnter = _T("次进");
	}
	CString strEnd;
	strEnd = ReadConfigText(_T("PluginText"), _T("MEMBER_End"));
	if (strEnd.IsEmpty())
	{
		strEnd = _T("结束");
	}
	static int nCount = 1;
	//OutputDebugString(_T("OnRoomMemberItemEnd( void ) ")+strBegin+"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	char pData[256] = {0};
	sprintf(pData,"第%d次进OnRoomMemberItemEnd",nCount);
	string strTemp = pData;
	//OutputDebugString(A2U(strTemp));
	//OutputDebugString(_T("\n"));
	//OutputDebugString(_T("OnRoomMemberItemEnd( void )")+ strEnd+" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
//	if (nCount == 1)
	{
// 		if (NULL == GetMemberList() && ::IsWindow(m_hParentWnd))
// 		{
// 			CreateMemberList(m_hParentWnd,U2A(m_strRoomName));
// 		}
		if (NULL != GetMemberList())
		{
			HandleInitMemberInfo();
		}
	}
	nCount ++;
}

void AVCONPlugin::CPlugin_Meeting_Member::OnMemberCardItemEnd( void )
{
	CString strBegin;
	strBegin = ReadConfigText(_T("PluginText"), _T("MEMBER_Begin"));
	if (strBegin.IsEmpty())
	{
		strBegin = _T("开始");
	}
	CString strThe;
	strThe = ReadConfigText(_T("PluginText"), _T("MEMBER_The"));
	if (strThe.IsEmpty())
	{
		strThe = _T("第");
	}
	CString strTimeToEnter;
	strTimeToEnter = ReadConfigText(_T("PluginText"), _T("MEMBER_TimeToEnter"));
	if (strTimeToEnter.IsEmpty())
	{
		strTimeToEnter = _T("次进");
	}
	CString strEnd;
	strEnd = ReadConfigText(_T("PluginText"), _T("MEMBER_End"));
	if (strEnd.IsEmpty())
	{
		strEnd = _T("结束");
	}
	static int nCount = 1;
	//OutputDebugString(_T("OnMemberCardItemEnd( void ) ") +strBegin+ " ***********************************************\n");
	char pData[256] = {0};
	sprintf(pData,"第%d次进OnMemberCardItemEnd",nCount);
	nCount ++;
	string strTemp = pData;
	//OutputDebugString(A2U(strTemp));
	//OutputDebugString(_T("\n"));
	//OutputDebugString(_T("OnMemberCardItemEnd( void ) ") +strEnd+ " ***********************************************\n");
	
}

void AVCONPlugin::CPlugin_Meeting_Member::OnMemberStatus( const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus )
{
	GetDataRoomMember().OnMeetMemberStatusChage(strMemberID, ucStatus, usMemberType);
}

void AVCONPlugin::CPlugin_Meeting_Member::ProcessTipMessage()
{
	if (GetMemberList())
	{
		GetMemberList()->ProcessTipMessage();
	}
}

