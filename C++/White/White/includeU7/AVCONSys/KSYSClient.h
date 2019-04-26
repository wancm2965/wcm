#pragma once

//=======================================================================================
// ����������
// ����: ������ͨѶ����SESSION,����ͨѶ����Ҫ��������
//=======================================================================================
#include "KXNCP.h"
#include "KBASE/CmdPacket.h"

class AFX_EXT_CLASS KSYSSession
{
public:
	KSYSSession(void){};
	virtual ~KSYSSession(void){};

	/* ����ʱ�����¼� 
	 * �Է�WIN32��ϵͳ��Ϊ��ʹ��ʱ����,��Ҫ�ⲿѭ�����øú����Դ����ѽ��ܵ������ݰ� */
	virtual void ProcessTimerEvent(void)=0;

	/****************************************************************
	 * ��ȡ���ز���(���ӳɹ������)
	 ****************************************************************/

	/* ������ӵķ�����ID */
	virtual std::string GetServerID(void)=0;

	/* ��÷���������: SERVER_TYPE_MCU | SERVER_TYPE_MTS | SERVER_TYPE_MCU_MTS */
	virtual unsigned short GetServerType(void)=0;

	/* ��÷�������¼ʱ�� */
	virtual std::string GetServerTime(void)=0;

	/* ���NAT��ַ */
	virtual std::string GetNatAddr(void)=0;

	/* ���LOCAL��ַ */
	virtual std::string GetLocalAddr(void)=0;

	/* ���LOCAL�˿� */
	virtual unsigned short GetLocalPort(void)=0;

	/****************************************************************
	 * ����Э�麯��
	 ****************************************************************/

	/* ��÷������б�,���͵���������ȡ�������б�(MCU/MTS/MCU+MTS)
	   �����¼�:OnServerListStart/OnServerList/OnServerListEnd. */
	virtual void GetServerList(const std::string& strUserID)=0;

	/* ��÷�����ʵʱʱ�� */
	virtual void GetServerRealTime(void)=0;

	/* ������������
	 * usType: 0=���� 1=��Ƶ 2=��Ƶ 3=���Ӱװ� 4=��Ļ��Ƶ 5=... 
	 * ulUpStream: �ϴ���������KB
	 * ulDownStream: �ϴ���������KB
	 * strStartTime: ��ʼͳ�Ƶ�ʱ��,��ʽ:YYYY-MM-DD HH:NN:SS
	 * strEditTime:  ����ͳ�Ƶ�ʱ��,��ʽ:YYYY-MM-DD HH:NN:SS	 */
	virtual void SetTraffic(unsigned short usType,unsigned long ulUpStream,unsigned long ulDownStream,
                            const std::string& strStartTime,const std::string& strEndTime,
							const std::string& strRoomID="")=0;

	/* ��ȡ�û���Դ */
	virtual std::string GetUserResource(void)=0;

	/****************************************************************
	 * �ڲ�ģ�����
	 ****************************************************************/

	/* ���͵�NODE */
	virtual void SetUserID(const std::string& strUserID)=0;

	/* ���͵�NODE */
	virtual void SendPacketToNode(const std::string& strNodeID,KCmdPacket& rCmdPacket)=0;

	/* ���͵�IMS AGENT */
	virtual void SendPacketToIMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�MMS AGENT */
	virtual void SendPacketToMMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�MON AGENT */
	virtual void SendPacketToMONAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�SMS AGENT */
	virtual void SendPacketToSMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�FAX AGENT */
	virtual void SendPacketToFAXAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�NDS AGENT */
	virtual void SendPacketToNDSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵�REC AGNET */
	virtual void SendPacketToRECAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* ���͵����� AGNET */
	virtual void SendPacketToCTSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	virtual void SendPacketToRECNode(char* pData, int nLen, const std::string& strNodeID)=0;

	/* ����P2P���ݵ�NODEID */
	virtual void SendP2PData(const std::string& strNodeID,KCmdPacket& rCmdPacket)=0;
	virtual void SendToCmdManager( KCmdPacket* pCmdPacket )=0;

	virtual void PushPacket(KCmdPacket* pCmdPacket)=0;
};

//=======================================================================================
// ���������¼���
//=======================================================================================
class AFX_EXT_CLASS KSYSSessionEvent
{
public:
	KSYSSessionEvent(void){};
	virtual ~KSYSSessionEvent(void){};

public:
	/* �������ӷ�����...... */
	virtual void OnServerConnecting(void)=0;

	/* ���������ӵ������� */
	virtual void OnServerReconnected(void)=0;

	/* ����������æ */
	virtual void OnServerBusy(void)=0;

	/* δ���ӵ������� */
	virtual void OnServerFailed(void)=0;

	/* ������������������ѶϿ�
	 * nErrCode: ERR_SYS_NETWORK=��������Ͽ�; ERR_SYS_SERVERTYPE=��¼���������͵ķ����� */
	virtual void OnServerDisconnected(unsigned int nErrCode,const std::string& strDomain="")=0;

	/* �����ӵ������� */
	virtual void OnServerConnected(void)=0;

	/* �յ��������б�ʼ */
	virtual void OnServerListStart(void)=0;

	/* �յ��������б�
	 * strMCUID:		������ID
	 * strMCUType:		����������(MCU/MTS/MCU+MTS)
	 * ulConnections:	������������
	 * strBandWidth:	���������۴���
	 * ulActivity:		��������Ծ��
	 * strIPAddr:		������IP��ַ
	 * usIPPort:		������IP�˿�
	 * strIPType:		������IP����
	 * strIPName:		������IP���� */
	virtual void OnServerList(const std::string& strMCUID,const std::string& strMCUType,
		                      unsigned long ulConnections,const std::string& strBandWidth,unsigned long ulActivity,
							  const std::string& strIPAddr,unsigned short usIPPort,const std::string& strIPName,int nMcuRightType)=0;

	/* �յ��������б���� */
	virtual void OnServerListEnd(void)=0;

	/* �յ�������ʵʱʱ�� */
	virtual void OnServerRealTime(const std::string& strServerTime)=0;

	/* �յ��������ͻ��˰汾�б�(�����յ�������¼)
	 * strProduct:	��Ʒ��,ÿһ���ͻ���Ψһ����,�ɿͻ��˺�WEB��ͳһȷ��
	 * strVersion:	��Ʒ�汾��
	 * bForce:		�Ƿ�ǿ�Ƹ��� */
	virtual void OnClientVersion(const std::string& strProduct,const std::string& strVersion,bool bForce)=0;
   
	/* ���յ�P2P���� */
	virtual void OnRcvP2PData(const std::string& strFromNodeID,KCmdUserDataPacket& rCmdPacket)=0;

	/* �汾��ƥ�� */
	virtual void OnVersionMismatch(const int nCmd)=0;
	/* ����ռ���ʹ��� */
	virtual void OnBandwidthOccupancyRate(const int nOccupancyRate)=0;
};

//=======================================================================================
// SYSʵ��
//=======================================================================================

/* ���ӷ�����
 * strServerType: ����������SERVER_TYPE_MCU/SERVER_TYPE_MTS
 * strUserID:     �û��ʺ�(���ڻ�ȡ������������ʺŵķ������б�,
 *                ��strUserIDΪ��ʱϵͳ���Զ�Ϊ����ѡ���·������б�,�ʺŴ�������Ϊ��¼����
 * strResource:   �û���Դ����������ͬϵͳ���û�(�����ͼ���ۺϹ���ϵͳ�û�����գ������Զ�̽�����ѵϵͳ����edu)
 */
AFX_EXT_CLASS bool SYSSessionConnect(KSYSSessionEvent* pSYSSessionEvent,unsigned short usServerType,const std::string& strUserID,const std::string& strUserResource="");

/* �Ͽ����������� */
AFX_EXT_CLASS void SYSSessionDisconnect(KSYSSessionEvent* pSYSSessionEvent);

/* ���SYSSessionʵ�� */
AFX_EXT_CLASS KSYSSession& GetSYSSession(void);

//=======================================================================================
// IMSģ����
//=======================================================================================
#include "KIMSDefine.h"
#include "KMMSDefine.h"

class AFX_EXT_CLASS KIMSSession
{
public:
	KIMSSession(void){};
	virtual ~KIMSSession(void){};

	/* SYS����ģ����û�ȡ�û�NODEID */
	virtual int OnGetUserNodeID(const std::string& strUserID,std::string& strNodeID)=0;

	/* SYS����ģ����ô��������(����) */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* ��ȡIM��¼״̬(����) 
	 * ����trueΪIM���� */
	virtual bool GetIMLoginStatus(void)=0;

public:


	/****************************************************************
	 * ���غ���
	 ****************************************************************/

	/* ��ձ������� */
	virtual void ClearLocalData(void)=0;

	/* ��ñ����û��ʺ� */
	virtual std::string GetUserID(void)=0;

	/* ��ñ����û��� */
	virtual std::string GetUserName(void)=0;

	/* ��ñ����û����� */
	virtual int GetUserLevel(void)=0;

	/* ��ñ�����ϵ���б� */
	virtual IMS_CONTACT_ITEM_MAP& GetFriendItemMap(void)=0;

	/* ��ñ�����ϵ���� */
	virtual PIMS_CONTACT_ITEM GetFriendItem(const std::string& strFriendID)=0;

	/* ���ͳ�ʼ��¼���� */
	virtual void SendLoginCommand(void)=0;

	/****************************************************************
	 * ���ܺ���(��Login�������¼����ܲ���)
	 ****************************************************************/

	/* �û���¼
	 * strStatus:   �û���¼��ʼ״̬ 
	 * strIdentifyID �ͻ��˻���Ψһʶ����
	 * usTermType:	�ն����� 0=���ն� 1=Ӳ�ն� 2=�ֻ��ն�
	 * strTermName: �ն����� PC Terminal record Digital*/
	virtual void Login(const std::string& strUserID,const std::string& strPassword,const std::string& strStatus,const std::string& strIdentifyID,const std::string& strAuthKey="",unsigned short usTermType=0,const std::string& strTermName="",const bool& Anonymous=false)=0;	

	/* �û�ע�� */
	virtual void Logout(void)=0;

	/* ��ȡ�����û��� */
	virtual void GetOnlineUserCount(const std::string& strDomain="")=0;

	/* �����ҵ����� */
	virtual void SetMyPassword(const std::string& strPassword)=0;

	/* �����ҵ����� */
	virtual void SetMyName(const std::string& strMyName)=0;

	/* �����ҵ���Ϣ */
	virtual void SetMyInfo(const IMS_USER_ITEM& item)=0;

	/* ��ȡ�ҵ���Ϣ */
	virtual void GetMyInfo(void)=0;

	/* ��ȡȨ�� */
	virtual void GetMyPriv(void)=0;

	/* ����ͷ�� */
	virtual void SetImage(const std::string& strImgType,const std::string& strImgFile)=0;

	/* ��ȡͷ�� */
	virtual void GetImage(const std::string& strFriendid)=0;

	/* ���õ�¼�û�״̬ */
	virtual void SetStatus(const std::string& strStatus,const std::string& strTermType="")=0;

	/****************************************************************
	* �ϴ�������־ */
	/****************************************************************/
	virtual void UploadFaultLog(const std::string& strLog)=0;

	/****************************************************************
	 * ��ϵ�����ຯ��
	 ****************************************************************/

	/* ��ȡ�������б� */
	virtual void FetchGroups(void)=0;

	/* ��Ӻ����� */
	virtual void InsertGroup(const std::string& strGroup,unsigned int nIndex)=0;

	/* ���ĺ��������� */
	virtual void UpdateGroupName(const std::string& strOldGroup,const std::string& strNewGroup)=0;

	/* ���ĺ������������� */
	virtual void UpdateGroupIndex(const std::string& strGroup,unsigned int nIndex)=0;

	/* ɾ�������� */
	virtual void DeleteGroup(const std::string& strGroup)=0;

	/****************************************************************
	 * ��ϵ���ຯ��
	 ****************************************************************/

	/* ��ȡ�����б� */
	virtual void FetchFriends(void)=0;

	/* ��ȡ��ϵ���� */
	virtual void FetchFriend(const std::string& strFriendID)=0;

	/* ��ȡ��ϵ����Ϣ */
	virtual void GetFriendInfo(const std::string& strFriendID)=0;

	/* ������ϵ���ǳ� */
	virtual void SetFriendNickname(const std::string& strFriendID,const std::string& strNickname)=0;

	/* ��ȡ��ϵ���ǳ� */
	virtual void GetFriendNickname(const std::string& strFriendID)=0;

	/* �����ϵ�� */
	virtual void ApplyFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* ������ϵ�� */
	virtual void AdmitFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* �ܾ��Է����Լ�Ϊ����strFriendID */
	virtual void RejectFriend(const std::string& strFriendID)=0;

	/* ���ĺ��������� */
	virtual void UpdateFriendGroup(const std::string& strFriendID,const std::string& group)=0;

	/* ɾ������ */
	virtual void DeleteFriend(const std::string& strFriendID)=0;

	/* �������� 
	 * nStatus: 1 ���ߣ�Ĭ�ϣ� 2 ȫ�� */
	virtual void SearchFriend(const std::string& strOrgID="", const std::string& strFriendID="", 
							  const std::string& strFriendName="", const IMS_SEARCH_TYPE& searchtype=IMS_SEARCH_AND, const std::string& strDepartName="",
							  const std::string& strUserInfo="", const std::string& strAddress="",
							  const std::string& strGender="",int nStartID=0, int nCount=20, int nStatus=1,
							  const std::string& strDomain="")=0;

	/* ��ȡ��Ա������Ϣ  */
	virtual void GetSysUserInfo(const std::string& strUserID)=0;

	/* ��ȡ��Ա����ʱ��Ϣ */
	virtual void GetSysUserRunInfo(const std::string& strUserID)=0;

	/*��ȡ�������ķ��������͵��˺�*/
	virtual void GetCallCenterServiceItem(void) = 0;

	/****************************************************************
	 * ��������
	 ****************************************************************/

	/* ��ȡ��������,��Ա�б�
	 * strUpdateTime: �ϴθ��»����б��ʱ�� */
	virtual void FetchOrgs(const std::string& strUpdateTime)=0;

	/* ��ȡ������Ա��ϸ��Ϣ */
	virtual void GetOrgUserInfo(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID)=0;

    /*��ȡָ�������µ��Ӳ��ź���Ա�б�*/
    virtual void GetOrgDepartItems(const std::string& strOrgID, const std::string& strDepartID)=0;

	/****************************************************************
	 * ��Ϣ����
	 ****************************************************************/

	/* ���ͼ�ʱ��Ϣ */
	virtual void SendOnlineMessage(const std::string& strNodeID,const std::string& strToID,XMSG& xmsg)=0;

	/* ����������Ϣ */
	virtual void SendOfflineMessage(const std::string& strToID,XMSG& xmsg)=0;

	/* ���ͻ�����Ϣ
	strDepartID��Ϊ������������г�Ա���ͣ�������˲��ŷ���
	bChildDepart�����ŷ���ʱ���Ƿ���˲��ŵ��Ӳ��ŷ���*/
	virtual void SendOrgMessage(const std::string& strOrgID,const std::string& strDepartID,XMSG& xmsg,bool bChildDepart)=0;

	/* ��ȡ������Ϣ */
	virtual void GetOfflineMessage(void)=0;

	/* ��ȡϵͳ��Ϣ */
	virtual void GetSystemMessage(const std::string& strDateTime)=0;

	/****************************************************************
	 * �Զ��庯��
	 ****************************************************************/

    /* �����û����ݵ�ָ���Է� */
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;

	/****************************************************************
	 * �칫�ຯ��
	 ****************************************************************/

	/* ��ȡURL�б� */
	virtual void FetchURLs()=0;

	/****************************************************************
	 * ��ͼ�ຯ��
	 ****************************************************************/

	/* ���ӵ�ͼ��Ϣ */
	virtual void AddMapInfo(const std::string& strGroupID,const std::string& strMapMD5)=0;

	/* ���µ�ͼ��Ϣ */
	virtual void UpdateMapInfo(const std::string& strGroupID,const std::string& strMapMD5)=0;

	/* ɾ����ͼ��Ϣ */
	virtual void DeleteMapInfo(const std::string& strGroupID)=0;

	/* �������ڵ�ID�õ���Ӧ�ĵ�ͼID���ڵ�ͼ�Ƚ� */
	virtual void GetMapInfo(const std::string& strGroupID)=0;

	/****************************************************************
	 * �ȵ��ຯ��
	 ****************************************************************/

	/* �����ȵ���Ϣ
	   strGroupID: ����ID
	   strTreeID:  �����������ͨ��ID */
	virtual void AddBzInfo(const std::string& strGroupID,const std::string& strTreeID,const std::string& strBzID)=0;

	/* ɾ�����ڵ�Ĺ�������Ϣ
	   strGroupID ������ID
	   strTreeID  �������������ͨ��ID ��strTreeID  Ϊ��ʱ��ɾ�������ȵ���Ϣ	*/
	virtual void DeleteBzInfo(const std::string& strGroupID,const std::string& strTreeID)=0;

	/* ��������ID�õ��������ȵ�ID */
	virtual void GetBzInfo(const std::string& strGroupID)=0; 

	/****************************************************************
	 * ��ط�������
	 ****************************************************************/
	/* ��ȡ�����Դ�� */
	virtual void GetMonGroupInfo(DOMAINUPDATETIMELST& lstTime)=0;

	/* ��ȡ��ǰ���е��豸 */
	virtual void GetMonDevInfo(const std::string& strGroupID,const std::string& strUpdateTime, bool bFromPersonlMeeting = false)=0;

	/* ��ȡ��ǰ�豸�µ�ͨ�� */
	virtual void GetMonChannelInfo(const std::string& strDevID,const std::string& strUpdateTime)=0;

	/* �����豸ͨ��״̬֪ͨ
		lstDev:�豸�б� eOperation:�������� ȡֵ���£�
														ONLYREAD = 0,//��ȡ����
														ONLYWRITE,//������
														READANDWRITE,//��ȡ�����ֶ���*/
	virtual void SubscribeStatusMsg(const std::list<std::string>& lstDev, const OPERATION &eOperation)=0;

	/* ȡ�������豸ͨ��״̬֪ͨ
		lstDev:�豸�б�strSubscribeDomain�������� */
	virtual void UnsubscribeStatusMsg(const std::list<std::string>& lstDev)=0;

	/* ������������豸��Ϣ,֧��ģ����������෵��50����Ϣ
	 * strDevString		�豸ID���豸����ƥ�䴮
	 * strDomain		Ϊ��ʱֻ�������򣬷�������ָ���� */
	virtual void SearchDevInfo(const std::string& strDevString,const std::string& strDomain="")=0;
	
	/* ��ȡ����ط����µ��豸�����������豸����*/
	virtual void GetMonDevCount(void)=0;

	/* ��ȡ����ط����µ�ͨ������������ͨ������*/
	virtual void GetMonCHLCount(void)=0;

	virtual void AddMonTmpGroup(const std::string& strGroupName)=0;
	virtual void DelMonTmpGroup(const std::string& strGroupID)=0;
	virtual void RenameMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void AddMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void DelMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void GetMonTmpGroup(void)=0;
	virtual void GetMonTmpGroupMember(const std::string& strGroupID)=0;

   	/* ��ȡ���¼��ƻ� */
	virtual void GetMONRecTask(const std::string& strUpdatetime) = 0;
	/* ��ȡ���ͨ��¼��ƻ� */
	virtual void GetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime) = 0;
	/* ��ȡ���¼�����(����:¼��ķָ�����,��С) */
	virtual void GetMONRecStrategy(const std::string& strUpdatetime) = 0;

	/****************************************************************
	 * ��¼�ຯ��
	 ****************************************************************/

	/* ��¼��ʷ��¼��������
	 * nType: ��¼����. 0=��Ե��ı������¼;1=��Ի����ı������¼;2=ͼƬ;3=�ļ�;4=����;5=����Ƶ����;6=�绰����
	 * strFromID: ������
	 * strToID: ������
	 * strContent: ��¼���� */
	virtual void SaveHistory(int nType,const std::string& strFromID,const std::string& strToID,const std::string& strContent)=0;
	/****************************************************************
	 * ������
	 ****************************************************************/
	virtual void SetUserRemark(const std::string& strUserID,const std::string& strRemark)=0;
	virtual void GetUserRemark(const std::string& strUserID)=0;
	
	/****************************************************************
	* IMȺ�鹦��
	****************************************************************/
	/****************************************************************
	* ������
	****************************************************************/
	/* ���������� */
	virtual void CreatDiscuss(const std::string& name,const std::list<std::string>& lst)=0;
	/* ��ȡ��������Ϣ */
	virtual void GetDiscussAreaData(void)=0;
	/* ��ȡ�������Ա��Ϣ */
	virtual void GetDiscussMemberInfo(void)=0;
	/* ����������Ա */
	virtual void AddDiscussMember(const unsigned long& id,const std::list<std::string>& lst)=0;
	/* �˳������� */
	virtual void ExitDiscuss(const unsigned long& id)=0;
	/* ������������Ϣ */
	virtual void SendDiscussMessage(const unsigned long& id,const XMSG& xmsg)=0;
	/* �޸����������� */
	virtual void ModDiscussName(const unsigned long& id,const std::string& name)=0;
	/****************************************************************
	* Ⱥ��
	********************************************D********************/
	/* ����Ⱥ */
	virtual void AddTeamArea(const TEAM_DES& team)=0;
	/* �������� */
	virtual void ProcessRequest(const TEAM_REQUEST& apply,const unsigned short& result)=0;
	/*��ӳ�Ա*/
	virtual void AddTeamMember(const unsigned long& id , const std::list<std::string>& lstMember)=0;
	/*����Ⱥ*/
	virtual void JoinTeam(const unsigned long& id,const std::string& strDesc)=0;
	/* ��ȡȺ��Ϣ */
	virtual void GetTeamInfo(void)=0;
	/* ��ȡȺ��Ա��Ϣ */
 	virtual void GetTeamMemberInfo(void)=0;
	/* ��ȡ����֤��Ϣ */
	virtual void GetRequestMsg()=0;
	/* ��ȡ��֤�����Ϣ */
	virtual void GetRequestReturnMsg()=0;
	/* �˳�Ⱥ */
	virtual void ExitTeam(const unsigned long& id)=0;
	/* �߳�Ⱥ */
	virtual void KickTeam(const unsigned long& id , const std::string& strMember)=0;
	/* ��ɢȺ */
	virtual void DissolveTeam(const unsigned long& id)=0;
	/* ����Ⱥ��Ϣ */
	virtual void SendTeamMessage(const unsigned long& id,const XMSG& xmsg)=0;
	/* ����Ϊ����Ա */
	virtual void Promotion(const unsigned long& id,const std::string& strMember)=0;
	/* ����Ϊ��ͨ��Ա */
	virtual void DemoteMember(const unsigned long& id,const std::string& strMember)=0;
	/* ����Ⱥ��Ϣ */
	virtual void UpdateTeamInfo(const TEAM_DES& team)=0;
	/* Ⱥ��ѯ */
	virtual void FindTeam(const unsigned long& id,const std::string& strName,const std::string& introduce)=0;
	/* ��ȡȺ��������ĳ��Ա��Ϣ*/
	virtual void GetTeamDiscussMemberInfo(const std::string& strMemberID)=0;
	/* ��Ⱥ����ͼƬ���ļ� */
	virtual void SendTeamData(PTNDSFileInfo ptNDSFileInfo, const int nType) = 0;
	virtual void RecvTeamData(PTNDSFileInfo ptNDSFileInfo, const std::string& strFilePath, const int nType) = 0;
	virtual void GetTeamShareFile(const unsigned long& ulTeamID) = 0;
	/* ɾ��Ⱥ�����ļ� */
	virtual	void DelTeamShareFile(const PTNDSFileInfo& ptNDSFileInfo) = 0;
	/* �༭Ⱥ�����ļ��� */
	virtual	void EditTeamShareFileName(const PTNDSFileInfo& ptNDSFileInfo,const std::string& strNewFilename) = 0;
    /************************************************************************
     * ���������
    /************************************************************************/
    /* ��ȡ�û����� */
    virtual void FetchUserTypes() = 0;

    /* �����û����ݵ������� */
    virtual void SendUserData2All(KCmdPacket& packet)=0;

	/* �Ƿ��а󶨵�¼������ */
	virtual void GetBindRecordServer()=0;

	/************************************************************************
	* �㲥������
	/************************************************************************/
	/* ��ȡ�㲥�������б� */
	virtual void GetVODSvrList()=0;

	/* ��ȡ�������������б� */
	virtual void GetLocalDomainList(void)=0;

	/* ��ȡ�����б� */
	virtual void GetOrgList(void)=0;

    /* ��ȡ�����б� */
	virtual void GetDepartList(void)=0;

	/* ��ȡָ�����µļ�ػ����б� 
	* strUpdateTime: �ϴθ��»����б��ʱ�� 
	* ���strDomainΪ�գ���Ĭ�ϻ�ȡ���������б�*/ 
	virtual void GetAppointDomainRoomList(const std::string& strUpdateTime,const std::string& strDomain="")=0;

	/* ��ȡĳ��ͨ������Ϣ */
	virtual void GetAppointDevChannelInfo(const std::string& strDevID,const std::string& strChannelID)=0;

	/************************************************************************
	* ��������ϵͳ
	************************************************************************/
	/* ��ȡdomain��Ϣ(�����¼�����Ϣ) */
	virtual void GetDomainInfo()=0;

	/* ��ȡָ����MCU��Ϣ */
	virtual void GetMCUInfo(const std::string& strDomain)=0;

	/* ��ȡ�豸�б� */
	virtual void GetMCUDeviceInfo(const std::string& strDomain, const std::string& strUpdatetime)=0;

	/* ��ȡ�����б� */
	virtual void GetMCUMGWInfo(const std::string& strDomain, const std::string& strUpdatetime)=0;

	virtual void SetMonTmpDefaultChannelNo(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;

	/*  ��ȡ���ͨ��
	nStartID  ���ݿ��ѯ��IDֵ����ʼ��ѯֵΪ0�������һ�β�ѯ nStartID = nStartID + nCount
	nCount    һ�β�ѯ������Ŀǰһ��һ����ѯ,�ⲿ����ֵΪ1��
	strUserData �û���ѯ�������ı�ʶ���ݣ����ѯ����ص�һ�𷵻أ���OnGetVQDChannelInfo
	*/
	//virtual void GetVQDChannelInfo(int nStartID,int nCount,const std::string& strUserData)=0;

	/************************************************************************/
	/* �����пͷ��˻�ȡҵ��������Ϣ                                         */
	/************************************************************************/
	virtual void GetBusinessInfo()=0;
	
	/*��ʯ��*/
	//uSearchId, ��ѯ���id; strBeginTime, ��ѯ��ʼ��ֹʱ��, strEndTime, ��ѯ����ʱ�� nAlarmType, ��������, ���ش�����(0��ʾ�ɹ�)
	//virtual int CreateAlarmSearch(unsigned long& ulSearchId, const std::string& strBeginTime, const std::string& strEndTime, const std::string& strAlarmType)=0;
	//uSearchId, ��ѯ���id; nBeginRow ��ѯ��ʼ��; nEndRow, ��ѯ������; bSort, ����(true, ���� false ����) ���ش�����(0��ʾ�ɹ�)
	//virtual int GetAlarmDoSearch(unsigned long uSearchId, int nBeginRow, int nEndRow, bool bSort=true)=0;
	//uSearchId, ��ѯ���id;
	//virtual int DestroyAlarmSearch(unsigned long ulSearchId)=0;

};

//=======================================================================================
// IMSģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KIMSSessionEvent
{
public:
	KIMSSessionEvent(void){};
	virtual ~KIMSSessionEvent(void){};

public:
	/* ��¼
	 * nErrCode: ��¼���ش�����,0=��¼�ɹ�,
	 * strModuleString: ϵͳģ���ַ���: IMS+MMS+MON+SMS or IMS+MMS  */
	virtual void OnLogin(int nErrCode,const std::string& strModuleString)=0;

	/* ע�� 
	 * nErrCode:ע�����ش�����,0=����ע�� */
	virtual void OnLogout(int nErrCode)=0;

	/* ����ͻ��˵�������֤ */
	virtual void OnClientAuth(unsigned char ucAuthType,const std::string& strAuthKey,const std::string& strAuthConnection)=0;

	/* �յ������û����� */
	virtual void OnOnlineUserCount(unsigned long ulCount)=0;

	/* �յ��ն˹�����־���� */
	virtual void OnFaultLogEnable(bool bEnable)=0;
	/* �յ��ҵ������¼� */
	virtual void OnMyName(const std::string& strUserName)=0;

	/* �յ��ҵ���Ϣ�¼� */
	virtual void OnMyInfo(const IMS_USER_ITEM& ui)=0;

	/* �յ�Ȩ���� */
	virtual void OnMyPriv(unsigned long ulPrivCode)=0;

	/* �յ�ͷ���¼� */
	virtual void OnImage(const std::string& strUserID,const std::string& strImgType,const std::string& strImgData)=0;

	/* �յ�״̬�¼�,bFirst��ʾ�Ƿ��һ��״̬���� */
	virtual void OnStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID)=0;

	/****************************************************************
	 * ��ϵ�������¼�
	 ****************************************************************/

	/* �յ���ϵ������ */
	virtual void OnGroupItem(const std::string& strGroup,unsigned int nIndex)=0;

	/* �յ���ϵ�������б���� */
	virtual void OnGroupItemEnd(void)=0;

	/* �յ�������ϵ�����¼�  */
	virtual void OnGroupInsert(const std::string& strGroup,unsigned int nIndex)=0;

	/* �յ���ϵ��������¼� */
	virtual void OnGroupUpdateName(const std::string& strOldGroup,const std::string& strNewGroup)=0;

	/* �յ���ϵ��������������¼� */
	virtual void OnGroupUpdateIndex(const std::string& strGroup,unsigned int nIndex)=0;

	/* �յ�ɾ����ϵ�����¼� */
	virtual void OnGroupDelete(const std::string& strGroup)=0;

	/****************************************************************
	 * ��ϵ�����¼�
	 ****************************************************************/

	/* �յ���ϵ���� */
	virtual void OnFriendItem(const IMS_CONTACT_ITEM& item)=0;

	/* �յ���ϵ�����б�����¼� */
	virtual void OnFriendItemEnd(void)=0;

	/* �յ���ϵ����Ϣ�¼� */
	virtual void OnFriendInfo(const std::string& strFriendID)=0;

	/* �յ���ϵ����������¼� */
	virtual void OnFriendApply(const std::string& strFriendID)=0;

	/* �յ������ϵ�˴����¼� */
	virtual void OnFriendApplyError(const std::string& strFriendID)=0;

	/* �յ���ϵ�˽��������¼� */
	virtual void OnFriendAdmit(const std::string& strFriendID)=0;

	/* �յ��Է��ܾ����Լ�Ϊ�����¼� */
	virtual void OnFriendReject(const std::string& strUserID)=0;

	/* �յ���ϵ�˸������¼� */
	virtual void OnFriendUpdateGroup(const std::string& strFriendID,const std::string& strGroup)=0;

	/* �յ�ɾ����ϵ���¼� */
	virtual void OnFriendDelete(const std::string& strFriendID)=0;

	/* �յ�������ϵ�˽���¼� */
	virtual void OnFriendSearch(const std::string& strOrgID, const std::string& strOrgName,
								const std::string& strFriendID, const std::string& strFriendName,
								const std::string& strDepartName, const unsigned short& usTermType,
								const std::string& strTermname,	const std::string& strUserInfo,								
								const std::string& strGender, const std::string& strProvince,
								const std::string& strCity, const std::string& strNodeID, 
								const std::string& nStatus)=0;

    /* �յ�������ϵ�˽�������¼� */
	virtual void OnFriendSearchEnd()=0;
    
	/* �յ���Ա������Ϣ */
	virtual void OnGetSysUserInfo(const IMS_USER_ITEM& item)=0;

	/* �յ���Ա����ʱ��Ϣ */
	virtual void OnGetSysUserRunInfo(const USER_SESSION& item)=0;

	/* �յ��������ķ���������Ա���͵���Ϣ */
	virtual void OnGetCallCenterServiceItem(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID, const std::string& strUserName, const std::string& strNodeID)=0;
	
	/* �յ��������ķ���������Ա���͵���Ϣ���� */
	virtual void OnGetCallCenterServiceItemEnd(void)=0;

	/* ��ȡ��ϵ���ǳ� */
	virtual void OnGetFriendNickname(const std::string& strFriendID,const std::string& strNickname)=0;

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
	virtual void OnOrgDepartItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strDepartName,int nLevelID,const std::string& strUpgradeID,int nOrderID,const std::string& strOrgTree)=0;

	/* �յ�������������� */
	virtual void OnOrgDepartItemEnd(const std::string& strOrgID,const std::string& strDepartID)=0;

	/* �յ����������û��� */
	virtual void OnOrgDepartUserItem(const std::string& strOrgID,const std::string& strDepartID,const IMS_USER_ITEM& item,int nOrderID)=0;

	/* �յ����������û������ */
	virtual void OnOrgDepartUserItemEnd(const std::string& strOrgID,const std::string& strDepartID)=0;

	/* �յ�����/����/�û�ɾ���� */
	virtual void OnOrgDeleteItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strUserID)=0;

	/* �յ�����/����/�û�ɾ������� */
	virtual void OnOrgDeleteItemEnd(void)=0;

	/* �յ��ṹ�б���� */
	virtual void OnOrgListEnd(const std::string& strUpdateTime)=0;

	/* �յ�������Ա��ϸ��Ϣ */
	virtual void OnGetOrgUserInfo(const IMS_CONTACT_ITEM& item)=0;

	/****************************************************************
	 * ��Ϣ���¼�
	 ****************************************************************/

	/* �յ�������Ϣ�¼� */
	virtual void OnOnlineMessage(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const XMSG& xmsg)=0;

	/* �յ�������Ϣ�¼� */
	virtual void OnOfflineMessage(const std::string& strUserID,const XOFLMSG& xmsg)=0;

    /* �յ�������Ϣ�����¼� */
	virtual void OnOfflineMessageEnd()=0;

	/* �յ�ϵͳ��Ϣ�¼� */
	virtual void OnSystemMessage(const std::string& strDomain,const std::string& strAdminName,const XMSG& xmsg)=0;

	/* �յ�ϵͳ��Ϣ�����¼� */
	virtual void OnSystemMessageEnd(const std::string& strDateTime)=0;

	/* �յ�������Ϣ�¼� */
	virtual void OnOrgMessage(const std::string& strOrgID,const std::string& strOrgName,
		                      const std::string& strDepartID,const std::string& strDepartName,
							  const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,
							  const XMSG& xmsg)=0;

	/****************************************************************
	 * �Զ����¼�
	 ****************************************************************/

	/* �յ��û��Զ��������¼� */
	virtual void OnUserData(KCmdUserDataPacket& packet)=0;

	/****************************************************************
	 * �������б��¼�
	 ****************************************************************/

	/* ����WEB��������ַ�Ͷ˿�,�ͻ��˽�����ַ�Ͷ˿� */
	virtual void OnWEBServer(const std::string& strHost)=0;

	/* ����PBX��������ַ,�ַ�����ʽӦ�ú�WEB��ӵ����ݿ��еĸ�ʽһ�� */
	virtual void OnPBXServer(const std::string& strHost)=0;

	/* �յ�FAX����,�ͻ��˽�����ַ�Ͷ˿� */
	virtual void OnFAXServer(const std::string& strHost)=0;

	/****************************************************************
	 * �칫���¼�
	 ****************************************************************/

	/* �յ�URL�б��� */
	virtual void OnGetURLItem(const std::string& strName,const std::string& strURL,const std::string& strParams)=0;

	/* �յ�URL�б������ */
	virtual void OnGetURLListEnd()=0;

	/* �յ��´������ */
	virtual void OnGetFAXCount(unsigned int nCount)=0;	

	/****************************************************************
	 * ��ͼ���¼�
	 ****************************************************************/

	/* �յ��ص���ͼ��Ϣ
	   strGroupID������ID
	   strMapMD5 ����ͼMD5ֵ	*/
	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;

	/****************************************************************
	 * �ȵ����¼�
	 ****************************************************************/

	/* �������ڵ�ID�õ��������ȵ�ID
	   strGroupID������ID
	   listMAPBZ �������������ȵ�list */
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;

	/****************************************************************
	 * ��ط����¼�
	 ****************************************************************/

	virtual void OnMonUpdateTime(const std::string& strUpdateTime,const std::string& strDomain)=0;

	 /* ��ȡ�����Դ��ص� */
	virtual void OnGetMonGroupInfo(const MONGROUP& item, const int nDomainLevel)=0;

	 /* ��ȡ�����Դ��ص����� */
	virtual void OnGetMonGroupInfoEnd(const std::string& strDomain)=0;

	/* ��ȡ��ǰ���е��豸�ص� */
	virtual void OnGetMonDevInfo(const MONDEV& item, const int nDomainLevel, bool bChngStatus=false)=0;

	/* ��ȡ��ǰ���е��豸�ص����� */
	virtual void OnGetMonDevInfoEnd(const std::string& strGroupID,const std::string& strUpdateTime,bool bFromPersonalMeeting)=0;

	/* ��ȡ��ǰ�豸�µ�ͨ���ص� */
	virtual void OnGetMonChannelInfo(const MONCHANNEL& item, const int nDomainLevel, bool bChngStatus=false)=0;

	/* ��ȡ��ǰ�豸�µ�ͨ���ص����� */
	virtual void OnGetMonChannelInfoEnd(const std::string& strDevID,const std::string& strUpdateTime)=0;

	/* ������������豸��Ϣ�ص�,֧��ģ����������෵��50����Ϣ��strDomainΪ�գ���ֻ�������򣬷�������ָ���� */
	virtual void OnSearchDevInfo(const MONDEV& item,const std::string& strDomain)=0;

	/* ������������豸��Ϣ�����ص�������Ϊ����ʱ�Ĳ��� */
	virtual void OnSearchDevInfoEnd(const std::string& strDevString,int nCount,const std::string& strDomain)=0;

	/* ������������豸���������������Ļص� */
	virtual void OnGetMonDevCount(const std::string& strGroupID,const unsigned int& nOnlineCount,const unsigned int& nAllCount,const std::string& strDomain)=0;

	/* �����������ͨ�����������������Ļص� */
	virtual void OnGetMonCHLCount(const std::string& strGroupID,const unsigned int& nOnlineCount,const unsigned int& nAllCount,const std::string& strDomain)=0;

	/* ĳ�������ӶϿ� */
	virtual void OnDomainDisconnect(const std::string& strDomain,int nFromLevel)=0;
	/* ��ȡĳ��ͨ������Ϣ */
	virtual void OnGetAppointDevChannelInfo(const MONCHANNEL& item)=0;
	/* �豸ͨ������ƵID���� */
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;
	virtual void OnMonAlert(const std::string& strChannelID,unsigned short usType,unsigned short usSubtype)=0;			

	/* ���Ӽ���豸�������ص� */
	virtual void OnAddMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnDelMonTmpGroup(const std::string& strGroupID)=0;
	virtual void OnRenameMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnAddMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void OnDelMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void OnGetMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnGetMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;
	virtual void OnSetMonTmpDefaultChannelNo(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;
	virtual void OnGetMonTmpGroupMemberEnd()=0;

	/* �յ�����豸���� */
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, const std::string& strParentID)=0;

	/* �յ����ͨ������ */
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName,const std::string& strDevID)=0;

   	/* ��ȡ���¼��ƻ� */
	virtual void OnGetMONRecTaskItem(const std::string& strChannelID, const std::string& strUpdatetime, const std::string& strRecTask) = 0;
	virtual void OnGetMONRecTaskEnd() = 0;
	/* ��ȡ���ͨ��¼��ƻ� */
	virtual void OnGetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime, const std::string& strChnlRecTask) = 0;
	/* ��ȡ���¼�����(����:¼��ķָ�����,��С) */
	virtual void OnGetMONRecStrategy(const std::string& strUpdatetime, const std::string& strStrategy) = 0;

		/*������*/
	virtual void OnAddDiscussArea(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnAddDiscussAreaEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnGetDiscussInfo(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnGetDiscussInfoEnd(const unsigned long& ulDiscussID)=0;
	/* �����������Ա��Ϣ */
	virtual void OnGetDiscussMemberInfo(const IMS_USER_ITEM& contactItem)=0;
	/* �����������Ա��Ϣ���� */
	virtual void OnGetDiscussMemberInfoEnd(void)=0;
	virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnExitDiscuss(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnRecvDiscussMSG(const unsigned long& ulDiscussID,const std::string& FromID,const XMSG& xmsg)=0;
	virtual void OnModDiscussName(const unsigned long& ulDiscussID,const std::string& strName)=0;
	/*Ⱥ��*/
	/*����Ⱥ��Ϣ*/
	virtual void OnCreatTeam(const TEAM_DES& team)=0;
	/*����Ⱥ��Ϣ����*/
	virtual void OnCreatTeamEnd(void)=0;
	/* ����Ⱥ��Ա��Ϣ */
	virtual void OnGetTeamMemberInfo(const IMS_USER_ITEM& contactItem)=0;
	/* ����Ⱥ��Ա��Ϣ���� */
	virtual void OnGetTeamMemberInfoEnd(void)=0;
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
	/* ��ȡȺ��������ĳ��Ա��Ϣ����*/
	virtual void OnGetTeamDiscussMemberInfo(const std::string& strMemberID,const IMS_USER_ITEM& contactItem)=0;
	/* Ⱥ�����߹��ܻص� */
	virtual void OnSendTeamData(const PTNDSFileInfo ptNDSFileInfo, const unsigned long& ulSerialNO, const unsigned long& ulError) = 0;
	virtual void OnSendTeamDataEnd(const unsigned long& ulFileID) = 0;
	virtual void OnDownloadTeamData(const unsigned long& ulSerialNO, const PTNDSFileInfo ptNDSFileINfo, const unsigned long& ulError) = 0;
	virtual void OnDownloadTeamDataEnd(const unsigned long& ulFileID) = 0;

	virtual void OnReadyDownloadTeamData(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;

	virtual void OnGetTeamShareFile(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;
	virtual void OnGetTeamShareFileEnd(const unsigned long& ulTeamID) = 0;
	virtual	void OnDelTeamShareFile(const TNDSFileInfo& tNDSFileInfo,const unsigned short& usErrCode) = 0;
	
	/************************************************************************
     *���������
    /************************************************************************/

    /* �յ��û��������ݿ�ʼ */
    virtual void OnUserTypeListStart(void)=0;

    /* �յ��û��������� */
    virtual void OnUserTypeItem ( const std::string& strOrgID ,const IMS_USERTYPE_ITEM& Item)=0;

    /* �յ��û��������ݽ��� */
    virtual void OnUserTypeListEnd (void)=0;

	/* �Ƿ��а󶨵�¼������ */
	virtual void OnGetBindRecordServer(const unsigned int& unCount)=0;

	/************************************************************************
	*�㲥������
	/************************************************************************/
	/* �յ��㲥�������б� */
	virtual void OnVODSvrList( const IMS_VOD_SVR_LST& listVodSvr)=0;

	
	/****************************************************************
	 * ��ȡ�������������б�
	 ****************************************************************/
	virtual void OnGetLocalDomainList(const DOMAININFOLIST& lstDomainInfo)=0;
	/* ��ȡ�����б� */
	virtual void OnGetOrgList(const ORGINFOLIST& lstOrgInfo)=0;
    /* ��ȡ�����б� */
	virtual void OnGetDepartList(const DEPARTINFOLIST& lstDepartInfo)=0;
	/* ��ȡָ�����µļ�ػ����б� 
	 * strUpdateTime: �ϴθ��»����б��ʱ�� 
	 * ���strDomainΪ�գ���Ĭ�ϻ�ȡ���������б�*/
	virtual void OnGetAppointDomainRoomList(const std::string& strDomain,const MMS_ROOM_LST& lstRoom)=0;
	virtual void OnGetAppointDomainRoomListEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

    /* ��Ȩ���� */
    virtual void OnLicenseExperied(unsigned int nErrCode,const std::string& strDomain="")=0;

	/****************************************************************
	 * ������
	 ****************************************************************/
	virtual void OnGetUserRemark(const std::string& strUserID,const std::string& strRemark)=0;

	/************************************************************************
	* ��������ϵͳ 
	************************************************************************/
	virtual void OnGetDomainInfo(const SUBSYSTEM_DOMAININFO& item)=0;
	virtual void OnGetDomainInfoEnd()=0;

	virtual void OnGetMCUInfo(const SUBSYSTEM_MCUINFO& item)=0;
	virtual void OnGetMCUInfoEnd(const std::string& strDomain)=0;
	
	virtual void OnGetMCUDeviceInfo(const SUBSYSTEM_DEVICEINFO& item)=0;
	virtual void OnGetMCUDeviceInfoEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

	
	virtual void OnGetMCUMGWInfo(const SUBSYSTEM_DEVICEINFO& item)=0;
	virtual void OnGetMCUMGWInfoEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

	virtual void OnMCUDeviceStatus(const SUBSYSTEM_DEVICEINFO& item)=0;
    
	/************************************************************************
	* ��������������
	************************************************************************/
    virtual void OnYaAnMonAlert(const std::string& strMemberID,unsigned long ulChannelIndex, bool bPopMsg = false)=0;

	/* ��ȡ��ǰ��Ҫ����Ƶ��ϵ�ͨ�� */
	//virtual void OnGetVQDChannelInfo(const unsigned int& uStartID, const bool& bIsEnd,const VQD_CHANNEL_LST& lstVQDChannel,const std::string& strUserData)=0;

	virtual void OnGetAlarmSearch(unsigned long ulSearchId, unsigned int unAlarmNum)=0;
	virtual void OnGetAlarmLogs(unsigned long ulSearchId, int nBeginRow, int nEndRow, bool bSort, const ALARMLOG_INFO_LST& lstAlarmLog, int nErrorCode)=0;
	virtual void OnDelAlarmSearch(unsigned long ulSearchId)=0;
	/* ������ strDomain:������ ��iLevel:strDomain����ڱ���ļ�������0��ͬ����,1(�ϼ�)��-1���¼� */
	virtual	void OnDomainConnected(const std::string& strDomain,const int& iLevel)=0;
	virtual void OnGetBusinessInfo(int id, int business_type, int parent_business_id, std::string business_name) = 0;
	virtual void OnGetBusinessInfoEnd() = 0;
};

//=======================================================================================
// IMSʵ��
//=======================================================================================
AFX_EXT_CLASS bool IMSSessionAddEvent(KIMSSessionEvent* pIMSSessionEvent);
AFX_EXT_CLASS void IMSSessionRemoveEvent(KIMSSessionEvent* pIMSSessionEvent);
AFX_EXT_CLASS void IMSSessionRelease(void);
AFX_EXT_CLASS KIMSSession& GetIMSSession(void);

#include "KMMSDefine.h"

class AFX_EXT_CLASS KMMSSessionEvent;
//=======================================================================================
// MMSģ����
//=======================================================================================
class AFX_EXT_CLASS KMMSSession
{
public:
	KMMSSession(void){};
	virtual ~KMMSSession(void){};

	/* SYS����ģ����û�ȡ�û�NODEID */
	virtual int OnGetUserNodeID(const std::string& strUserID,std::string& strNodeID)=0;
	virtual void OnDispatchCmd(KCmdPacket& t)=0;
	virtual void ClearLocalData(void)=0;

	virtual void SetUserID(const std::string& strUserID)=0;
	virtual std::string GetUserID(void)=0;
	virtual void SetUserName(const std::string& strUserName)=0;
	virtual std::string GetUserName(void)=0;

	/****************************************************************
	 * ��ȡ���ػ������б���
	 ****************************************************************/
	virtual MMS_ROOM_LST& GetLocalRoomList(void)=0;
	virtual MMS_MEMBER_LST& GetLocalMemberList(void)=0;

	virtual PMMS_ROOM GetLocalRoomItem(const std::string& strDomain,const std::string& strRoomID)=0;
	virtual PMMS_MEMBER_EX GetLocalMemberItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strMemberID)=0;

	/*��ȡָ�������ҵ�һ�����߳�Ա*/
	virtual void GetAppointRoomOnlineMember(const std::string& strRoomID)=0;

	/*��ȡָ�������ҵ�һ�����ߺϳ��ն˻�������ͨ����Ϣ*/
	virtual void GetAppointRoomOnlineMemberInfo(const std::string& strRoomID,const std::string& strDomain="",const std::string& strTag="")=0;

	/* ���ͳ�ʼ��¼���� */
	virtual void SendLoginCommand(void)=0;

	/****************************************************************
	 * ������
	 ****************************************************************/
	virtual void SetRoomRemark(const std::string& strRoomID,const std::string& strRemark)=0;
	virtual void GetRoomRemark(const std::string& strRoomID)=0;

	/****************************************************************
	 * ��ȡIPCAM�б���
	 ****************************************************************/
	virtual void GetIPCamList(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * �������б���
	 ****************************************************************/
	/* ������ʱ����
	 * IM��½�ɹ������*/
	virtual void ActiveTemporaryRoom()=0;
	/* ��û����б�
	 * bAllRoomType=true: ��ȡ���л�����/��/��Ա
	 * bAllRoomType=false:ֻȡ��׼������+��+��Ա+�߼�������*/
	virtual void GetRoomList(bool bAllRoomType=false)=0;

	/* ���ĳ�����������Ϣ
	 *����ȡ����ǰ�û������䵽��(�����������)ĳ����������Ϣ*/
	virtual void GetRoomItem(const std::string& strRoomID)=0;

	/****************************************************************
	 * ��ʱ�����ҷ���
	 ****************************************************************/

	/* �����ʱ������
	 * strGroupName: ��ʱ���������� */
	virtual void InsertTempGroup(const std::string& strGroupName)=0;

	/* �޸���ʱ����������
	 * ulGroupID: ��ʱ������ID
	 * strGroupName: ��ʱ���������� */
	virtual void UpdateTempGroupName(const std::string& strGroupID,const std::string& strGroupName)=0;

	/* ɾ����ʱ������
	 * ulGroupID: ��ʱ������ID */
	virtual void DeleteTempGroup(const std::string& strGroupID)=0;

	/* �����ʱ�����Ա */
	virtual void InsertTempMember(const std::string& strGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;

	/* ɾ����ʱ�����Ա */
	virtual void DeleteTempMember(const std::string& strGroupID,const std::string& strMemberID)=0;

	/****************************************************************
	 * ���鹦�ܺ���
	 ****************************************************************/
	/* ��¼������
	 * strDomain:			������������
	 * strRoomID:			������ID
	 * strPassword:			��ҪMD5����
	 * strInviteeGroupID:	����������ID
	 * bPresider:			���Ϊ��������,����Ҫȷ�����������Ƿ�Ϊ������
	 * strUpdateTime:		������Ϣ����ʱ��,�͵��������ж��Ƿ�һ��,�����һ�·����������س�Ա�б�
	 * strTermType:			�ն����� 0=���ն� 1=Ӳ�ն� 2=�ֻ�ƽ��� 3=TR*
	 * strTermName:			�ն����� PC Terminal record Digital MOB TR2..
	 * uDigitalCode			���ֹ��ܽ��������� */
	virtual void Login(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword,
		               const std::string& strMemberID,const std::string& strMemberName,
					   const std::string& strInviteeGroupID,bool bPresider=false,
					   const std::string& strUpdateTime="",unsigned short usTermType=0,const std::string& strTermName="",
					   bool bHasVideo=false, bool bInvite= false,const unsigned int& uDigitalCode=0)=0;

	/* �˳������� */
	virtual void Logout(const std::string& strRoomID, const bool bAllLogout=false)=0;

	/* �����Ա���
	 * strInviteeID:		�������ߵ�ID
	 * strPassword:			��������
	 * bPresider:			�Ƿ�����Ϊ������,true=�����������,false=���������� 
	 * ulTempGroupID		��ʱ����ʱ�ã���ΪҪ���ݴ�ID�õ�����ʱ���������ơ� */
	virtual void Invite(const std::string& strInviteeID,const std::string& strPassword,bool bPresider, const std::string& strTempGroupID = "",const std::string& strTempRoomNickname = "")=0;
	
	
	/*	һ������������в��ڻ�(��IM����)��Ա
	 * strPassword:			���ڻ��������
	 * bPresider:			�Ƿ�����Ϊ������,true=�����������,false=���������� */
	virtual void InviteAll(const std::string& strPassword,bool bPresider,const std::string& strTempRoomNickname = "")=0;

	/* �����Ա�����˻���
	 * strRoomID:			�������Ļ���ID
	 * strRoomName:			�������Ļ�������
	 * strInviteeID:		�������ߵ�ID
	 * strPassword:			��������
	 * bPresider:			�Ƿ�����Ϊ������,true=�����������,false=���������� 
	 * ulTempGroupID		��ʱ����ʱ�ã���ΪҪ���ݴ�ID�õ�����ʱ���������ơ� */

	virtual void InviteToPersonalRoom(const std::string& strRoomID, const std::string& strRoomName,const std::string& strInviteeID,const std::string& strPassword,bool bPresider, const std::string& strDevID, const std::string& strTempGroupID = "")=0;

	/* �ܾ�������� */
	virtual void Reject(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID)=0;

	/* �߳����� */
	virtual void Kickout(const std::string& strMemberID)=0;

	/* ���ͻ�����Ϣ */
	virtual void SendMessage(const std::string& strHead,const std::string& strBody,const std::string& strFormat)=0;

	/* �����û��Զ�������(�����͵����鵱�еĳ�Ա)
	 * ucObject����: 
	 *   0:���������������(�������Լ�)��������
	 *   1:����鼤�������г�Ա��������
	 *   2:��������г�Ա��������
	 *   >=3:��packet�е�toid�������� */
	virtual void SendUserData(KCmdUserDataPacket& packet,unsigned char ucObject=0)=0;

	/****************************************************************
	 * �����������
	 ****************************************************************/

	/* ��������鲢�㲥 */
	virtual void InviteInRoomBroadcast(const std::string& strMemberName,const std::string& strMeetingName,const std::string& strMeetingPass)=0;

	/* ���û������ID */
	virtual void SetActiveGroupID(const std::string& strActiveGroupID)=0;

	/* ���û����鵱ǰ��Ļ�� */
	virtual void SetCurrentScreenID(const std::string& strGroupID,int nCurScrID,int nPrevTM)=0;

	/* ��ȡ��Աͨ�����б�(VID/AUD/DOC) */
	virtual void GetCardList(void)=0;

	/* ����ͨ������,���㲥��������
	 * strCardType: ȱʡΪ��,�����3G����������"3G" */
	virtual void SetCardItem(int nCardIndex,const std::string& strCardName,MMS_CHANNEL_LST& lstChannel,const std::string& strCardType="")=0;

	/* ������Ƶͨ������,���㲥�������� */
	virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioChannelID)=0;

	/* ����ͨ��������(ֻ�����ñ����û���) */
	virtual void SetCardName(int nCardIndex,const std::string& strCardName)=0;

	/* �㲥��Ա�� */
	virtual void BroadcastCard(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType,const std::string& strTag)=0;

	/* �رչ㲥��Ա�� */
	virtual void CloseBroadcastCard(const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* �л����� */
	virtual void DragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* ���û���ģ�� */
	virtual bool SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber)=0;

	/* ���û����Զ�ģ��
	 * bAutoTem=true:�Զ�ģ��
	 * bAutoTem=false:�ֶ�ģ��*/
	virtual void SetAutoTemplet(bool bAutoTem)=0;

	/* ���û���������
	 * strAllAttribute�����������Դ�
	 * strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void SetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* ���û��鴰������virtual void OnSetTemplet
	* strAllAttribute���д��ڵ����Դ�
	* strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void SetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* ���ø�������λ��
	 * x = ���ر��� * 100
	 * y = ���ر��� * 100
	 * cx= ��ȱ��� * 100
	 * cy= �߶ȱ��� * 100 */
	virtual bool SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY)=0;	

	/* ͨ�û���������ýӿ� */
	virtual void SetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID, const std::string& strKeyword, const std::string& strParameter, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL)=0;
	
	/* ��ȡ����������ýӿ� */
	virtual void GetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID)=0;

	/* ɾ��ͨ�û���������� */
	virtual void DelMeetingParameter(const std::string& strRoomID, const std::string& strGroupID, const std::string& strKeyword, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL)=0;

	/* ���ø��˻����Ȩ�� */
	virtual void SetRight(int nRight, bool bEnable, const std::string& strMemberID="")=0;

	/* ��ȡ���˻����Ȩ�� */
	virtual void GetRight(const std::string& strMemberID="")=0;

	/* ��ȡ��ͬ����Ϣ */
	virtual void GetGroupSynInfo(const std::string& strGroupID)=0;

	/* ����strGroupID���ٿ�������� */
	virtual void AllowSubMeet(const std::string& strGroupID,const std::string& strMemberID)=0;

	/* Ҫ��strGroupID�鼰������������strGroupID��ͬ�� */
	virtual void Sync(const std::string& strGroupID,const std::string& strMemberID)=0;
	
	/* ������ʱ������ */
	virtual void SetTempPresider(const std::string& strMemberID,bool bSet)=0;

	/* �����ɾ�������� */
	virtual void SetProlocutor(const std::string& strMemberID="")=0;

	/* �㲥��ȡ���㲥������ */
	virtual void EnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* �����ɾ�������� */
	virtual void SetQuestioner(const std::string& strMemberID="")=0;

	/* ���ӻ�ȡ������������*/
	virtual void EnableQuestioner(const std::string& strMemberID,bool bEnable)=0; 

	/* �������� */
	virtual void SetAnnexData(int nType,const std::string& strData,const std::string& strFormat)=0;

	/* ���� */
	virtual void EnableAnnex(int nType,bool bEnable=true)=0;
	
	/* 
	��ȡ���桢��������
	*/
	virtual void GetAnnexData(void)=0;

	/*��ȡָ�������ҵĻ����ն�������������*/
	virtual void GetRoomMemberCount(const std::string& strRoomID)=0;

	/* ���������ȡ��ǰ�����ҵ���Ա�б�
		bOnlyOnline��true  ֻȡ������Ա
		bOnlyOnline��false ȡ���ߺͲ�������Ա */
	virtual void GetServerMemberList(bool bOnlyOnline=true)=0;

	/****************************************************************
	 * ����ͶƱ����
	 ****************************************************************/

	/* ֻ��λ��root��������˲ſ��Է���ͶƱ,��������в��ܷ���
	/* ����ͶƱ 
	topic:		ͶƱ����
	type:		ͶƱ���ͣ������򲻼�����"public"��"anonymous"��
	ulTime:		ͶƱʱ�䣬��ʱ��ͶƱ����Ȩ
	bMulti:		ͶƱʱ�Ƿ����ѡ����ѡ��
	strItems:	ͶƱѡ��*/
	virtual void StartVote(const std::string& strTopic,const bool& bType,const unsigned long& ulTime,const bool& bMulti,MMS_VOTE_LST& strItems)=0;

	/* ����ͶƱ 
	endtype:	ͶƱ������ʽ���ֶ����Զ���"hand" �� "auto"��*/
	virtual void StopVote(const std::string& strVoteID,const std::string& strEndType)=0;

	/* ͶƱ
	result:		ͬ�⡢���Ի���Ȩ��"agree"��"disagree"��"abstain"��
	strMemberID:	ͶƱ�ߵ�ID�����ڲ�����ͶƱ��Ϊ��("")*/
	virtual void Vote(const std::string& strMemberID,const std::string& strVoteID,MMS_VOTE_LST& strItems)=0;

	/****************************************************************
	 * ������ֺ���
	 ****************************************************************/

	/* ��ʼ����:�����˵��� */
	virtual void StartHandle(const unsigned long& ulTime)=0;

	/* ����:�����˵��� */
	virtual void Handle(const unsigned long& ulID,const bool& bAgree)=0;

	/* ֹͣ����:�����˵��� */
	virtual void StopHandle(const unsigned long& ulID)=0;

	/****************************************************************
	 * �����������
	 ****************************************************************/
	virtual void Calling(const unsigned int& usTime)=0;
	virtual void CloseCalling()=0;
	virtual void CallingInConference(const std::string& strFile)=0;

	/****************************************************************
	 * �����غ���
	 ****************************************************************/

	//���ͼ������
	virtual void MonUserData(KCmdUserDataPacket& packet)=0;

	//���P2P
	virtual void MonP2P(const std::string& strChannelID, KCmdUserDataPacket& packet)=0;

	/****************************************************************
	 * ����ǽ����
	 ****************************************************************/
	virtual void InsertTVMonitor(unsigned short usIndex,const std::string& strData)=0;
	virtual void UpdateTVMonitor(unsigned short usIndex,const std::string& strData)=0;
	virtual void DeleteTVMonitor(unsigned short usIndex)=0;
	virtual void InsertTVMatrix(unsigned short usIndex,const std::string& strData)=0;
	virtual void UpdateTVMatrix(unsigned short usIndex,const std::string& strData)=0;
	virtual void DeleteTVMatrix(unsigned short usIndex)=0;
	virtual void GetTVList(void)=0;

	/****************************************************************
	 * SIP ������ҶϺ���
	 ****************************************************************/
	virtual void SIPCallIn(void)=0;
	virtual void SIPHangUp(void)=0;

	/****************************************************************
	 * ¼������
	 ****************************************************************/
	virtual void GetStorageServer()=0;
	virtual void GetUploadPlan()=0;
	virtual void GetRecordFormat()=0;
	virtual void SetRecordFormat(const MMS_RECFORMAT& recFormat)=0;

	virtual void StartRecord()=0;
	virtual void PauseRecord()=0;
	virtual void ContinueRecord()=0;
	virtual void StopRecord(const std::string & strRoomID)=0;

	// ����ֱ���Ľӿ�
	virtual void StartLive( const std::string& liveIP, unsigned short livePort, const std::string& liveChannelName)=0;
	virtual void StopLive() = 0;

	/****************************************************************
	 * TV����
	 ****************************************************************/

	/* �����û��Զ�������(�����͵����鵱�еĳ�Ա)
	* ucObject����: 
	*   0=���������������(�������Լ�)��������
	*   1=��������г�Ա��������
	*   ��0��1�����������=��packet�е�toid�������� */
	virtual void SendUserDataEx(const std::string& strConferenceID, KCmdUserDataPacket& packet,unsigned char ucObject=0)=0;
	virtual void RegisterConference(const std::string& strConferenceID,const std::string& strUpdateTime,unsigned short usTermType,const std::string& strTermName)=0;
	virtual void UnregisterConference(const std::string& strConferenceID)=0;

    /************************************************************************
    * ���������
    /************************************************************************/
    // ��ȡ�����������б�
    virtual void GetRoomTypeList(void) = 0;

    // ��ȡ���ػ����������б�
    virtual MMS_ROOMTYPE_LST& GetLocalRoomTypeList(void) = 0;
    
    // ��ȡ��������б�
    virtual void GetPhysicalRoomList() = 0;

    // ��ȡ������������б�
    virtual MMS_PHY_ROOM_LST& GetLocalPhysicalRoomList() = 0;

    // ��ȡ�Ự����
    virtual void GetTalkSet() = 0;

	/************************************************************************
	* ���ɻ���
	/************************************************************************/
	// ��ȡ�������ɻ����
	virtual void GetFreeRoomID() = 0;

    /************************************************************************
    * ��������Ȩ
    /************************************************************************/
    // ��ȡ���ͷŵ��Ӵ�������Ȩ
    virtual void StartTouchScreen() = 0;
    virtual void StopTouchScreen() = 0;

    /************************************************************************
    * ���������
    /************************************************************************/
    // �û���ǰ���ڻ�������֪�ģ�������Ϊ����
	virtual void AddGroup( const std::string& strGroupName, const std::string& strParentGroupID, const std::string& strTopic /*levelid, orderid, topic*/)=0;
	virtual void DelGroup( const std::string& strGroupID)=0;
    // virtual void AddGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // virtual void DelGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // �ı��û������飬����
	virtual void ChangeMemberGroup( const std::string& strGroupID, const std::string& strMemberID)=0;

	//��ȡ���������Ա��Ϣ
	virtual void GetRoomGroupMemberInfo(const std::string& roomid, const std::string& groupid)=0;

    /************************************************************************
    * ���˷������
    /************************************************************************/
	virtual void GetPersonalRoom(void)=0;
	virtual void GetPersonalRoomGroup(const std::string& strRoomID)=0;
	virtual void GetPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void AddPersonalRoom(const std::string& strRoomName)=0;
	virtual void DelPersonalRoom(const std::string& strRoomID)=0;
	virtual void AddPersonalRoomGroup(const MMS_GROUP& group)=0;
	virtual void DelPersonalRoomGroup(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void StartPersonalRoom(const std::string& strRoomID)=0;

    virtual void GetRoomMemberList()=0;

	/* ��ȡ������ԱȨ������
	strRoomID:		����ID
	strMemberID:	��ԱID*/
	virtual void GetRoomMemberType(const std::string& strRoomID,const std::string& strMemberID)=0;

	/* ��Ӹ��˻����� */
	virtual void SendPersonalAddGroupEnd(const std::string& strRoomID)=0;

	//������������û������ͣ�Ĭ�ϱ�������޲�νṹ
	virtual void SetPersonalRoomType(const std::string& strRoomID,bool bTreeMode=false)=0;
	virtual void RenamePersonalRoom(const std::string& strRoomID,const std::string& strRoomName)=0;
	virtual void AddPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void SetPersonalRoomMemberType(const std::string& strRoomID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void DelPersonalRoomMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	/************************************************************************/
	/* ��ϵ�˷���                                                           */
	/************************************************************************/
	virtual void GetContactGroup(void)=0;
	virtual void GetContactGroupMember(const std::string& strGroupID)=0;

	/* ��ȡδ����������ն��б�
	 * nBegin ��������ʼȡ
	 * nEnd   ȡ����������
	 * bOnline : Ϊtrueʱ��ȡ���ߵģ�Ϊfalseʱ��ȡ���е�
	 * strDevName �豸���� */
	virtual void GetNoInRoomDevList(int nBegin/*=1*/,int nEnd/*=50*/,bool bOnline = false, const std::string& strDevName = "")=0;

    virtual void GetLoginInfo()=0;
    
    //������������
	virtual void SetMicSeqCfg(std::string& strRoomID,unsigned short usType,unsigned int nDuration,unsigned int nMaxNum,unsigned int nSpUserNum,std::string &strTag)=0;
	
	//����
	virtual void GrapMicSeq(std::string& strRoomID,const std::string& strMemberID)=0;
	
	//����ͨ��
	virtual void ExitMicSeq(std::string& strRoomID,const std::string& strMemberID)=0;
	
	//���������˸�������
	virtual void ModifyMicSeq(std::string& strRoomID,MMS_SEQ_MEMBER_LST & lstMember)=0; 	
	//����������Ϊĳ��Ա���Ӷ���ʱ��
    virtual void SetExTime(std::string strMemberID,std::string& strRoomID,unsigned long nExTime)=0;
	
	//��������б�������������
	virtual void ClearMicSeq(std::string& strRoomID)=0; 
    
	/*���� MPS*/
	virtual void StartMP(void)=0;
	virtual void Start3GC(void)=0;
	/*���� MPS*/
	virtual void StopMPS(const std::string& strMPSUserID)=0;
	/*����DTS��ʼ���ݽ������*/

	/************************************************************************/
	/* ����ת�� 
	strMemberID_unCardIndex:����ת���ͨ��
	/************************************************************************/
	virtual void RequestTranscode(const std::string strMemberID,unsigned short unCardIndex)=0;
	virtual void StopTranscode(const std::string strMemberID,unsigned short unCardIndex)=0;

	//virtual void SetDTSDataRecive(const bool& bDTSDataRecive = true)=0;
	//virtual void SendDelayCmd()=0;
	virtual std::string GetGroupID()=0;

    //�ƶ����
    //��ȡ�ƶ����Ƶ���б�
    virtual void GetMobileChannelList(const std::string& strDomain)=0;
    //����Ƶ������Ȩ
    virtual void ApplyMobileChannelControl()=0;
    //�����յ��Ķ�Ƶ������Ȩ������
    virtual void HandleApplyMobileChannelControl(std::string& strApplyID,bool bIsSuccess)=0;
    //�ͷ�Ƶ������Ȩ
    virtual void ReleaseMobileChannelControl()=0;

	//��ȡ���ڷ�����ip
	virtual void GetSerialServerIP(const std::string& strDevID)=0;

	virtual void GetMultiplyRoom(const std::string& strDomain)=0;
	
	/****************************************************************
	 * ��������
	 ****************************************************************/
	//���ۿͷ�StrServiceID:�ͷ�ID,usSatisfaction:����ȣ�0:�������ۣ�1:�����⣬2:�������⣬3:���⣬4:�ǳ����⣩��usServiceTime:����ʱ������λ�룩
	virtual	void JudgeService(const std::string& strServiceID,const unsigned short& usSatisfaction=0,const unsigned long& ulServiceTime=0) = 0;
	virtual void UploadServiceSumary(int business_type, int business_first_level, int business_second_level, std::string strDepart = "", unsigned long ulTalkTime = 0)=0;
	//����֤ȯsip�������
	/************************************************************************/
	/* ����˵��     
	strCallID		�ն˺�������������id
	strUserID		�ն˺�������������Աid
	strUserName		�ն˺�������������Ա����
	strMcuID		mpserver id
	strRoomID		�ն��������Ļ���id
	bAccept			�Ƿ�ͬ���sip�ն���� =trueͬ�⣻=false��ͬ��
	*/
	/************************************************************************/
	virtual void ResponseForSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID, bool bAccept)=0;

	//�������ֹ�����������룬�����ɷ������ɺ����͸�������
	virtual void SetDigitalInvitationCode(const std::string& strRoomID) = 0;

	//���û�����С����
	virtual void SetRoomMinCodestream(const std::string& strRoomID,double dRoomMinCodestream) = 0;

	//�����û�������Ϣ
	virtual void SetRoomMemberInfo(const MEMBER_DEFINED_INFO& memberinfo) = 0;

	//��ȡ�������û���Ϣ
	/* ����˵��     
	strMemberID		=""ʱ����ȡ�������������߳�Ա�б���Ϣ;!=""ʱ��ȡ�˳�Ա��Ϣ
	*/
	virtual void GetRoomMemberInfo(const std::string& strMemberID ="") = 0;
	
	/***************************************************
	//�������ܣ���ʼ��ѭ
	//������
	//		strRoundRobinGroupID	��ѭ����id
	//		nTMWndCnt				������
	//		nPrerecvCnt				���Ԥ��·��
	//		strReceiverlist			������userid�б�(�ԡ�;���������磺cs0001;cs0002���������)
	//		bRecvAudio				�Ƿ����Ƶ
	//����ֵ����
	***************************************************/
	virtual void StartRoundRobin(const std::string& strRoundRobinGroupID, const int& nTMWndCnt, const int& nPrerecvCnt, const std::string& strReceiverlist, const bool bRecvAudio=false) = 0;

	/***************************************************
	//�������ܣ�ֹͣ��ѭ
	//������
	//		strRoundRobinGroupID		��ѭ����id
	//����ֵ����
	***************************************************/
	virtual void StopRoundRobin(const std::string& strRoundRobinGroupID) = 0;

	/***************************************************
	//�������ܣ����õ�ǰԤ����Ϣ
	//������
	//		strPreRecvData			��ǰԤ����Ϣ
	//����ֵ����
	***************************************************/
	virtual void SetRRCurPreRecvAV(const std::string& strPreRecvData) = 0;

	/***************************************************
	//�������ܣ����õ�ǰ��ʾ��Ϣ
	//������
	//		strViewShowData			��ǰ��ʾ��Ϣ
	//����ֵ����
	***************************************************/
	virtual void SetRRCurViewShowAV(const std::string& strViewShowData) = 0;

	/***************************************************
	//�������ܣ�����ȫ��
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uChannelID		ͨ��ID
	//		bFull				true:����ȫ����false��ȡ��ȫ��
	//����ֵ����
	***************************************************/
	virtual void SetFullScreen(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, bool bFull) = 0;

	/***************************************************
	//�������ܣ�ģ��Ԥ��
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		uNumber				ģ���
	//		uBandwidth			ģ�����
	//		uWidth				ģ��ֱ��ʿ�
	//		uHeight				ģ��ֱ��ʸ�
	//		strPara				��������
	//����ֵ����
	***************************************************/
	virtual void TemplatePreset(const std::string& strRoomID, const std::string& strGroupID, const unsigned int uNumber, const unsigned int uBandwidth, const unsigned int uWidth, const unsigned int uHeight, const std::string& strPara) = 0;

	/***************************************************
	//�������ܣ�����Ԥ��
	//������
	//		WndPreSet			��ο��ṹ�嶨��
	//����ֵ����
	***************************************************/
	virtual void WindowPreset(const MMS_WNDPRESET& WndPreSet) = 0;

	/***************************************************
	//�������ܣ��ı�ͨ��������Ƶ��Ϣ
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uChannelID			ͨ��ID
	//		uStream				����
	//		uWidth				ģ��ֱ��ʿ�
	//		uHeight				ģ��ֱ��ʸ�
	//����ֵ����
	***************************************************/
	virtual void ChangeAudioVedioInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const unsigned int uStream, const unsigned int uWidth, const unsigned int uHeight) = 0;

	/***************************************************
	//�������ܣ��ɼ�����
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uChannelID			ͨ��ID
	//		bLock				�Ƿ�ɼ�������true��������֮��
	//����ֵ����
	***************************************************/
	virtual void CollectionLock(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const bool bLock) = 0;

	/***************************************************
	//�������ܣ��ϱ�ͨ���豸��Ϣ
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMerberID			��ԱID
	//		uChannelID			ͨ��ID
	//		strDevInfo			�豸��ϸ��Ϣ
	//ע�⣺
	//		����strDevInfo�����ʽ��lcodec��ʾ����Э�飬chldef��ʾ�����ȣ�chlrate��ʾ֡��
	//		h264��h265��HD��ʾ���壬SUD���壬SD��ʾ����
	//		"chlcodec:h264;chldef:HD;chlrate:30;" ��ʾh264����� �����豸 30֡
	//		�ַ���������һ�����߶������ϣ�Ҳ�����Զ���
	//����ֵ����
	***************************************************/
	virtual void ReportChlDevInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMerberID, const unsigned int uChannelID, const std::string& strDevInfo) = 0;
};

//=======================================================================================
// MMSģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KMMSSessionEvent
{
public:
	KMMSSessionEvent(void){};
	virtual ~KMMSSessionEvent(void){};

public:
	/****************************************************************
	 * ��ʱ�����¼�
	 ****************************************************************/
	/* ������ʱ����ص���nErrorCode=0���״μ���ɹ� 1������ʧ�ܣ�δ��Ȩ�� 2���ѱ������ */
	virtual void OnActiveTemporaryRoom(int nErrorCode)=0;
	virtual void OnTempGroupItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strGroupName,bool bDefault)=0;
	virtual void OnTempGroupUpdateName(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnTempGroupDelete(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnTempMemberItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;
	virtual void OnTempMemberDelete(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID)=0;

	/****************************************************************
	 * IPCAM�б��¼�
	 ****************************************************************/
	virtual void OnIPCamItem(unsigned long ulIPCamID,const std::string& strIPCamName,const std::string& strOrgID,const std::string& strModel,const std::string& strIPAddr,const std::string& strAdminID,const std::string& strAdminPass,const std::string& strOperID,const std::string& strOperPass,unsigned char ucStatus)=0;
	virtual void OnIPCamListEnd(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * �����б��¼�
	 ****************************************************************/
	virtual void OnRoomListEnd(const std::string& strDomain)=0;

	virtual void OnRoomItem( const PMMS_ROOM& pItem)=0;

	/****************************************************************
	 * �����¼�
	 ****************************************************************/

	/* �յ���¼�������¼�
	 * errcode���ص�¼���ֵ
	 * 0: �ɹ���¼������
	 * 1: ������֤ʧ��
	 * 2: �û��Ѿ��������
	 * 3: �����Ѿ�������������
	 * 4: ����û���ʵ���MCU������
	 * 5: �����������뿪����
	 * 6: ��¼�����Ͳ���ȷ
	 * 7: �����Ѿ�����
	 * 8: �������Ͳ���ȷ
	 *99: �����ڲ��������� */
	virtual void OnLogin(unsigned int nErrCode,const std::string& strDomainID,const std::string& strRoomID,const std::string& strRoomName,unsigned short usRoomType,
		const std::string& strRootGroupID,const std::string& strGroupID,const std::string& strActiveGroupID, const std::string& strTempGroupName,
        const std::string& strMemberID,unsigned short usMemberType,bool bAllowVideo,double dMaxRoomCodestream,double dMinRoomCodestream,const std::string& strPassword)=0;

	/* �յ��˳��������¼�
	 * nErrCode���ص�¼���ֵ
	 * 0: ��ʾ�����˳�������
	 * 1: ��ʾ�������˳�������
	 * 2: ��ʾ���߳�������
	 * 3: �����Ա�ʺ��ڸ÷������ϲ����ڻ�÷�����û�м��ػ���ģ�� 
	 * 4: ����������������� 
	 * 5: ����Ͽ����ӣ��˳��ϼ�����*/
	virtual void OnLogout(const std::string& strRoomID,unsigned int nErrCode)=0;

	/* �յ��������� */
	virtual void OnRoomGroupItem(MMS_GROUP_EX& item)=0;

	/* �յ������������ */
	virtual void OnRoomGroupListEnd(const std::string& strUpdateTime="")=0;

	/* �յ������Ա�� */
	virtual void OnRoomMemberItem(MMS_MEMBER_EX& item)=0;

	/* �յ������Ա����� */
	virtual void OnRoomMemberListEnd(void)=0;

	/* �յ�Ԥ�����ڳ�Ա�� */
	virtual void OnWindowMemberItem(const std::string& strGroupID,unsigned int nScreenID,unsigned int nWindowID,const std::string& strMemberID)=0;

	/* �յ�Ԥ�����ڳ�Ա����� */
	virtual void OnWindowMemberListEnd(void)=0;

	/* �յ�����������׼��,�յ�����Ϣ��ʾ�����¼�ɹ����յ�����ʵʱ���� */
	virtual void OnReady(void)=0;

	/* �յ���Ա״̬ */
	virtual void OnStatus(const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,
		                  const std::string& strSvrID,const std::string& strSvrAddr,unsigned short usSvrPort,const std::string& strSvrType,
						  const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
						  unsigned short usTermType,const std::string& strTermName,unsigned char ucStatus,const std::string& strstaoff)=0;

	/* �յ��������� */
	virtual void OnInvite(const std::string& strRoomDomain,const std::string& strRoomID,const std::string& strGroupID,
		const std::string& strRoomName,const std::string& strTempRoomNickname,const std::string& strInviterNodeID,const std::string& strInviterID,
		const std::string& strInviterName,const std::string& strPassword,bool bPresider, const std::string& strTempGroupID, 
		const std::string& strTempGroupName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;

	/* �յ��ܾ����� */
	virtual void OnReject(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName)=0;

	/* �յ�������Ϣ */
	virtual void OnMessage(const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat)=0;

	/* �յ��û���������
     * ucObject����: 
	 *   0=�յ��������˷��͵�����
	 *   1=�յ��㲥����
	 *   ��0��1��=�յ��㵽������ */
	virtual void OnUserData(KCmdUserDataPacket& packet,unsigned char ucObject)=0;

	/* ���ü����� */
	virtual void OnSetActiveGroupID(const std::string& strGroupID)=0;

	/* ���õ�ǰ��Ļ�� */
	virtual void OnSetCurrentScreenID(const std::string& strOwnerID,const std::string& strGroupID,int nCurScrID,int nPrevTM)=0;

	/* ɾ��ͨ�� */
	virtual void OnCardItemOffline(const std::string& strMemberID,int nCardIndex,const std::string& strCardType)=0;

	/* �յ�ͨ���� */
	virtual void OnCardItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardType,const std::string& strCardName,
		                    unsigned int nChannelType,unsigned long ulChannnelID,
							const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
							const std::string& strDevInfo, unsigned long ulCodeType)=0;

	/* ͨ�����б���� */
	virtual void OnCardListEnd(void)=0;

	/* �յ���Ƶͨ���� */
	virtual void OnCardAudioItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,unsigned long ulAudioID)=0;

	/* ����ͨ������ */
	virtual void OnSetCardName(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName)=0;

	/* ��ͬ����Ϣ��ȡ��� */
	virtual void OnGroupSynInfoEnd(void)=0;
	
	/*���û������*/
	virtual void OnSetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword, const std::string& strParameter, const std::string& strAdminID)=0; 
	
	/*��ȡ�������  ������Ϊ0��ʾ��ȡ����Ӧ������� */
	virtual void OnGetMeetingParameter(int ErrCode,const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword = "", const std::string& strParameter = "")=0; 

	/*ɾ���������*/
	virtual void OnDelMeetingParameter(const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword)=0;

	/* ���ø��˻����Ȩ�� */
	virtual void OnSetRight(int nRight, bool bEnable, const std::string& strMemberID)=0;
	/* ��ȡ���˻����Ȩ�� */
	virtual void OnGetRight(const std::string& strMemberID,const std::string& strMemRight)=0;

	/* �����ɾ�������� */
	virtual void OnSetProlocutor(const std::string& strMemberID,bool bRequest)=0;

	/* �㲥��ȡ���㲥������ */
	virtual void OnEnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* �����ɾ�������� */
	virtual void OnSetQuestioner(const std::string& strMemberID,bool bRequest)=0;

	/* ���ӻ�ȡ������������ */
	virtual void OnEnableQuestioner(const std::string& strMemberID,bool bEnable)=0;

	/* ���ù������� */
	virtual void OnAnnexData(int nType,const std::string& strData,const std::string& strFormat)=0;

	/* ���ù������� */
	virtual void OnEnableAnnex(int nType,bool bEnable)=0;

	/* ������� */
	virtual void OnUnlocked(void)=0;

	/* �㲥�� */
	virtual void OnBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,int nScreenID,int nWindowID,
		                         const std::string& strMemberID,int nCardIndex,const std::string strCardName,int nChannelType,unsigned long ulChannelID,
								 const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
                                 const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
								 const std::string& strTag,const MMS_MEMBER_TYPE& memberType, const std::string& strMemberName, unsigned short usCodeType)=0;

	/* �رչ㲥�� */
	virtual void OnCloseBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* �϶����� */
	virtual void OnDragWindow(const std::string& strOwnerID,int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* �ı�ģ�� */
	virtual void OnSetTemplet(const std::string& strOwnerID,int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber,unsigned short unErr)=0;

	/* �Զ�ģ�� */
	virtual void OnSetAutoTemplet(const std::string& strFromID,unsigned int nErr,const std::string& strRoomID,const std::string& strGroupID,bool bAutoTem)=0;

	/* ���û���������
	 * strAllAttribute�����������Դ�
	 * strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void OnSetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* ���û��鴰������
	* strAllAttribute���д��ڵ����Դ�
	* strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void OnSetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* �������� */
	virtual void OnSetFloatWindow(const std::string& strOwnerID,int nScreenID,int nX,int nY,int nCX,int nCY)=0;	

	/* ���뵽�����Ҳ��㲥 */
	virtual void OnInviteInRoomBroadcast(const std::string& strRoomDomain,const std::string& strRoomID,const std::string& strGroupID,
		                                 const std::string& strMemberID,const std::string& strMeetingName,const std::string& strMeetingPass,
										 const std::string& strMemberName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;

	/* ������ʱ������
	 * bSet: true=����;false=ȡ�� */
	virtual void OnSetTempPresider(const std::string& strMemberID,bool bSet)=0;

	/* ͶƱ */
	virtual void OnStartVote(const std::string& strVoteID,const std::string& strInitiator,const std::string& strTopic,const bool& bType,const unsigned long& ulTime,const bool& bMulti,MMS_VOTE_LST& strItems)=0;
	virtual void OnStartVoteErr(const unsigned int& nErrCode)=0;
	virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,const int& nSum,const int& nAnswer,MMS_VOTE_LST& strItems)=0;

	/* ���� */
	virtual void OnStartHandle(const unsigned long& ulID,const unsigned long& ulTime)=0;
	virtual void OnStartHandleErr(const unsigned int& nErrCode)=0;
	virtual void OnStopHandle(const unsigned long& ulID,const unsigned short& usTotal,const unsigned short& usCount)=0;

	/* ������ͬ�� */
	virtual void OnAllowSubMeet(const std::string& strGroupID)=0;
	virtual void OnSync(const std::string& strGroupID)=0;

	/* ���� */
	virtual void OnCalling(const std::string& strMemberID,const unsigned int& usTime)=0;
	virtual void OnCallingErr(const unsigned int& nErrCode)=0;
	virtual void OnCloseCalling(const std::string& strMemberID)=0;

	/* ���ؼ������ */
	virtual void OnMonUserData(KCmdUserDataPacket& packet)=0;

	/* ���ؼ��P2P */
	virtual void OnMonP2PData(const std::string& strDevID,KCmdUserDataPacket& packet)=0;
    
	/* ����ǽ */
	virtual void OnInsertTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;

	/* ����ǽ������ */
	virtual void OnInsertTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnTVServerItem(const std::string& strTV,const std::string& strTVName,const std::string& strStatus, 
		const std::string& strMemNodeid, unsigned long ulTVWallTemplate)=0;
	virtual void OnTVMonitorItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVMatrixItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVListEnd(void)=0;
	
	/*��ȡָ�������ҵĻ����ն�������������*/
	virtual void OnGetRoomMemberCount(const std::string& strRoomID,int nOnline,int nTotal)=0;
	/****************************************************************
	* ¼������
	****************************************************************/
	virtual void OnGetStorageServer(const std::string& strServer)=0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan)=0;
	virtual void OnGetRecordFormat(MMS_RECFORMAT& recFormat)=0;

	virtual void OnStartRecord(const std::string& strUserID,const std::string& strServerID="")=0; // strServerID��Ϊʧ��ֻ�������߻ص�����������������
	virtual void OnPauseRecord()=0;
	virtual void OnContinueRecord()=0;
	virtual void OnStopRecord()=0;

	/****************************************************************
	 * TV����
	 ****************************************************************/
	/*usRet��0Ϊ�ɹ���1Ϊʧ��*/
	virtual void OnRegisterConference(const std::string& strConferenceID,const std::string& strConferenceName,const std::string& strConferenceType,unsigned short usRet)=0;

	/****************************************************************
	 * �ƶ��ֻ��ն�
	 ****************************************************************/

	/* �ֻ�֪ͨ�û��л���GSM���� */
	virtual void OnOpenAudioChannelForMobile(const std::string& strMemberID,unsigned int nCardIndex)=0;

	/* �ֻ�֪ͨ�û��ָ���ԭ�б��� */
	virtual void OnCloseAudioChannelForMobile(const std::string& strMemberID,unsigned int nCardIndex)=0;

    /************************************************************************
    * ���������
    /************************************************************************/
    // ��ȡ���������б����
    virtual void OnRoomTypeListEnd(const std::string& strDomain) = 0;

    // ��ȡ������������б����
    virtual void OnPhysicalRoomListEnd(const std::string& strDomain) = 0;

    // �յ���������
    virtual void OnGetTalkSet( const MMS_TALK_SET& tTalkSet ) = 0;

	/************************************************************************
	* ���ɻ���
	/************************************************************************/
	// ��ȡ���ɻ���ŷ���
	virtual void OnFreeRoomID( const std::string& strFreeRoomID ) = 0;

    /************************************************************************
    * ��������Ȩ
    /************************************************************************/
    // ��������Ȩ����
    virtual void OnStartTouchScreen( int nErrCode ) = 0;

    /************************************************************************
    * ���������
    /************************************************************************/
	virtual void OnAddGroup( const std::string& strGroupID, const std::string& strGroupName, const std::string& strParentGroupID, const std::string& strTopic, unsigned int nErrCode) = 0;
	virtual void OnDelGroup( const std::string& strGroupID, unsigned int nErrCode) = 0;
	virtual void OnChangeGroupMember( const std::string& strGroupID, const std::string& strMemberID, unsigned int nErrCode) = 0;

	/************************************************************************
    * ���˷������
    /************************************************************************/
	virtual void OnGetPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,bool bHaveFrame,unsigned short usStatus)=0;
	virtual void OnGetPersonalRoomEnd(void)=0;

	virtual void OnGetPersonalRoomGroup(const std::string& strRoomID,const MMS_GROUP& group)=0;
	virtual void OnGetPersonalRoomGroupEnd(const std::string& strRoomID)=0;

	virtual void OnGetPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void OnGetPersonalRoomMemberEnd(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnAddPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,const std::string& strOwnerID)=0;
	virtual void OnDelPersonalRoom(const std::string& strRoomID)=0;
	virtual void OnRenamePersonalRoom(const std::string& strRoomID,const std::string& strMeetingName)=0;
	virtual void OnAddPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,bool bSuccessFlag)=0;
	virtual void OnSetPersonalRoomMemberType(const std::string& strRoomID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void OnDelPersonalRoomMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	virtual void OnAddPersonalRoomGroup(const MMS_GROUP& group)=0;
	virtual void OnAddPersonalRoomGroupEnd(const std::string& strRoomID)=0;
	
	/*��������Ա���˳�*/
	virtual void OnStopPersonalRoom(const std::string& strRoomID)=0;

	/*��ȡָ�������ҵ�һ�����߳�Ա*/
	virtual void OnGetAppointRoomOnlineMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	/*��ȡָ�������ҵ�һ�����ߺϳ��ն˻�������ͨ����Ϣ*/
	virtual void OnGetAppointRoomOnlineMemberInfo(const std::string& strDomain,const std::string& strRoomID,std::string& strMemberID,unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
		std::string& strMCUID,std::string& strMCUAddr,unsigned short usMCUPort,
		std::string& strNodeID,std::string& strNatAddr,std::string& strLocalAddr,unsigned short usLocalPort,const std::string& strTag="")=0;

	/*֪ͨ���˻��鷢���˻����Ѿ���ʼ(��һ����Ա�Ѿ���¼����)*/
	virtual void OnNotifyPersonalAdminMeetingStart(const std::string& strRoomID)=0;

	/************************************************************************/
	/* ��ϵ�˷���                                                           */
	/************************************************************************/
	virtual void OnGetContactGroup(const std::string& strParentGroupID,const std::string& strGroupID,const std::string& strGroupName,const unsigned int nLevelID, const unsigned int nOrderID)=0;
	virtual void OnGetContactGroupEnd()=0;

	virtual void OnGetContactGroupMember(const std::string& strGroupID,const IMS_USER_ITEM_BASE& item)=0;
	virtual void OnGetContactGroupMemberEnd(const std::string& strGroupID)=0;

	virtual void OnContactGroupMemberStatus(const std::string& strMemberID,const std::string& strStatus)=0;

    //�ƶ����
	//��ȡ���ƶ�Ƶ���б�
    virtual void OnGetMobileChannelList(const std::string& strDomain,const MMS_ROOM_LST& lstChnl)=0;
    //���������û��Կ���Ȩ������
    virtual void OnApplyMobileChannelControl(const std::string& strUserID,const std::string& strRoomID)=0;
    //�յ��������Ȩ�Ĵ�����
    virtual void OnHandleApplyMobileChannelControl(const std::string& strAdminID,const std::string& strRoomID,bool bIsSuccess)=0;
    //�ͷſ���Ȩ
    virtual void OnReleaseMobileChannelControl(const std::string& strRoomID)=0;

	/****************************************************************
	 * ������
	 ****************************************************************/
	virtual void OnGetRoomRemark(const std::string& strRoomID,const std::string& strRemark)=0;

   	//�㲥����
	virtual void OnBroadCastMicSeq(MMS_SEQ_MEMBER_LST& lstMember,unsigned long ulServerTime)=0;
	
	//�㲥��������
	virtual void OnBroadCastMicSeqCfg(unsigned short usType,unsigned int nDuration,unsigned int nMaxNum,unsigned int nSpUserNum,std::string &strTag)=0;
	
    //���Ӷ���ʱ��
    virtual void OnSetExTime(std::string& strMemberID,unsigned int nTime)=0;
    
    //����б�
    virtual void OnClearMicSeq()=0;

    //�б�����
    virtual void OnMicSeqFull()=0;

	/* ��ȡδ�������ն��б�ص� */
	virtual void OnGetNoInRoomDevList(const NOINROOMDEV& dev)=0;

	/* ��ȡδ����������ն��б�ص����� 
	 * nCount ����ȡ��������*/
	virtual void OnGetNoInRoomDevListEnd(int nCount)=0;

	//��ȡ���ڷ�����IP
	virtual void OnGetSerialServerIP(const std::string& strDevID,const std::string& strSerialServerIP)=0;

    virtual void OnGetMultiplyRoom(const std::string& strRoomID,const std::string& strPasswd,const unsigned int nMultiplyLimit,const unsigned int nErrCode)=0;

	//��ȡ������ԱȨ������
	virtual void OnGetRoomMemberType(const std::string& strRoomID,const std::string& strMerberID,const unsigned short& usMemberType)=0;
	//����֤ȯͶƱ�����sip�����������
	/************************************************************************/
	/* ����˵����
	ulVoteID		�˴�ͶƱid
	strTopic		�˴�ͶƱ����
	strUserID		��ѡƱ��ͶƱ��id
	strUserName	��ѡƱ��ͶƱ������
	itemid		��ѡƱ��ѡ��id
	itemnam		��ѡƱ��ѡ������                                            */
	/************************************************************************/
	virtual void OnGetVoteDetail(unsigned long ulVoteID, const std::string& strTopic, const std::string& strUserID,const std::string& strUserName, unsigned int itemid, const std::string& itemname)=0;
	virtual void OnGetVoteDetailEnd(unsigned long ulVoteID)=0;
	/************************************************************************/
	/* ����˵����
	strCallID		�ն˺�������������id
	strUserID		�ն˺�������������Աid
	strUserName		�ն˺�������������Ա����
	strMcuID		mpserver id
	strRoomID		�ն��������Ļ���id
	                                            */
	/************************************************************************/
	virtual void OnRequestSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID)=0;
    virtual void OnSipQuitRequest(const std::string& strCallID, const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID)=0;

	/************************************************************************/
	/* ����˵����
	strCallID		�ն˺�������������id
	strUserID		�ն˺�������������Աid
	strUserName		�ն˺�������������Ա����
	strMcuID		mpserver id
	strRoomID		�ն��������Ļ���id
	bAccept			�����������Ƿ�ͬ��sip�ն����=trueͬ�⣬=false��ͬ��
	                                            */
	/************************************************************************/
	virtual void OnResponseForOtherPresider(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID, bool bAccept)=0;
	//�յ�������Ա�仯(�������б�ʹ��)
	virtual void OnRoomMemberStatusChange(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMerberID,const unsigned short& usStatus,const unsigned short& usMemberType,const unsigned int& nOnlineNum, unsigned int& nTotalNum)=0;
	//�յ����ֹ��ܻ���������
	virtual void OnRoomDigitalCode(const std::string& strRoomID,const unsigned int& unDigitalCode)=0;

	virtual void OnSetRoomMinCodestream(unsigned short usErr, const std::string& strRoomID,double dRoomMinCodstream)=0;
	//webͨ��
	virtual void OnRoomMaxCodestream(const std::string& strRoomID,double dRoomCodstream)=0;
	//�û����������Ϣ
	virtual void OnRoomMemberInfo(const MEMBER_DEFINED_INFO& memberdefinfo)=0;
	virtual void OnRoomMemberLstInfo(const MEMBER_DEFINED_INFO_LST& lstmemberdefinfo)=0;
	//֪ͨ�ϳɰװ�/�ĵ��������ڴ��ڵ���
	virtual void OnRoomDigitalComp(const _MMS_CHLTYPE& channelType)=0;

	/***************************************************
	//�������ܣ���ʼ��ѭ�ص�
	//������
	//		strRoundRobinGroupID	��ѭ����id
	//		nTMWndCnt				������
	//		nPrerecvCnt				���Ԥ��·��
	//		strReceiverlist			������userid�б�(�ԡ�;���������磺cs0001;cs0002)
	//		bRecvAudio				�Ƿ����Ƶ
	//		uErr					�����루ֵ:ERR_MMS_RRGOUPRUNNING��ERR_MMS_STARTRRFAILED��
	//����ֵ����
	***************************************************/
	virtual void OnStartRoundRobin (const std::string& strRoundRobinGroupID, const int& nTMWndCnt, const int& nPrerecvCnt, const std::string& strReceiverlist, const bool bRecvAudio, const unsigned int& uErr) = 0;

	/***************************************************
	//�������ܣ�ֹͣ��ѭ�ص�
	//������
	//		strRoundRobinGroupID	��ѭ����id
	///		uErr					�����루ֵ��ERR_MMS_STOPRRFAILED��
	//����ֵ����
	***************************************************/
	virtual void OnStopRoundRobin(const std::string& strRoundRobinGroupID, const unsigned int& uErr) = 0;

	/***************************************************
	//�������ܣ����õ�ǰԤ����Ϣ�ص�
	//������
	//		strPreRecvData			��ǰԤ����Ϣ
	//����ֵ����
	***************************************************/
	virtual void OnSetRRCurPreRecvAV(const std::string& strPreRecvData) = 0;

	/***************************************************
	//�������ܣ����õ�ǰ��ʾ��Ϣ�ص�
	//������
	//		strViewShowData			��ǰ��ʾ��Ϣ
	//����ֵ����
	***************************************************/
	virtual void OnSetRRCurViewShowAV(const std::string& strViewShowData) = 0;

	//ת��ص�
	virtual void OnRevNewTransCode(const std::string& strTRSMCUID,const std::string& strTRSMCUAddr,unsigned short usTRSMCUPort,const std::string& strTRSNodeID,const std::string& strTRSNatAddr,unsigned short usTRSNATPort,const std::string& strLocalAddr,unsigned short usLocalPort,unsigned long ulOldVideID,unsigned long ulNewAudio,unsigned long ulNewVideID)=0;

	/***************************************************
	//�������ܣ�����ȫ����Ϣ�ص�
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		strChannelID		ͨ��ID
	//		bFull				true:����ȫ����false��ȡ��ȫ��
	//		uErr				������
	//����ֵ����
	***************************************************/
	virtual void OnSetFullScreen(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const std::string& strChannelID, bool bFull, const unsigned int& uErr) = 0;
	
	/***************************************************
	//�������ܣ�ģ��Ԥ�ûص�
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		uNumber				ģ���
	//		uBandwidth			ģ�����
	//		uWidth				ģ��ֱ��ʿ�
	//		uHeight				ģ��ֱ��ʸ�
	//		strPara				��������
	//		uErr				������
	//����ֵ����
	***************************************************/
	virtual void OnTemplatePreset(const std::string& strRoomID, const std::string& strGroupID, const unsigned int uNumber, const unsigned int uBandwidth, const unsigned int uWidth, const unsigned int uHeight, const std::string& strPara, const unsigned int& uErr) = 0;

	/***************************************************
	//�������ܣ�����Ԥ��ص�
	//������
	//		WndPreSet			��ο��ṹ�嶨��
	//		uErr				������
	//����ֵ����
	***************************************************/
	virtual void OnWindowPreset(const MMS_WNDPRESET& WndPreSet, const unsigned int& uErr) = 0;

	/***************************************************
	//�������ܣ��ı�ͨ��������Ƶ��Ϣ�ص�
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uChannelID			ͨ��ID
	//		uStream				����
	//		uWidth				ģ��ֱ��ʿ�
	//		uHeight				ģ��ֱ��ʸ�
	//		bLock				�Ƿ�ɼ�������true��������֮��
	//		uErr				������
	//����ֵ����
	***************************************************/
	virtual void OnChangeAudioVedioInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const unsigned int uStream, const unsigned int uWidth, const unsigned int uHeight, const unsigned int& uErr) = 0;

	/***************************************************
	//�������ܣ��ɼ������ص�
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uChannelID			ͨ��ID
	//		bLock				�Ƿ�ɼ�������true��������֮��
	//		uErr				������
	//����ֵ����
	***************************************************/
	virtual void OnCollectionLock(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const bool bLock, const unsigned int& uErr ) = 0;

	/***************************************************
	//�������ܣ�����ͻ��˵ı������ݻص�
	//������
	//		strRoomID			����ID
	//		strGroupID			��ID
	//		strMemberID			��ԱID
	//		uCardIndex
	//		uHeiStream
	//		uHWidth
	//		uHHeight
	//		uMidStream
	//		uMWidth
	//		uMHeight
	//		uLowStream
	//		uLWidth
	//		uLHeight
	//����ֵ����
	***************************************************/
	virtual void OnBitStreamCalced(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uCardIndex, const unsigned int uHeiStream, const unsigned int uHWidth, const unsigned int uHHeight, const unsigned int uMidStream, const unsigned int uMWidth, const unsigned int uMHeight, const unsigned int uLowStream, const unsigned int uLWidth, const unsigned int uLHeight) = 0;
};

//=======================================================================================
// MMSʵ��
//=======================================================================================
AFX_EXT_CLASS bool MMSSessionAddEvent(KMMSSessionEvent* pMMSSessionEvent);
AFX_EXT_CLASS void MMSSessionRemoveEvent(KMMSSessionEvent* pMMSSessionEvent);
AFX_EXT_CLASS void MMSSessionRelease(void);
AFX_EXT_CLASS KMMSSession& GetMMSSession(void);


#include "KMONDefine.h"

class AFX_EXT_CLASS KMONSessionEvent;
//=======================================================================================
// MMSģ����
//=======================================================================================
class AFX_EXT_CLASS KMONSession
{
public:
	KMONSession(void){};
	virtual ~KMONSession(void){};

	virtual void OnDispatchCmd(KCmdPacket& t)=0;
	virtual void ClearLocalData(void)=0;

	/* ���ͳ�ʼ��¼���� */
	virtual void SendLoginCommand(void)=0;

	/* ��¼
	 * nTermType(�ն�����):0-����ն� 1-Ӳ���ն� 2-�ƶ��ն�
	 * nTermName(�ն�����):�Զ���
	 * strClientType(�ͻ�������): "SYS" "MON" "OCX" "MOB" *
     * strVersion(ϵͳ/����汾��): �� "<version> <system>XP</system> <software>6.2.3.7.2292</software> </version>" */
	virtual void Login(const std::string& strUserID,const std::string& strPassword, MON_DEVTYPE eDevType,const std::string& strName,const std::string& strSerialno, unsigned short usTermType=0,const std::string& strTermName="", const std::string& strVersion ="", const std::string& strMGSID = "")=0;

	/* ע�� */
	virtual void Logout(void)=0;

	/* ���Ͷ��� */
	virtual void SendSMS(const std::string& strMobilNumber,const std::string& strMessage)=0;

	/* �豸���� 
	cstrBindMemberID���ڻ����ն�,����Ϊ��
	usFactory����MON_FACTORY*/
	virtual void DeviceOnline(const MON_DEVTYPE& eDevType, const std::string& strDevID,const std::string& strPwd,const std::string& strMask,
		const std::string& strName, unsigned short usFactory = 0, const std::string& strVersion = "",const std::string& strBindMemberID = "",
		const std::string& strMGSID = "")=0;

	/* �豸���� */
	virtual void DeviceOffline(const std::string& strDevID,const std::string& strBindMemberID = "")=0;

	/* ��֤�豸���ʺź����кŵİ󶨹�ϵ
	 * strDevID: Ϊ�����豸�ʺ�
	 * strNewDevID: Ϊ�µ��豸�ʺ�
	 * strSerialNo: Ϊ�豸�����к� */
	virtual void DeviceCheck(const std::string& strDevID,const std::string& strNewDevID,const std::string& strSerialNo)=0;

	/* �޸��豸���� */
	virtual void UpdateDeviceName(const std::string& strDevID,const std::string& strName)=0;

	/* ����DVRͨ��������Ϣ */
	virtual void ChannelOnline(const MON_CHANNEL_INFO& tMonChannelInfo)=0;

	/* ����DVRͨ��ע������ nCHNum=-1 ����ͨ��ע�� */
	virtual void ChannelOffline(const std::string& strDevID,unsigned short nCHNum)=0; 

	/* �޸�ͨ������ */
	virtual void UpdateChannelName(const std::string& strChannelID,const std::string& strName)=0;
	
	/* ��ȡ������Ϣ */
	virtual void GetDeviceBasicInfo(const std::string& strDevID)=0;

	/* ���ڸ���(�ͻ���ÿ��10���ӷ���һ������,
	 * strXMLData: �����豸CPU/MEM����Ϣ��ʽ�ַ���,
	 * strSwitchSkedTime: ��ǰ������ϴλ�ȡ�ػ��ƻ���ʱ��,
	 * �첽����: �ػ��ƻ�OnDeviceClick */
	virtual void DeviceClick(const std::string& strXMLData,const std::string& strSwitchSkedTime)=0;

	/* ����Խ� */
	virtual void A2AInvite(const std::string &strCALLID,const MON_CHANNEL_INFO &tMonChannelInfo,const std::string &strSDPData)=0;

	/* ��Ӧ�Խ� */
	virtual void A2ARespond(const std::string &strCALLID,const MON_CHANNEL_INFO &tMonChannelInfo,const std::string &strSDPData)=0;

	/* �ж϶Խ� */
	virtual void A2ABye(const std::string &strCALLID)=0;

	/* ���� */
	virtual void Alert(const std::string &strDevID,unsigned short nCHNum,unsigned short nLevel,unsigned short nsubLevel=0)=0;
	
	/* ��̨���� */
	virtual int YTControl(const std::string &strDevID,unsigned short nCHNum,unsigned short nYTCmd, unsigned short nSpeed, unsigned short nPresetNO,const std::string &strPresetName )=0;// ��̨����

	/* ������ȡ��̨���Ʋ������� */
	virtual void GetYTControlParameters(const std::string &strDevID, unsigned short nCHNum)=0;

	/* ������ȡ��̨���Ӳ������� */
	virtual void GetYTConnectionParameters(const std::string &strDevID, unsigned short nCHNum)=0;

	/* ������̨���Ʋ��� */
	virtual void SetYTControlParameters(const std::string &strDevID,const std::string &strToID, const YT_CONTROL_PARAMETERS & YTParam)=0;

	/* ������̨���Ӳ��� */
	virtual void SetYTConnectionParameters(const std::string &strDevID,const std::string &strToID, unsigned short nChannelNum, const std::string &strType, unsigned short nCom,  unsigned short  nSerialNo)=0;

	/* ����P2P */
	virtual void P2P(const std::string& strUserID, KCmdUserDataPacket& packet)=0;

	/* ��ȡ¼���������ַ */
	virtual void GetVRSServer(void)=0;

	/* ���ڸ���(�ͻ���ÿ��10���ӷ���һ������,
	* strXMLData: �����豸CPU/Disk����Ϣ��ʽ�ַ���,
	* strSwitchSkedTime: ��ǰ������ϴλ�ȡ�ƻ���ʱ��,
	* strSwitchStrategyTime: ��ǰ������ϴλ�ȡ���Ե�ʱ��,
	* �첽����: NVR¼���ϴ��ļƻ�OnNVRSchedule��NVR¼�����OnNVRStrategy
	*/
	virtual void MGSStatusReport(const std::string& strXMLData,const std::string& strSwitchSkedTime,const std::string& strSwitchStrategyTime)=0;


	/* �ϱ�ͨ��¼���ϴ�״̬��ÿ������״̬�ı�ʱ����
	* strDevID: �豸ID��
	* nCHNum��ͨ��ID��
	* nRecordStatus��¼��״̬
	* nUploadStatus���ϴ�״̬
	*/
	virtual void ChannelStatusReport(const std::string &strDevID,unsigned short nCHNum, unsigned short nRecordStatus, unsigned short nUploadStatus)=0;

	/* �豸ͨ������ƵID���� */
	virtual void DevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;

   	//¼��ƻ�MTS_R��װ��MONģ�� ������Ҫ����userid(���������ӿ���IMSession�е���)
	/* ��ȡ���¼��ƻ� */
	virtual void GetMONRecTask(const std::string& strUpdatetime, const std::string& strUserID) = 0;

	/* ��ȡ���ͨ��¼��ƻ� */
	virtual void GetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime, const std::string& strUserID) = 0;

	/* ��ȡ���¼�����(����:¼��ķָ�����,��С) */
	virtual void GetMONRecStrategy(const std::string& strUpdatetime, const std::string& strUserID) = 0;
	
	/* ��ȡ�豸��Ϣ */
	virtual void GetMonDevInfo(const std::string& strDevID) = 0;
};

//=======================================================================================
// MONģ���¼���
//=======================================================================================
class KMONSessionEvent
{
public:
	KMONSessionEvent(void){};
	virtual ~KMONSessionEvent(void){};
	
public:
	/* ��¼������nErrCode: 
	 * 0=��¼�ɹ�
	 * 1=�û��˺Ų�����
	 * 2=�û�������Ч
	 * 3=�û��ѵ�¼
	 * 99=ϵͳ���� */
	virtual void OnLogin(int nErrCode)=0;

	/* ע��������nErrCode: 
	 * 0=��¼�ɹ�
	 * 1=ϵͳ�Ͽ�
	 * 99=ϵͳ���� */
	virtual void OnLogout(int nErrCode,const std::string& strDomain)=0;

	/* �豸����
	* nErrCode=0 ���߳ɹ�
	* nErrCode!=0����ʧ��
	*/
	virtual void OnDeviceOnline(int nErrCode,KCmdPacket& rCmdPacket)=0;

	/* �豸���� */
	virtual void OnDeviceOffline(KCmdPacket& rCmdPacket)=0;

	/* �豸��� */
	virtual void OnDeviceCheck(KCmdPacket& rCmdPacket)=0;

	/* �յ�ͨ������ (DEVID,CHNUM,ERRCODE) */
	virtual void OnChannelOnline(KCmdPacket& rCmdPacket)=0;

	/* �յ�ͨ������ */
	virtual void OnChannelOffline(KCmdPacket& rCmdPacket)=0;

	/* ��ȡ������Ϣ */
	virtual void OnDeviceBasicInfo(KCmdPacket& rCmdPacket)=0;

	/* �յ��ػ��ƻ�
	 * GetAttrib("XMLDAT"): �ػ��ƻ�,
	 * GetAttrib("SSTIME"): ���λ�ȡ�Ĺػ��ƻ�ʱ�� */
	virtual void OnDeviceClick(KCmdPacket& rCmdPacket)=0;

	/* ���� */
	virtual void OnAlert(KCmdPacket& rCmdPacket)=0;

	/* ���� */
	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;

	/* ����Խ� */
	virtual void OnA2A(KCmdPacket& rCmdPacket)=0;

	/* ����Խ� */
	virtual void OnA2ABye(KCmdPacket& rCmdPacket)=0;

	/* ������̨�������� */
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;

	/* ��ȡ��̨���Ӳ������� */
	virtual void OnGetYTConnectionParameters(KCmdPacket& rCmdPacket)=0;

	/* ��ȡ��̨���Ʋ������� */
	virtual void OnGetYTControlParameters(KCmdPacket& rCmdPacket)=0;

	/* ������̨���Ӳ��� */
	virtual void OnSetYTConnectionParameters(KCmdPacket& rCmdPacket)=0;

	/* ������̨���Ʋ��� */
	virtual void OnSetYTControlParameters(KCmdPacket& rCmdPacket)=0;

	/* ���ػ���P2P */
	virtual void OnP2P(const std::string& strUserID,KCmdUserDataPacket& packet)=0;

	/* ���VRS������VRSID,VRSADDR */
	virtual void OnVRSServer(KCmdPacket& rCmdPacket)=0;

	/* �յ�NVR¼�����
	* GetAttrib("XMLDAT_STRATEGY"): ¼�����,
	* GetAttrib("SSTIME"): ���λ�ȡ�ƻ���ʱ�� 
	*/
	virtual void OnNVRStrategy(KCmdPacket& rCmdPacket)=0;


	/* �յ�NVR¼���ϴ��ļƻ�
	* GetAttrib("DEVID"): �豸ID��
	* GetAttrib("CHANNELNUM:")��ͨ��ID��
	* GetAttrib("XMLDAT_RECORD"): ¼��ƻ�,
	* GetAttrib("XMLDAT_UPLOAD"): �ϴ��ƻ�,
	* GetAttrib("SSTIME"): ���λ�ȡ�ƻ���ʱ��
	*/
	virtual void OnNVRSchedule(KCmdPacket& rCmdPacket)=0;
	/* ��ȡ�豸��Ϣ */
	virtual void OnGetMonDevInfo(const MONDEV& item,const std::string& strDomain)=0;
};

//=======================================================================================
// MONʵ��
//=======================================================================================
AFX_EXT_CLASS bool MONSessionAddEvent(KMONSessionEvent* pMONSessionEvent);
AFX_EXT_CLASS void MONSessionRemoveEvent(KMONSessionEvent* pMONSessionEvent);
AFX_EXT_CLASS void MONSessionRelease(void);
AFX_EXT_CLASS KMONSession& GetMONSession(void);

//=======================================================================================
// AVCģ����
//=======================================================================================
class AFX_EXT_CLASS KAVCSession
{
public:
	KAVCSession(void){};
	virtual ~KAVCSession(void){};

	enum{CALLINGOUT,CALLINGIN,TALKING};

public:
	//-------------------------------------------
	// ����¼�
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual AVC_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PAVC_CALL_ITEM GetCallItem(const std::string& toid)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// AVCģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KAVCSessionEvent
{
public:
	KAVCSessionEvent(void){};
	virtual ~KAVCSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// AVCʵ��
//=======================================================================================
AFX_EXT_CLASS bool AVCSessionAddEvent(KAVCSessionEvent* pAVCSessionEvent);
AFX_EXT_CLASS void AVCSessionRemoveEvent(KAVCSessionEvent* pAVCSessionEvent);
AFX_EXT_CLASS void AVCSessionRelease(void);
AFX_EXT_CLASS KAVCSession& GetAVCSession(void);

//=======================================================================================
// WHBģ����
//=======================================================================================
class AFX_EXT_CLASS KWHBSession
{
public:
	KWHBSession(void){};
	virtual ~KWHBSession(void){};
	
	/* ����SESSION */
	virtual void Close()=0;	

	enum{CALLINGOUT,CALLINGIN,TALKING};

	//-------------------------------------------
	// ����¼�
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual WHB_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PWHB_CALL_ITEM GetCallItem(const std::string& strNodeID)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// WHBģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KWHBSessionEvent
{
public:
	KWHBSessionEvent(void){};
	virtual ~KWHBSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// WHBʵ��
//=======================================================================================
AFX_EXT_CLASS bool WHBSessionAddEvent(KWHBSessionEvent* pWHBSessionEvent);
AFX_EXT_CLASS void WHBSessionRemoveEvent(KWHBSessionEvent* pWHBSessionEvent);
AFX_EXT_CLASS void WHBSessionRelease(void);
AFX_EXT_CLASS KWHBSession& GetWHBSession(void);

//=======================================================================================
// SVRģ����
//=======================================================================================
class AFX_EXT_CLASS KSRVSession
{
public:
	KSRVSession(void){};
	virtual ~KSRVSession(void){};
	
	/* ����SESSION */
	virtual void Close()=0;

	enum{CALLINGOUT,CALLINGIN,TALKING};

public:
	//-------------------------------------------
	// ����¼�
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual SRV_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PSRV_CALL_ITEM GetCallItem(const std::string& toid)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// SVRģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KSRVSessionEvent
{
public:
	KSRVSessionEvent(void){};
	virtual ~KSRVSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// SRVʵ��
//=======================================================================================
AFX_EXT_CLASS bool SRVSessionAddEvent(KSRVSessionEvent* pSRVSessionEvent);
AFX_EXT_CLASS void SRVSessionRemoveEvent(KSRVSessionEvent* pSRVSessionEvent);
AFX_EXT_CLASS void SRVSessionRelease(void);
AFX_EXT_CLASS KSRVSession& GetSRVSession(void);

//=======================================================================================
// FILģ����
//=======================================================================================
class AFX_EXT_CLASS KFILSession
{
public:
	KFILSession(void){};
	virtual ~KFILSession(void){};
	
	/* ����SESSION */
	virtual void Close()=0;

public:
	//-------------------------------------------
	// ����¼�
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual std::string GetUserID(void)=0;

	virtual void Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strFileName,unsigned long ulFileSize)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strFileName)=0;
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// FILģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KFILSessionEvent
{
public:
	KFILSessionEvent(void){};
	virtual ~KFILSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strFileName,unsigned long ulFileSize)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strFileName)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// FILʵ��
//=======================================================================================
AFX_EXT_CLASS bool FILSessionAddEvent(KFILSessionEvent* pFILSessionEvent);
AFX_EXT_CLASS void FILSessionRemoveEvent(KFILSessionEvent* pFILSessionEvent);
AFX_EXT_CLASS void FILSessionRelease(void);
AFX_EXT_CLASS KFILSession& GetFILSession(void);

//=======================================================================================
// SMSģ����
//=======================================================================================
class AFX_EXT_CLASS KSMSSession
{
public:
	KSMSSession(void){};
	virtual ~KSMSSession(void){};

	/* ����SESSION */
	virtual void Close()=0;

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* ���͸��˶���
	 * strReceiverCode: �������ֻ�����,֧�ֶ������,�÷ֺ�(;)���� */
	virtual void Send(const std::string& strReceiverCode,const std::string& strContent)=0;

	/* ������ҵ����
	 * strOrgID:        ����ID
	 * strDepartID;		����ID,�����û������������û�������strDepartID��Ϊ�� */
	virtual void Send(const std::string& strOrgID,const std::string& strDepartID,const std::string& strContent)=0;
};

//=======================================================================================
// SMSģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KSMSSessionEvent
{
public:
	KSMSSessionEvent(void){};
	virtual ~KSMSSessionEvent(void){};

public:
	/* ���ŷ��ͺ�ķ�����Ϣ
	 * nType: 0=���˶��� 1=��ҵ���� */
	virtual void OnSend(unsigned short nType,bool bSuccess,const std::string& strErrText,const std::string& strRecvCode,const std::string& strContent)=0;

	/* �յ�����
	 * nType: 0=���˶��� 1=��ҵ���� */
	virtual void OnReceive(unsigned short nType,const std::string& strUserID,const std::string& strSenderCode,const std::string& strContent,const std::string& strRecvTime)=0;
};

//=======================================================================================
// SMSʵ��
//=======================================================================================
AFX_EXT_CLASS bool SMSSessionAddEvent(KSMSSessionEvent* pSMSSessionEvent);
AFX_EXT_CLASS void SMSSessionRemoveEvent(KSMSSessionEvent* pSMSSessionEvent);
AFX_EXT_CLASS void SMSSessionRelease(void);
AFX_EXT_CLASS KSMSSession& GetSMSSession(void);

//=======================================================================================
// NDSģ����
//=======================================================================================
#include "KNDSDefine.h"
class AFX_EXT_CLASS KNDSSession
{
public:
	KNDSSession(void){};
	virtual ~KNDSSession(void){};

	/* ����SESSION */
	virtual void Close()=0;

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

    /* ��ȡȫ�����ò��� */
    virtual void GetGlobalParam()=0;

    /* ��ȡ���û���NDS�ļ��б������� */
    virtual void GetNDSFileInfo() = 0;

    /* �ϴ��ļ�����ʱ�� fileid�����ɣ��ļ��Ѵ��ڷ���״̬ */
    virtual void UploadFile( PTNDSFileInfo ptNDSFileInfo) = 0;

    /* �����ļ� */
    virtual void DownloadFile( PTNDSFileInfo ptNDSFileInfo, const std::string& strFilePath) = 0;

    /* ֹͣ�ϴ��������ݿ�����޹أ�ֻ���ļ� fileid ���� */
    virtual void StopUploadFile( unsigned long ulFileID/*, const std::string& strMCUNodeID*/) =0;

    /* ֹͣ���� */
    virtual void StopDownloadFile( unsigned long ulFileID/*, const std::string& strMCUNodeID*/) =0;

    /* ȡ���ϴ���Ҫ�������ݿ����� */
    virtual void CancelUploadFile( PTNDSFileInfo ptNDSFileInfo/*, unsigned long ulNETECFileID, const std::string& strMCUNodeID*/) = 0;

    /* ȡ������ */
    virtual void CancelDownloadFile( PTNDSFileInfo ptNDSFileInfo/*, unsigned long ulNETECFileID = 0, const std::string& strMCUNodeID = ""*/) = 0;


    //////////////////////////////////////////////////////////////////////////
    // ���²μ� NETEC_File.h
    virtual unsigned long GetFileTransferredBytes( unsigned long ulFileID, bool bUpload )=0;
    virtual int GetBitrate( unsigned long ulFileID, bool bUpload )=0;
    virtual int GetPercent( unsigned long ulFileID, bool bUpload )=0;
};

//=======================================================================================
// NDSģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KNDSSessionEvent
{
public:
	KNDSSessionEvent(void){};
	virtual ~KNDSSessionEvent(void){};

public:
    /* ����ȫ�����ò���(����ļ���С,��λ: M) */
    virtual void OnGlobalParam( unsigned long ulMaxFileSize) = 0;

    /* ���������ļ��б� */
    virtual void OnNDSFile( PTNDSFileInfo ptNDSFileInfo) = 0;

    /* ���������ļ��б���� */
    virtual void OnNDSFileEnd() = 0;

    /* �ϴ���ʼ */
	virtual void OnUploadFileStart(PTNDSFileInfo ptNDSFileInfo ) = 0;

    /* �ϴ���� */
    virtual void OnUploadFileEnd( unsigned long ulFileID )=0;

    /* ����������Ϣ���� */
    virtual void OnDownloadFileStart( PTNDSFileInfo ptNDSFileInfo )=0;

    /* ����������Ϣ���� */
    virtual void OnDownloadFileEnd( unsigned long ulFileID )=0;

    /* �ϴ����� */
    virtual void OnUploadError( PTNDSFileInfo ptNDSFileInfo, int nErrCode)=0;

    /* ���ش��� */
    virtual void OnDownloadError( PTNDSFileInfo ptNDSFileInfo, int nErrCode)=0;
};

//=======================================================================================
// NDSʵ��
//=======================================================================================
AFX_EXT_CLASS bool NDSSessionAddEvent(KNDSSessionEvent* pNDSSessionEvent);
AFX_EXT_CLASS void NDSSessionRemoveEvent(KNDSSessionEvent* pNDSSessionEvent);
AFX_EXT_CLASS void NDSSessionRelease(void);
AFX_EXT_CLASS KNDSSession& GetNDSSession(void);

//=======================================================================================
// FAXģ����
//=======================================================================================
class AFX_EXT_CLASS KFAXSession
{
public:
	KFAXSession(void){};
	virtual ~KFAXSession(void){};

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* ��ȡ�����������Ϣ */
	virtual void GetServer(void)=0;
	virtual void GetFax(void)=0;

	/*���ʹ��濪ʼ*/
	virtual void SendFaxStart(const std::string& strNodeID, unsigned long ulFileID,unsigned long ulFileLength,const std::string& strMCUID,const std::string& strMCUIP,unsigned short usMCUPort,const std::string& strMCUType,const std::string& strFaxNum)=0;
	/*���ʹ������*/
	virtual void SendFaxStop(const std::string& strNodeID, unsigned long nFileID)=0;
	/*���մ��濪ʼ*/
	virtual void RecvFaxStart(const std::string& strNodeID, unsigned long ulFaxID)=0;
	/*���մ������*/
	virtual void RecvFaxStop(const std::string& strNodeID, unsigned long ulFaxID, unsigned long nFileID)=0;
};

//=======================================================================================
// FAXģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KFAXSessionEvent
{
public:
	KFAXSessionEvent(void){};
	virtual ~KFAXSessionEvent(void){};

public:
	/* ��ȡ�����������Ϣ */
	virtual void OnServer(const std::string& strNodeID)=0;

	/* ������ʾ */
	virtual void OnFax(const std::string& strNodeID, unsigned long ulFaxID)=0;
	virtual void OnFaxEnd()=0;
	/*���մ��濪ʼ*/
	virtual void OnRecvFaxStart(const std::string& strNodeID, unsigned long ulFaxID, unsigned long ulFileID,unsigned long ulFileLength,const std::string& strMCUID,const std::string& strMCUIP,unsigned short usMCUPort,const std::string& strMCUType)=0;
};

//=======================================================================================
// FAXʵ��
//=======================================================================================
AFX_EXT_CLASS bool FAXSessionAddEvent(KFAXSessionEvent* pFAXSessionEvent);
AFX_EXT_CLASS void FAXSessionRemoveEvent(KFAXSessionEvent* pFAXSessionEvent);
AFX_EXT_CLASS void FAXSessionRelease(void);
AFX_EXT_CLASS KFAXSession& GetFAXSession(void);

//=======================================================================================
// �����ļ�ģ����
//=======================================================================================
#include "KNFSDefine.h"

class AFX_EXT_CLASS KNFSSession
{
public:
	KNFSSession(void);
	virtual ~KNFSSession(void);

	static bool Start(void);
	static void Stop(void);
};

//---------------------------------------------------------
// KNFSUploadSessionEvent �¼��ӿ���
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSUploadSessionEvent
{
public:
	KNFSUploadSessionEvent(void){};
	virtual ~KNFSUploadSessionEvent(void){};
	
	/* ���ӳɹ�  */
	virtual void OnConnected(void)=0;

	/* ���ӶϿ�  */
	virtual void OnDisconnected(int nErrCode)=0;

	/* �ϴ����� */
	virtual void OnUploadError(int nErrCode)=0;

	/* �ϴ����� */
	virtual void OnUploadProgress(double dPercent,double dSpeed)=0;

	/* �ϴ����� */
	virtual void OnUploadEnd(void)=0;

	/* ɾ�� */
	virtual void OnDelete(int nErrCode)=0;
};

//---------------------------------------------------------
// KNFSUploadSession �ӿ���
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSUploadSession
{
public:
	KNFSUploadSession(void){};
	virtual ~KNFSUploadSession(void){};

	/* ���ӷ����� */
	virtual bool Connect(const std::string& strServerAddr,unsigned short usServerPort)=0;

	/* �Ͽ����������� */
	virtual void Disconnect(void)=0;

	/* �ϴ��ļ�:
	 * strLocalFileFullName:  �����ϴ��ļ�ȫ·����(�����ļ���)
	 * strUploadFilePath:     �ϴ��ļ�·��(���������·��):/emaps/part01
	 * strUploadFileName:     �ϴ��ļ�����: map0001.jpg
     * ˵��:���浽�������ϵ��ļ�λ��: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * ����: 0=�ɹ�ִ�� */
	virtual int Upload(const std::string& strLocalFileFullName,const std::string& strUploadFilePath,const std::string& strUploadFileName)=0;

	/* ɾ���ļ�
	 * strUploadFilePath:     �ϴ��ļ�·��(���������·��):/emaps/part01
	 * strUploadFileName:     �ϴ��ļ�����: map0001.jpg
     * ˵��:���浽�������ϵ��ļ�λ��: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * ����: 0=�ɹ�ִ�� */
	virtual int Delete(const std::string& strDeleteFilePath,const std::string& strDeleteFileName)=0;
};

//---------------------------------------------------------
// KNFSDownloadSessionEvent �¼��ӿ���
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSDownloadSessionEvent
{
public:
	KNFSDownloadSessionEvent(void){};
	virtual ~KNFSDownloadSessionEvent(void){};
	
	/* ���ӳɹ�  */
	virtual void OnConnected(void)=0;

	/* ���ӶϿ�  */
	virtual void OnDisconnected(int nErrCode)=0;

	/* ���ؿ�ʼ */
	virtual void OnDownloadStart(unsigned long ulFileSize)=0;

	/* ���ش��� */
	virtual void OnDownloadError(int nErrCode)=0;

	/* ���ؽ��� */
	virtual void OnDownloadProgress(double dPercent,double dSpeed)=0;

	/* ���ؽ��� */
	virtual void OnDownloadEnd(void)=0;

	/* ɾ�� */
	virtual void OnDelete(int nErrCode)=0;
};

//---------------------------------------------------------
// KNFSDownloadSession �ӿ���
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSDownloadSession
{
public:
	KNFSDownloadSession(void){};
	virtual ~KNFSDownloadSession(void){};

	/* ���ӷ����� */
	virtual bool Connect(const std::string& strServerAddr,unsigned short usServerPort)=0;

	/* �Ͽ����������� */
	virtual void Disconnect(void)=0;

	/* �ϴ��ļ�:
	 * strLocalFileFullName: �����ļ�ȫ·��
	 * strDownloadFilePath:  �����ļ�·��(���������·��):/emaps/part01
	 * strDownloadFileName:  �����ļ�����: map0001.jpg
     * ˵��:�ӷ������������ļ�/usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * ����: 0=�ɹ�ִ�� */
	virtual int Download(const std::string& strLocalFileFullName,const std::string& strDownloadFilePath,const std::string& strDownloadFileName)=0;

	/* ɾ���ļ�
	 * strUploadFilePath:     �ϴ��ļ�·��(���������·��):/emaps/part01
	 * strUploadFileName:     �ϴ��ļ�����: map0001.jpg
     * ˵��:���浽�������ϵ��ļ�λ��: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * ����: 0=�ɹ�ִ�� */
	virtual int Delete(const std::string& strDeleteFilePath,const std::string& strDeleteFileName)=0;
};

//---------------------------------------------------------
// �����ϴ�SESSION
//---------------------------------------------------------
AFX_EXT_CLASS KNFSUploadSession* CreateNFSUploadSession(KNFSUploadSessionEvent& rNFSUploadSessionEvent);

//---------------------------------------------------------
// ��������SESSION
//---------------------------------------------------------
AFX_EXT_CLASS KNFSDownloadSession* CreateNFSDownloadSession(KNFSDownloadSessionEvent& rNFSDownloadSessionEvent);


//=======================================================================================
// AAA
//=======================================================================================

//---------------------------------------------------------------------------------------
/* ��֤�û�LDAP�ʻ�
 * Return Values: true=��֤�ɹ�, false=��֤ʧ�� */
 AFX_EXT_CLASS bool AAA_LDAP_Authorize(const std::string& strServerHost,unsigned short usServerPort,
	                     const std::string& strUserID,const std::string& strPassword);

 
//=======================================================================================
// ¼���������ӿ�
//=======================================================================================

#include "KRECDefine.h"
//---------------------------------------------------------
// KRECSession �ӿ���
//---------------------------------------------------------
class KRECSession
{
public:
	KRECSession(){};
	~KRECSession(){};

	virtual void SetUserID(const std::string& strUserID)=0;

	virtual void SetDomain(const std::string& strDomain)=0;

	/* SYS����ģ����ô�������� */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

public:
	//Ĭ��¼��avms����Ҫ¼��mp4��ʽ���뽫recfile.ePreRecType��ֵΪREC_FILE_TYPE_MP4
	virtual void StartChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopRecAll(const std::string& strRoomID)=0;
	//Ĭ��¼��tpc����Ҫ¼��mrec��ʽ���뽫recfile.ePreRecType��ֵΪREC_FILE_TYPE_MREC
	virtual void StartConfRec(const RECFILEINFO& recfile)=0;
	virtual void StopConfRec(const std::string& strRoomID)=0;
	//¼��mrec��ʽʱ���뽫recfile.ePreRecType��ֵΪREC_FILE_TYPE_MREC����ע���recfile.strCheckNum��ֵ
	virtual void StartConfAVRec(const RECFILEINFO& recfile)=0;
	//ֹͣ¼��mrec��ʽʱ���뽫recfile.ePreRecType��ֵΪREC_FILE_TYPE_MREC
	virtual void StopConfAVRec(const RECFILEINFO& recfile)=0;
	virtual void SaveConfStream(const std::string& strRecSvrNodeID,char* outPacket, int nLen)=0;
	virtual void StartConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;
	virtual void StopConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;
	//�ͻ��˴�����¼�����ƵץȡͼƬ���浽¼��������
	virtual void SendPicToRecServer(const std::string& strRoomID, const std::string& strMemberID, const std::string& strChnID, const std::string& strFile, bool bConfRecord)=0;
	//uStreamType��0��������1��������2������
	virtual void ChangeStreamType(const std::string& strRoomID,const std::string& strDevid,const std::string& strChnID,unsigned short uStreamType,const std::string& strUserid,const REC_FILE_TYPE eRecType=REC_FILE_TYPE_TPC)=0;
};

//---------------------------------------------------------
// KRECSessionEvent �ӿڻص���
//---------------------------------------------------------
class KRECSessionEvent
{
public:
	KRECSessionEvent(){};
	~KRECSessionEvent(){};

public:
	/************************************************************************
	* ERRCODE ˵����
	*  ��                           ֵ                       ˵��
	* ERR_REC_SVRREC_BUSY			    70001					¼��������æ����û�п��õ�¼��������
	* ERR_REC_SVR_EXCEPTION				70002					¼���������쳣
	* REC_START_SUCCESS					0						��ʼ¼��
	* REC_START_RECMAXCNT				1						�ﵽ��Ȩ��
	* REC_STREAM_FAILED					2						����ʧ��
	* REC_START_EXIST					3						�Ѿ���¼
	* REC_START_DISKFULL				4						���̿ռ䲻��
	* REC_START_NODISE					5						û�д���
	* REC_START_OTHER					6						��������
	* ����и�����ο�HPRecordDefine.h -> RECERR_CODE����
	************************************************************************/
	virtual void OnRecServerStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecLiveStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecServerConfStatus(const RECFILEINFO& recfile, const std::string& strRecSvrNodeID, int nErrorCode)=0;

	virtual void OnRecServerConfAVStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecServerConfFull() = 0;
};

//=======================================================================================
// RECʵ��
//=======================================================================================
AFX_EXT_CLASS bool RECSessionAddEvent(KRECSessionEvent* pRECSessionEvent);
AFX_EXT_CLASS void RECSessionRemoveEvent(KRECSessionEvent* pRECSessionEvent);
AFX_EXT_CLASS void RECSessionRelease(void);
AFX_EXT_CLASS KRECSession& GetRECSession(void);

#include "KCTSDefine.h"
//---------------------------------------------------------
// KCTSession ����ҵ��ӿڻ���
//---------------------------------------------------------
class AFX_EXT_CLASS KCTSSession
{
public:
	KCTSSession(){};
	~KCTSSession(){};

	virtual void SetDomain(const std::string& strDomain)=0;
	/* SYS����ģ����ô�������� */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

public:
//���ֻ�����ʾ��ʹ��---------------------------------------------------------------------------------
	//����󶨻���()
	//strRoomID:����󶨻��󶨵Ļ�����ID��bBind��true:����󶨣�false:�������󶨣�
	virtual void ApplyBindMeeting(const std::string& strRoomID, const bool& bBind)=0;
	//��Ӳ�����Ϣ
	//strOperator:��Ϣ����ߣ�patientInfo��������Ϣ
	virtual void AddPatientInfo(const std::string& strOperator, const MEDICAID_PATIENT& patientInfo)=0;
	//���Ҳ�����Ϣ
	//strInpatientNO:����סԺ��,strPatientName:��������
	virtual void SearchPatientInfo(const std::string& strInpatientNO, const std::string& strPatientName)=0;
	//ɾ�����в�����Ϣ
	//strOperator:ɾ��������Ϣ������
	virtual void DelAllPatients(const std::string& strOperator) = 0;
	//ɾ��ĳ��������Ϣ
	//strOperator:ɾ��������Ϣ������, strInpatientNO:��ɾ��������Ϣ��סԺ��
	virtual	void DelOnePatient(const std::string& strOperator,const std::string& strInPatientNO) = 0;
	//���²�����Ϣ
	//strOperator:���»�����Ϣ�����ߣ� strPatientNO:סԺ��,newPatientInfo:�µĲ�����Ϣ
	virtual void UpdateOnePatientInfo(const std::string& strOperator, std::string& strInPatientNO,const MEDICAID_PATIENT& newPatientInfo) = 0;
	//��ȡ�����б�
	//iCounts:��Ҫ��ȡ�Ļ���������С��0����ȡ���У�
	virtual void FetchPatients(const int& iCounts) = 0;
//���ֻ�����ʾ��ʹ��---------------------------------------------------------------------------------

//edu����--------------------------------------------------------------------------------------------
	//�ύ���ò�����־
	//ulLogID:���β�����־��id������Ψһ��,ĳ�β�����־��ʼ�ͽ�������ͬһID,ֵ�ɿͻ����Լ����ɣ�,strOperateMemberID:�˴β�����ID��
	//operateLogType:������־���ͣ�bStart:��true������ʼ��false:����������
	virtual void SendClassOperateLog(const unsigned long& ulClassLogID,const unsigned long& ulLogID,const std::string& strOperateMemberID,const std::string& strRoomID,const std::string& strRoomName,const MMS_CALSSOPERATELOG_TYPE& operateLogType,const bool& bStart) = 0;
//edu����--------------------------------------------------------------------------------------------
};

//---------------------------------------------------------
// KCTSessionEvent ����ҵ��ص��ӿڻ���
//---------------------------------------------------------
class AFX_EXT_CLASS KCTSSessionEvent
{
public:
	KCTSSessionEvent(){};
	~KCTSSessionEvent(){};
//���ֻ�����ʾ��ʹ��---------------------------------------------------------------------------------
	//�յ��󶨡�����󶨻���ʧ�ܻص�
	virtual void OnApplyBindMeetingErr(const std::string& strRoomID, const unsigned int& uError)=0;
	//�յ�����󶨻����󶨻ص�(bBind:true���鱻�󶨣�false���鱻�����)
	virtual void OnMeetingBind(const std::string& strRoomID,const bool& bBind)=0;
	//�յ�����ӻ�����Ϣ
	virtual void OnAddPatientInfo(const std::string& strOperator,const MEDICAID_PATIENT& patientInfo) = 0;
	//�յ���ӻ�����Ϣʧ��
	virtual void OnAddPatientInfoErr(const unsigned int& uError)=0;
	virtual void OnSearchPatientInfo(const MEDICAID_PATIENT& patientInfo)=0;
	virtual void OnSearchPatientInfoEnd()=0;
	//�յ���ѯ������Ϣʧ��
	virtual void OnSearchPatientInfoErr(const unsigned int& uError)=0;
	//�յ�ɾ��������Ϣʧ�ܻص�
	virtual void OnDelOnePatientErr(const std::string& strPatientNO,const unsigned int& uError) = 0;
	//�յ�ɾ��������Ϣ�ص�
	virtual void OnDelOnePatient(const std::string& strOperator,const std::string& strPatientNO) = 0;
	//�յ�ɾ�����л�����Ϣ�ص�
	virtual void OnDelAllPatients(const std::string& strOperator) = 0;
	//�յ�����ĳ������Ϣ�ص�
	//�ɹ����»�����Ϣ��strOldPatientNO��ԭסԺ��,newPatientInfo���µĻ�����Ϣ
	virtual	void OnUpdatePatientInfo(const std::string& strOperator,const std::string& strOldPatientNO, const MEDICAID_PATIENT& newPatientInfo) = 0;
	//���»�����Ϣʧ��
	virtual void OnUpdatePatientErr(const std::string& strOldPatientNO,const unsigned int& uError) = 0;
//���ֻ�����ʾ��ʹ��---------------------------------------------------------------------------------

//edu����--------------------------------------------------------------------------------------------
	//���Ϳ���ģʽ�б�
	virtual void OnRecvEduRoomModeList(const EDUROOMMODEINFOLST& lstEduRoomModeInfo)=0;
	//�յ�����Ȩ����
	virtual void OnRecevEduRoomPrivcode(const std::string& strRoomID,const std::string& strMemberID,const unsigned long& ulPriv)=0;
//edu����--------------------------------------------------------------------------------------------
};

//=======================================================================================
// ����ҵ��ȫ�ֽӿ�
//=======================================================================================
AFX_EXT_CLASS bool CTSSessionAddEvent(KCTSSessionEvent* pCTSSessionEvent);
AFX_EXT_CLASS void CTSSessionRemoveEvent(KCTSSessionEvent* pCTSSessionEvent);
AFX_EXT_CLASS void CTSSessionRelease(void);
AFX_EXT_CLASS KCTSSession& GetCTSSession(void);