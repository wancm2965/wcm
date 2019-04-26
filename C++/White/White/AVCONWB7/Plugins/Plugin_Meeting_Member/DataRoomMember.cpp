#include "stdafx.h"
#include "DataRoomMember.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
using namespace AVCONPlugin;

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;

CDataRoomMember g_DataRoomMember;
CDataRoomMember& GetDataRoomMember(void)
{
	return g_DataRoomMember;
}

CDataRoomMember::CDataRoomMember(void)
{
}

CDataRoomMember::~CDataRoomMember(void)
{
	ReleaseData();
}

void CDataRoomMember::AddObserver( IDataRoomMember* pObserver )
{
	if (NULL == pObserver)
		return;
	m_lstObserver.remove(pObserver);
	m_lstObserver.push_back(pObserver);
}

void CDataRoomMember::RemoveObserver( IDataRoomMember* pObserver )
{
	m_lstObserver.remove(pObserver);
}

void CDataRoomMember::ReleaseData(void)
{
	m_mapGroupInfo.clear();
	m_mapMemberInfo.clear();
}

void CDataRoomMember::ClearGroupCount( void )
{
	MAP_MMS_GROUP_INFO_IT it = m_mapGroupInfo.begin();
	for (;it!=m_mapGroupInfo.end();it++)
	{
		it->second.nInMeetingCount = 0;
		it->second.nMemberCount = 0;
	}
}

const MMS_GROUP_INFO* CDataRoomMember::GetGroupInfo(string strGroupID)
{
	MAP_MMS_GROUP_INFO::const_iterator it = m_mapGroupInfo.find(strGroupID);
	if (it == m_mapGroupInfo.end())
	{
		return NULL;
	}
	return &(it->second);
}

const MEMEBER_INFO* CDataRoomMember::GetMemberInfo(string strMemberID)
{
	MAP_MEMEBER_INFO::const_iterator it = m_mapMemberInfo.find(strMemberID);
	if (it == m_mapMemberInfo.end())
	{
		return NULL;
	}
	return &(it->second);
}

void CDataRoomMember::SetMapGroupInfo( string strGroupID, const MMS_GROUP_INFO& groupInfo )
{
	m_mapGroupInfo[strGroupID] = groupInfo;
}

void CDataRoomMember::SetMapMemberInfo( string strMemberID, const MEMEBER_INFO& memberInfo )
{
	m_mapMemberInfo[strMemberID] = memberInfo;
}

void CDataRoomMember::DelMemberInfo( string strMemberID )
{
	MAP_MEMEBER_INFO::const_iterator it = m_mapMemberInfo.find(strMemberID);
	if (it != m_mapMemberInfo.end())
	{
		m_mapMemberInfo.erase(it);
	}
}

void CDataRoomMember::OnMeetMemberStatusChage( string strMemID, unsigned char ucStatus, unsigned short usMemberType)
{
	//判断是否为临时人员
	BOOL bTemporary = FALSE;
	if (usMemberType == MMT_PRESIDERINVITEE || usMemberType == MMT_INVITEE)//被邀请人员,是临时人员，进会添加，出会删除
	{
		bTemporary = TRUE;
	}
	//对邀请人员的情况进行操作
	const MEMEBER_INFO* pMemberInfo = GetMemberInfo(strMemID);
	if(bTemporary && 1 == ucStatus && NULL == pMemberInfo)//假如是临时人员上线且当前列表中没有该人员，添加之
	{
		IPlugin_U7_MM_Member* pPluginMm = (IPlugin_U7_MM_Member*)g_System->GetPluginByName("Plugin_U7_MM_Member");
		if (pPluginMm == NULL)
		{
			return;
		}
		PMMS_MEMBER_EX pMemberEX = pPluginMm->GetMemberItem(strMemID.c_str());
		if (pMemberEX)
		{		
			MEMEBER_INFO MemberInfo;
			MemberInfo.strMemberID = strMemID;
			MemberInfo.strMemberName = pMemberEX->membername;
			MemberInfo.nMemberType = pMemberEX->membertype;
			MemberInfo.strParentID = pMemberEX->groupid;
			MemberInfo.ucStatus = pMemberEX->status;
			SetMapMemberInfo(strMemID, MemberInfo);
		}
	}
	else if (bTemporary && 0 == ucStatus && pMemberInfo)//假如是临时人员，下线，其列表中该人存在，删除之
	{
		GetDataRoomMember().DelMemberInfo(strMemID);
	}
	else if (pMemberInfo)
	{
		//if(ucStatus == pMemberInfo->ucStatus)
		//	return;
		MEMEBER_INFO MemberInfo = *pMemberInfo;
		MemberInfo.ucStatus = ucStatus;
		MemberInfo.nMemberType = usMemberType;
		SetMapMemberInfo(strMemID, MemberInfo);
	}
	std::list<IDataRoomMember*>::iterator it = m_lstObserver.begin();
	for (; it != m_lstObserver.end(); it++)
	{
		(*it)->OnMeetMemStatus(strMemID, ucStatus, usMemberType);
	}
}

void CDataRoomMember::UpdateMemberGroupCount( string strParentID, unsigned char ucStatus, BOOL bTemporary )
{
	MAP_MMS_GROUP_INFO_IT it = m_mapGroupInfo.find(strParentID);
	if (it == m_mapGroupInfo.end())
		return;

	MMS_GROUP_INFO& GroupInfo = it->second;
	if (ucStatus)
	{
		GroupInfo.nInMeetingCount++;
	}
	else
	{
		if (GroupInfo.nInMeetingCount >0)
		{		
			GroupInfo.nInMeetingCount--;
		}
	}

	UpdateMemberGroupCount(GroupInfo.strParentID, ucStatus, bTemporary);
}
