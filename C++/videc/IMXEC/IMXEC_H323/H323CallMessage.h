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
	int	m_nNum;	//有效数据个数
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
    H323Call&			m_rH323Call;		// 所属的呼叫 
private:
	void SetBuildPathValue(buildPathValue & pathValue,const char * path,int nValue,char * strValue=NULL);
	int SendBuildMessage(BuildMessageType nType,buildPathValue & pathValue);

public:
	/******************************************************************************
	* SendCallH245ControlMessage
	*描述：发送H245控制消息 包含flowControlIndication H239令牌相关 会控消息
	*输入：	
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendCallH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendSendThisSourceRequest
	*描述：发送切换源或设为发言人请求消息 sendThisSource
	*输入：
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendSendThisSourceRequest(const char * cszPacket,int nPacketLen);
	//发送切换源或设为发言人响应消息 sendThisSourceResponse
	int SendSendThisSourceResponse(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendMakeMeChairRequest
	*描述：发送申请主席请求消息 makeMeChair
	*输入：
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendMakeMeChairRequest(const char * cszPacket,int nPacketLen);
	//发送发送申请主席响应消息 makeMeChairResponse
	int SendMakeMeChairResponse(const char * cszPacket,int nPacketLen);

 	/******************************************************************************
	* SendTerminalListRequest
	*描述：发送 终端列表请求消息 terminalListRequest
	*输入：
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendTerminalListRequest(const char * cszPacket,int nPacketLen);
	//发送终端列表响应消息 terminalListResponse
	int SendTerminalListResponse(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendCancelMakeMeChairRequest
	*描述：发送取消主席请求消息 cancelMakeMeChair 
	*输入：
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendCancelMakeMeChairRequest(const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendDropTerminalRequest
	*描述：发送终端离会请求消息 dropTerminal  
	*输入：
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendDropTerminalRequest(const char * cszPacket,int nPacketLen);


	//发送终端ID请求消息 requestTerminalID  
	int SendTerminalIDRequest(const char * cszPacket,int nPacketLen);
	//发送终端ID响应消息
	int SendTerminalIDResponse(const char * cszPacket,int nPacketLen);

	//请求主席令牌 requestChairTokenOwner
	int SendChairTokenOwnerRequest(const char * cszPacket,int nPacketLen);
	//主席令牌响应
	int SendChairTokenOwnerResponse(const char * cszPacket,int nPacketLen);

	//请求广播终端 makeTerminalBroadcaster
	int SendMakeTerminalBroadcasterRequest(const char * cszPacket,int nPacketLen);
	//广播终端响应 makeTerminalBroadcasterResponse
	int SendMakeTerminalBroadcasterResponse(const char * cszPacket,int nPacketLen);

	//请求所有终端ID requestAllTerminalIDs
	int SendAllTerminalIDsRequest(const char * cszPacket,int nPacketLen);
	//所有终端ID响应 requestAllTerminalIDsResponse      
	int SendAllTerminalIDsResponse(const char * cszPacket,int nPacketLen);

	//请求远端MC设置 remoteMCRequest
	int SendRemoteMCRequest(const char * cszPacket,int nPacketLen);
	//远端MC设置响应 remoteMCResponse    
	int SendRemoteMCResponse(const char * cszPacket,int nPacketLen);

	//发送广播终端命令 makeTerminalBroadcaster
	int SendMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen);

	//发送取消广播终端命令 cancelMakeTerminalBroadcaster
	int SendCancelMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen);

	//发送切换源或设为发言人命令消息 sendThisSource
	int SendSendThisSourceCommand(const char * cszPacket,int nPacketLen);

	//发送取消切换源或取消发言人命令消息 cancelSendThisSource
	int SendCancelSendThisSourceCommand(const char * cszPacket,int nPacketLen);

	//发送取消切换源或取消发言人命令消息 dropConference
	int SendDropConferenceCommand(const char * cszPacket,int nPacketLen);


	//发送终端号码分配指示消息 terminalNumberAssign
	int SendTerminalNumberAssignIndication(const char * cszPacket,int nPacketLen);

	//发送终端加入会议指示消息 terminalJoinedConference
	int SendTerminalJoinedConferenceIndication(const char * cszPacket,int nPacketLen);
	//发送终端离开会议指示消息 terminalLeftConference
	int SendTerminalLeftConferenceIndication(const char * cszPacket,int nPacketLen);

	//发送至少选看一个其他视频指示消息 seenByAtLeastOneOther
	int SendSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen);
	//发送取消至少选看一个其他视频指示消息 cancelSeenByAtLeastOneOther
	int SendCancelSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen);

	//发送选看所有视频指示消息 seenByAll
	int SendSeenByAllIndication(const char * cszPacket,int nPacketLen);
	//发送取消选看所有视频指示消息 cancelSeenByAll
	int SendCancelSeenByAllIndication(const char * cszPacket,int nPacketLen);
	//发送指定为观看会场消息   terminalYouAreSeeing
	int SendTerminalYouAreSeeingIndication(const char * cszPacket,int nPacketLen);

	//发送请求发言消息 TERM-->MCU
	int SendRequestForFloorIndication(const char * cszPacket,int nPacketLen);
	//发送请求发言消息 MCU-->Chair
	int	SendFloorRequestedIndication(const char * cszPacket,int nPacketLen);

	//发送逻辑通道流控消息 flowControlCommand
	int SendFlowControlCommand(const char * cszPacket,int nPacketLen);
	//发送逻辑通道流控消息 flowControlIndication
	int SendFlowControlIndication(const char * cszPacket,int nPacketLen);
	//发送请求关键帧消息
	int	SendVideoFastUpdatePicture(const char * cszPacket,int nPacketLen);
	//发送请求图像冻结消息 Add VCF
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

	//nValue 视频源ID或预置点
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
