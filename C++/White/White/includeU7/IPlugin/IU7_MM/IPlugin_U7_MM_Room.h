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
		virtual void OnRoomListEnd(void){};

		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType){};
		/* �յ��˳��������¼�
		* nErrCode���ص�¼���ֵ
		* 0: ��ʾ�����˳�������
		* 1: ��ʾ�������˳�������
		* 2: ��ʾ���߳�������
		* 3: �����Ա�ʺ��ڸ÷������ϲ����ڻ�÷�����û�м��ػ���ģ�� */
		virtual void OnExitRoom(unsigned int nErrCode){};
		virtual void OnEnterRoomReady(){};

		// ���˷������
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
		/*��������Ա���˳�*/
		virtual void OnStopPersonalRoom(const char* strRoomID){};
		/*��ȡָ�������ҵ�һ�����߳�Ա*/
		virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID){};
		/*��ȡָ�������ҵ�һ�����ߺϳ��ն˻�������ͨ����Ϣ*/
		virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
			unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
			const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
			const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag){};
		/*֪ͨ���˻��鷢���˻����Ѿ���ʼ(��һ����Ա�Ѿ���¼����)*/
		virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID){};
		virtual void OnGetRoomMemberType(const std::string& strRoomID,const std::string& strMerberID,const unsigned short& usMemberType){};

		//����֤ȯͶƱ�����sip�����������
		virtual void OnGetVoteDetail(unsigned long ulVoteID, const std::string& strTopic, const std::string& strUserID,
			const std::string& strUserName, unsigned int itemid, const std::string& itemname){};
		virtual void OnGetVoteDetailEnd(unsigned long ulVoteID){};
		virtual void OnRequestSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID){};
		virtual void OnSipQuitRequest(const std::string& strCallID, const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID){};

		virtual void OnResponseForOtherPresider(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, 
			const std::string& strMcuID, const std::string& strRoomID, bool bAccept){};
		//�յ�������Ա�仯(�������б�ʹ��)
		virtual void OnRoomMemberStatusChange(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMerberID,
			const unsigned short& usStatus,const unsigned short& usMemberType){};

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
		* strRoomID:			������ID����������ؼ��֣�
										��LocalMeeting��	���ػ���
										��Channel��			Ƶ��
		* strPassword:		��ҪMD5����
		* strTermName:		�ն����� PC Terminal record */
		virtual void EnterRoom(const char* strDomain, const char* strRoomID, const char* strPassword, const char* strTermName, bool bPrisider = false, unsigned short usTermType = 0, bool bInvite = false) = 0;
		/* �˳������� */
		virtual void ExitRoom(bool bAllLogout = false, int nReason = 0) = 0;

		/* �Ƿ��Ѿ��ڻ� */
		virtual bool IsInMeeting(void) = 0;
		/* �Ƿ������������ */
		virtual bool IsInSpecialRoom(void) = 0;
		/* �ж�strRoomID�Ƿ�Ϊ�������ID */
		virtual bool IsSpecialRoomID(const char* strRoomID) = 0;

		// ���˷������
		virtual void GetPersonalRoom(void) = 0;
		virtual void GetPersonalRoomGroup(const char* strRoomID) = 0;
		virtual void GetPersonalRoomMember(const char* strRoomID,const char* strGroupID) = 0;
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
		// ��ȡ��ǰ���������
		virtual std::string GetActiveGroup(void) = 0;

		/* ��ȡ������ԱȨ������
		strRoomID:		����ID
		strMemberID:	��ԱID*/
		virtual void GetRoomMemberType(const std::string& strRoomID,const std::string& strMemberID)=0;
	};
}
