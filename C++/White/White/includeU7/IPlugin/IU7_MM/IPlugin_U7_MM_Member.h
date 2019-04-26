// UCC7-MM��Ա��ع���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_MMSDefine.h"

#ifndef STDSTRINGMAP
#define STDSTRINGMAP
typedef std::map<std::string,std::string> STRING_MAP_EX;
#endif

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
		virtual void OnMemberCardItemOffline(const std::string& strMemberID,int nCardIndex,const std::string& strCardType){};
		virtual void OnMemberCardItemEnd(void){};
		virtual void OnCardAudioItem(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, unsigned long ulAudioID){};
		virtual void OnSetCardName(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};

		// �յ�����͸����Ϣ
		virtual void OnMMUserData(const char* strFromID, const char* strCmdType, const char* strUserData){};
		// �յ�����������
		virtual void OnMMMonData(const char* strFromID, const char* strCmdType, const char* strUserData){};

		virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus){};

		virtual void OnInvite(const std::string& strRoomID,const std::string& strRoomName,const std::string& strMemberID,const std::string& strMemberName){};
		virtual void OnReject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName ){};

		virtual void OnGetSerialServerIP(const std::string& strDevID,const std::string& strSerialServerIP){};
	};

	// UCC7-MM��Ա��ع���ʵ�ֲ��
	class IPlugin_U7_MM_Member : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Member(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Member_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Member_Observer* pObserver) = 0;

		virtual std::string GetUserIDDomain() = 0;
		virtual std::string GetUserName() = 0;
		virtual void LoadRoomGroupMemberInfo(const std::string& strGroupID) = 0;
		virtual MMS_GROUP_EX_MAP& GetGroupList() = 0;
		virtual MMS_MEMBER_MAP& GetMemberList(void) = 0;
		virtual PMMS_MEMBER_EX GetMemberItem(const char* strMemberID) = 0;
		virtual MMS_CARD_MAP& GetCardList(void) = 0;
		virtual PMMS_CARD GetCardItem(const char* strMemberID, int nCardIndex) = 0;
		virtual ROOM_MPS* GetRoomMPSItem(const char* strUserID) = 0;

		// ���ͻ���͸����Ϣ��0�����͸�ָ����Ա 1�����͸������� 2�����͸������ˣ�
		virtual void SendMMUserData(const char* strToID, unsigned char ucTag, const char* strCmdType, const char* strUserData) = 0;
		virtual void SendMMUserData(const char* strToID, unsigned char ucTag, const STRING_MAP_EX& mapUserData) = 0;
		// ���ͻ���������
		virtual void SendMMMonData(const char* strToID, const char* strCmdType, const char* strUserData) = 0;

		virtual void InviteIn(const char* strMemberID,bool bPresider) = 0;
		virtual void InviteAll(bool bPresider) = 0;
		virtual void InviteToPersonalRoom(const std::string& strRoomID, const std::string& strRoomName, const std::string& strRoomPwd, const std::string& strInviteeID, bool bPresider, const std::string& strDevID) = 0;
		virtual void InviteOut(const char* strMemberID) = 0;
		virtual void InviteLoginMeet(const std::string& strMemID, const std::string& strDomain, std::string strTermName = "PC") = 0;
		virtual void Reject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID ) = 0;
		virtual void AcceptMeetingInvite(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID, bool bArgee, std::string strTermName = "PC") = 0;

		// ��ȡ���ڷ�����IP
		virtual void GetSerialServerIP(const std::string& strDevID) = 0;
	};
}
