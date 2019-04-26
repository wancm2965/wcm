//========================================================================
//��������ؿͻ��˽ӿ�
//���ܣ�����Ƶ���ӡ��Խ�����̨���ơ�¼��طš����ӵ�ͼ�ȹ���
//���ߣ������@
//ʱ�䣺2008-12-19
//˵����
//------------------------------------------------------------------------
using namespace std;
#pragma once
#include "KSYSClient.h"
#include "StructDefine.h"


/*
 *	��ؿͻ��˿�ܽӿ�
 */
class HPMC_Client
{
public:
	HPMC_Client() {}
	virtual ~HPMC_Client() {}

public:
	/*	
	�������ܣ� ���������ļ����ô�����ʾ����Ļ
	��    ���� 
	�� �� ֵ�� �ɹ�/ʧ��
	*/
	virtual void ShowHPMC(bool bIsXPE = false) = 0;

	/*	
	�������ܣ� ��ʾ����
	��    ���� nCmdShow
	�� �� ֵ�� �ɹ�/ʧ��
	*/
	virtual BOOL ShowWindow(int nCmdShow) = 0;

	/*	
	�������ܣ� ���ô���λ��
	��    ���� const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags
	�� �� ֵ�� �ɹ�/ʧ��
	*/
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;

	/*	
	�������ܣ� ��ȡ���ھ��
	��    ���� ��
	�� �� ֵ�� ���ھ��
	*/
	virtual HWND GetSafeHwnd() = 0;	

	/*	
	�������ܣ� ��ȡ����Ϣ����
	��    ���� ��
	�� �� ֵ�� ��
	*/
	virtual void OnGetGroupInfoEnd(const std::string& strDomain) = 0;

	/*	
	�������ܣ� ��ȡͨ����Ϣ����
	��    ���� ��
	�� �� ֵ�� ��
	*/
	virtual void OnChannelInfoEnd(const std::string& strDomain) = 0;

	/*	
	�������ܣ�����ͨ����Ϣ
	��    ���� CHANNELINFO
	�� �� ֵ�� ��
	*/
	virtual void OnChannelInfo(const CHANNELINFO& item) = 0;

	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID) =0;


	/*
	�������ܣ� ���ؼ��P2P
	��    ���� strDevID�豸ID
	�� �� ֵ�� ��
	*/
	virtual void OnMonP2PData(const std::string& strDevID,KCmdUserDataPacket& packet) = 0;

	/*
	�������ܣ� ���ؼ�����ݣ���Ҫ��������ת��
	��    ���� 
	�� �� ֵ�� ��
	*/
	virtual void OnMonUserData(KCmdUserDataPacket& packet) = 0;

	/*
	�������ܣ� ��ر���
	��    ���� strChannelID	ͨ��ID
			   usType		��������
			   usSubtype	������
	�� �� ֵ�� ��
	*/
	virtual void OnMonAlertData(const std::string strChannelID,unsigned short usType,unsigned short usSubtype) = 0;

	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;

	virtual void OnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;

	//P2P͸��
	virtual void OnUserData(KCmdUserDataPacket& packet,unsigned char ucObject) = 0;

	//����
	virtual void OnLoss(MMS_LOSS& loss, unsigned short usErrorCode) = 0;

	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;


	//��ͷ����
	virtual void OnDEVAddMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVDelMyGroup(const std::string& strTmpGrpID)=0;
	virtual void OnDEVRenameMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVAddMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;
	virtual void OnDEVDelMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;

	virtual void OnDEVGetMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVGetMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;

	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	//�������ڵ�ID�õ��������ȵ�ID
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;

	//����ǽ
	virtual void OnInsertTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;

	//����ǽ������
	virtual void OnInsertTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;

	virtual void OnTVServerItem(const std::string& strTV,const std::string& strTVName,const std::string& strStatus)=0;
	virtual void OnTVMonitorItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVMatrixItem(unsigned short usIndex,const std::string& strData)=0;

	virtual void OnTVListEnd(void)=0;
	virtual void SetMonClientMCU(MMS_MCU* mcu)=0;
	virtual void OnSetAVCONType(int type)=0;
	virtual void OnScreenControlOnKeyDown(MSG* pMsg)=0;

	//����ǽ����
	virtual void OnRoomGroupItem(MMS_GROUP_EX& item)=0;
	virtual void OnRoomGroupListEnd(const std::string& strUpdateTime)=0;
	/* �յ������Ա�� */
	virtual void OnRoomMemberItem(MMS_MEMBER_EX& item,bool bAllGeted)=0;
	/* �յ������Ա����� */
	virtual void OnRoomMemberListEnd(void)=0;

	/* �յ�Ԥ�����ڳ�Ա�� */
	virtual void OnWindowMemberItem(const std::string& strGroupID,unsigned int nScreenID,unsigned int nWindowID,const std::string& strMemberID)=0;

	/* �յ�Ԥ�����ڳ�Ա����� */
	virtual void OnWindowMemberListEnd(void)=0;

	/****************************************************************
	* H323MP����
	****************************************************************/
	// ����MPID����SIP�豸

	// ����MPID����SIP�豸�б����
	virtual void OnGetSIPSetByMPEnd(const std::string& strMPID)=0;

	virtual void ProTVCInitTreeMember(const std::string& strRoomID,const std::string& strRoomName,const unsigned short& uRoomType)=0;

	virtual void OnStatus(const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,
		const std::string& strSvrID,const std::string& strSvrAddr,unsigned short usSvrPort,const std::string& strSvrType,
		const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
		unsigned short usTermType,const std::string& strTermName,unsigned char ucStatus)=0;


	virtual void OnCardItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName,
		unsigned int nChannelType,unsigned long ulChannnelID,
		const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
		const std::string& strDevInfo)=0;
	virtual void SetTVCMemberRoom(const string& strRoomID)=0;

	//������ʾ
	virtual void PopuMsg(CString sMsg)=0;

	virtual void OnRcvP2PData(const std::string&strNodeID,KCmdPacket& packet)=0;
};

/*
�������ܣ� ������ؿͻ���ʵ��
��    ���� �ص�ʵ��
�� �� ֵ�� ʵ��ָ��
*/
AFX_EXT_API HPMC_Client* Create_HPMC_Client(void);

/*	
�������ܣ� ���ټ�ؿͻ���ʵ��
��    ���� ʵ�����
�� �� ֵ�� ��
*/
AFX_EXT_API void Destroy_HPMC_Client(HPMC_Client*pClient);
