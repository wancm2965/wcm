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
		virtual void OnRoomListEnd(void){};

		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType){};
		/* 收到退出会议室事件
		* nErrCode返回登录结果值
		* 0: 表示正常退出会议室
		* 1: 表示所有者退出会议室
		* 2: 表示被踢出会议室
		* 3: 会议成员帐号在该服务器上不存在或该服务器没有加载会议模块 */
		virtual void OnExitRoom(unsigned int nErrCode){};
		virtual void OnEnterRoomReady(){};

		// 个人分组会议
		virtual void OnGetPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,bool bHaveFrame,unsigned short usStatus){};
		virtual void OnGetPersonalRoomEnd(void){};
		virtual void OnGetPersonalRoomGroup(const char* strRoomID,const MMS_GROUP& group){};
		virtual void OnGetPersonalRoomGroupEnd(const char* strRoomID){};
		virtual void OnGetPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType){};
		virtual void OnGetPersonalRoomMemberEnd(const char* strRoomID,const char* strGroupID){};
		virtual void OnAddPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,const char* strOwnerID){};
		virtual void OnDelPersonalRoom(const char* strRoomID){};
		virtual void OnRenamePersonalRoom(const char* strRoomID,const char* strMeetingName){};
		virtual void OnAddPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType,bool bSuccessFlag){};
		virtual void OnSetPersonalRoomMemberType(const char* strRoomID,const char* strMemberID,unsigned short usMemberType){};
		virtual void OnDelPersonalRoomMember(const char* strRoomID,const char* strMemberID){};
		virtual void OnAddPersonalRoomGroup(const MMS_GROUP& group){};
		virtual void OnAddPersonalRoomGroupEnd(const char* strRoomID){};
		/*会议中人员都退出*/
		virtual void OnStopPersonalRoom(const char* strRoomID){};
		/*获取指定会议室的一个在线成员*/
		virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID){};
		/*获取指定会议室的一个在线合成终端或主持人通道信息*/
		virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
			unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
			const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
			const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag){};
		/*通知个人会议发起人会议已经开始(第一个成员已经登录会议)*/
		virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID){};
		virtual void OnGetRoomMemberType(const std::string& strRoomID,const std::string& strMerberID,const unsigned short& usMemberType){};

		//招商证券投票详情和sip呼叫申请入会
		virtual void OnGetVoteDetail(unsigned long ulVoteID, const std::string& strTopic, const std::string& strUserID,
			const std::string& strUserName, unsigned int itemid, const std::string& itemname){};
		virtual void OnGetVoteDetailEnd(unsigned long ulVoteID){};
		virtual void OnRequestSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID){};
		virtual void OnSipQuitRequest(const std::string& strCallID, const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID){};

		virtual void OnResponseForOtherPresider(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID, bool bAccept){};
		//收到会议人员变化(供会议列表使用)
		virtual void OnRoomMemberStatusChange(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMerberID,
			const unsigned short& usStatus,const unsigned short& usMemberType){};

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
		* strRoomID:			会议室ID，特殊情况关键字：
										“LocalMeeting”	本地会议
										“Channel”			频道
		* strPassword:		需要MD5编码
		* strTermName:		终端名称 PC Terminal record */
		virtual void EnterRoom(const char* strDomain, const char* strRoomID, const char* strPassword, const char* strTermName, bool bPrisider = false, unsigned short usTermType = 0, bool bInvite = false) = 0;
		/* 退出会议室 */
		virtual void ExitRoom(bool bAllLogout = false, int nReason = 0) = 0;

		/* 是否已经在会 */
		virtual bool IsInMeeting(void) = 0;
		/* 是否在特殊会议中 */
		virtual bool IsInSpecialRoom(void) = 0;
		/* 判断strRoomID是否为特殊会议ID */
		virtual bool IsSpecialRoomID(const char* strRoomID) = 0;

		// 个人分组会议
		virtual void GetPersonalRoom(void) = 0;
		virtual void GetPersonalRoomGroup(const char* strRoomID) = 0;
		virtual void GetPersonalRoomMember(const char* strRoomID,const char* strGroupID) = 0;
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
		// 获取当前激活会议组
		virtual std::string GetActiveGroup(void) = 0;

		/* 获取会议人员权限类型
		strRoomID:		会议ID
		strMemberID:	人员ID*/
		virtual void GetRoomMemberType(const std::string& strRoomID,const std::string& strMemberID)=0;
	};
}
