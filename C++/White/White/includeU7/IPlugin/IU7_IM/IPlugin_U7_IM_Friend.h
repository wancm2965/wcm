// UCC7-IM��¼����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "AVCONSys/KIMSDefine.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Friend_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Friend_Observer(){};

	public:
		/* �յ��ҵ���Ϣ�¼� */
		virtual void OnMyInfo(const IMS_USER_ITEM& MyInfo){};

		/* �յ�ͷ���¼� */
		virtual void OnImage(const char* strUserID,const char* strImgType,const char* strImgData){};

		/* �յ�״̬�ı��¼� */
		virtual void OnUserStatus(const char* strUserID,const char* strStatus){};

		/****************************************************************
		 * ��ϵ�������¼�
		 ****************************************************************/

		/* �յ���ϵ������ */
		virtual void OnGroupItem(const char* strGroup){};

		/* �յ���ϵ�������б���� */
		virtual void OnGroupItemEnd(void){};

		/* �յ�������ϵ�����¼�  */
		virtual void OnGroupInsert(const char* strGroup){};

		/* �յ���ϵ��������¼� */
		virtual void OnGroupUpdateName(const char* strOldGroup,const char* strNewGroup){};

		/* �յ�ɾ����ϵ�����¼� */
		virtual void OnGroupDelete(const char* strGroup){};

		/****************************************************************
		 * ��ϵ�����¼�
		 ****************************************************************/

		/* �յ���ϵ���� */
		virtual void OnFriendItem(const IMS_CONTACT_ITEM& item){};

		/* �յ���ϵ�����б�����¼� */
		virtual void OnFriendItemEnd(void){};

		/* �յ���ϵ����������¼� */
		virtual void OnFriendApply(const char* strFriendID){};

		/* �յ������ϵ�˴����¼� */
		virtual void OnFriendApplyError(const char* strFriendID){};

		/* �յ���ϵ�˽��������¼� */
		virtual void OnFriendAdmit(const char* strFriendID){};

		/* �յ��Է��ܾ����Լ�Ϊ�����¼� */
		virtual void OnFriendReject(const std::string& strFriendID){};

		/* �յ���ϵ�˸������¼� */
		virtual void OnFriendUpdateGroup(const char* strFriendID,const char* strGroup){};

		/* �յ�ɾ����ϵ���¼� */
		virtual void OnFriendDelete(const char* strFriendID){};

		/* �յ�������ϵ�˽���¼� */
		virtual void OnFriendSearch(const std::string& strOrgID, const std::string& strOrgName,
			const std::string& strFriendID, const std::string& strFriendName,
			const std::string& strDepartName, const unsigned short& usTermType,
			const std::string& strTermname, const std::string& strUserInfo,
			const std::string& strGender, const std::string& strProvince,
			const std::string& strCity, const std::string& strNodeID, 
			const std::string& nStatus){};

		/* �յ�������ϵ�˽�������¼� */
		virtual void OnFriendSearchEnd(){};

		/* �յ���ȡ�û���Ϣ����¼� */
		virtual void OnGetSysUserInfo(const IMS_USER_ITEM& item){};

		/****************************************************************
		 * �������¼�
		 ****************************************************************/

		/* �յ������� */
		virtual void OnOrgItem(const char* strOrgID,const char* strOrgName){};

		// ��ȡ�����б�
		virtual void OnGetOrgList(const ORGINFOLIST& lstOrgInfo){};

		/* �յ���������� */
		virtual void OnOrgItemEnd(void){};

		/* �յ����������� */
		virtual void OnOrgDepartItem(const char* strOrgID,const char* strDepartID,const char* strDepartName,int nLevelID,const char* strUpgradeID,int nOrderID){};

		/* �յ�������������� */
		virtual void OnOrgDepartItemEnd(const std::string& strOrgID,const std::string& strDepartID){};

		/* �յ����������û��� */
		virtual void OnOrgDepartUserItem(const char* strOrgID,const char* strDepartID,const IMS_USER_ITEM& item,int nOrderID){};

		/* �յ����������û������ */
		virtual void OnOrgDepartUserItemEnd(const std::string& strOrgID,const std::string& strDepartID){};

		/* �յ��ṹ�б���� */
		virtual void OnOrgListEnd(void){};

		/* �յ������û���ϸ��Ϣ */
		virtual void OnGetOrgUserInfo(const IMS_CONTACT_ITEM& item){};

		/****************************************************************
		 * ��Ϣ���¼�
		 ****************************************************************/

		/* �յ�������Ϣ�¼� */
		virtual void OnOnlineMessage(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const XMSG& xmsg){};
		
		/* �յ�������Ϣ�¼� */
		virtual void OnOfflineMessage(const std::string& strUserID,const XOFLMSG& xmsg){};
		
		/* �յ�������Ϣ�����¼� */
		virtual void OnOfflineMessageEnd(){};

		/* �յ�ϵͳ��Ϣ�¼� */
		virtual void OnSystemMessage(const char* strDomain, const char* strAdminName, const char* strMsgTime,
			 const char* strHead, const char* strMessage, const char* strMsgFormat){};

		/* �յ�ϵͳ��Ϣ�����¼� */
		virtual void OnSystemMessageEnd(const char* strDateTime){};

		/* �յ�������Ϣ�¼� */
		virtual void OnOrgMessage(const char* strOrgID,const char* strOrgName,
			const char* strDepartID,const char* strDepartName, const char* strUserID,
			const char* strUserName,const char* strNodeID, const char* strMsgTime, 
			const char* strMessage, const char* strMsgFormat){};

		/*�յ�����������Ļص�*/
		virtual void OnRecvIntoMeetingRequset(const std::string& strUserID, const std::string& strNodeID, const std::string& strUserName, bool bNoAsk){};
		/*�����Ƿ�ͬ��������Ļص�*/
		virtual void OnProcessIntoMeetingRequest(bool bAgree, const std::string& strUserName, bool bPresider, const std::string& strDomainID,
			const std::string& strRoomID, const std::string& strRoomPwd, const std::string& strGroupID, const std::string& strMeetingType, const std::string& strErrorText){};

		/* �յ��ҵ�Ȩ�� */
		virtual void OnMyPriv(){};
};

	// UCC7-IM��¼����ʵ�ֲ��
	class IPlugin_U7_IM_Friend : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Friend(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Friend_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Friend_Observer* pObserver) = 0;

		// ���غ����б�
		virtual void LoadFriendList(void) = 0;
		// ���ػ����б�
		virtual void LoadOrgList(void) = 0;
		// �����б��Ƿ��Ѿ��������,����򷵻�true
		virtual bool IsLoadOrgListEnd(void) = 0;
		// ��ȡ������Ա��ϸ��Ϣ
		virtual void GetOrgUserInfo(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID) = 0;
		// ���õ�¼�û�״̬������������U7_IMSDefine.h�ļ�������
		virtual void ModifyStatus(const char* strStatus,const std::string& strTermType="") = 0;
		// �޸ĵ�¼�û�����
		virtual void ModifyPassword(const char* strPassword) = 0;
		// �޸ĵ�¼�û���Ϣ
		virtual void SetMyInfo(const IMS_USER_ITEM& item) = 0;
		// ����ͷ��
		virtual void SetImage(const char* strImgType, char* strImgFile) = 0;
		// ��ȡͷ��
		virtual void GetImage(const char* strFriendid) = 0;
		// ���һ�����ѷ��飬 strGroup Ϊ������
		virtual void InsertGroup(const char* strGroup) = 0;
		// �޸ĺ��ѷ�����
		virtual void UpdateGroupName(const char* strOldGroup,const char* strNewGroup) = 0;
		// ɾ��һ�����ѷ���
		virtual void DeleteGroup(const char* strGroup) = 0;

		// �������ѣ��������ֻ������������б��д���ʼ��������������������
		virtual void SearchFriend(const std::string& strOrgID="", const std::string& strFriendID="", 
			const std::string& strFriendName="", const std::string& strDepartName="",
			const std::string& strUserInfo="", const std::string& strAddress="",
			const std::string& strGender="",int nStartID=0, int nCount=20, int nStatus=1) = 0;

		// ��ȡ�û���Ϣ
		virtual void GetSysUserInfo(const std::string& strUserID) = 0;

		// ��Ӻ��ѽ���ĳ�����ѷ���
		virtual void AddFriend(const char* strFriendID,const char* strGroup) = 0;
		// ������ϵ�˵�ĳ������
		virtual void AdmitFriend(const char* strFriendID,const char* strGroup) = 0;
		// �ܾ��Է����Լ�Ϊ����
		virtual void RejectFriend(const char* strFriendID) = 0;
		// ɾ����ϵ��
		virtual void DelFriend(const char* strFriendID) = 0;
		// ���ĺ���������
		virtual void UpdateFriendGroup(const char* strFriendID,const char* strGroup) = 0;
		
		// ������Ϣ��ָ���Է�
		virtual void SendIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, const char* strHead = "") = 0;
		// ���ͻ�����Ϣ
		// strDepartID��Ϊ������������г�Ա���ͣ�������˲��ŷ���
		// bChildDepart�����ŷ���ʱ���Ƿ���˲��ŵ��Ӳ��ŷ���
		virtual void SendOrgMessage(const char* strOrgID,const char* strDepartID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, bool bChildDepart) = 0;

		// ��ȡ������Ϣ
		virtual void GetOfflineMessage(void) = 0;
		// ��ȡĳ��ʱ��֮���ϵͳ���棬ʱ���ʽΪ 2012-01-01 00:00:00
		virtual void GetSysMsg(const char* strMsgTime) = 0;

		// ������ز���
		// ��ȡĳ�������л������Ա����ϵ�˵���Ϣ
		virtual const PIMS_CONTACT_ITEM GetUserItem(const std::string& strUserID) = 0;	
		// ��ȡ��������ַ���˿ں�
		virtual void GetSvrInfo(std::string& strHost, unsigned short& usPort) = 0;
		// ��ȡ��ǰ�û�ID
		virtual const char* GetUserID(void) = 0;
		virtual const char* GetPassword(void) = 0;
		// ��ȡ��ǰ�û�ID�����������ƣ�
		virtual const char* GetUserIDWithDomain(void) = 0;
		// ��ȡ��ǰ�û�����
		virtual std::string GetUserName(void) = 0;
		// ��ȡ��ǰ�û���Ϣ
		virtual const IMS_CONTACT_ITEM& GetMyInfo(void) = 0;

		// ��ȡ���ѷ����б�
		virtual const IMS_FRIEND_GROUP_MAP& GetFriendGroupList(void) = 0;
		// ��ȡ�����б���Ҫ��OnFriendItemEnd֮��ʹ��
		virtual const IMS_CONTACT_ITEM_MAP& GetFriendList(void) = 0;
		// ������л����б�(�����¼�����)
		virtual void GetAllOrgList(void) = 0;
		// ��ȡ�����б�
		virtual const IMS_ORG_ITEM_MAP& GetOrgList(void) = 0;
		// ��ȡ��չ�Ļ����б����������Ĳ㼶��ϵ��
		virtual const IMS_ORG_ITEM_EX_MAP& GetOrgListEx(void) = 0;
		// ��ȡ���������б�
		virtual const IMS_DEPART_ITEM_MAP& GetDepartList(void) = 0;
		// ��ȡ������Ա�б�
		virtual const IMS_ORGUSER_ITEM_EX_MAP& GetOrgUserList(void) = 0;
		// ��ȡĳ�������л������Ա����չ��Ϣ
		virtual const PIMS_ORGUSER_ITEM_EX GetUserExItem(const std::string& strUserID) = 0;
		//��ȡ������Ϣ
		virtual const PIMS_DEPART_ITEM GetDepartItem(const std::string&strDepartID)=0;

		// ���������ѻ���
		// �Ƿ���Ҫѯ��
		virtual void RequsetIntoUserMeeting(const std::string& strUserID, const std::string& strNodeID, bool bNoAsk = false) = 0;
		//�������������Ϣ,��Ҫ���������Ϣ
		virtual void ProcessIntoUserMeeting(const std::string& strUserID,const std::string& strNodeID, bool bAgree,bool bPresider,const std::string& strDomainID,const std::string& strRoomID, 
											const std::string& strRoomName,const std::string& strRoomPwd, const std::string& strGroupID, const std::string& strMeetingType, const std::string& strErrorText) = 0;
	};
}
