
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

//ע ����Ԥ�������м���ADAPTER_EXPORT 

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
//	IMXEC_CALL_MEDIA_AUDIO,              /*��Ƶ����*/
//	IMXEC_CALL_MEDIA_MAINVIDEO,          /*��Ƶ����*/
//	IMXEC_CALL_MEDIA_H239VIDEO,          /*��������*/
//	IMXEC_CALL_MEDIA_H224DATA            /*H224��������*/
//
//}IMXEC_CALL_MEDIA_TYPE;

/* H225 RAS message */
typedef enum 
{
	h225RASMessageNone=0,
	h225RASMessageGRQ,									//���ز�����Ϣ
	h225RASMessageGCF,									//���ط��ֽ�����Ϣ
	h225RASMessageGRJ,									//���ؾܾ���Ϣ
	h225RASMessageRRQ,									//����ע����Ϣ
	h225RASMessageRCF,									//����ע�������Ϣ
	h225RASMessageRRJ,									//����ע��ܾ���Ϣ
	h225RASMessageARQ,									//���غ��н���������Ϣ
	h225RASMessageACF,									//���غ��н������������Ϣ
	h225RASMessageARJ,									//���غ��н�������ܾ���Ϣ
	h225RASMessageDRQ,									//���عҶ�������Ϣ
	h225RASMessageDCF,									//���عҶ����������Ϣ
	h225RASMessageDRJ,									//���عҶ�����ܾ���Ϣ
	h225RASMessageURQ,									//����ע��������Ϣ
	h225RASMessageUCF,									//����ע�����������Ϣ
	h225RASMessageURJ,									//����ע������ܾ���Ϣ
	h225RASMessageIRQ,									//����״̬��ѯ������Ϣ
	h225RASMessageIRQTimeOut,							//����״̬��ѯ����ʱ��Ϣ
	h225RASMessageIRR,									//����״̬��ѯ������Ӧ��Ϣ
	h225RASMessageBRQ,									//���ش���������Ϣ
	h225RASMessageBCF,									//���ش���ȷ����Ϣ
	h225RASMessageBRJ,									//���ش���ܾ���Ϣ
	h225RASMessageLRQ,									//���ض�λ������Ϣ
	h225RASMessageLCF,									//���ض�λȷ����Ϣ
	h225RASMessageLRJ									//���ض�λ�ܾ���Ϣ
} H225RASMessageType;

/* H245 channel message */
typedef enum  
{
	h245ChanMessageNone=0,
	h245ChanFlowControlCommand,							//������������
	h245ChanFlowControlIndication						//��������ָʾ
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
//PTZ[�����ң������]
typedef enum 
{
	PTZ_START_PAN_UP=0,		//��ʼ����
	PTZ_STOP_PAN_UP,		//ֹͣ����
	PTZ_START_PAN_DOWN,		//��ʼ����
	PTZ_STOP_PAN_DOWN,		//ֹͣ����
	PTZ_START_TILT_LEFT,	//��ʼ����
	PTZ_STOP_TILT_LEFT,		//ֹͣ����
	PTZ_START_TILT_RIGHT,	//��ʼ����
	PTZ_STOP_TILT_RIGHT,	//ֹͣ����
	PTZ_START_ZOOM_IN,		//��ʼ�Ŵ�
	PTZ_STOP_ZOOM_IN,		//ֹͣ�Ŵ�
	PTZ_START_ZOOM_OUT,		//��ʼ��С
	PTZ_STOP_ZOOM_OUT,		//ֹͣ��С
	PTZ_START_FOCUS_IN,		//��ʼ������۽�
	PTZ_STOP_FOCUS_IN,		//ֹͣ������۽�
	PTZ_START_FOCUS_OUT,	//��ʼ��Զ���۽�
	PTZ_STOP_FOCUS_OUT,		//ֹͣ��Զ���۽�
	PTZ_START_FOCUS_AUTO,	//��ʼ�Զ��۽�
	PTZ_STOP_FOCUS_AUTO,	//ֹͣ�Զ��۽�
	PTZ_START_IRIS_MINUS,	//��ʼ��Ȧ��С
	PTZ_STOP_IRIS_MINUS,	//ֹͣ��Ȧ��С
	PTZ_START_IRIS_PLUS,	//��ʼ��Ȧ���
	PTZ_STTOPIRIS_PLUS,		//ֹͣ��Ȧ���
} PTZ_ACTION;

*/


#endif //__IMXEC_ADAPTERDEFINE_H__