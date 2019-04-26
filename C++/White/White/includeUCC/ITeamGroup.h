#pragma once

#include <map>
using namespace std;

class AFX_EXT_CLASS ITeamGroupNotify
{
public:
	virtual std::string OnGetUserIDDomain(void) = 0;
	virtual void OnGetFriendGroup(std::map<UINT, string>& mapFriendGroup)	= 0;
	virtual void OnShowHistoryRecord(void) = 0;
	virtual void OnIntoChatModel(const std::string& strUserID, const std::string& strUserName) = 0;
	virtual void OnSetUserNameCardPoint(const CPoint pt) = 0;
	virtual void OnProcessShowInfoCard(const PIMS_CONTACT_ITEM pItem, HWND hWnd = NULL) = 0;

	virtual std::string OnGetUserID(void) = 0;
	virtual PIMS_CONTACT_ITEM OnGetFriendItem(const std::string& strFriendID) = 0;
	virtual void OnGetFriendItemMap(IMS_CONTACT_ITEM_MAP& mapFriend) = 0;
	virtual PIMS_CONTACT_ITEM OnGetMyInfo(void) = 0;
	/* ��ȡȺ��������ĳ��Ա��Ϣ*/
	virtual void OnGetTeamDiscussMemberInfo(const std::string& strMemberID)=0;

	//������
	virtual void OnCreateDiscussGroup(const std::string& strName, const std::list<std::string>& lstMember)= 0;
	virtual void OnGetDiscussGroup(void)= 0;
	virtual void OnAddDiscussGroupMember(unsigned long ulDiscussGroupID, const std::list<std::string>& lstMember)= 0;
	virtual void OnExitDiscussGroup(unsigned long ulDiscussGroupID)= 0;
	virtual void OnSendDiscussGroupMsg(unsigned long ulDiscussGroupID, const std::string& strMsgTime,const std::string& strMsgTxt,const std::string& strMsgFormat)= 0;
	virtual void OnModDiscussGroupName(unsigned long ulDiscussGroupID, const std::string& strName)= 0;
	virtual void OnGetDiscussGroupOfflineMsg(void)= 0;
	//Ⱥ
	virtual void OnCreateTeam(const std::string& strTeamName, const std::string& strFounder, unsigned int nTeamType, const std::string& strNotice, const std::string& strIntroduce, unsigned int nAuthenType, std::map<std::string,unsigned short>& mapMember)= 0;
	virtual void OnProcessTeamRequest(unsigned long ulTeamID, const std::string& strTeamName, const std::string& strSenderID, const std::string& strReceiverID, unsigned int nResult, const std::string& strDesc, unsigned int nMsgType)= 0;
	virtual void OnAddTeamMember(unsigned long ulTeamID, const std::list<std::string>& lstMember)= 0;
	virtual void OnJoinTeam(unsigned long ulTeamID, const std::string& strDesc)= 0;
	virtual void OnGetTeam(void)= 0;
	virtual void OnGetTeamUnAuthenMsg(void)= 0;
	virtual void OnGetTeamAuthenedMsg(void)= 0;
	virtual void OnGetTeamOfflineMsg(void)= 0;
	virtual void OnExitTeam(unsigned long ulTeamID)= 0;
	virtual void OnKickTeam(unsigned long ulTeamID, const std::string& strMemberID)= 0;
	virtual void OnDissolveTeam(unsigned long ulTeamID)= 0;
	virtual void OnSendTeamMsg(unsigned long ulTeamID, const std::string& strMsgTime,const std::string& strMsgTxt,const std::string& strMsgFormat)= 0;
	virtual void OnSetTeamAdmin(unsigned long ulTeamID, const std::string& strMemberID)= 0;
	virtual void OnSetTeamMember(unsigned long ulTeamID, const std::string& strMemberID)= 0;
	virtual void OnUpdateTeam(unsigned long ulTeamID, const std::string& strTeamName, const std::string& strFounder, unsigned int nTeamType, const std::string& strNotice, const std::string& strIntroduce, unsigned int nAuthenType)= 0;
	virtual void OnFindTeam(unsigned long ulTeamID, const std::string& strTeamName, const std::string& strIntroduce)= 0;
};

class AFX_EXT_CLASS ITeamGroup
{
public:
	virtual void OnIMLoginDone(void) = 0;
	virtual void ResizeDlgTeamGroupList(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow)=0;
	virtual void ShowFindTeam(void)=0;
	virtual void ShowInvite(const std::string& strUserID)=0;
	virtual void GetTeam(std::map<unsigned long, std::string>& mapTeam) = 0;
	virtual void GetDiscussGroup(std::map<unsigned long, std::string>& mapGroup) = 0;
	virtual void SetOrgAuth(bool bHasOrgAuth) = 0;
	virtual PIMS_CONTACT_ITEM GetTGUserItem(const std::string& strUserID) = 0;
	virtual void RefreshOrg(void) = 0;



	/****************************************************************
	* �������¼�
	****************************************************************/

	/* �յ������б�ʼ�� */
	virtual void OnOrgListStart(void)=0;

	/* �յ������� */
	virtual void OnOrgItem(const std::string& strOrgID,const std::string& strOrgName)=0;

	/* �յ���������� */
	virtual void OnOrgItemEnd(void)=0;

	/* �յ����������� */
	virtual void OnOrgDepartItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strDepartName,int nLevelID,const std::string& strUpgradeID,int nOrderID)=0;

	/* �յ�������������� */
	virtual void OnOrgDepartItemEnd(void)=0;

	/* �յ����������û��� */
	virtual void OnOrgDepartUserItem(const std::string& strOrgID,const std::string& strDepartID,const IMS_CONTACT_ITEM& item,int nOrderID)=0;

	/* �յ����������û������ */
	virtual void OnOrgDepartUserItemEnd(void)=0;

	/* �յ�����/����/�û�ɾ���� */
	virtual void OnOrgDeleteItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strUserID)=0;

	/* �յ�����/����/�û�ɾ������� */
	virtual void OnOrgDeleteItemEnd(void)=0;

	/* �յ��ṹ�б���� */
	virtual void OnOrgListEnd(const std::string& strUpdateTime)=0;

	/*������*/
	virtual void OnAddDiscussArea(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnAddDiscussAreaEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnGetDiscussInfo(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnGetDiscussInfoEnd(const unsigned long& ulDiscussID)=0;
	/* �����������Ա��Ϣ */
	virtual void OnGetDiscussMemberInfo(const IMS_CONTACT_ITEM& contactItem)=0;
	virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnExitDiscuss(const unsigned long& ulDiscussID, const std::string& member)=0;
	virtual void OnRecvDiscussMSG(const unsigned long& ulDiscussID,const std::string& FromID,const XMSG& xmsg)=0;
	virtual void OnGetOfflineDiscussMSG(const unsigned long& ulDiscussID,const std::string& strDiscussName,const std::string& strFromID,const XMSG& xmsg)=0;
	virtual void OnGetOfflineDiscussMSGEnd(const unsigned long& ulDiscussID)=0;
	virtual void OnModDiscussName(const unsigned long& ulDiscussID,const std::string& strName)=0;

	/*Ⱥ*/
	virtual void OnCreatTeam(const TEAM_DES& team)=0;
	/* ����Ⱥ��Ա��Ϣ */
	virtual void OnGetTeamMemberInfo(const IMS_CONTACT_ITEM& contactItem)=0;
	/*���������͵Ĵ�������Ϣ*/
	virtual void OnTeamRequest(const TEAM_REQUEST& apply)=0;
	/*�³�Ա���뷵��*/
	virtual void OnAddTeamMember(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*������Ϸ���*/
	virtual void OnRequestReturn(const TEAM_REQUEST_RESULT& HandleReturn)=0;
	/*�뿪Ⱥ�鷵��*/
	virtual void OnExitTeam(const unsigned long& ulTeamID)=0;
	/*�˳���Ա����*/
	virtual void OnMinusTeamMember(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*����Ⱥ��Ϣ����*/
	virtual void OnRecvTeamMsg(const unsigned long& ulTeamID,const std::string& strFromID,const XMSG& xmsg)=0;
	/*��������*/
	virtual void OnPromotion(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*��������*/
	virtual void OnDemote(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*����Ⱥ��Ϣ����*/
	virtual void OnUpdateTeamInfo(const TEAM_DES& team)=0;
	/*����Ⱥ�󷵻�*/
	virtual void OnFindTeam(const unsigned long& ulTeamID,const std::string& strName,const std::string& strIntroduce,const size_t& set)=0;
	/*������Ϻ󷵻�*/
	virtual void OnFindTeamEnd()=0;
	/* ��ȡȺ��������ĳ��Ա��Ϣ*/
	virtual void OnGetTeamDiscussMemberInfo(const std::string& strMemberID, const IMS_CONTACT_ITEM& contactItem)=0;

	//�����õ�������¼�
	virtual void OnStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort) = 0;
	virtual void OnFriendDelete(const std::string& friendid) = 0;

};

AFX_EXT_CLASS ITeamGroup* CreateTeamGroup(ITeamGroupNotify& rNotify);
AFX_EXT_CLASS ITeamGroup* GetTeamGroup(void);
AFX_EXT_CLASS void DestroyTeamGroup(void);
