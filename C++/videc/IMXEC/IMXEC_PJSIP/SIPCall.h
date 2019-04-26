#ifndef __SIPCALL_H__
#define __SIPCALL_H__

#include <string>

#include "../DefineHeader/SIPDefineHeader.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <MPVideo/MPVideo.h>
#include <AVCONMP/IMXEC_Call.h>
#else

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPSIP.h>
#else
#include <IMXEC/IMXEC_SIP.h>
#endif

#include <VIDEC/VIDEC.h>
#include <IMXEC/IMXEC_Call.h>

#endif


#include <AUDEC/AUDEC_CodecID.h>
#include "VideoRTPSession.h"
#include "AudioRTPSession.h"
#include "XAutoLock.h"
#include "SIPAPIType.h"
#include "pjsip.h" 
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "SIPCallCapability.h"
#include "../SIPBFCP/SIPBFCP.h"
#include "../IMXEC_H323/EncryptionCapability.h"

//#define VideoFrameBaseSIP  30.0 

class SIPCall;

class DualVideoRTPSessionCallback
	: public VideoRTPSessionCallback
{
public:
	DualVideoRTPSessionCallback(void){m_pSipCall=NULL;};
	virtual~DualVideoRTPSessionCallback(void){};
public:
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen);
	virtual void OnVideoRTPSessionCallbackRequestKeyFrame(void);
public: 
	void SetSipCall(SIPCall * pSipCall){m_pSipCall=pSipCall; };
	SIPCall * m_pSipCall;

};

class SIPUser;
class SIPCall 
	: public VideoRTPSessionCallback
	, public AudioRTPSessionCallback
	, public SIP_BFCPCallback
{
public:
	SIPCall(SIPUser&rSIPUser,const std::string&strCallID,const std::string&strLocalIP,unsigned short nLocalAudioPort,unsigned short nLocalVideoPort,ConfInfo*pConfInfo);
	virtual ~SIPCall(void);
public:
	typedef enum
	{
		CALL_DIR_IN=0,
		CALL_DIR_OUT,
		CALL_DIR_COUNT,
	}CALL_DIRECTION;

	typedef enum
	{
		CALL_STATUS_IDLE=0,
		CALL_STATUS_RING,
		CALL_STATUS_ESTABLISHED,
	}CALL_STATUS;
public:
	virtual void Close(void);
	virtual void CloseMedia(void){};
	virtual int AcceptCall(ConfInfo *pConfInfo=NULL);
	virtual int HangupCall(void);
	virtual void OnMediaUpdate(const std::string&strRemoteIP,unsigned short nRemoteAudioPort,int nAudioPayloadType,const std::string&strAudioPayloadName,int nAudioSampleRate,X_AUDIO_CODEC_ID AudioCodecID,unsigned short nRemoteVideoPort,int nVideoPayloadType,const std::string&strVideoPayloadName,int nVideoBitrate,int nVideoWidth,int nVideoHeight);

	int CheckMediaHangupCall(void); //hungup sip protocol call
	virtual int HangupSIPCall(void);					//hungup sip protocol call

	//����DTMF
	int SendDTMF(const char dtmf,int nType=1);
	int GetHasVideo(void);

	virtual const std::string&GetCallID(void); 
	virtual int GetLineID(void);
	virtual SIPCall::CALL_DIRECTION GetCallDirection(void);
	virtual SIPCall::CALL_STATUS GetCallStatus(void);
	virtual const std::string&GetLocalIP(void);
	virtual void SetLocalIP(const std::string&strLocalIP);
	virtual unsigned short GetLocalAudioPort(void);
	virtual void SetLocalAudioPort(unsigned short nLocalAudioPort);
	virtual unsigned short GetLocalVideoPort(void);
	virtual void SetLocalVideoPort(unsigned short nLocalVideoPort);
	virtual const std::string&GetUserID(void);
	virtual void SetUserID(const std::string&strUserID);
	virtual const std::string&GetUserName(void);
	virtual void SetUserName(const std::string&strUserName);

	virtual void OnCallEstablished(void);

	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen);

	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;

	virtual int OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)=0;
	virtual int OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)=0;

	virtual int OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nNetworkType=VIDEC_NETWORK_PRIVATE)=0;
	virtual int OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType)=0;

	virtual void OnSecondVideoRecvChannelConnected(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType){};
	virtual void OnSecondVideoRecvChannelDestroyed(void){};
	
	virtual void OnSecondVideoSendChannelConnected(void){};
	virtual void OnSecondVideoSendChannelDestroyed(void){};

	virtual const unsigned char GetUserIDIndex(void);
	virtual void SetUserIDIndex(const unsigned char nUserIDIndex);

	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir);

	virtual void OnLocalSecondVideoStarted();
	virtual void OnLocalSecondVideoStop();
	virtual int GetSupportSecondVideo(void);

	void SendGenericRequest();
	void SendGenericResponse(unsigned short transactionId,unsigned short userId,unsigned short floorId,unsigned short status);
	void SendGenericCommand();
	void OnRecvGenericRequest( unsigned short transactionId,unsigned short userId,unsigned short floorId);
	void OnRecvGenericCommand( unsigned short transactionId,unsigned short userId,unsigned short floorReqId);
	void OnRecvGenericResponse( unsigned short floorReqId, unsigned short reqStatus);
	void SendBFCPHello();		//Hello
	void SendBFCPHelloACK();	//Hello ack

protected:
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen);
	virtual void OnVideoRTPSessionCallbackRequestKeyFrame(void);
	virtual void OnAudioRTPSessionCallbackRTPPacket(AudioRTPSession*pAudioRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
protected:
	SIPUser&					m_rSIPUser;
	std::string					m_strLocalIP;
	std::string					m_strRemoteLastIP;
	unsigned char				m_nUserIDIndex;
	std::string					m_strUserID;
	std::string					m_strUserName;
	CALL_STATUS					m_CallStatus;

	XCritSec					m_csVideoRTPSession;
	VideoRTPSession*			m_pVideoRTPSession;
	XCritSec					m_csAudioRTPSession;
	AudioRTPSession*			m_pAudioRTPSession;
	unsigned long				m_nLastSentAudioRTPTimestamp;

	XCritSec					m_csDualVideoRTPSession;
	VideoRTPSession*			m_pDualVideoRTPSession;

	unsigned long				m_nDTMFTimestamp;
	unsigned short				m_nDTMFSeqNo;
	unsigned long				m_nDTMFSSRC;

public:
	bool						m_bAccept;									//accept call state 20120315
	bool						m_bHunguped;

public:
	SIPVideoCap					m_RemoteVideoCap;
	SIPAudioCap					m_RemoteAudioCap;
	SIPDualVideoCap				m_RemoteDualVideoCap;
	SIPDataCap					m_RemoteDataCap;

	EBFCPFloorControlType		m_BfcpFloorControlType;						//remote bfcp info
	EBFCPSetupType				m_BfcpSetupType;							//remote bfcp info
	
	EBFCPTransportType			m_nBFCPTransportType;						//remote bfcp info
	int							m_nBFCPConfId;								//remote bfcp info conference id
	int							m_nBFCPUserId;								//remote bfcp info
	int							m_nBFCPFloorId;								//remote bfcp info
	int							m_nBFCPMediaLabel;							//remote bfcp info
	int							m_nDualVideoLabel;							//remote bfcp info
	int							m_nVideoLabel;								//remote bfcp info
	int							m_nBFCPTransportPort;						//remote bfcp info
	int							m_nBFCPLastTransportPort;

	unsigned int				m_nRemoteVideoPort;
	unsigned int				m_nRemoteVideoRTCPPort;
	unsigned int				m_nRemoteAudioPort;
	unsigned int				m_nRemoteAudioRTCPPort;

	int							m_nDualVideoDevID;
	int							m_nDualVideoFrameRate;

	int							m_nDualVideoPayloadType;
	std::string					m_strDualVideoPayloadName;
	int							m_nDualVideoBitrate;
	int							m_nDualVideoWidth;
	int							m_nDualVideoHeight;
	unsigned int				m_nRemoteDualVideoPort;
	unsigned int				m_nRemoteDualVideoRTCPPort;

	unsigned int				m_nRemoteDataPort;
	unsigned int				m_nRemoteDataRTCPPort;
	IMXEC_CALL_DIRECTION		m_nSecondVideoDir;
	bool						m_bHasRemoteH239Video;
	bool						m_bH239ReceiverOpen;

	//int							m_nNegVideoCapLocation;			//SDPЭ�̺�ѡ����type��m_RemoteVideoCap�е�λ��
	//int							m_nNegDualVideoCapLocation;
	//int							m_nNegAudioCapLocation;
	//int							m_nNegDataCapLocation;

	XCritSec					m_csCapNeg;						//��ȡЭ�̽�����������
	//SIPVIDEOCAPBILITY			m_CapNegVideo;					//����Э�̽��������
	//SIPVIDEOCAPBILITY			m_CapNegDualVideo;
	//SIPAUDIOCAPBILITY			m_CapNegAudio;
	//SIPDATACAPBILITY			m_CapNegData;
	bool						m_bNegCapVideo;					//Э�̽��
	bool						m_bNegCapDualVideo;
	bool						m_bNegCapAudio;
	bool						m_bNegCapData;

	char						m_strRemoteIP[64];
	int							m_nCallId;						//pjsip use to hungup call id 
	int							m_nLineId;						//pjsip use to hungup call id 
	std::string					m_strCallID;
	bool						m_bRemoteIsMaster;				//Զ��SDP����
	int							m_nRemoteAudioReliableRTP;
	int							m_nRemoteVideoReliableRTP;
	int							m_nRemoteDualVideoReliableRTP;
	int							m_nRemoteDataReliableRTP;

	int							m_nAudioReliableRTP;			//Э�� Ĭ�ϱ����г�ʼΪ1�������ʼ��Ϊ0��SDPȥ������a=reliablertp:fec-enable
	int							m_nVideoReliableRTP;
	int							m_nDualVideoReliableRTP;
	int							m_nDataReliableRTP;
	DualVideoRTPSessionCallback m_DualVideoRTPCallBack;

	unsigned long				m_nMainVideoPacketTimestamp;
	unsigned long				m_nSecondVideoPacketTimestamp;
	unsigned short				m_nLocalDualVideoPort;
	unsigned short				m_nLocalDualVideoRtcpPort;
	unsigned short				m_nLocalBFCPTransPort;
	unsigned int				m_nRequestConfId;					//local bfcp info
	unsigned short				m_nRequestUserId;					//local bfcp info
	unsigned short				m_nRequestFloorId;					//local bfcp info
	unsigned short				m_nRequestPriority;					//local bfcp info
	EBFCPTransportType			m_nRequestTransportType;			//local bfcp info
	EBFCPFloorControlType		m_nRequestFloorControlType;			//local bfcp info
	EBFCPSetupType				m_nRequestSetupType;				//local bfcp info
	SIP_BFCP*					m_pBFCPUDP;
	SIP_BFCP*					m_pBFCPTCPClient;
	SIP_BFCP*					m_pBFCPTCPServer;
	SIP_BFCP*					m_pBFCPTCP;
	SIP_BFCP*					m_pBFCPPJSIPUDP;


public:
	//SDP Э��
	int SDP_Negotiate() ;
	int GetLocalVideoCodecType(bool first,VIDEC_CODEC_TYPE type);
	int GetLocalAudioCodecType(bool first,X_AUDIO_CODEC_ID type);
	int GetLocalDualVideoCodecType(bool first,VIDEC_CODEC_TYPE type);
	int GetLocalH224CodecType(bool first,DATA_CODEC_ID type);

	void OnDualVideoRTPClose();
	int StartSecondVideo (void);
	int StopSecondVideo (void);

	void OnMediaUpdate();
	int  OnOffer0(pjmedia_sdp_session **p_sdp);
	bool GetAudioRemoteCodecTypeFromPalyloadType(X_AUDIO_CODEC_ID &type,int PT);
	bool GetVideoRemoteCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int PT);

	//SDPЭ�̺�ѡ����type ���ڽ��յ�Invite SDP���Ӧ��
	void AddAnswerAudioCodecType(SIPCallCapability & answerCap);
	void AddAnswerVideoCodecType(SIPCallCapability & answerCap);
	void AddAnswerDualVideoCodecType(SIPCallCapability & answerCap);
	void AddAnswerDataCodecType(SIPCallCapability & answerCap);

	bool IsCallEstablished();
	void SetPJSIPCallID(int call_id);
	void SetLineID(int LineID);
	void SetRemoteIP(char * ip=NULL);
	void SetRemoteVideoPort(int RtpPort,int RtcpPort);
	void SetRemoteAudioPort(int RtpPort,int RtcpPort);
	void SetRemoteDualVideoPort(int RtpPort,int RtcpPort);

	//accept call state
	void SetAcceptCallState(bool bState);
	void SetLocalDualVideoPort(int RtpPort,int RtcpPort);
	void SetLocalBFCPTransPort(int nBfcpPort);
	bool CreateLocalSDP(pj_pool_t *pool,pjmedia_sdp_session **p_sdp,int nSupportDual);


	SIPVideoCap &GetSIPCallVideoCap(void);
	SIPAudioCap &GetSIPCallAudioCap(void);

	//����Զ��SDP
	pj_bool_t ParseRemoteSDP(const pjmedia_sdp_session *sdp);
	void GetAllPayloadTypeFromAttr(pjmedia_sdp_media *m,int payloadTypeArray[], unsigned & ptCount);

	//bfcp transport
	void CreateBFCPTransports(EBFCPTransportType type=e_BFCP_Transport_UDP);
	void CloseBFCPTransports();

public:
	//set bfcp local info
	void SetBFCPUserId(int userId);
	void SetBFCPConfId(int confId);
	void SetBFCPFloorId(int floorId);
	void SetBFCPRequestPriority(unsigned short priority);
	void SetBFCPSetupType(EBFCPSetupType type);
	void SetBFCPFloorControlType(EBFCPFloorControlType type);
	void SetBFCPTransportType(EBFCPTransportType type);

public:
	virtual void OnDualVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnDualVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen);
	virtual void OnDualVideoRTPSessionCallbackRequestKeyFrame(void);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp){};

public:
	virtual void OnSIPBFCPTCPConnected(EBFCPTCPCONType nType);
	virtual void OnSIPBFCPCallbackFloorRequest(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorId);
	virtual void OnSIPBFCPCallbackFloorRequestStatus(unsigned short nFloorReqId, unsigned short nReqStatus);
	virtual void OnSIPBFCPCallbackFloorRelease(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorReqId);
	virtual void OnSIPBFCPCallbackHello();

public:

	virtual int NotAcceptCall(int callId);
	pj_status_t SetSipCallHold(int callId);
	pj_bool_t OnRecvInviteRequest( pjsip_rx_data *rdata ,int nFindSDP);
	virtual void SetInvSessionUAC(pjsip_inv_session  *pInvSessionUac);
	virtual void SetInvSessionUAS(pjsip_inv_session  *pInvSessionUas);
	virtual void SetSIPDialog(pjsip_dialog * pSipDlg);
	virtual void SetCallDirection(CALL_DIRECTION dir);
	virtual void GetAudioCodecInfo(X_AUDIO_CODEC_ID nAudioCodecID,int & nSamplesPerFrame);
	AudioRTPSession* GetOpenAudioRTPSession();						//use ice
	VideoRTPSession* GetOpenVideoRTPSession();						//use ice
	VideoRTPSession* GetOpenDualVideoRTPSession();					//use ice

	void CopySIPSDP(bool bLocal,pjmedia_sdp_session *sdp);			//bLocal true is local sdp,false is remote sdp
	void OnICETransCheckComplete(pjmedia_transport * m_pMediaTransport,int nMediaType);
	//���Է���û��ICE���ԣ�nMediaIndexý������������0��ʼ,��û�з���false,�з���true
	bool ICECheckCand(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,pjmedia_transport * pMediaTransport=NULL);
	void MediaOpenSetState();
	//�Ƚ�pMediaTransport���Ա�Ƿ���ͬ������ý������,û����ͬ�ķ���0
	int CompareMediaTransport(pjmedia_transport * pMediaTransport);
	void CloseRingTone();

	pj_bool_t OnRecvUpdateRequest( pjsip_rx_data *rdata ,int nFindSDP);
	pj_bool_t Send200SDPResponse( pjsip_rx_data *rdata ,int nFindSDP,pjmedia_sdp_session *sdp);

	//Send update request message.
	pj_bool_t SIPSendUpdateRequest(pjmedia_sdp_session *sdp);
	int GenerateSDP(pjmedia_sdp_session **p_sdp);

	//Send SUBSCRIBE request message.
	pj_bool_t SIPSendSubscribeRequest(pjmedia_sdp_session *sdp=NULL);

	//Send update request recv response 180 200
	void Send200ACKNoSDP();
	void SetSDPNegStateDone();
	void CheckZeroSetBitrate(int nWidth ,int nHeight,int & nBitrate);

	//Send INFO request message.
	pj_bool_t SIPSendInfoRequest(char * content,char * mimeType);
	pj_bool_t SIPMainVideoSendPictureFastUpdate();
	pj_bool_t SIPDualVideoSendPictureFastUpdate();
	pj_bool_t SIPSendPRACKRequest(pjmedia_sdp_session *sdp);
	pj_bool_t SIPSendACKRequest(pjsip_rx_data * rdata,int type,int st_code);//1 request 2 response

public:
	pjsip_inv_session  *		m_pInvSessionUAC;
	pjsip_inv_session  *		m_pInvSessionUAS;
	pjsip_dialog *				m_pSipDialog;
	pjmedia_sdp_session *		m_pCallLocalSDP;
	pjmedia_sdp_session *		m_pCallRemoteSDP;
	pj_pool_t *					m_pPOOL;
	pjmedia_transport *			m_pAudioTransport;
	pjmedia_transport *			m_pVideoTransport;
	pjmedia_transport *			m_pDualVideoTransport;
	int							m_nAudioOpenState;						//ý���״̬ ��ֵ̬ 0��ʼ�� ,1 �м�״̬ ,2 ���Դ�״̬
	int							m_nVideoOpenState;						//ý���״̬ ��ֵ̬ 0��ʼ�� ,1 �м�״̬ ,2 ���Դ�״̬
	int							m_nDualVideoOpenState;					//ý���״̬ ��ֵ̬ 0��ʼ�� ,1 �м�״̬ ,2 ���Դ�״̬
	int 						m_nMediaCount;							//ý����������
	bool						m_bSendUpdateSDP;
	pjsip_tx_data *				m_pSend180Data;
	pjsip_tx_data *				m_pSend200Data;

	int							m_nInviteFindSDP;						//����յ���invite�Ƿ�����SDP ��Ϊ1��û��Ϊ0
	int							m_nRecvDV;								//��������ʹ�����ƽ���ʱ���յ��������ݼ���,���ڱ�ʾ�򿪸�����ʾ���ڵ�
	CALL_DIRECTION				m_CallDirection;
	unsigned short				m_nLocalAudioPort;
	unsigned short				m_nLocalVideoPort;
	int							m_nRSeq;

	//use to zhongxing sip dualvideo token interactive
public:
	int							m_nRecvWithdraw;
	std::string					m_strCalledID;

	pj_bool_t SIPSendApplyDualVideoToken();
	pj_bool_t SIPSendReleaseDualVideoToken();
	pj_bool_t SIPSendPermitDualVideoToken();
	pj_bool_t SIPSendWithdrawDualVideoToken();
	void OnRecvSIPDualVideoTokenApply(int value=0);
	void OnRecvSIPDualVideoTokenRelease(int value=0);
	void OnRecvSIPDualVideoTokenPermit(int value=0);
	void OnRecvSIPDualVideoTokenWithdraw(int value=0);
	void SetCalledID(std::string strCalledID);


public:

	SIPCallCapability &		GetMgrCapability(void);
	int						GetMgrMainVideoMaxBitrate(void);
	int						GetMgrSecondVideoMaxBitrate(void);
	void					SetSupportH239(int nSupportH239);
	int						GetMgrSupportH239(void);
	VIDEO_FORMAT_TYPE		GetMgrMainVideoFormatType(void);
	VIDEO_FORMAT_TYPE		GetMgrSecondVideoFormatType(void);
	int						SetLocalCapabilitis( SIPCallCapability & sdpCap,bool bAddG7221C=true,int nSupportDual =1);
	void					AssignLocalMediaRTPPort(int nSupportDual);
	int						GenerateLocal183SDP(pjmedia_sdp_session **p_sdp,int nSupportDual=1);

public:

	/******************************************************************************
	* SetLocalMainVideoH264Level
	*���������ñ���������ƵH264����ʹ�õ����Levelֵ ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	
			nLevel			-��������ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoH264Level(int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264Level
	*���������ñ��ظ�����ƵH264����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	
			nLevel			-���ظ�����ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalSecondVideoH264Level(int nLevel=0);

	/******************************************************************************
	* SetLocalMainVideoH264HPLevel
	*���������ñ���������ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	
			nLevel			-��������ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoH264HPLevel(int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264HPLevel
	*���������ñ��ظ�����ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	
			nLevel			-���ظ�����ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalSecondVideoH264HPLevel(int nLevel=0);

	/******************************************************************************
	* SetRTPMediaMulticast 
	*����������RTP ý�����鲥 ���������鲥IP��������鲥
	*���룺 cszMulticastIP	-�鲥IP
			ttl				-�鲥���ݰ���TTL(����ʱ��)
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRTPMediaMulticast(const char*cszMulticastIP,int ttl);

	/******************************************************************************
	*���������͸�������������Ϣ
	*���룺	
	nType			-��������������Ϣ���� 1 ���� 2 ��Ӧ 3 ָʾ 4 ȡ��
	transactionId	-����ID  
	userId			-�û�ID  
	floorId			-����������ID 
	status			-״̬ �緢����Ӧʱ 0��ʾ�ܾ� 1��ʾ����
	*****************************************************************************/
	int SendDualVideoControlMsg(int nType,int transactionId=0,int userId=0,int floorId=0,int status=0);

private:
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH263;
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH264;
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH264HP;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH263;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH264;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH264HP;

public:
	ConfInfo				m_ConfInfo;												//���ڱ���MCU������������Ϣ
	SIPCallCapability		m_sdpCap;
	int						m_nSupportH239;
	int						m_nMainVideoBitrate;

	int						m_nSecondVideoMaxBitrate;
	int						m_nLocalMainVideoH264Level;
	int						m_nLocalSecondVideoH264Level;
	int						m_nLocalMainVideoH264HPLevel;
	int						m_nLocalSecondVideoH264HPLevel;
	int						m_nMainVideoMaxFrame;
	int						m_nSecondVideoMaxFrame;
	bool					m_bAudioCapItemSetFirst;								//����������������״̬ ���������������
	bool					m_bVideoCapItemSetFirst;								//����������������״̬ ���������������
	bool					m_bDualVideoCapItemSetFirst;							//����������������״̬ ���������������
	VIDEO_FORMAT_TYPE		m_nMainVideoFormatType;									//����������Ƶ���õ����ֱ���
	VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;								//���˸�����Ƶ���õ����ֱ���
	std::string				m_strMediaMulticastIP;									//ý�����鲥��ַ
	bool					m_bMediaMulticastEnable;								//ý���������鲥 true���� false������ Ĭ��false
	int						m_nMediaMulticastTTL;									//�鲥���ĵ����ݰ���TTL(����ʱ��) 0..255


#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	/******************************************************************************
	* AddCapAudioAll
	*�����������Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioAll(void);

	/******************************************************************************
	* RemoveCapAudioAll
	*�������Ƴ���Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapAudioAll(void);

	/******************************************************************************
	* AddCapG711A_64K
	*�����������ƵG711A������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG711A_64K(void);

	/******************************************************************************
	* AddCapG711U_64K
	*�����������ƵG711U������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG711U_64K(void);

	/******************************************************************************
	* AddCapAudioG722_64K
	*�����������ƵG722-64K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG722_64K(void);

	/******************************************************************************
	* AddCapAudioG7221_24K
	*�����������ƵG7221-24K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221_24K(void);

	/******************************************************************************
	* AddCapAudioG7221_32K
	*�����������ƵG7221-32K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221_32K(void);

	/******************************************************************************
	* AddCapAudioG7221C_24K
	*�����������ƵG7221C-24K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_24K(void);

	/******************************************************************************
	* AddCapAudioG7221C_32K
	*�����������ƵG7221C-32K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_32K(void);

	/******************************************************************************
	* AddCapAudioG7221C_48K
	*�����������ƵG7221C-48K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_48K(void);

	/******************************************************************************
	* AddCapAudioG729A
	*�����������ƵG729A������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG729A(void);

	/******************************************************************************
	* AddCapAudioG719_32K
	*�����������ƵG719-32K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_32K(void);

	/******************************************************************************
	* AddCapAudioG719_48K
	*�����������ƵG719-48K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_48K(void);

	/******************************************************************************
	* AddCapAudioG719_64K
	*�����������ƵG719_64K������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_64K(void);

	/******************************************************************************
	* AddCapVideoAll
	*���������������Ƶ������������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoAll(void);

	/******************************************************************************
	* RemoveCapVideoAll
	*�������Ƴ�������Ƶ������������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapVideoAll(void);

	/******************************************************************************
	* AddCapH263
	*���������������ƵH263������
	*���룺	
			nHasQCIF		-ʹ��QCIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasCIF			-ʹ��CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas4CIF		-ʹ��4CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263(int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapVideoH263Plus
	*���������������ƵH263+������
	*���룺	
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263Plus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH263PlusPlus
	*���������������ƵH263++������
	*���룺	
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263PlusPlus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH264QCIF
	*���������������Ƶ��H264QCIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264QCIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264CIF
	*���������������Ƶ��H264CIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264CIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2644CIF
	*���������������Ƶ��H264 4CIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH2644CIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264720P
	*���������������Ƶ��H264720P������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264720P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2641080P
	*���������������Ƶ��H2641080P������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH2641080P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP720P
	*���������������Ƶ��H264HighProfile������,Ĭ�����720P
	*���룺	
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264HP720P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP1080P
	*���������������Ƶ��H264HighProfile������,Ĭ�����1080P
	*���룺	
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264HP1080P(int nLevel=0);


	/******************************************************************************
	* AddCapDualVideoAll
	*��������Ӹ�����Ƶ������������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoAll(void);

	/******************************************************************************
	* RemoveCapDualVideoAll
	*�������Ƴ�������Ƶ������������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapDualVideoAll(void);

	/******************************************************************************
	* AddCapDualVideoH263
	*��������Ӹ�����ƵH263������
	*���룺	
			nHasQCIF		-ʹ��QCIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasCIF			-ʹ��CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas4CIF		-ʹ��4CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263(int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapDualVideoH263Plus
	*��������Ӹ�����ƵH263+������
	*���룺	
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263Plus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH263PlusPlus
	*��������Ӹ�����ƵH263++������
	*���룺	
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263PlusPlus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH264QCIF
	*��������Ӹ�����Ƶ��H264QCIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264QCIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264CIF
	*��������Ӹ�����Ƶ��H264CIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264CIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2644CIF
	*��������Ӹ�����Ƶ��H264 4CIF������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH2644CIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264720P
	*��������Ӹ�����Ƶ��H264720P������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264720P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2641080P
	*��������Ӹ�����Ƶ��H2641080P������
	*���룺	
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH2641080P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP720P
	*��������Ӹ�����Ƶ��H264HighProfile������,Ĭ�����720P
	*���룺	
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264HP720P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP1080P
	*��������Ӹ�����Ƶ��H264HighProfile������,Ĭ�����1080P
	*���룺	
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264HP1080P(int nLevel=0);

	int AddCapDualVideoH264VGA(int nLevel=0);
	int AddCapDualVideoH264SVGA(int nLevel=0);
	int AddCapDualVideoH264XGA(int nLevel=0);
	int AddCapDualVideoH264SXGA(int nLevel=0);

	int AddCapVideoH264VGA(int nLevel=0);
	int AddCapVideoH264SVGA(int nLevel=0);
	int AddCapVideoH264XGA(int nLevel=0);
	int AddCapVideoH264SXGA(int nLevel=0);


	/******************************************************************************
	* AddCapFECCData
	*���������FECC����������
	*���룺
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapFECCData(void);

	/******************************************************************************
	* RemoveCapAll
	*�������Ƴ�����ý�����͵�����������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapAll(void);

	/******************************************************************************
	* RemoveCapFECCDataAll
	*�������Ƴ�FECC��������������
	*���룺	
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapFECCDataAll(void);

	int DAPSetLocalCapabilitis(void);

	int AddCapAudioItem(X_AUDIO_CODEC_ID codecID);
	int AddCapVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);
	int AddCapDualVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);

#endif

	void CopyVideoCapOtherParam0(ConfInfo *pConfInfo,int setBitrate,int setFormat);
	void AllowToSendSecondVideo(bool bAllow); // ����Զ˷���˫��

protected:
	int m_nMainVideoPayloadType;
	std::string m_strMainVideoPayloadName;
	int m_nMainVideoNegBitrate;
	int m_nMainVideoWidth;
	int m_nMainVideoHeight;
	int m_nRemoteMainVideoFrame;		//�Է����֡����
	int m_nRemoteSecondVideoFrame;		//�Է����֡����

public:
	//notice test , if config,only use the config cap add
	void               AddCodecCapFromConfig(SIPCallCapability & sdpCap);
	//���ں�ͨ���ٴ�ͨ��
	void                SaveCallMediaUpdatePacket(std::string & strMediaText);
	std::string &       GetCallMediaUpdatePacket();
	void                OnChannelRequestKeyFrame(int type);  //1 MainVideo , 2 DualVideo
	virtual void        OnRecvdMainVideoRequestKeyFrame(void){};
	virtual void        OnRecvdSecondVideoRequestKeyFrame(void){};
	void                OnCallRingIn(void);
	void                OnCallRingOut(void);
	void                SetVideoFormatTypeFromMaxFSBitrate(int nVideoBitrate,VIDEC_CODEC_TYPE videoCodecType,int nH264Level,VIDEO_FORMAT_TYPE & videoFmt,int nType=0); //0 terminal use 1 mp mcu use
	VIDEO_FORMAT_TYPE   GetVideoFormatTypeFromMaxBitrate(int nVideoBitrate,VIDEC_CODEC_TYPE videoCodecType);
	VIDEO_FORMAT_TYPE   GetCompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2,int nType=0); //0 get max 1 get min
	VIDEO_FORMAT_TYPE   GetFormatTypeFromLevel(int nLevel,int & nVideoWidth,int & nVideoHeight);
	void                GetVideoFormatTypeWH(VIDEO_FORMAT_TYPE videoFormatType,int & w,int & h);
	void                SetCallNum(int num);
	int                 GetCallNum( );
	pj_bool_t           SIPSend200Status(pjsip_rx_data *rdata ,int code=PJSIP_SC_OK);
	int                 GetLocalMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);
	int                 GetSecondVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);
	int                 GetRemoteMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);

	pj_bool_t           SIPSendInviteRequest(pjmedia_sdp_session *sdp);
	bool                m_bHasRemoteDualSDP;
	bool                m_bSendSDPAll;

	int                 m_nSendDTMFRTP;
	unsigned short      m_nTransactionId;    //use to mcu
	unsigned short      m_nUserId;           //use to mcu
	unsigned short      m_nFloorId;          //use to mcu
	bool                m_bCallState;        //default false,true is Established
	bool                m_bCallDestroy;
	bool &              CallDestroyStatus();
	std::string         m_strMediaUpdate;    //���ڱ����ͨ���ٴ�ͨ����Ϣ
	int                 m_nLocalHungUP;      //1 hungup call ���������Ҷ� 2 ����Ҷ� 3 Զ�������Ҷ�(���յ�releaseComplete)
	IMXEC_CALL_DIRECTION	m_nCallDir;
	int					m_nCallNum;          //�к�������Ϊ1 Ĭ��Ϊ0 �ն���

	int					m_nVideoWidthBack;
	int					m_nVideoHeightBack;
	int					m_nFrameRateBack;
	int                 m_nCodecTypeBack;
	int					m_nVideoBitrateBack;
	int					m_nVideoPayloadTypeBack;
	int					m_nNetworkTypeBack;
	int                 m_nMainChannelState; //1 connect 0 default
	int                 m_nDualChannelState; //1 connect 0 default



	//{{����������������������Э���������ض��崦�� 20141103
public:
	int                 m_nCapDirectionDefault;             //���䷽�� Ĭ��1 ,cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	RecvCallCaps        m_RecvCallCaps;
	std::string	        m_strAudioPayloadName;
	int                 m_nAudioPayloadType ;
	int                 m_nAudioSampleRate ;
	X_AUDIO_CODEC_ID	m_AudioCodecType;
	DATA_CODEC_ID	    m_DataCodecID;
	VIDEC_CODEC_TYPE	m_nMainVideoCodecType;
	VIDEC_CODEC_TYPE	m_nDualVideoCodecType;
	int                 m_nDataPayloadType ;
	VIDEO_FORMAT_TYPE   m_nMainVideoFormat;
	VIDEO_FORMAT_TYPE   m_nDualVideoFormat;
	int                 m_nAudioClockRate;

	int                 GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType);
	float               GetFrameRateFromH264Param(int nMaxFS,int nMBPS);
	VIDEO_FORMAT_TYPE   GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate);
	VIDEO_FORMAT_TYPE   GetCallFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType=0); //nType 0 get min ,1 get max
	VIDEO_FORMAT_TYPE   GetFormatTypeFromLevel(int nLevel);
	void                SetTerminalConfInfoCaps(void);       //�����ն������� �ն���
	void                AddCodecDapCapFromConfig();          //���ں͵������豸���Ե�����
	void                AddVideoConfInfoCaps(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nFrame,int nDirection,int nBitrate,int nDual); //nDual 0 is main video ,1 is dual video
	void                AddVideoConfInfoBaseCaps2(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nVideoFrameRate,int nDirection,int nVideoBitrate,int nDual);
	void                ConfCapabilitisDefault(void);
	void                DapCapCopyToOriginalCaps(SIPCallCapability & sdpCapability,DapCallCapability & DapCap);
	void                RecvCapCopyToDapCaps(void);

	bool                H235CapNegotiate(int aEncry,int bEncry,unsigned int remoteAlgo,EncDecAlgorithmID& algoID);
	int                 CapabilitisNegotiate(int nLocalPriority,bool bEncryOption); //nLocalPriority 1 �������� 2 �ն˶Զ����� 3 A7MCU�������� 4 A7MCU�Զ�����
	bool                CapabilitisAudioItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisDataItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisVideoItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem); //nDual 0 is main video ,1 is dual video
	int                 OnOffer(pjmedia_sdp_session **p_sdp);
	void                GetVideoPayloadName(VIDEC_CODEC_TYPE nCodecType,std::string & strVideoPayloadName);
	void                CopyVideoCapOtherParam(ConfInfo *pConfInfo,int setBitrate,int setFormat);
	int                 GetVideoCapMaxBitrate(ConfInfo *pConfInfo,int nDual);  //0 is main video ,1 is dual video
	void                LogRecvCapDapCaps(void);
	void                RecvCapDapCapsSort(VIDEC_CODEC_TYPE codecType);

};


#endif
