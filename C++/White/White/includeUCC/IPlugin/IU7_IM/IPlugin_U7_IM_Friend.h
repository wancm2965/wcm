// UCC7-IM��¼����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "KIMSDefine.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Friend_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Friend_Observer(){};

	public:
		/* �յ��ҵ���Ϣ�¼� */
		virtual void OnMyInfo(const IMS_CONTACT_ITEM& MyInfo) = 0;

		/* �յ�ͷ���¼� */
		virtual void OnImage(const char* strUserID,const char* strImgType,const char* strImgData) = 0;

		/* �յ�״̬�ı��¼� */
		virtual void OnUserStatus(const char* strUserID,const char* strStatus) = 0;

		/****************************************************************
		 * ��ϵ�������¼�
		 ****************************************************************/

		/* �յ���ϵ������ */
		virtual void OnGroupItem(const char* strGroup) = 0;

		/* �յ���ϵ�������б���� */
		virtual void OnGroupItemEnd(void) = 0;

		/* �յ�������ϵ�����¼�  */
		virtual void OnGroupInsert(const char* strGroup) = 0;

		/* �յ���ϵ��������¼� */
		virtual void OnGroupUpdateName(const char* strOldGroup,const char* strNewGroup) = 0;

		/* �յ�ɾ����ϵ�����¼� */
		virtual void OnGroupDelete(const char* strGroup) = 0;

		/****************************************************************
		 * ��ϵ�����¼�
		 ****************************************************************/

		/* �յ���ϵ���� */
		virtual void OnFriendItem(const IMS_CONTACT_ITEM& item) = 0;

		/* �յ���ϵ�����б�����¼� */
		virtual void OnFriendItemEnd(void) = 0;

		/* �յ���ϵ����������¼� */
		virtual void OnFriendApply(const char* strFriendID) = 0;

		/* �յ������ϵ�˴����¼� */
		virtual void OnFriendApplyError(const char* strFriendID) = 0;

		/* �յ���ϵ�˽��������¼� */
		virtual void OnFriendAdmit(const char* strFriendID) = 0;

		/* �յ���ϵ�˸������¼� */
		virtual void OnFriendUpdateGroup(const char* strFriendID,const char* strGroup) = 0;

		/* �յ�ɾ����ϵ���¼� */
		virtual void OnFriendDelete(const char* strFriendID) = 0;

		/* �յ�������ϵ�˽���¼� */
		virtual void OnFriendSearch(const char* strFriendID, const char* strUserName, const char* strGender,const char* strAge) = 0;

		/****************************************************************
		 * �������¼�
		 ****************************************************************/

		/* �յ������� */
		virtual void OnOrgItem(const char* strOrgID,const char* strOrgName) = 0;

		/* �յ���������� */
		virtual void OnOrgItemEnd(void) = 0;

		/* �յ����������� */
		virtual void OnOrgDepartItem(const char* strOrgID,const char* strDepartID,const char* strDepartName,int nLevelID,const char* strUpgradeID,int nOrderID) = 0;

		/* �յ�������������� */
		virtual void OnOrgDepartItemEnd(void) = 0;

		/* �յ����������û��� */
		virtual void OnOrgDepartUserItem(const char* strOrgID,const char* strDepartID,const IMS_CONTACT_ITEM& item,int nOrderID) = 0;

		/* �յ����������û������ */
		virtual void OnOrgDepartUserItemEnd(void) = 0;

		/* �յ��ṹ�б���� */
		virtual void OnOrgListEnd(void) = 0;

		/****************************************************************
		 * ��Ϣ���¼�
		 ****************************************************************/

		/* �յ�������Ϣ�¼� */
		virtual void OnIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		/* �յ�ϵͳ��Ϣ�¼� */
		virtual void OnSystemMessage(const char* strDomain, const char* strAdminName, const char* strMsgTime,
			 const char* strHead, const char* strMessage, const char* strMsgFormat) = 0;

		/* �յ�ϵͳ��Ϣ�����¼� */
		virtual void OnSystemMessageEnd(const char* strDateTime) = 0;
		
		/* �յ�������Ϣ�¼� */
		virtual void OnOrgMessage(const char* strOrgID,const char* strOrgName,
			const char* strDepartID,const char* strDepartName, const char* strUserID,
			const char* strUserName,const char* strNodeID, const char* strMsgTime, 
			const char* strMessage, const char* strMsgFormat) = 0;
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
		// ���õ�¼�û�״̬������������U7_IMSDefine.h�ļ�������
		virtual void ModifyStatus(const char* strStatus) = 0;
		// �޸ĵ�¼�û�����
		virtual void ModifyPassword(const char* strPassword) = 0;
		// �޸ĵ�¼�û���Ϣ
		virtual void SetMyInfo(IMS_CONTACT_ITEM& item) = 0;
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

		//************************************
		// ˵��:    �������ѣ��������ֻ������������б��д���ʼ��������������������
		// ����:	item		��ϵ�˽ṹ�壬������������
		// ����:	nStartID	���������������ʼ
		// ����:	nEndID		������������Ľ���
		// ����:	bOnlineOnly	ֻ��������
		// ����ֵ:  void
		//************************************
		virtual void SearchFriend(const IMS_CONTACT_ITEM& item, int nStartID, int nEndID, bool bOnlineOnly) = 0;
		// ��Ӻ��ѽ���ĳ�����ѷ���
		virtual void AddFriend(const char* strFriendID,const char* strGroup) = 0;
		// ������ϵ�˵�ĳ������
		virtual void AdmitFriend(const char* strFriendID,const char* strGroup) = 0;
		// ɾ����ϵ��
		virtual void DelFriend(const char* strFriendID) = 0;
		// ���ĺ���������
		virtual void UpdateFriendGroup(const char* strFriendID,const char* strGroup) = 0;
		
		// ������Ϣ��ָ���Է�
		virtual void SendIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		// ���ͻ�����Ϣ
		// strDepartID��Ϊ������������г�Ա���ͣ�������˲��ŷ���
		// bChildDepart�����ŷ���ʱ���Ƿ���˲��ŵ��Ӳ��ŷ���
		virtual void SendOrgMessage(const char* strOrgID,const char* strDepartID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, bool bChildDepart) = 0;

		// ��ȡĳ��ʱ��֮���ϵͳ���棬ʱ���ʽΪ 2012-01-01 00:00:00
		virtual void GetSysMsg(const char* strMsgTime) = 0;
		// ��ȡ������Ϣ
		virtual void GetOfflineMsg(void) = 0;

		// ������ز���
		// ��ȡĳ�������л������Ա����ϵ�˵���Ϣ
		virtual const PIMS_CONTACT_ITEM GetUserItem(const std::string& strUserID) = 0;	
		// ��ȡ��ǰ�û�ID
		virtual const char * GetUserID(void) = 0;
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
		// ��ȡ�����б�
		virtual const IMS_ORG_ITEM_MAP& GetOrgList(void) = 0;
		// ��ȡ���������б�
		virtual const IMS_DEPART_ITEM_MAP& GetDepartList(void) = 0;
		// ��ȡ������Ա�б�
		virtual const IMS_ORGUSER_ITEM_EX_MAP& GetOrgUserList(void) = 0;
		// ��ȡĳ�������л������Ա����չ��Ϣ
		virtual const PIMS_ORGUSER_ITEM_EX GetUserExItem(const std::string& strUserID) = 0;
		//��ȡ������Ϣ
		virtual const PIMS_DEPART_ITEM GetDepartItem(const std::string&strDepartID)=0;
	};
}
