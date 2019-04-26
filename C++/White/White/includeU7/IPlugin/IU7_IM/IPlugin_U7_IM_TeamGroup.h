// UCC7-IMȺ�鹦��ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_TeamGroup_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_TeamGroup_Observer(){};

	public:
		/*������*/
		// ����������ص�
		virtual void OnAddDiscussArea(const unsigned long& ulDiscussID,const char* strGroupName,const char* strMember) = 0;

		//************************************
		// ˵��:    ��������������ص�
		// ����:	ulDiscussID ������ID
		// ����:	ErrorCode	ErrorCode��Ϊ0�򴴽�ʧ��,����������KXNCP.h�ļ�
		// ����ֵ:  void
		//************************************
		virtual void OnAddDiscussAreaEnd(const unsigned long& ulDiscussID, unsigned int ErrorCode) = 0;

		// �յ�������ص�
		virtual void OnGetDiscussInfo(const unsigned long& ulDiscussID,const char* strGroupName,const char* strMember) = 0;
		// �յ�����������ص�
		virtual void OnGetDiscussInfoEnd(const unsigned long& ulDiscussID) = 0;
		// �յ������������Ա 
		virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const char* member) = 0;
		// �յ������������Ա����
		virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID, unsigned int ErrorCode) = 0;
		// �յ��˳�������ص�
		virtual void OnExitDiscuss(const unsigned long& ulDiscussID,const char* member)=0;
		// �յ���������Ϣ�ص�
		virtual void OnDiscussMessage(const unsigned long& ulDiscussID, const char* strFromID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		// �յ��޸����������ƻص�
		virtual void OnModDiscussName(const unsigned long& ulDiscussID, const char*strName) = 0;

		/*Ⱥ*/
		/*����Ⱥ�ص���LoadTeamList��Ҳ����˻ص�*/
		virtual void OnCreatTeam(const TEAM_DES& team) = 0;
		/*���������͵Ĵ�������Ϣ*/
		virtual void OnTeamRequest(const TEAM_REQUEST& apply) = 0;
		/*�³�Ա���뷵��*/
		virtual void OnAddTeamMember(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*������Ϸ���*/
		virtual void OnRequestReturn(const TEAM_REQUEST_RESULT& HandleReturn) = 0;
		/*�뿪Ⱥ�ص�*/
		virtual void OnExitTeam(const unsigned long& ulTeamID) = 0;
		/*�˳���Ա����*/
		virtual void OnMinusTeamMember(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*�յ�Ⱥ��Ϣ�ص�(�Լ����͵���ϢҲ�����˻ص�)*/
		virtual void OnTeamMessage(const unsigned long& ulTeamID, const char* strFromID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		/*��������*/
		virtual void OnPromotion(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*��������*/
		virtual void OnDemote(const unsigned long& ulTeamID,const char*strMember) = 0;
		/*����Ⱥ��Ϣ����*/
		virtual void OnUpdateTeamInfo(const TEAM_DES& team) = 0;
		/*����Ⱥ�󷵻�*/
		virtual void OnFindTeam(const unsigned long& ulTeamID,const char* strName,const char* strIntroduce, const unsigned short nAuthenType) = 0;
		/*������Ϻ󷵻�*/
		virtual void OnFindTeamEnd() = 0;

		/* Ⱥ�����߹��ܻص� */
		//************************************
		// ˵��:    ��ʼ����Ⱥ������
		// ����:	ptNDSFileInfo	�����ļ��ṹ
		// ����:	ulSerialNO		�ļ�����ÿ��������0��ʼ��ÿ�ϴ�һ���ļ�+1
		// ����:	ulError			�������ERR_NO��ʾ�޴�
		// ����ֵ:  void
		//************************************
		virtual void OnSendTeamData(const PTNDSFileInfo& ptNDSFileInfo, const unsigned long& ulSerialNO, const unsigned long& ulError) = 0;
		virtual void OnSendTeamDataEnd(const unsigned long& ulFileID) = 0;

		virtual void OnDownloadTeamData(const unsigned long& ulSerialNO, const PTNDSFileInfo& ptNDSFileINfo, const unsigned long& ulError) = 0;
		virtual void OnDownloadTeamDataEnd(const unsigned long& ulFileID) = 0;

		//************************************
		// ˵��:    Ⱥ�����ϴ���ɣ����Կ�ʼ���ص�֪ͨ
		// ����:	ulTeamID		ȺID
		// ����:	ptNDSFileInfo	�����ļ��ṹ
		// ����ֵ:  void
		//************************************
		virtual void OnReadyDownloadTeamData(const unsigned long& ulTeamID, const PTNDSFileInfo& ptNDSFileInfo) = 0;

		//************************************
		// ˵��:    ȡ�õ�Ⱥ�����ļ��б�
		// ����:	ulTeamID		ȺID
		// ����:	ptNDSFileInfo	�����ļ��ṹ
		// ����ֵ:  void
		//************************************
		virtual void OnGetTeamShareFile(const unsigned long& ulTeamID, const PTNDSFileInfo& ptNDSFileInfo) = 0;
		virtual void OnGetTeamShareFileEnd(const unsigned long& ulTeamID) = 0;
		
		//************************************
		// ˵��:    ɾ��Ⱥ�����ļ�
		// ����:	tNDSFileInfo	�����ļ��ṹ
		// ����:	usErrCode		�������ERR_NO��ʾ�޴�
		// ����ֵ:  void
		virtual	void OnDelTeamShareFile(const TNDSFileInfo& tNDSFileInfo,const unsigned short& usErrCode) = 0;
	};

	// UCC7-IMȺ�鹦��ʵ�ֲ��
	class IPlugin_U7_IM_TeamGroup : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_TeamGroup(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_TeamGroup_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_TeamGroup_Observer* pObserver) = 0;

		//////////////////////////////////////////////////////////////////////////
		// ������

		// �ӷ������������������б�
		virtual void LoadDiscussGroupList(void) = 0;

		// ��ȡ�������б�������LoadDiscussGroupList����֮����ò��ܵõ��������б�
		virtual const IMS_GROUP_MAP& GetGroupList(void) = 0;

		// ��ȡĳ��������Ľṹ�壨������LoadDiscussGroupList����֮����ò��ܵõ���ȷ�õ���
		virtual const PIMS_GROUP_INFO GetGroupItem(unsigned long ulGroupID) = 0;

		// ����������
		virtual void CreateDiscussGroup(const char* strName, const std::list<std::string>& lstMember) = 0;

		// ����������Ա
		virtual void AddDiscussGroupMember(unsigned long ulDiscussGroupID, const std::list<std::string>& lstMember) = 0;

		// �˳�������
		virtual void ExitDiscussGroup(unsigned long ulDiscussGroupID) = 0;

		// ������������Ϣ
		virtual void SendGroupMessage(unsigned long ulDiscussGroupID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		// �޸�����������
		virtual void ModDiscussGroupName(unsigned long ulDiscussGroupID, const char* strName) = 0;

		//////////////////////////////////////////////////////////////////////////
		// Ⱥ

		// �ӷ�����������Ⱥ�б�
		virtual void LoadTeamList(void) = 0;

		// ��ȡȺ�б�������LoadTeamList����֮����ò��ܵõ��������б�
		virtual const IMS_TEAM_MAP& GetTeamList(void) = 0;

		// ��ȡĳ��Ⱥ�Ľṹ�壨������LoadTeamList����֮����ò��ܵõ���ȷ�õ���
		virtual const PTEAM_DES GetTeamItem(unsigned long ulTeamID) = 0;

		// ����Ⱥ��Ϣ
		virtual void SendTeamMessage(unsigned long ulTeamID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		//************************************
		// ˵��:    ����Ⱥ
		// ����:	strTeamName		Ⱥ����
		// ����:	strFounder		������ID
		// ����:	nTeamType		Ⱥ���ͣ��ݲ�ʹ�ã�
		// ����:	strIntroduce	Ⱥ����
		// ����:	strNotice		Ⱥ����
		// ����:	nAuthenType		Ⱥ����1:����Ҫ��֤ 2:��Ҫ��֤ 3:��ֹ��������
		// ����:	mapMember		Ⱥ��Ա�б�,key��ԱID��value��Ա�ȼ���
		//							1:��ͨ��Ա	2:����Ա	3:������
		// ����ֵ:  void
		//************************************
		virtual void CreateTeam(const char* strTeamName, const char* strFounder, unsigned int nTeamType,
			const char* strIntroduce, const char* strNotice, unsigned int nAuthenType,
			const std::map<std::string,unsigned short>& mapMember) = 0;

		//************************************
		// ˵��:    ����Ⱥ����
		// ����:	ulTeamID		ȺID
		// ����:	strTeamName		Ⱥ����
		// ����:	strSenderID		������ID
		// ����:	strReceiverID	������ID���ɺ��Դ˲�����ͨ������´˲������ᱻʹ�ã�
		// ����:	nResult			������ 1.�ɹ� 2.ʧ��
		// ����:	strDesc			��֤��Ϣ
		// ����:	nMsgType		�������� ��KIMSDefine.h�� _TEAM_REQUEST ����
		//							msgInvite = 1	����
		//							msgApply = 2	����
		// ����ֵ:  void
		//************************************
		virtual void ProcessTeamRequest(unsigned long ulTeamID, const char* strTeamName, const char* strSenderID,
			const char* strReceiverID, unsigned short nResult, const char* strDesc, unsigned int nMsgType) = 0;

		// ���Ⱥ��Ա
		virtual void AddTeamMember(unsigned long ulTeamID, const std::list<std::string>& lstMember) = 0;

		//************************************
		// ˵��:    �������Ⱥ
		// ����:	ulTeamID	ȺID
		// ����:	strDesc		��֤��Ϣ
		// ����ֵ:  void
		//************************************
		virtual void JoinTeam(unsigned long ulTeamID, const char* strDesc) = 0;

		// ��ȡ����֤��Ϣ
		virtual void GetTeamUnAuthenMsg(void) = 0;

		// ��ȡ��֤�����Ϣ
		virtual void GetTeamAuthenedMsg(void) = 0;

		// �˳�Ⱥ
		virtual void ExitTeam(unsigned long ulTeamID) = 0;

		// �߳�Ⱥ
		virtual void KickTeam(unsigned long ulTeamID, const char* strMemberID) = 0;

		// ��ɢȺ
		virtual void DissolveTeam(unsigned long ulTeamID) = 0;

		// ����Ϊ����Ա
		virtual void SetTeamAdmin(unsigned long ulTeamID, const char* strMemberID) = 0;

		// ����Ϊ��ͨ��Ա
		virtual void SetTeamMember(unsigned long ulTeamID, const char* strMemberID) = 0;

		//************************************
		// ˵��:    ����Ⱥ��Ϣ�����޸�Ⱥ����
		// ����:	ulTeamID	ȺID
		// ��������μ�CreateTeam
		//************************************
		virtual void UpdateTeam(unsigned long ulTeamID, const char* strTeamName, const char* strFounder,
			unsigned int nTeamType, const char* strIntroduce, const char* strNotice, 
			unsigned int nAuthenType, const std::map<std::string,unsigned short>& mapMember) = 0;

		// Ⱥ��ѯ
		virtual void FindTeam(unsigned long ulTeamID, const char* strTeamName, const char* strIntroduce) = 0;

		//************************************
		// ˵��:    ��Ⱥ����ͼƬ���ļ�
		// ����:	ulTeamID		ȺID
		// ����:	strPicPath		ͼƬ·��
		// ����:	nType	Ⱥ�ļ����ͣ���KXNCP.h�ļ��� _IMS_TEAMTYPE
		//			IMS_TEAM_PIC	ȺͼƬ
		//			IMS_TEAM_FILE	Ⱥ�ļ�
		// ����ֵ:  void
		//************************************
		virtual void SendTeamData(unsigned long ulTeamID, const char* strPicPath, int nType) = 0;

		// ��ȡȺ�ļ�����
		virtual void GetTeamData(unsigned long ulTeamID) = 0;

		//************************************
		// ˵��:    ����Ⱥ�����ݣ�ͼƬ�����ļ���
		// ����:	ptNDSFileInfo	�����ļ��ṹ
		// ����:	strFilePath		�����ļ�·��
		// ����:	nType			IMS_TEAM_PIC	ȺͼƬ
		//							IMS_TEAM_FILE	Ⱥ�ļ�
		// ����ֵ:  void
		//************************************
		virtual void RecvTeamData(const PTNDSFileInfo& ptNDSFileInfo, const std::string& strFilePath, const int nType) = 0;

		//************************************
		// ˵��:    ɾ��Ⱥ�����ݣ�ͼƬ�����ļ���
		// ����:	ptNDSFileInfo	�����ļ��ṹ
		// ����ֵ:  void
		//************************************
		virtual	void DelTeamShareFile(const PTNDSFileInfo& ptNDSFileInfo) = 0;
	};
}
