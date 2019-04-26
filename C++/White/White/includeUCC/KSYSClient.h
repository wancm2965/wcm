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
							unsigned long ulRoomID=0)=0;

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

	virtual void SendPacketToRECNode(char* pData, int nLen, const std::string& strNodeID)=0;

	/* ����P2P���ݵ�NODEID */
	virtual void SendP2PData(const std::string& strNodeID,KCmdPacket& rCmdPacket)=0;
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
	virtual void OnRcvP2PData(const std::string& strFromNodeID,KCmdPacket& rCmdPacket)=0;
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

	/* SYS����ģ����ô�������� */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

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
	 * usTermType:	�ն����� 0=���ն� 1=Ӳ�ն� 2=�ֻ��ն�
	 * strTermName: �ն����� PC Terminal record */
	virtual void Login(const std::string& strUserID,const std::string& strPassword,const std::string& strStatus,const std::string& strAuthKey="",unsigned short usTermType=0,const std::string& strTermName="")=0;	

	/* �û�ע�� */
	virtual void Logout(void)=0;

	/* ��ȡ�����û��� */
	virtual void GetOnlineUserCount(void)=0;

	/* �����ҵ����� */
	virtual void SetMyPassword(const std::string& strPassword)=0;

	/* �����ҵ����� */
	virtual void SetMyName(const std::string& strMyName)=0;

	/* �����ҵ���Ϣ */
	virtual void SetMyInfo(IMS_CONTACT_ITEM& item)=0;

	/* ��ȡ�ҵ���Ϣ */
	virtual void GetMyInfo(void)=0;

	/* ��ȡȨ�� */
	virtual void GetMyPriv(void)=0;

	/* ����ͷ�� */
	virtual void SetImage(const std::string& strImgType,const std::string& strImgFile)=0;

	/* ��ȡͷ�� */
	virtual void GetImage(const std::string& strFriendid)=0;

	/* ���õ�¼�û�״̬ */
	virtual void SetStatus(const std::string& strStatus)=0;

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

	/* �����ϵ�� */
	virtual void ApplyFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* ������ϵ�� */
	virtual void AdmitFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* ���ĺ��������� */
	virtual void UpdateFriendGroup(const std::string& strFriendID,const std::string& group)=0;

	/* ɾ������ */
	virtual void DeleteFriend(const std::string& strFriendID)=0;

	/* �������� */
	virtual void SearchFriend(const IMS_CONTACT_ITEM& item,int nStartID=0,int nEndID=0,bool bOnlineOnly=false)=0;

	/****************************************************************
	 * ��������
	 ****************************************************************/

	/* ��ȡ�����б�
	 * strUpdateTime: �ϴθ��»����б��ʱ�� */
	virtual void FetchOrgs(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * ��Ϣ����
	 ****************************************************************/

	/* ���ͼ�ʱ��Ϣ */
	virtual void SendOnlineMessage(const std::string& strNodeID,XMSG& xmsg)=0;

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
	virtual void GetMonGroupInfo(DOMAINUPDATETIMELST& lstTime)=0;
	virtual void AddMonTmpGroup(std::string& strGroupName)=0;
	virtual void DelMonTmpGroup(unsigned long ulGroupID)=0;
	virtual void RenameMonTmpGroup(unsigned long ulGroupID,const std::string& strGroupName)=0;
	virtual void AddMonTmpGroupMember(unsigned long ulGroupID,const std::string& strMemberID)=0;
	virtual void DelMonTmpGroupMember(unsigned long ulGroupID,const std::string& strMemberID)=0;
	virtual void GetMonTmpGroup(void)=0;
	virtual void GetMonTmpGroupMember(unsigned long ulGroupID)=0;

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
	* IMȺ�鹦��
	****************************************************************/
	/****************************************************************
	* ������
	********************************************D********************/
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
	/* ��ȡ������Ϣ  */
	virtual void GetOfflineDiscussMessage(void)=0;
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
	/* ��ȡ������Ϣ */
	virtual void GetOfflineTeamMsg()=0;
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

	/* ��ȡָ�����µļ�ػ����б� 
	* strUpdateTime: �ϴθ��»����б��ʱ�� 
	* ���strDomainΪ�գ���Ĭ�ϻ�ȡ���������б�*/ 
	virtual void GetAppointDomainRoomList(const std::string& strUpdateTime,const std::string& strDomain="")=0;

	/* ��ȡĳ��ͨ������Ϣ */
	virtual void GetAppointDevChannelInfo(const std::string& strDevID,const std::string& strChannelID)=0;
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

	/* �յ��ҵ������¼� */
	virtual void OnMyName(const std::string& strUserName)=0;

	/* �յ��ҵ���Ϣ�¼� */
	virtual void OnMyInfo(const IMS_CONTACT_ITEM& ui)=0;

	/* �յ�Ȩ���� */
	virtual void OnMyPriv(unsigned long ulPrivCode)=0;

	/* �յ�ͷ���¼� */
	virtual void OnImage(const std::string& strUserID,const std::string& strImgType,const std::string& strImgData)=0;

	/* �յ�״̬�¼�,bFirst��ʾ�Ƿ��һ��״̬���� */
	virtual void OnStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort)=0;

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

	/* �յ���ϵ�˸������¼� */
	virtual void OnFriendUpdateGroup(const std::string& strFriendID,const std::string& strGroup)=0;

	/* �յ�ɾ����ϵ���¼� */
	virtual void OnFriendDelete(const std::string& strFriendID)=0;

	/* �յ�������ϵ�˽���¼� */
	virtual void OnFriendSearch(const IMS_CONTACT_ITEM& item)=0;

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

	/****************************************************************
	 * ��Ϣ���¼�
	 ****************************************************************/

	/* �յ�������Ϣ�¼� */
	virtual void OnOnlineMessage(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const XMSG& xmsg)=0;

	/* �յ�������Ϣ�¼� */
	virtual void OnOfflineMessage(const std::string& strUserID,const XMSG& xmsg)=0;

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
	virtual void OnGroupInfo(const MONGROUP& item)=0;
	virtual void OnChannelInfo(const MONCHANNEL& item,bool bChngStatus=false)=0;
	virtual void OnGetGroupInfoEnd(const std::string& strDomain)=0;
	virtual void OnChannelInfoEnd(const std::string& strDomain)=0;
	/* ĳ�������ӶϿ� */
	virtual void OnDomainDisconnect(const std::string& strDomain,int nFromLevel)=0;
	/* ��ȡĳ��ͨ������Ϣ */
	virtual void OnGetAppointDevChannelInfo(const MONCHANNEL& item)=0;
	/* �豸ͨ������ƵID���� */
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;
	virtual void OnMonAlert(const std::string& strChannelID,unsigned short usType,unsigned short usSubtype)=0;			

	/* ���Ӽ���豸�������ص� */
	virtual void OnAddMonTmpGroup(unsigned long ulGroupID,const std::string& strGroupName)=0;
	virtual void OnDelMonTmpGroup(unsigned long ulGroupID)=0;
	virtual void OnRenameMonTmpGroup(unsigned long ulGroupID,const std::string& strGroupName)=0;
	virtual void OnAddMonTmpGroupMember(unsigned long ulGroupID,const std::string& strMemberID)=0;
	virtual void OnDelMonTmpGroupMember(unsigned long ulGroupID,const std::string& strMemberID)=0;
	virtual void OnGetMonTmpGroup(unsigned long ulGroupID,const std::string& strGroupName)=0;
	virtual void OnGetMonTmpGroupMember(unsigned long ulGroupID,const std::string& strMemberID)=0;
	virtual void OnGetMonTmpGroupMemberEnd()=0;

	/* �յ�����豸���� */
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;

	/* �յ����ͨ������ */
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName,unsigned long ulGroupID)=0;

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
	virtual void OnGetDiscussMemberInfo(const IMS_CONTACT_ITEM& contactItem)=0;
	/* �����������Ա��Ϣ���� */
	virtual void OnGetDiscussMemberInfoEnd(void)=0;
	virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnExitDiscuss(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnRecvDiscussMSG(const unsigned long& ulDiscussID,const std::string& FromID,const XMSG& xmsg)=0;
	virtual void OnGetOfflineDiscussMSG(const unsigned long& ulDiscussID,const std::string& strDiscussName,const std::string& strFromID,const XMSG& xmsg)=0;
    virtual void OnGetOfflineDiscussMSGEnd(const unsigned long& ulDiscussID)=0;
	virtual void OnModDiscussName(const unsigned long& ulDiscussID,const std::string& strName)=0;
	/*Ⱥ��*/
	/*����Ⱥ��Ϣ*/
	virtual void OnCreatTeam(const TEAM_DES& team)=0;
	/* ����Ⱥ��Ա��Ϣ */
	virtual void OnGetTeamMemberInfo(const IMS_CONTACT_ITEM& contactItem)=0;
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
	virtual void OnGetTeamDiscussMemberInfo(const std::string& strMemberID,const IMS_CONTACT_ITEM& contactItem)=0;
	/* Ⱥ�����߹��ܻص� */
	virtual void OnSendTeamData(const PTNDSFileInfo ptNDSFileInfo, const unsigned long& ulSerialNO, const unsigned long& ulError) = 0;
	virtual void OnSendTeamDataEnd(const unsigned long& ulFileID) = 0;
	virtual void OnDownloadTeamData(const unsigned long& ulSerialNO, const PTNDSFileInfo ptNDSFileINfo, const unsigned long& ulError) = 0;
	virtual void OnDownloadTeamDataEnd(const unsigned long& ulFileID) = 0;

	virtual void OnReadyDownloadTeamData(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;

	virtual void OnGetTeamShareFile(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;
	virtual void OnGetTeamShareFileEnd(const unsigned long& ulTeamID) = 0;

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
	/* ��ȡָ�����µļ�ػ����б� 
	 * strUpdateTime: �ϴθ��»����б��ʱ�� 
	 * ���strDomainΪ�գ���Ĭ�ϻ�ȡ���������б�*/
	virtual void OnGetAppointDomainRoomList(const std::string& strDomain,const MMS_ROOM_LST& lstRoom)=0;
	virtual void OnGetAppointDomainRoomListEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

	/************************************************************************
	* ��������������
	************************************************************************/
    virtual void OnYaAnMonAlert(const std::string& strMemberID,unsigned long ulChannelIndex, bool bPopMsg = false)=0;
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
	 * ��ȡIPCAM�б���
	 ****************************************************************/
	virtual void GetIPCamList(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * �������б���
	 ****************************************************************/

	/* ��û����б�
	 * bAllRoomType=true: ��ȡ���л�����/��/��Ա
	 * bAllRoomType=false:ֻȡ������ͱ�׼������+��+��Ա+�߼������� */
	virtual void GetRoomList(bool bAllRoomType=false)=0;

	/****************************************************************
	 * ��ʱ�����ҷ���
	 ****************************************************************/

	/* �����ʱ������
	 * strGroupName: ��ʱ���������� */
	virtual void InsertTempGroup(const std::string& strGroupName)=0;

	/* �޸���ʱ����������
	 * ulGroupID: ��ʱ������ID
	 * strGroupName: ��ʱ���������� */
	virtual void UpdateTempGroupName(unsigned ulGroupID,const std::string& strGroupName)=0;

	/* ɾ����ʱ������
	 * ulGroupID: ��ʱ������ID */
	virtual void DeleteTempGroup(unsigned ulGroupID)=0;

	/* �����ʱ�����Ա */
	virtual void InsertTempMember(unsigned ulGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;

	/* ɾ����ʱ�����Ա */
	virtual void DeleteTempMember(unsigned ulGroupID,const std::string& strMemberID)=0;

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
	 * strTermType:			�ն����� 0=���ն� 1=Ӳ�ն�
	 * strTermName:			�ն����� PC Terminal record */
	virtual void Login(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword,
		               const std::string& strMemberID,const std::string& strMemberName,
					   const std::string& strInviteeGroupID,bool bPresider=false,
					   const std::string& strUpdateTime="",unsigned short usTermType=0,const std::string& strTermName="",bool bHasVideo=false)=0;

	/* �˳������� */
	virtual void Logout(void)=0;

	/* �����Ա���
	 * strInviteeID:		�������ߵ�ID
	 * strPassword:			��������
	 * bPresider:			�Ƿ�����Ϊ������,true=�����������,false=���������� 
	 * ulTempGroupID		��ʱ����ʱ�ã���ΪҪ���ݴ�ID�õ�����ʱ���������ơ� */
	virtual void Invite(const std::string& strInviteeID,const std::string& strPassword,bool bPresider, unsigned long ulTempGroupID = 0)=0;

	/* �����Ա�����˻���
	 * strRoomID:			�������Ļ���ID
	 * strRoomName:			�������Ļ�������
	 * strInviteeID:		�������ߵ�ID
	 * strPassword:			��������
	 * bPresider:			�Ƿ�����Ϊ������,true=�����������,false=���������� 
	 * ulTempGroupID		��ʱ����ʱ�ã���ΪҪ���ݴ�ID�õ�����ʱ���������ơ� */
	virtual void InviteToPersonalRoom(const std::string& strRoomID, const std::string& strRoomName, const std::string& strInviteeID,const std::string& strPassword,bool bPresider, const std::string& strDevID, unsigned long ulTempGroupID = 0)=0;

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

	/* �㲥��Ա��(nChannelTypeȡֵ:MMS_CHLTYPE_VID / MMS_CHLTYPE_SVD / MMS_CHLTYPE_AUD / MMS_CHLTYPE_CHT / MMS_CHLTYPE_WHB / MMS_CHLTYPE_DTS) */
	virtual void BroadcastCard(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType,const std::string& strTag)=0;

	/* �رչ㲥��Ա��(nChannelTypeȡֵ:MMS_CHLTYPE_VID / MMS_CHLTYPE_SVD / MMS_CHLTYPE_AUD / MMS_CHLTYPE_CHT / MMS_CHLTYPE_WHB / MMS_CHLTYPE_DTS) */
	virtual void CloseBroadcastCard(const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* �л����� */
	virtual void DragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* ���û���ģ�� */
	virtual bool SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber)=0;


	/* ���û���������
	 * strAllAttribute�����������Դ�
	 * strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void SetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* ���û��鴰������
	* strAllAttribute���д��ڵ����Դ�
	* strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
	virtual void SetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;


	/* ���ø�������λ��
	 * x = ���ر��� * 100
	 * y = ���ر��� * 100
	 * cx= ��ȱ��� * 100
	 * cy= �߶ȱ��� * 100 */
	virtual bool SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY)=0;	

	/* ���ø��˻����Ȩ�� */
	virtual void SetRight(int usRightType,unsigned char ucRight,const std::string& strMemberID="")=0;

	/* ��ȡ��ͬ����Ϣ */
	virtual void GetGroupSynInfo(const std::string& strGroupID)=0;

	/* ����strGroupID���ٿ�������� */
	virtual void AllowSubMeet(const std::string& strGroupID,const std::string& strMemberID)=0;

	/* Ҫ��strGroupID�鼰������������strGroupID��ͬ�� */
	virtual void Sync(const std::string& strGroupID,const std::string& strMemberID)=0;
	
	/* ������ʱ������ */
	virtual void SetTempPresider(std::string& strMemberID,bool bSet)=0;

	/* �����ɾ�������� */
	virtual void SetProlocutor(const std::string& strMemberID="")=0;

	/* �㲥��ȡ���㲥������ */
	virtual void EnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* �����ɾ�������� */
	virtual void SetQuestioner(const std::string& strMemberID="")=0;

	/* ���ӻ�ȡ������������*/
	virtual void EnableQuestioner(const std::string& strMemberID,bool bEnable)=0; 

	/* �������� */
	virtual void SetAnnexData(int usAnnexType,const std::string& strData,const std::string& strFormat)=0;

	/* ���� */
	virtual void EnableAnnex(int usAnnexType,bool bEnable=true)=0;

	/****************************************************************
	 * ����ͶƱ����
	 ****************************************************************/

	/* ֻ��λ��root��������˲ſ��Է���ͶƱ,��������в��ܷ���
	 * ԤͶƱ
	 * strMemberID:	����ͶƱ�������� */
	virtual void PrepVote(const std::string& strMemberID)=0;

	/* ����ͶƱ 
	voteid:		ͶƱ��ʶID
	topic:		ͶƱ����
	type:		ͶƱ���ͣ������򲻼�����"public"��"anonymous"��
	desc:		����������Ϣ */
	virtual void StartVote(const std::string& strVoteID,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems)=0;

	/* ����ͶƱ 
	endtype:	ͶƱ������ʽ���ֶ����Զ���"hand" �� "auto"��*/
	virtual void StopVote(const std::string& strVoteID,const std::string& strEndType)=0;

	/* ͶƱ
	result:		ͬ�⡢���Ի���Ȩ��"agree"��"disagree"��"abstain"��
	strMemberID:	ͶƱ�ߵ�ID�����ڲ�����ͶƱ��Ϊ��("")
	desc:		��ע */
	virtual void Vote(const std::string& strMemberID,const std::string& strVoteID,MMS_VOTE_LST& strItems)=0;

	/****************************************************************
	 * ������ֺ���
	 ****************************************************************/

	/* ��ʼ����:�����˵��� */
	virtual void StartHandle(unsigned long ulTime)=0;

	/* ����:�����˵��� */
	virtual void Handle(unsigned long ulID,bool bAgree)=0;

	/* ֹͣ����:�����˵��� */
	virtual void StopHandle(unsigned long ulID)=0;

	/****************************************************************
	 * �����������
	 ****************************************************************/
	virtual void Calling(unsigned int usTime)=0;
	virtual void CloseCalling()=0;
	virtual void CallingInConference(const std::string& strFile)=0;

	/****************************************************************
	 * �����غ���
	 ****************************************************************/

	//���ͼ������
	virtual void MonUserData(KCmdUserDataPacket& packet)=0;

	//���P2P
	virtual void MonP2P(const std::string strChannelID, KCmdUserDataPacket& packet)=0;

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
	virtual void StopRecord()=0;

	// ����ֱ���Ľӿ�
	virtual void StartLive( const string& liveIP, unsigned short livePort, const string& liveChannelName)=0;
	virtual void StopLive() = 0;

	/****************************************************************
	 * TV����
	 ****************************************************************/

	/* �����û��Զ�������(�����͵����鵱�еĳ�Ա)
	* ucObject����: 
	*   0=���������������(�������Լ�)��������
	*   1=��������г�Ա��������
	*   ��0��1�����������=��packet�е�toid�������� */
	virtual void SendUserDataEx(unsigned long ulConferenceID, KCmdUserDataPacket& packet,unsigned char ucObject=0)=0;
	virtual void RegisterConference(unsigned long ulConferenceID,std::string strUpdateTime,unsigned short usTermType,const std::string& strTermName)=0;
	virtual void UnregisterConference(unsigned long ulConferenceID)=0;

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
    virtual void AddGroup( const std::string& strGroupName, unsigned long ulParentGroupID, const std::string& strTopic /*levelid, orderid, topic*/)=0;
    virtual void DelGroup( unsigned long ulGroupID)=0;
    // virtual void AddGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // virtual void DelGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // �ı��û������飬����
    virtual void ChangeMemberGroup( unsigned long ulGroupID, const std::string& strMemberID)=0;

	//��ȡ���������Ա��Ϣ
	virtual void GetRoomGroupMemberInfo(std::string roomid, std::string groupid)=0;

    /************************************************************************
    * ���˷������
    /************************************************************************/
	virtual void GetPersonalRoom(void)=0;
	virtual void GetPersonalRoomGroup(const std::string& strRoomID)=0;
	virtual void GetPersonalRoomMember(const std::string& strRoomID)=0;
	virtual void AddPersonalRoom(const std::string& strRoomName)=0;
	virtual void DelPersonalRoom(const std::string& strRoomID)=0;
	virtual void AddPersonalRoomGroup(const MMS_GROUP& group)=0;
	virtual void DelPersonalRoomGroup(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void StartPersonalRoom(const std::string& strRoomID)=0;

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

	/*���� MPS*/
	virtual void StartMP(void)=0;
	virtual void Start3GC(void)=0;
	/*���� MPS*/
	virtual void StopMPS(const std::string& strMPSUserID)=0;

    //�ƶ����
    //��ȡ�ƶ����Ƶ���б�
    virtual void GetMobileChannelList(const std::string& strDomain)=0;
    //����Ƶ������Ȩ
    virtual void ApplyMobileChannelControl()=0;
    //�����յ��Ķ�Ƶ������Ȩ������
    virtual void HandleApplyMobileChannelControl(std::string& strApplyID,bool bIsSuccess)=0;
    //�ͷ�Ƶ������Ȩ
    virtual void ReleaseMobileChannelControl()=0;
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

	virtual void OnTempGroupItem(const std::string& strDomain,unsigned long ulRoomID,unsigned long ulGroupID,const std::string& strGroupName,bool bDefault)=0;
	virtual void OnTempGroupUpdateName(const std::string& strDomain,unsigned long ulRoomID,unsigned long ulGroupID,const std::string& strGroupName)=0;
	virtual void OnTempGroupDelete(const std::string& strDomain,unsigned long ulRoomID,unsigned long ulGroupID)=0;
	virtual void OnTempMemberItem(const std::string& strDomain,unsigned long ulRoomID,unsigned long ulGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;
	virtual void OnTempMemberDelete(const std::string& strDomain,unsigned long ulRoomID,unsigned long ulGroupID,const std::string& strMemberID)=0;

	/****************************************************************
	 * IPCAM�б��¼�
	 ****************************************************************/
	virtual void OnIPCamItem(unsigned long ulIPCamID,const std::string& strIPCamName,const std::string& strOrgID,const std::string& strModel,const std::string& strIPAddr,const std::string& strAdminID,const std::string& strAdminPass,const std::string& strOperID,const std::string& strOperPass,unsigned char ucStatus)=0;
	virtual void OnIPCamListEnd(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * �����б��¼�
	 ****************************************************************/
	virtual void OnRoomListEnd(const std::string& strDomain)=0;

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
        const std::string& strMemberID,unsigned short usMemberType,bool bAllowVideo)=0;

	/* �յ��˳��������¼�
	 * nErrCode���ص�¼���ֵ
	 * 0: ��ʾ�����˳�������
	 * 1: ��ʾ�������˳�������
	 * 2: ��ʾ���߳�������
	 * 3: �����Ա�ʺ��ڸ÷������ϲ����ڻ�÷�����û�м��ػ���ģ�� */
	virtual void OnLogout(unsigned int nErrCode)=0;

	/* �յ��������� */
	virtual void OnRoomGroupItem(MMS_GROUP_EX& item)=0;

	/* �յ������������ */
	virtual void OnRoomGroupListEnd(const std::string& strUpdateTime)=0;

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
						  unsigned short usTermType,const std::string& strTermName,unsigned char ucStatus)=0;

	/* �յ��������� */
	virtual void OnInvite(const std::string& strRoomDomain,const std::string& strRoomID,const std::string& strGroupID,
		const std::string& strRoomName,const std::string& strInviterNodeID,const std::string& strInviterID,
		const std::string& strInviterName,const std::string& strPassword,bool bPresider, unsigned long ulTempGroupID, 
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
		const std::string& strDevInfo)=0;

	/* ͨ�����б���� */
	virtual void OnCardListEnd(void)=0;

	/* �յ���Ƶͨ���� */
	virtual void OnCardAudioItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,unsigned long ulAudioID)=0;

	/* ����ͨ������ */
	virtual void OnSetCardName(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName)=0;

	/* ��ͬ����Ϣ��ȡ��� */
	virtual void OnGroupSynInfoEnd(void)=0;

	/* ���ø��˻����Ȩ�� */
	virtual void OnSetRight(int usRightType,unsigned char ucRight,const std::string& strMemberID)=0;

	/* �����ɾ�������� */
	virtual void OnSetProlocutor(const std::string& strMemberID,bool bRequest)=0;

	/* �㲥��ȡ���㲥������ */
	virtual void OnEnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* �����ɾ�������� */
	virtual void OnSetQuestioner(const std::string& strMemberID,bool bRequest)=0;

	/* ���ӻ�ȡ������������ */
	virtual void OnEnableQuestioner(const std::string& strMemberID,bool bEnable)=0;

	/* ���ù������� */
	virtual void OnSetAnnexData(int usAnnexType,const std::string& strData,const std::string& strFormat)=0;

	/* ���ù������� */
	virtual void OnEnableAnnex(int usAnnexType,bool bEnable)=0;

	/* ������� */
	virtual void OnUnlocked(void)=0;

	/* �㲥�� */
	virtual void OnBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,int nScreenID,int nWindowID,
		                         const std::string& strMemberID,int nCardIndex,int nChannelType,unsigned long ulChannelID,
								 const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
                                 const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
								 const std::string& strTag)=0;

	/* �رչ㲥�� */
	virtual void OnCloseBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* �϶����� */
	virtual void OnDragWindow(const std::string& strOwnerID,int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* �ı�ģ�� */
	virtual void OnSetTemplet(const std::string& strOwnerID,int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber)=0;

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
	virtual void OnSetTempPresider(std::string& strMemberID,bool bSet)=0;

	/* ͶƱ */
	virtual void OnPrepVote(const std::string& strMemberID,const std::string& strVoteID,bool bAllow)=0;
	virtual void OnStartVote(const std::string& strVoteID,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems)=0;
	virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,int nSum,int nAnswer,MMS_VOTE_LST& strItems)=0;

	/* ���� */
	virtual void OnStartHandle(unsigned long ulID,unsigned long ulTime)=0;
	virtual void OnStopHandle(unsigned long ulID,unsigned short usTotal,unsigned short usCount)=0;

	/* ������ͬ�� */
	virtual void OnAllowSubMeet(const std::string& strGroupID)=0;
	virtual void OnSync(const std::string& strGroupID)=0;

	/* ���� */
	virtual void OnCalling(const std::string& strMemberID,unsigned int usTime)=0;
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
	virtual void OnTVServerItem(const std::string& strTV,const std::string& strTVName,const std::string& strStatus)=0;
	virtual void OnTVMonitorItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVMatrixItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVListEnd(void)=0;

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
	virtual void OnRegisterConference(unsigned long ulConferenceID,const std::string& strConferenceName,const std::string& strConferenceType,unsigned short usRet)=0;

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
    virtual void OnAddGroup( unsigned long ulGroupID, const std::string& strGroupName, unsigned long ulParentGroupID, const std::string& strTopic, unsigned int nErrCode) = 0;
    virtual void OnDelGroup( unsigned long ulGroupID, unsigned int nErrCode) = 0;
    virtual void OnChangeGroupMember( unsigned long ulGroupID, const std::string& strMemberID, unsigned int nErrCode) = 0;

	/************************************************************************
    * ���˷������
    /************************************************************************/
	virtual void OnGetPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,bool bHaveFrame,unsigned short usStatus)=0;
	virtual void OnGetPersonalRoomEnd(void)=0;

	virtual void OnGetPersonalRoomGroup(const std::string& strRoomID,const MMS_GROUP& group)=0;
	virtual void OnGetPersonalRoomGroupEnd(const std::string& strRoomID)=0;

	virtual void OnGetPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void OnGetPersonalRoomMemberEnd(const std::string& strRoomID)=0;
	virtual void OnAddPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,const std::string& strOwnerID)=0;
	virtual void OnDelPersonalRoom(const std::string& strRoomID)=0;
	virtual void OnRenamePersonalRoom(const std::string& strRoomID,const std::string& strMeetingName)=0;
	virtual void OnAddPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
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
	virtual void OnNotifyPersonalAdminMeetingStart(std::string& strRoomID)=0;

	/************************************************************************/
	/* ��ϵ�˷���                                                           */
	/************************************************************************/
	virtual void OnGetContactGroup(const std::string& strParentGroupID,const std::string& strGroupID,const std::string& strGroupName,const unsigned int nLevelID,const unsigned int nOrderID)=0;
	virtual void OnGetContactGroupEnd()=0;

	virtual void OnGetContactGroupMember(const std::string& strGroupID,const IMS_CONTACT_ITEM& item)=0;
	virtual void OnGetContactGroupMemberEnd(const std::string& strGroupID)=0;
	virtual void OnContactGroupMemberStatus(const std::string& strMemberID,const std::string& strStatus)=0;

    //�ƶ����
	//��ȡ���ƶ�Ƶ���б�
    virtual void OnGetMobileChannelList(const std::string& strDomain,const MMS_ROOM_LST& lstChnl)=0;
    //���������û��Կ���Ȩ������
    virtual void OnApplyMobileChannelControl(const std::string& strUserID,const unsigned long ulRoomID)=0;
    //�յ��������Ȩ�Ĵ�����
    virtual void OnHandleApplyMobileChannelControl(const std::string& strAdminID,const unsigned long ulRoomID,bool bIsSuccess)=0;
    //�ͷſ���Ȩ
    virtual void OnReleaseMobileChannelControl(const unsigned long ulRoomID)=0;

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
	virtual void Login(const std::string& strUserID,const std::string& strPassword, MON_DEVTYPE eDevType,const std::string& strName,const std::string& strSerialno, unsigned short usTermType=0,const std::string& strTermName="", const std::string& strVersion ="")=0;

	/* ע�� */
	virtual void Logout(void)=0;

	/* ���Ͷ��� */
	virtual void SendSMS(const std::string& strMobilNumber,const std::string& strMessage)=0;

	/* �豸���� 
	cstrBindMemberID���ڻ����ն�,����Ϊ��
	usFactory����MON_FACTORY*/
	virtual void DeviceOnline(const MON_DEVTYPE& eDevType, const std::string &strDevID,const std::string &strPwd,const std::string &strMask,const std::string &strName, unsigned short usFactory = 0, const std::string& strVersion = "",const std::string strBindMemberID = "")=0;

	/* �豸���� */
	virtual void DeviceOffline(const std::string &strDevID,const std::string strBindMemberID = "")=0;

	/* ��֤�豸���ʺź����кŵİ󶨹�ϵ
	 * strDevID: Ϊ�����豸�ʺ�
	 * strNewDevID: Ϊ�µ��豸�ʺ�
	 * strSerialNo: Ϊ�豸�����к� */
	virtual void DeviceCheck(const std::string &strDevID,const std::string &strNewDevID,const std::string &strSerialNo)=0;

	/* �޸��豸���� */
	virtual void UpdateDeviceName(const std::string &strDevID,const std::string &strName)=0;

	/* ����DVRͨ��������Ϣ */
	virtual void ChannelOnline(const MON_CHANNEL_INFO &tMonChannelInfo)=0;

	/* ����DVRͨ��ע������ nCHNum=-1 ����ͨ��ע�� */
	virtual void ChannelOffline(const std::string &strDevID,unsigned short nCHNum)=0; 

	/* �޸�ͨ������ */
	virtual void UpdateChannelName(const std::string &strChannelID,const std::string &strName)=0;
	
	/* ��ȡ������Ϣ */
	virtual void GetDeviceBasicInfo(const std::string &strDevID)=0;

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
	virtual void P2P(const std::string strUserID, KCmdUserDataPacket& packet)=0;

	/* GPS�ź�
	 * strPosition: GPS��Ϣ��ʽ��(LOT,;LAT, ;WSL, ;SPD, ;)
	 *              LOT=����;LAT=γ��;WSL=���θ߶�;SPD=�ƶ��ٶ� */
	virtual void GPS(const std::string& strPosition)=0;

	/* ��ȡ������������� */
	virtual void GetRegion(void)=0;
	virtual void GetRegion(const std::string &strUserID)=0;
	virtual bool GetRegion(const std::string strRegionID,unsigned short nNodeType)=0;

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

	/* �ۺ��ֿ�����ǽ*/
	virtual void OnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;

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

	/* ������� */
	virtual void OnRegion(KCmdItem& rCmdPacket)=0;

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
    virtual void UploadFile( PTNDSFileInfo ptNDSFileInfo ) = 0;

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
    virtual void OnUploadFileStart( PTNDSFileInfo ptNDSFileInfo) = 0;

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

	virtual void SetUserID(std::string strUserID)=0;

	virtual void SetMeetingDomain(std::string strDomain)=0;

	/* SYS����ģ����ô�������� */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

public:
	virtual void StartChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopRecAll(const std::string& strRoomID)=0;

	virtual void StartConfRec(const RECFILEINFO& recfile)=0;
	virtual void StopConfRec(const std::string& strRoomID)=0;
	virtual void StartConfAVRec(const RECFILEINFO& recfile)=0;	
	virtual void StopConfAVRec(const RECFILEINFO& recfile)=0;
	virtual void SaveConfStream(const std::string& strRecSvrNodeID,char* outPacket, int nLen)=0;
	virtual void StartConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;
	virtual void StopConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;

	//�ͻ��˴�����¼�����ƵץȡͼƬ���浽¼��������
	virtual void SendPicToRecServer(const std::string& strRoomID, const std::string& strMemberID, const std::string& strChnID, const std::string& strFile, bool bConfRecord)=0;
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
