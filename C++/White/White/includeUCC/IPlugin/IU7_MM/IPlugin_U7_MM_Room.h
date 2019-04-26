// UCC7-MM���书��ʵ�ֲ��
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

		// ���˷������
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
		/*��������Ա���˳�*/
		virtual void OnStopPersonalRoom(const char* strRoomID) = 0;
		/*��ȡָ�������ҵ�һ�����߳�Ա*/
		virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID) = 0;
		/*��ȡָ�������ҵ�һ�����ߺϳ��ն˻�������ͨ����Ϣ*/
		virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
			unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
			const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
			const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag) = 0;
		/*֪ͨ���˻��鷢���˻����Ѿ���ʼ(��һ����Ա�Ѿ���¼����)*/
		virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID) = 0;
	};

	// UCC7-MM���书��ʵ�ֲ��
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

		/* ��¼������
		* strDomain:			������������
		* strRoomID:			������ID
		* strPassword:			��ҪMD5����
		* strTermName:			�ն����� PC Terminal record */
		virtual void EnterRoom(const char* strDomain, const char* strRoomID, const char* strPassword, const char* strTermName) = 0;

		/* ������*/
		virtual void ExitRoom(void) = 0;

		/* �Ƿ��Ѿ��ڻ�*/
		virtual bool IsInMeeting(void) = 0;

		// ���˷������
		virtual void GetPersonalRoom(void) = 0;
		virtual void GetPersonalRoomGroup(const char* strRoomID) = 0;
		virtual void GetPersonalRoomMember(const char* strRoomID) = 0;
		virtual void AddPersonalRoom(const char* strRoomName) = 0;
		virtual void DelPersonalRoom(const char* strRoomID) = 0;
		virtual void AddPersonalRoomGroup(const MMS_GROUP& group) = 0;
		virtual void DelPersonalRoomGroup(const char* strRoomID,const char* strGroupID) = 0;
		virtual void StartPersonalRoom(const char* strRoomID) = 0;

		/* ��Ӹ��˻����� */
		virtual void SendPersonalAddGroupEnd(const char* strRoomID) = 0;

		/* ������������û������ͣ�Ĭ�ϱ�������޲�νṹ */
		virtual void SetPersonalRoomType(const char* strRoomID,bool bTreeMode=false) = 0;
		virtual void RenamePersonalRoom(const char* strRoomID,const char* strRoomName) = 0;
		virtual void AddPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void SetPersonalRoomMemberType(const char* strRoomID,const char* strMemberID,unsigned short usMemberType) = 0;
		virtual void DelPersonalRoomMember(const char* strRoomID,const char* strMemberID) = 0;

		virtual void GetRoomInfo(std::string& strDomainID, std::string& strRoomID, std::string& strRoomName, std::string& strRoomPwd) = 0;
	};
}
