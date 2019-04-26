#pragma once
#ifndef	_DATA_ROOMMEMBER_H__
#define _DATA_ROOMMEMBER_H__
#include "MMComFunction.h"
class IDataRoomMember //回调函数更新会议成员状态
{
public:
	virtual void OnMeetMemStatus(string strMemberID, unsigned char ucStatus,  unsigned short usMemberType) = 0;
};
//不再保存数据，只当监视器用，用于内部转发消息
class CDataRoomMember
{
public:
	CDataRoomMember(void);
	~CDataRoomMember(void);

public:
	void AddObserver(IDataRoomMember* pObserver);
	void RemoveObserver(IDataRoomMember* pObserver);
	void SetCurRoomID(std::string strRoomID) { m_strCurRoomID = strRoomID; };
	std::string GetCurRoomID() { return m_strCurRoomID; };
	void SetMapGroupInfo(string strGroupID, const MMS_GROUP_INFO& groupInfo);
	void SetMapMemberInfo(string strMemberID, const MEMEBER_INFO& memberInfo);

	const MAP_MMS_GROUP_INFO&	GetMapGroupInfo() const { return m_mapGroupInfo; }
	const MMS_GROUP_INFO*		GetGroupInfo(string strGroupID);
	const MAP_MEMEBER_INFO&	GetMapMemberInfo() const { return m_mapMemberInfo; }
	const MEMEBER_INFO*		GetMemberInfo(string strMemberID);

	void DelMemberInfo(string strMemberID);
	void ClearGroupCount(void);
	void ReleaseData(void);

	void OnMeetMemberStatusChage(string strMemID, unsigned char ucStatus,unsigned short usMemberType);

	void UpdateMemberGroupCount(string strParentID, unsigned char ucStatus, BOOL bTemporary);
private:
	std::string						m_strCurRoomID;
	std::list<IDataRoomMember*>		m_lstObserver;
	MAP_MMS_GROUP_INFO			m_mapGroupInfo;
	MAP_MEMEBER_INFO					m_mapMemberInfo;
};

CDataRoomMember& GetDataRoomMember(void);
#endif

