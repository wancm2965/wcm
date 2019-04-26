#ifndef __H323CALLMESSAGE_H__
#define __H323CALLMESSAGE_H__

#include "../DefineHeader/H323DefineHeader.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
#include <AVCONMP/AdapterHeaderDefine.h>
#else

#include <IMXEC/AdapterHeaderDefine.h>
#endif

#include <RVINC/cm.h>
#include "../IMXEC_KBase/CmdPacket.h"


#define BuildPathMaxLen 256
#define BuildPathMaxCaps 16

struct buildPathValueItem
{
	char strPath[BuildPathMaxLen];
	unsigned int  nValue;
	char strValue[BuildPathMaxLen];
	unsigned int  nIsString;
	unsigned int  nGetPathValue;
};

struct buildPathValue
{
	int	m_nNum;	//��Ч���ݸ���
	buildPathValueItem	m_BuildPVCaps[BuildPathMaxCaps];
	buildPathValue()
	{
		m_nNum = 0;
	}
};

typedef enum 
{
	MessageConfReq=1,
	MessageConfResp,
	MessageConfCmd,
	MessageConfInd,
	MessageCommand,
	MessageIndication
}BuildMessageType;

class H323Call;
/////////////////////////////////////////////////////////////////////////////////
class H323CallMessage 
{
	friend class H323Call;

public:
	H323CallMessage(H323Call& rH323Call);
	~H323CallMessage();

public:
    H323Call&			m_rH323Call;		// �����ĺ��� 
private:
	void SetBuildPathValue(buildPathValue & pathValue,const char * path,int nValue,char * strValue=NULL);
	int SendBuildMessage(BuildMessageType nType,buildPathValue & pathValue);

public:
	/******************************************************************************
	* SendCallH245ControlMessage
	*����������H245������Ϣ ����flowControlIndication H239������� �����Ϣ
	*���룺	
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendCallH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendSendThisSourceRequest
	*�����������л�Դ����Ϊ������������Ϣ sendThisSource
	*���룺
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendSendThisSourceRequest(const char * cszPacket,int nPacketLen);
	//�����л�Դ����Ϊ��������Ӧ��Ϣ sendThisSourceResponse
	int SendSendThisSourceResponse(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendMakeMeChairRequest
	*����������������ϯ������Ϣ makeMeChair
	*���룺
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendMakeMeChairRequest(const char * cszPacket,int nPacketLen);
	//���ͷ���������ϯ��Ӧ��Ϣ makeMeChairResponse
	int SendMakeMeChairResponse(const char * cszPacket,int nPacketLen);

 	/******************************************************************************
	* SendTerminalListRequest
	*���������� �ն��б�������Ϣ terminalListRequest
	*���룺
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendTerminalListRequest(const char * cszPacket,int nPacketLen);
	//�����ն��б���Ӧ��Ϣ terminalListResponse
	int SendTerminalListResponse(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendCancelMakeMeChairRequest
	*����������ȡ����ϯ������Ϣ cancelMakeMeChair 
	*���룺
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendCancelMakeMeChairRequest(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendDropTerminalRequest
	*�����������ն����������Ϣ dropTerminal  
	*���룺
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendDropTerminalRequest(const char * cszPacket,int nPacketLen);


	//�����ն�ID������Ϣ requestTerminalID  
	int SendTerminalIDRequest(const char * cszPacket,int nPacketLen);
	//�����ն�ID��Ӧ��Ϣ
	int SendTerminalIDResponse(const char * cszPacket,int nPacketLen);

	//������ϯ���� requestChairTokenOwner
	int SendChairTokenOwnerRequest(const char * cszPacket,int nPacketLen);
	//��ϯ������Ӧ
	int SendChairTokenOwnerResponse(const char * cszPacket,int nPacketLen);

	//����㲥�ն� makeTerminalBroadcaster
	int SendMakeTerminalBroadcasterRequest(const char * cszPacket,int nPacketLen);
	//�㲥�ն���Ӧ makeTerminalBroadcasterResponse
	int SendMakeTerminalBroadcasterResponse(const char * cszPacket,int nPacketLen);

	//���������ն�ID requestAllTerminalIDs
	int SendAllTerminalIDsRequest(const char * cszPacket,int nPacketLen);
	//�����ն�ID��Ӧ requestAllTerminalIDsResponse      
	int SendAllTerminalIDsResponse(const char * cszPacket,int nPacketLen);

	//����Զ��MC���� remoteMCRequest
	int SendRemoteMCRequest(const char * cszPacket,int nPacketLen);
	//Զ��MC������Ӧ remoteMCResponse    
	int SendRemoteMCResponse(const char * cszPacket,int nPacketLen);

	//���͹㲥�ն����� makeTerminalBroadcaster
	int SendMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen);

	//����ȡ���㲥�ն����� cancelMakeTerminalBroadcaster
	int SendCancelMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen);

	//�����л�Դ����Ϊ������������Ϣ sendThisSource
	int SendSendThisSourceCommand(const char * cszPacket,int nPacketLen);

	//����ȡ���л�Դ��ȡ��������������Ϣ cancelSendThisSource
	int SendCancelSendThisSourceCommand(const char * cszPacket,int nPacketLen);

	//����ȡ���л�Դ��ȡ��������������Ϣ dropConference
	int SendDropConferenceCommand(const char * cszPacket,int nPacketLen);


	//�����ն˺������ָʾ��Ϣ terminalNumberAssign
	int SendTerminalNumberAssignIndication(const char * cszPacket,int nPacketLen);

	//�����ն˼������ָʾ��Ϣ terminalJoinedConference
	int SendTerminalJoinedConferenceIndication(const char * cszPacket,int nPacketLen);
	//�����ն��뿪����ָʾ��Ϣ terminalLeftConference
	int SendTerminalLeftConferenceIndication(const char * cszPacket,int nPacketLen);

	//��������ѡ��һ��������Ƶָʾ��Ϣ seenByAtLeastOneOther
	int SendSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen);
	//����ȡ������ѡ��һ��������Ƶָʾ��Ϣ cancelSeenByAtLeastOneOther
	int SendCancelSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen);

	//����ѡ��������Ƶָʾ��Ϣ seenByAll
	int SendSeenByAllIndication(const char * cszPacket,int nPacketLen);
	//����ȡ��ѡ��������Ƶָʾ��Ϣ cancelSeenByAll
	int SendCancelSeenByAllIndication(const char * cszPacket,int nPacketLen);
	//����ָ��Ϊ�ۿ��᳡��Ϣ   terminalYouAreSeeing
	int SendTerminalYouAreSeeingIndication(const char * cszPacket,int nPacketLen);

	//������������Ϣ TERM-->MCU
	int SendRequestForFloorIndication(const char * cszPacket,int nPacketLen);
	//������������Ϣ MCU-->Chair
	int	SendFloorRequestedIndication(const char * cszPacket,int nPacketLen);

	//�����߼�ͨ��������Ϣ flowControlCommand
	int SendFlowControlCommand(const char * cszPacket,int nPacketLen);
	//�����߼�ͨ��������Ϣ flowControlIndication
	int SendFlowControlIndication(const char * cszPacket,int nPacketLen);
	//��������ؼ�֡��Ϣ
	int	SendVideoFastUpdatePicture(const char * cszPacket,int nPacketLen);
	//��������ͼ�񶳽���Ϣ Add VCF
	int	SendVideoFreezePicture(const char * cszPacket,int nPacketLen);

	void RecvH245ControlMessage(int message,cmH245MessageType messageType);
	void OnRecvH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);
	void SetBuildPath(buildPathValue & pathValue,const char * path);
	int GetBuildPathValue(int nMessageID,buildPathValue & pathValue);

	void RecvSendThisSourceRequest(int message);
	void RecvSendThisSourceResponse(int message);
	void RecvMakeMeChairResponse(int message);
	void RecvMakeMeChairRequest(int message);
	void RecvTerminalListRequest(int message);
	void RecvCancelMakeMeChairRequest(int message);
	void RecvDropTerminalRequest(int message);
	void RecvTerminalIDRequest(int message);
	void RecvTerminalIDResponse(int message);
	void RecvChairTokenOwnerRequest(int message);
	void RecvChairTokenOwnerResponse(int message);
	void RecvAllTerminalIDsRequest(int message);
	void RecvMakeTerminalBroadcasterResponse(int message);
	void RecvMakeTerminalBroadcasterRequest(int message);
	void RecvRemoteMCRequest(int message);
	void RecvRemoteMCResponse(int message);
	void RecvMakeTerminalBroadcasterCommand(int message);
	void RecvCancelMakeTerminalBroadcasterCommand(int message);
	void RecvSendThisSourceCommand(int message);
	void RecvDropConferenceCommand(int message);
	void RecvCancelSendThisSourceCommand(int message);
	void RecvCancelSeenByAllIndication(int message);
	void RecvSeenByAllIndication(int message);
	void RecvCancelSeenByAtLeastOneOtherIndication(int message);
	void RecvSeenByAtLeastOneOtherIndication(int message);
	void RecvTerminalLeftConferenceIndication(int message);
	void RecvTerminalJoinedConferenceIndication(int message);
	void RecvTerminalNumberAssignIndication(int message);
	int GetBuildPathValueSetPacket(int nMessageID,buildPathValue & pathValue,KCmdUserDataPacket &packet,int nHasTermID=0);
	void RecvTerminalListResponse(int message);
	void RecvAllTerminalIDsResponse(int message);
	void RecvRequestForFloorIndication(int message);
	void RecvFloorRequestedIndication(int message);
	void RecvTerminalYouAreSeeingIndication(int message);
	void RecvTerminalNumberAssign(int mcuNumber, int terminalNumber);
	void RecvVideoFastUpdatePicture(std::string strMedia);
	//Add recv VCF
	void RecvVideoFreezePicture(std::string strMedia);

	void RecvGenericMessage(cmH245MessageType messageType,int nBitrate, int terLabel, int chID, int symBreaking,int response,int respValue);
	void RecvFlowControlCommand(std::string strMediaName,unsigned int rate);
	void RecvFlowControlIndication(int message);

	void SendGenericRequestMessage(const char * cszPacket,int nPacketLen);
	void SendGenericResponseMessage(const char * cszPacket,int nPacketLen);
	void SendGenericCommandMessage(const char * cszPacket,int nPacketLen);
	void SendGenericIndicationMessage(const char * cszPacket,int nPacketLen);

	//nValue ��ƵԴID��Ԥ�õ�
	void RecvRemoteCameraControl(std::string strAction,int nValue);
	int	SendCameraControl(const char * cszPacket,int nPacketLen);
	int SendMediaChannelActive(const char * cszPacket,int nPacketLen);
	int SendMediaChannelInActive(const char * cszPacket,int nPacketLen);
	void MediaChannelActiveStateMessage(std::string strMediaName);
	void MediaChannelInActiveStateMessage(std::string strMediaName);

	//test
	void SendH245Message();

public:
	std::string m_strConfID;

};


#endif	//#ifndef __H323CALLMESSAGE_H__
