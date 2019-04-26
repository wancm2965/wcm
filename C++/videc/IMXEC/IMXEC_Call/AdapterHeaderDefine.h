
/****************************************************************************************************
                       Copyright (c) 2013 AVCON Ltd.
*****************************************************************************************************
NOTICE: 
Statck Adapter define implementation.
Author:gsh
DATE: 
1. 2013-04-18  gsh create.
2. 
***************************************************************************************************/

#ifndef __IMXEC_ADAPTERDEFINE_H__
#define __IMXEC_ADAPTERDEFINE_H__

//注 编译预处理器中加入ADAPTER_EXPORT 

#ifdef ADAPTER_EXPORT
#define ADAPTER_API _declspec(dllexport)
#elif ADAPTER_DLL
#define ADAPTER_API _declspec(dllimport)
#else
#define ADAPTER_API
#endif

//add
#define DAP_DECLARE_HANDLE(_handleName) \
	typedef struct { int unused; } _handleName##__ ; \
	typedef const _handleName##__ * _handleName; \
	typedef _handleName*  LP##_handleName

DAP_DECLARE_HANDLE(HDAPAPPCALL);


//typedef enum
//{
//	IMXEC_CALL_MEDIA_UNKOWN=0,
//	IMXEC_CALL_MEDIA_AUDIO,              /*音频类型*/
//	IMXEC_CALL_MEDIA_MAINVIDEO,          /*视频类型*/
//	IMXEC_CALL_MEDIA_H239VIDEO,          /*辅流类型*/
//	IMXEC_CALL_MEDIA_H224DATA            /*H224数据类型*/
//
//}IMXEC_CALL_MEDIA_TYPE;

/* H225 RAS message */
typedef enum 
{
	h225RASMessageNone=0,
	h225RASMessageGRQ,									//网守查找消息
	h225RASMessageGCF,									//网守发现接受消息
	h225RASMessageGRJ,									//网守拒绝消息
	h225RASMessageRRQ,									//网守注册消息
	h225RASMessageRCF,									//网守注册接受消息
	h225RASMessageRRJ,									//网守注册拒绝消息
	h225RASMessageARQ,									//网守呼叫接入请求消息
	h225RASMessageACF,									//网守呼叫接入请求接受消息
	h225RASMessageARJ,									//网守呼叫接入请求拒绝消息
	h225RASMessageDRQ,									//网守挂断请求消息
	h225RASMessageDCF,									//网守挂断请求接受消息
	h225RASMessageDRJ,									//网守挂断请求拒绝消息
	h225RASMessageURQ,									//网守注销请求消息
	h225RASMessageUCF,									//网守注销请求接受消息
	h225RASMessageURJ,									//网守注销请求拒绝消息
	h225RASMessageIRQ,									//网守状态查询请求消息
	h225RASMessageIRQTimeOut,							//网守状态查询请求超时消息
	h225RASMessageIRR,									//网守状态查询请求响应消息
	h225RASMessageBRQ,									//网守带宽请求消息
	h225RASMessageBCF,									//网守带宽确认消息
	h225RASMessageBRJ,									//网守带宽拒绝消息
	h225RASMessageLRQ,									//网守定位请求消息
	h225RASMessageLCF,									//网守定位确认消息
	h225RASMessageLRJ									//网守定位拒绝消息
} H225RASMessageType;

/* H245 channel message */
typedef enum  
{
	h245ChanMessageNone=0,
	h245ChanFlowControlCommand,							//码流控制命令
	h245ChanFlowControlIndication						//码流控制指示
} H245ChanMessageType;


/* H245 control message reference cmH245MessageType*/
typedef enum
{
	/* Conference Request */
	h245CONcrqTerminalListRequest,
	h245CONcrqMakeMeChair,
	h245CONcrqCancelMakeMeChair,
	h245CONcrqDropTerminal,
	h245CONcrqRequestTerminalID,
	h245CONcrqEnterH243Password,
	h245CONcrqEnterH243TerminalID,
	h245CONcrqEnterH243ConferenceID,
	h245CONcrqRequestChairTokenOwner,
	h245CONcrqRequestTerminalCertificate,
	h245CONcrqBroadcastMyLogicalChannel,
	h245CONcrqMakeTerminalBroadcaster,
	h245CONcrqSendThisSource,
	h245CONcrqRequestAllTerminalIDs,
	h245CONcrqRemoteMCRequest,

	/* Conference Response*/
	h245CONcrsMCTerminalIDResponse,
	h245CONcrsTerminalIDResponse,
	h245CONcrsConferenceIDResponse,
	h245CONcrsPasswordResponse,
	h245CONcrsTerminalListResponse,
	h245CONcrsVideoCommandReject,
	h245CONcrsTerminalDropReject,
	h245CONcrsMakeMeChairResponse,
	h245CONcrsChairTokenOwnerResponse,
	h245CONcrsTerminalCertificateResponse,
	h245CONcrsBroadcastMyLogicalChannelResponse,
	h245CONcrsMakeTerminalBroadcasterResponse,
	h245CONcrsSendThisSourceResponse,
	h245CONcrsRequestAllTerminalIDsResponse,
	h245CONcrsRemoteMCResponse,

	/* Conference Indications */
	h245CONciSbeNumber,
	h245CONciTerminalJoinedConference,
	h245CONciTerminalLeftConference,
	h245CONciSeenByAtLeastOneOther,
	h245CONciCancelSeenByAtLeastOneOther,
	h245CONciSeenByAll,
	h245CONciCancelSeenByAll,
	h245CONciTerminalYouAreSeeing,
	h245CONciRequestForFloor,
	h245CONciWithdrawChairToken,
	h245CONciFloorRequested,
	h245CONciTerminalYouAreSeeingInSubPictureNumber,
	h245CONciVideoIndicateCompose,

	/* Conference Commands */
	h245CONccBroadcastMyLogicalChannel,
	h245CONccCancelBroadcastMyLogicalChannel,
	h245CONccMakeTerminalBroadcaster,
	h245CONccCancelMakeTerminalBroadcaster,
	h245CONccSendThisSource,
	h245CONccCancelSendThisSource,
	h245CONccDropConference,
	h245CONccSubstituteConferenceIDCommand,

	/* Miscellaneous Command */
	h245CONmcEqualiseDelay,
	h245CONmcZeroDelay,
	h245CONmcMaxH223MUXPDUsize,
	h245CONmcEncryptionUpdate,
	h245CONmcEncryptionUpdateRequest,
	h245CONmcProgressiveRefinementStart,
	h245CONmcProgressiveRefinementAbortOne,
	h245CONmcProgressiveRefinementAbortContinuous,
	h245CONmcVideoBadMBs,
	h245CONmcLostPicture,
	h245CONmcLostPartialPicture,
	h245CONmcRecoveryReferencePicture,

	/* Indication */
	h245CONiJitterIndication,
	h245CONiH223SkewIndication,
	h245CONiFunctionNotSupported,
	h245CONiFlowControlIndication,
	h245CONiVendorIdentification,

	/* Miscellaneous Indication */
	h245CONmiMultipointZeroComm,
	h245CONmiCancelMultipointZeroComm,
	h245CONmiMultipointSecondaryStatus,
	h245CONmiCancelMultipointSecondaryStatus,
	h245CONmiVideoIndicateReadyToActivate,
	h245CONmiVideoNotDecodedMBs,

	/* Generic Messages */
	h245CONgrqGenericRequest,
	h245CONgrsGenericResponse,
	h245CONgcGenericCommand,
	h245CONgiGenericIndication,

	h245CONciTerminalNumberAssign, //add
	h245CONcFlowControlCommand,    //add
	h245CONcVideoFastUpdatePicture,//add
	h245CONCameraControl           //add

} H245ControlMessageType;


/*H245 Logical channel Reject Reason  reference cmRejectLcnReason*/
typedef enum
{
	ChanRejectReasonUnspecified=0,
	ChanRejectReasonUnsuitableReverseParameters,
	ChanRejectReasonDataTypeNotSupported,
	ChanRejectReasonDataTypeNotAvailable,
	ChanRejectReasonUnknownDataType,
	ChanRejectReasonDataTypeALCombinationNotSupported,
	ChanRejectReasonMulticastChannelNotAllowed,
	ChanRejectReasonInsufficientBandwidth,
	ChanRejectReasonSeparateStackEstablishmentFailed,
	ChanRejectReasonInvalidSessionID,
	ChanRejectReasonMasterSlaveConflict,
	ChanRejectReasonWaitForCommunicationMode,
	ChanRejectReasonInvalidDependentChannel,
	ChanRejectReasonReplacementForRejected,
	ChanRejectReasonSecurityDenied,
	ChanRejectReasonLast
} H245ChanRejectReason;


/*
//PTZ[摄像机遥控命令]
typedef enum 
{
	PTZ_START_PAN_UP=0,		//开始向上
	PTZ_STOP_PAN_UP,		//停止向上
	PTZ_START_PAN_DOWN,		//开始向下
	PTZ_STOP_PAN_DOWN,		//停止向下
	PTZ_START_TILT_LEFT,	//开始向左
	PTZ_STOP_TILT_LEFT,		//停止向左
	PTZ_START_TILT_RIGHT,	//开始向右
	PTZ_STOP_TILT_RIGHT,	//停止向右
	PTZ_START_ZOOM_IN,		//开始放大
	PTZ_STOP_ZOOM_IN,		//停止放大
	PTZ_START_ZOOM_OUT,		//开始缩小
	PTZ_STOP_ZOOM_OUT,		//停止缩小
	PTZ_START_FOCUS_IN,		//开始向近处聚焦
	PTZ_STOP_FOCUS_IN,		//停止向近处聚焦
	PTZ_START_FOCUS_OUT,	//开始向远处聚焦
	PTZ_STOP_FOCUS_OUT,		//停止向远处聚焦
	PTZ_START_FOCUS_AUTO,	//开始自动聚焦
	PTZ_STOP_FOCUS_AUTO,	//停止自动聚焦
	PTZ_START_IRIS_MINUS,	//开始光圈变小
	PTZ_STOP_IRIS_MINUS,	//停止光圈变小
	PTZ_START_IRIS_PLUS,	//开始光圈变大
	PTZ_STTOPIRIS_PLUS,		//停止光圈变大
} PTZ_ACTION;

*/


#endif //__IMXEC_ADAPTERDEFINE_H__