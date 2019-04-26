// UCC7-MM房间功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_MM_Room_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_Room_Observer(){};

	public:
		virtual void OnRoomListEnd(void) = 0;

		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType) = 0;
		virtual void OnExitRoom(unsigned int nErrCode) = 0;
		virtual void OnEnterRoomReady() = 0;

		// 个人分组会议
		virtual void OnGetPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,bool bHaveFrame,unsigned short usStatus) = 0;
		virtual void OnGetPersonalRoomEnd(void) = 0;
		virtual void OnGetPersonalRoomGroup(const char* strRoomID,const MMS_GROUP& group) = 0;
		virtual void OnGetPersonalRoomGroupEnd(const char* strRoomID) = 0;
		virtual void OnGetPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void OnGetPersonalRoomMemberEnd(const char* strRoomID) = 0;
		virtual void OnAddPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,const char* strOwnerID) = 0;
		virtual void OnDelPersonalRoom(const char* strRoomID) = 0;
		virtual void OnRenamePersonalRoom(const char* strRoomID,const char* strMeetingName) = 0;
		virtual void OnAddPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void OnSetPersonalRoomMemberType(const char* strRoomID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void OnDelPersonalRoomMember(const char* strRoomID,const char* strMemberID) = 0;
		virtual void OnAddPersonalRoomGroup(const MMS_GROUP& group) = 0;
		virtual void OnAddPersonalRoomGroupEnd(const char* strRoomID) = 0;
		/*会议中人员都退出*/
		virtual void OnStopPersonalRoom(const char* strRoomID) = 0;
		/*获取指定会议室的一个在线成员*/
		virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID) = 0;
		/*获取指定会议室的一个在线合成终端或主持人通道信息*/
		virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
			unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
			const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
			const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag) = 0;
		/*通知个人会议发起人会议已经开始(第一个成员已经登录会议)*/
		virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID) = 0;
	};

	// UCC7-MM房间功能实现插件
	class IPlugin_U7_MM_Room : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Room(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Room_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Room_Observer* pObserver) = 0;

		virtual void LoadRoomList(void) = 0;
		virtual MMS_ROOM_LST& GetRoomList(void) = 0;
		virtual PMMS_ROOM GetRoomItem(const char* strDomain, const char* strRoomID) = 0;

		/* 登录会议室
		* strDomain:			会议室所在域
		* strRoomID:			会议室ID
		* strPassword:			需要MD5编码
		* strTermName:			终端名称 PC Terminal record */
		virtual void EnterRoom(const char* strDomain, const char* strRoomID, const char* strPassword, const char* strTermName) = 0;

		/* 会议室*/
		virtual void ExitRoom(void) = 0;

		/* 是否已经在会*/
		virtual bool IsInMeeting(void) = 0;

		// 个人分组会议
		virtual void GetPersonalRoom(void) = 0;
		virtual void GetPersonalRoomGroup(const char* strRoomID) = 0;
		virtual void GetPersonalRoomMember(const char* strRoomID) = 0;
		virtual void AddPersonalRoom(const char* strRoomName) = 0;
		virtual void DelPersonalRoom(const char* strRoomID) = 0;
		virtual void AddPersonalRoomGroup(const MMS_GROUP& group) = 0;
		virtual void DelPersonalRoomGroup(const char* strRoomID,const char* strGroupID) = 0;
		virtual void StartPersonalRoom(const char* strRoomID) = 0;

		/* 添加个人会议组 */
		virtual void SendPersonalAddGroupEnd(const char* strRoomID) = 0;

		/* 如果不重新设置会议类型，默认保存的是无层次结构 */
		virtual void SetPersonalRoomType(const char* strRoomID,bool bTreeMode=false) = 0;
		virtual void RenamePersonalRoom(const char* strRoomID,const char* strRoomName) = 0;
		virtual void AddPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void SetPersonalRoomMemberType(const char* strRoomID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void DelPersonalRoomMember(const char* strRoomID,const char* strMemberID) = 0;

		virtual void GetRoomInfo(std::string& strDomainID, std::string& strRoomID, std::string& strRoomName, std::string& strRoomPwd) = 0;
	};
}
