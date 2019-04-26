// UCC7-MM人员相关功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_MM_Member_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_Member_Observer(){};

	public:
		virtual void OnRoomGroupItem(const MMS_GROUP_EX& item){};
		virtual void OnRoomGroupItemEnd(void){};
		virtual void OnRoomMemberItem(const MMS_MEMBER_EX& item){};
		virtual void OnRoomMemberItemEnd(void){};
		virtual void OnMemberCardItem(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};
		virtual void OnMemberCardItemEnd(void){};
		virtual void OnCardAudioItem(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, unsigned long ulAudioID){};
		virtual void OnSetCardName(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};

		// 收到会议透传消息
		virtual void OnMMUserData(const char* strFromID, const char* strCmdType, const char* strUserData){};

		virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus){};

		virtual void OnInvite(const std::string& strRoomID,const std::string& strRoomName,const std::string& strMemberID,const std::string& strMemberName){};
		virtual void OnReject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName ){};
	};

	// UCC7-MM人员相关功能实现插件
	class IPlugin_U7_MM_Member : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Member(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Member_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Member_Observer* pObserver) = 0;

		virtual std::string GetUserIDDomain() = 0;
		virtual MMS_GROUP_EX_MAP& GetGroupList() = 0;
		virtual MMS_MEMBER_LST& GetMemberList(void) = 0;
		virtual PMMS_MEMBER_EX GetMemberItem(const char* strMemberID) = 0;
		virtual MMS_CARD_MAP& GetCardList(void) = 0;
		virtual PMMS_CARD GetCardItem(const char* strMemberID, int nCardIndex) = 0;

		// 发送会议透传消息（0：发送给指定人员 1：发送给主持人 2：发送给所有人）
		virtual void SendMMUserData(const char* strToID, unsigned char ucTag, const char* strCmdType, const char* strUserData) = 0;

		virtual void InviteIn(const char* strMemberID,bool bPresider) = 0;
		virtual void InviteToPersonalRoom(const std::string& strRoomID, const std::string& strRoomName, const std::string& strRoomPwd, const std::string& strInviteeID, bool bPresider, const std::string& strDevID) = 0;
		virtual void InviteOut(const char* strMemberID) = 0;
		virtual void InviteLoginMeet(const std::string& strDomain) = 0;
		virtual void Reject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID ) = 0;
		virtual void AcceptMeetingInvite(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID, bool bArgee) = 0;
	};
}
