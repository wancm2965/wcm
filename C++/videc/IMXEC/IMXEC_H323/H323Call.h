#ifndef __H323CALL_H__
#define __H323CALL_H__

#include "H323Channel.h"
#include "CallCapability.h"
#include "XAutoLock.h"
#include "../DefineHeader/H323DefineHeader.h"
//#include <IMXEC/AdapterHeaderDefine.h>

#include <RVINC/cm.h>
#include <NETEC/NETEC_Node.h>

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/AdapterHeaderDefine.h>
#include <AVCONMP/IMXEC_H323.h>
#include <AVCONMP/IMXEC_Call.h>

#else

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPH323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#include <IMXEC/AdapterHeaderDefine.h>
#include <IMXEC/IMXEC_Call.h>

#endif

#include "EncryptionCapability.h"
#include <map>
#include <RVINC/h235_api.h>
#include <RVINC/stkutils.h>
#include <RVINC/fwnat_api.h>

#define MAX_CAPS_NUM  MAX_AUDIOCAP_NUM + MAX_VIDEOCAP_NUM + MAX_H239VIDEOCAP_NUM + MAX_DATACAP_NUM

//�ú�õ�ĳ����ĳλ
#define BitGet(Number,pos) ((Number) >> (pos) & 1) 
//��ĳλ��1
#define BitSet(Number,pos) ((Number) |= 1<<(pos))
//��ĳλ��0
#define BitClr(Number,pos) ((Number) &= ~(1<<(pos)))
//��Number��POSλȡ��
#define BitRes(Number,pos) ((Number) ^= 1<<(pos))


#define MaxPathValue 160  /*�洢���PATH VALUE��*/

//H235 Media use
struct pathValueItem
{
	int pathlength;
	int value;
	unsigned int isValueString;
	unsigned char * buffer;
};

struct pathValue
{
	int	m_nNum;	//��Ч���ݸ���
	pathValueItem	m_PathValueCaps[MaxPathValue];
	pathValue()
	{
		m_nNum = 0;
	}
};


struct h235MediaCapItem
{
unsigned int entryNum;
unsigned int algorithm;
};


struct h235MediaCap
{
	unsigned int	m_nCapNum;	//��Ч��������������
	h235MediaCapItem		m_Caps[MAX_CAPS_NUM];

	h235MediaCap()
	{
		m_nCapNum = 0;
	}
};


//recv and send local rtp port set same 20120229
#define MAX_ALLOCATE_NUM  12
struct MediaRtpPortAllocateItem
{
	unsigned short m_nRtpPort;
	ChannelMediaType m_nMediaType;
	unsigned char m_nDirection; //0 recv ,1 send
	//HRTPSESSION m_hRtp;
	void * m_hRtp;
	H323Channel* m_pSendChannel;
	H323Channel* m_pRecvChannel;
	int    m_nUseCount;         //use count
};

struct MediaRtpPortAllocate
{
	int	            m_nCount;	//��Ч���ݸ���
	MediaRtpPortAllocateItem    m_Array[MAX_ALLOCATE_NUM];

	MediaRtpPortAllocate()
	{
		m_nCount = 0;
	}
};

typedef struct tagCodecItem
{
	VIDEC_CODEC_TYPE	m_VideoCodecType ;
	VIDEO_FORMAT_TYPE	m_VideoFormatType ;
}CodecItem;

typedef struct tagCodecItemArr
{
	int m_nCount;
	CodecItem m_Items[MAX_VIDEO_CAP];
}CodecItemArr;


class H323RegUser;

class H323CallMgr;
////////////////////////////////////////////////////////////////////////////////////////
#if ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) )
class H323Call
#else
class H323Call: public NETEC_Node
#endif

{
	friend class H323Channel;
public:
	H323Call(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo);
	~H323Call();
public:
	virtual void Close(void);

	const std::string& GetCallID(void);

	const std::string GetUserFullName(void);
	const std::string GetUserID(void);

	//��ݺ���
	void OpenAudioOutChannel(void);
	void OpenVideoOutChannel(void);
	void OpenH239VideoOutChannel(void);
	void OpenFECCOutChannel(void);

	int  SetLocalCapabilitis(int nodeId);

	int SendMainVideo(char* RTPPacketData, int PacketDataLen);
	int SendAudio(char* RTPPacketData, int PacketDataLen);
	//����DTMF
	int SendDTMF(const char dtmf,int nType=1);

	int SendSecondVideo(char* RTPPacketData, int PacketDataLen);

	void OnRcvH245CapabilitiesSet(void* capabilities);
	void OnRcvH245OpenChannel(HANDLE hsChannel, HANDLE* lphaChannel);

	//Զң�¼�
	void OnRemoteCameraLeft();//Զң����
	void OnRemoteCameraStopLeft();//Զңֹͣ����
	void OnRemoteCameraRight();//Զң����
	void OnRemoteCameraStopRight();//Զңֹͣ����
	void OnRemoteCameraUp();//Զң����
	void OnRemoteCameraStopUp();//Զңֹͣ����
	void OnRemoteCameraDown();//Զң����
	void OnRemoteCameraStopDown();//Զңֹͣ����
	void OnRemoteCameraZoomIn();//Զң�������Ŵ�
	void OnRemoteCameraStopZoomIn();//Զңֹͣ����
	void OnRemoteCameraZoomOut();//Զң��Զ����С��
	void OnRemoteCameraStopZoomOut();//Զңֹͣ��Զ
	
	void OnRemoteSourceSelect(int vsID);//Զңѡ����ƵԴ
	void OnRemoteSourceSwitched(int vsID);//Զң֪ͨ������ƵԴ�л���
	void OnRemoteCameraPresetStore(int position);//Զң����ͷԤ������
	void OnRemoteCameraPresetActive(int position);//Զң����ͷԤ�ü���

	virtual void OnCallRingIn(void);
	virtual void OnCallRingOut(void);

	//ý�����
	virtual void OnAudioRecvChannelConnected(void){};
	virtual void OnAudioRecvChannelDestroyed(void){};

	virtual void OnMainVideoRecvChannelConnected(void){};
	virtual void OnMainVideoRecvChannelDestroyed(void){};

	virtual void OnSecondVideoRecvChannelConnected(void){};
	virtual void OnSecondVideoRecvChannelDestroyed(void){};

	virtual void OnH224RecvChannelConnected(void){};
	virtual void OnH224RecvChannelDestroyed(void){};


	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp){};
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp){};
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp){};

	virtual void OnRecvdMainVideoRequestKeyFrame(void){};
	virtual void OnRecvdSecondVideoRequestKeyFrame(void){};

	virtual void OnAudioSendChannelConnected(void){};
	virtual void OnAudioSendChannelDestroyed(void){};

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	virtual void OnMainVideoSendChannelConnected(void){};
	virtual void OnMainVideoSendChannelDestroyed(void){};
#else
	virtual void OnMainVideoSendChannelConnected(void);
	virtual void OnMainVideoSendChannelDestroyed(void);
#endif

	virtual void OnSecondVideoSendChannelConnected(void){};
	virtual void OnSecondVideoSendChannelDestroyed(void){};

	virtual void OnH224SendChannelConnected(void){};
	virtual void OnH224SendChannelDestroyed(void){};

	virtual void OnChannelRequestKeyFrame(H323Channel*pH323Channel);
	virtual void OnChannelRequestFreeze(H323Channel*pH323Channel);


	H323Channel* NewChannel(HCHAN hsChan);

	void AddChannel(H323Channel*pH323Channel);
	void RemoveChannel(H323Channel*pH323Channel);

	int RequestMainVideoKeyFrame(void);
	int RequestSecondVideoKeyFrame(void);

	//Add VCF
	int RequestMainVideoFreeze(void);
	int RequestSecondVideoFreeze(void);

	void SendGenericRequest(int nSymBreking=128);
	void SendGenericResponse(bool bAccept);
	void SendGenericCommand();
	void SendGenericIndication();

	void OnRecvGenericRequest(H323Channel *pH323Channel, int terLabel, int chID, int symBreaking,int type=1);
	void OnRecvGenericCommand(H323Channel *pH323Channel, int terLabel, int chID,int type=1);

	void OnRecvGenericResponse(H323Channel *pH323Channel, int response, int terLabel, int chID,int respValue);
	void OnRecvGenericIndication(H323Channel *pH323Channel, int terLabel, int chID);
	
	void RemoteSourceSelect(int vsID);
	void RemoteSourceSwitched(int vsID);
	void RemoteCameraPresetStore(int position);
	void RemoteCameraPresetActive(int position);

	virtual void OnChannelConnected(H323Channel*pH323Channel);
	virtual void OnChannelDisconnected(H323Channel*pH323Channel);
	virtual void OnChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendPTZ (PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*��������ʼ����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartSecondVideo (void);

	/******************************************************************************
	* StopSecondVideo
	*������ֹͣ����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StopSecondVideo (void);

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir);

	virtual void OnLocalSecondVideoStarted(void);
	virtual void OnLocalSecondVideoStop(void);

	virtual int GetSupportSecondVideo(void);
	virtual unsigned short GetCallRTPPort(void);
	virtual bool HasAVCONTransport(void){return false;};

	void DoCheckPTZAction(void);
	std::string GetLocalNodeID(){return m_nLocalNodeID;}
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

public:
	HCALL			m_hsCall;

	IMXEC_CALL_DIRECTION	m_nSecondVideoDir;

	bool				m_bHasRemoteAudio;
	X_AUDIO_CODEC_ID	m_nRemoteAudioCodecID;

	bool				m_bHasRemoteVideo;
	VIDEC_CODEC_TYPE	m_nRemoteMainVideoCodecType;
	int					m_nRemoteMainVideoMaxBitrate;		//�Է��������
	VIDEO_FORMAT_TYPE	m_nRemoteMainVideoFormatType;
	int					m_nMainVideoQCIF;
	int					m_nMainVideoCIF;
	int					m_nMainVideo4CIF;
	int					m_nMainVideoH264Level;

	bool				m_bHasRemoteH239Video;
	VIDEC_CODEC_TYPE	m_nRemoteH239VideoCodecType;
	int					m_nRemoteH239VideoMaxBitrate;		//�Է��������
	VIDEO_FORMAT_TYPE	m_nRemoteH239VideoFormatType;
	int					m_nH239QCIF;
	int					m_nH239CIF;
	int					m_nH2394CIF;
	int					m_nH239VideoH264Level;

	bool			m_bHasRemoteData;

	H323Channel*	m_pAudioSendChannel;
	H323Channel*	m_pAudioRecvChannel;
	XCritSec		m_csMainVideoSendChannel;
	H323Channel*	m_pMainVideoSendChannel;
	H323Channel*	m_pMainVideoRecvChannel;
	H323Channel*	m_pSecondVideoSendChannel;
	H323Channel*	m_pSecondVideoRecvChannel;
	H323Channel*	m_pH224SendChannel;
	H323Channel*	m_pH224RecvChannel;

	int				m_nH225Version;
	std::string		m_strProductID;
	std::string		m_strVersionID;

	std::string		m_strLocalIP;
	std::string		m_strUserName;
	std::string		m_strUserID;

protected:
	MapH323Channel	m_MapH323Channel;					// �ŵ��б�

	H323CallMgr&		m_rH323CallMgr;					// ������H323ʵ��
	std::string			m_strCallID;


	//===˫��
	int				m_nterLabelReqSend;
	int				m_nchIDReqSend;
	int				m_nterLabelReqRecv;
	int				m_nchIDReqRecv;


	int				m_nPTZSpeed;
	bool			m_bH239ReceiverOpen;

	unsigned long	m_nAudioSSRC;
	unsigned short	m_nAudioSeqNo;
	unsigned long	m_nAudioTimestamp;


	PTZ_ACTION		m_nCurrentPTZAction;
	unsigned long	m_nLastCheckPTZActionTimestamp;
	PTZ_ACTION		m_nLastRecvdPTZAction;

	std::string		m_strRecvUserNonstandardInfo;
	std::string		m_strSetUserNonstandardInfo;
	bool			m_bMakeCall;
	std::string		m_strRecvUserInfomationNonstandData;

	std::string		m_nLocalNodeID;
	unsigned long	m_nPeerAudioID;
	unsigned long	m_nPeerVideoID;
	unsigned long	m_nPeerSecondVideoID;
	unsigned short	m_nPeerLocalPort;
	std::string		m_strPeerLocalIP;
	std::string		m_strPeerMCUID;
	unsigned long	m_nEnableMulticast;

public:
	bool m_bSecondSendCap;										//true �޸��������ط� false ����
	int m_nRemoteTerminalType;									//1 һ�� 2 MCU  ע�� ���ڱ��˷�������  

	bool SetG729ChannelCapability(char * pName,int type=2) ;	//1=G729 2=G729A 3=G729B
	int m_nRemoteG729Type;										//1=G729 2=G729A 3=G729B
	bool SetG719ChannelCapability(char * pName,int nMaxBitrate);

public:
	std::string m_strPrintPathName;								//����H235Media OLC
	bool				m_bHasRemoteAudioFirst;					//����H235ǿ�Ƽ���
	X_AUDIO_CODEC_ID	m_nRemoteAudioCodecIDFirst;
	bool				m_bHasRemoteVideoFirst;
	VIDEC_CODEC_TYPE	m_nRemoteMainVideoCodecTypeFirst;
	VIDEO_FORMAT_TYPE	m_nRemoteMainVideoFormatTypeFirst;
	bool				m_bHasRemoteH239VideoFirst;
	VIDEC_CODEC_TYPE	m_nRemoteH239VideoCodecTypeFirst;
	VIDEO_FORMAT_TYPE	m_nRemoteH239VideoFormatTypeFirst;
	
	int					m_nMainVideoQCIFFirst;
	int					m_nMainVideoCIFFirst;
	int					m_nMainVideo4CIFFirst;
	int					m_nMainVideoH264LevelFirst;

	int					m_nH239QCIFFirst;
	int					m_nH239CIFFirst;
	int					m_nH2394CIFFirst;
	int					m_nH239VideoH264LevelFirst;

	EncDecAlgorithmID   m_AudioEncDecAlgoId;
	bool				m_bAudioHasRemoteEncryption;
	EncDecAlgorithmID   m_VideoEncDecAlgoId;
	bool				m_bVideoHasRemoteEncryption;
	EncDecAlgorithmID   m_H239EncDecAlgoId;
	bool				m_bH239HasRemoteEncryption;
	EncDecAlgorithmID   m_DataEncDecAlgoId;
	bool				m_bDataHasRemoteEncryption;


	char				m_strSharedSecret[256];		//˫��Э�̵Ĺ�����Կ
	char				m_strSecVector[64];			//������� OLC ��OLC ACKʹ��
	int					m_nSecVector;				//7ΪDES�����������  21Ϊ3des�����������
	H235HCALL			m_hSecCall;					//H235 Call ���
	bool				m_bUseDiffieHellman;		//DiffieHellman״̬
	entityId_t			m_remoteEndpoint;			//�Զ�ENTITY ID
	bool				m_bShareSecret;				//ý�������ܹ�����ԿЭ�̽�� falseʧ�� true�ɹ�
	int					m_nH239SessionId;

	//ȡ���Զ˵�����H235�������������
	void GetH235MediaCapSet(h235MediaCap * pRemoteH235Cap,void* capabilities);

	//���Ҽ��������� ȡ�뱾��Э�̽�� ȡ���ӽ����㷨 return >0 - successful, =0 - failure.
	int GetH235MediaCapNegotiate(h235MediaCap * pRemoteH235Cap,cmCapStruct* pCapAlt,int codecType,bool h239,int nLevel);
	
	//ý��������״̬֪ͨ
	void OnChannelMediaEncryptionState(H323Channel*pH323Channel);

	//recv and send local rtp port set same 20120229
	MediaRtpPortAllocate m_MediaRtpPortAllocate;
	void InitMediaRtpPortAllocate();
	bool SaveMediaRtpHandle(void * hRtp,unsigned short port);
	void * GetCreateMediaRtpHandle(unsigned short port);
	//dir 0 recv ,1 send
	unsigned short GetMediaRtpPort(ChannelMediaType type,H323Channel* pChannel,unsigned char dir=0);
	//related media channel close rtp  only use hungup
	bool RelatedMediaChannelCloseRTP(void * hRtp,H323Channel* pChannel);

	//get rtp related media channel only use hungup
	H323Channel* GetRelatedMediaChannel(void * hRtp,H323Channel* pChannel);
	//close related channel media 
	void OnChannelRelatedDisconnected(H323Channel*pH323Channel);

	void SendCallUserInfomation(std::string strNonstandData);	//Send H225 User Infomation strNonstandData length range 1<= <=131
	void SetCallNonStandardData(cmCallState_e state);			//Set H225 NonStandard Data
	void GetCallUserInfomation(HCALL hsCall,cmCallState_e state ,int nodeId = -1);
	const std::string& GetRecvUserNonstandardInfo(void);
	void SetUserNonstandardInfo(const char * info);
	void GetUserNonstandardInfo(std::string & strUserNonstand);

	void SetH264LevelFromProductId(VIDEO_FORMAT_TYPE nFormat,int & nLevel);


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


public:
	bool					m_bTokenOwnState;							//����ӵ��״̬ trueΪӵ�У�falseΪû��ӵ��

	//�������ӵ��״̬������CALLָ����� ����trueΪӵ�У�falseΪû��ӵ��
	bool CheckH239TokenOwnState();

	//������ʹ��״̬������CALLָ�������bState trueΪӵ�У�falseΪû��ӵ��
	void SetH239TokenOwnState(bool bState);

	std::string				m_strConferenceID;
	void SetConferenceID(std::string strConferenceID);
	std::string & GetConferenceID(void);

	bool					m_bVirtualMark;			//true ��ʾΪ�����ն�����MP��������
	void SetCallID(std::string strCallID,H323Call * pCall);
	void SetVitualMark(bool mark);
	H323Call *				m_pVirtualCall;			//�����ն�����MP��������

	CHANNEL_INFO			m_struChannelInfo;		//�����ն���
	int GetChannelInfo(CHANNEL_INFO& rChannelInfo); //�����ն���
	int SetSendSecondVideoParam(int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nPayloadType=0);


#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1

	int						m_nRecvH239TokenReq;	//0 not recv ,1 is recved
	void SetH239TokenRequestStatus(int status);
	int GetH239TokenRequestStatus(void);


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

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

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

	int						AddCapAudioItem(X_AUDIO_CODEC_ID codecID);
	int						AddCapVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);
	int						AddCapDualVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);

#endif

	/******************************************************************************
	* BuildLocalCapabilitisHeader
	*�������齨CALL������ͷ�������
	*���룺	
	*�������
	*����ֵ���ɹ����طǸ�ֵ��ʧ�ܷ���-1
	*****************************************************************************/
	int BuildLocalCapabilitisHeader();

	/******************************************************************************
	* BuildSendLocalCapabilitis
	*�������齨CALL����������㲢����
	*���룺	
	*�������
	*����ֵ���ɹ����طǸ�ֵ��ʧ�ܷ���-1
	*****************************************************************************/
	int BuildSendLocalCapabilitis();

	/******************************************************************************
	* BuildSendVideoOLCCapabilitis
	*�������齨CALL��Ƶͨ������������㲢����
	*���룺	
	*�������
	*����ֵ���ɹ����طǸ�ֵ��ʧ�ܷ���-1
	*****************************************************************************/
	int BuildSendVideoOLCCapabilitis();
	int BuildSendDataOLCCapabilitis();
	int BuildSendAudioOLCCapabilitis();
	int BuildSendDualVideoOLCCapabilitis();


public:
	CallCapability			m_CallCapability;
	EncryptionCapability    m_EncryptionCapability;
	int						m_nMainVideoMaxBitrate;
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
	int						m_nCapabilityNodeID;									//CALL�����������
	VIDEO_FORMAT_TYPE		m_nMainVideoFormatType;									//����������Ƶ���õ����ֱ���
	VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;								//���˸�����Ƶ���õ����ֱ���
	std::string				m_strMediaMulticastIP;									//ý�����鲥��ַ
	bool					m_bMediaMulticastEnable;								//ý���������鲥 true���� false������ Ĭ��false
	int						m_nMediaMulticastTTL;									//�鲥���ĵ����ݰ���TTL(����ʱ��) 0..255
public:
	CallCapability &		GetMgrCapability();
	EncryptionCapability &	GetMgrEncryptionCapability();
	int						GetMgrCallType(IMXEC_CALL_TYPE&nCallType);
	void					SetMgrRTPBasePort(unsigned short nRTPBasePort);
	unsigned short			GetMgrRTPBasePort(void);
	void					OnCallMgrCallSecondVideoStop(const std::string&strCallID);
	void					OnCallMgrRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);
	unsigned long			GetMgrExternalIPAddress(void);
	//1013 add
	std::string            GetMgrExternalIPAddressString(void);

	void					GetMgrLocalIPByRemoteIP(std::string & strLocalIP,const std::string&strHost,unsigned short nPort);
	int						GetMgrMainVideoMaxBitrate(void);
	int						GetMgrSecondVideoMaxBitrate(void);
	VIDEO_FORMAT_TYPE		GetMgrMainVideoFormatType(void);
	VIDEO_FORMAT_TYPE		GetMgrSecondVideoFormatType(void);
	int						GetH264ProfileLevel(HPVT pvtTree,int nodeId,char * subBuf,int & nProfile,int & nLevel);

private:
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH263;
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH264;
	VIDEO_FORMAT_TYPE		m_nMainVideoFTH264HP;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH263;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH264;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFTH264HP;


//add adapter
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

	void RecvH245ControlMessage(RvPvtNodeId message,cmH245MessageType messageType);
	void OnRecvH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);
	void RecvH245TerminalNumberAssign(int mcuNumber, int terminalNumber);
	void RecvGenericMessage(cmH245MessageType messageType,int nBitrate, int terLabel, int chID, int symBreaking=0,int response=0,int respValue=0);
	void RecvFlowControlCommand(H323Channel* pH323Channel, HCHAN hsChan, RvUint32 rate);
	int  GetH323ChannelLogicalNumber(int type,std::string & strMediaName);						//type=0 ȡ����ͨ����,����ֵȡ����ͨ����,ý�����͹ؼ���  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	void GetH323ChannelLogicalNumMediaName(int type,int nLogicalNum,std::string &strMediaName);	//type=0 ȡ����ͨ����,����ֵȡ����ͨ����

	void OnAdapCallRecvChannelOpen(H323Channel * pH323Channel);
	void OnAdapCallChannelConnected(H323Channel * pH323Channel);
	void OnAdapCallChannelDisConnected(H323Channel * pH323Channel);
	virtual void OnA7ChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void OnAdapChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void OnRemoteCameraControl(H323Channel * pH323Channel,std::string strAction,int nValue);//Զң����ͷ���� //nValue ��ƵԴID��Ԥ�õ�
	int	 SendCameraControl(std::string strAction,int nValue);								//nValue ��ƵԴID��Ԥ�õ�

public:
	IMXEC_CALL_DIRECTION	m_nCallDir;
	std::string				m_strTelNum;
	std::string				m_strRemoteUserNonstandardInfo;
	std::string				m_strDisconnectReason;
	ConfInfo				m_ConfInfo;							//���ڱ���MCU������������Ϣ
	ConfInfo				m_TCSTerminalConfInfo;				//ʹ���ն���Ƶ����������
	int                     m_nTCSUseTerminal;                  //0��ʹ�� 1ʹ��

	int						m_nSupportH239;
	int						m_nInvalidSIDOpenCount;				//recv InvalidSessionID reason ,open dualvideo send channel count

	std::string&			DisconnectReason(void);
	int GetCallQ931TransportAddr(char * szAddr,int &nPort,int &nLen);
	int GetCallH245TransportAddr(char * szAddr,int &nPort,int &nLen);
	int GetCallTPKTTransportAddr(char * szAddr,int &nPort,int &nLen,cmTpktChanHandleType type);
	int SendMediaChannelActive(std::string strMediaName);
	int SendMediaChannelInActive(std::string strMediaName);
	void RecvChannelMessage(H323Channel * pH323Channel,int nodeID);
	void RecvChannelInORActiveMessage(H323Channel * pH323Channel,bool bActive,bool bInActive);
	int CloseDualVideoOutChannel();
	int CloseFECCOutChannel();
	int SetSupportH239(int nSupportH239);
	void H239VideoOutChannelInvSIDCntAdd();
	int GetH239VideoOutChannelInvSIDCnt();
	void AcceptCallSetCapabilities(ConfInfo *pConfInfo=NULL);
	int AnswerMediaRecvChannel(int type);												//1 audio 2 video 3 H239 4 Data
	int RejectMediaRecvChannel(int type,cmRejectLcnReason reason);						//1 audio 2 video 3 H239 4 Data
	int GetH264CapOptionParam(HPVT pvtTree,int nodeId,int& nMaxFS,int& nMBPS,char * subBuf,int type=0);		//0 is main video ,1 is dual video

	unsigned long	m_nLastRcvdTripTimestamp;
	void OnRecvRoundTripResponse(HCALL hsCall, int delay);
	bool CheckTimestamp();
	void SendRoundTripDelay(int delay=32);												//delay seconds

	//H.235v3.6 8.6.2 �Ľ�����Կ����
	void SendEncryptionUpdateRequest(H323Channel * pH323Channel);						//EncryptionUpdateRequest Command
	void SendEncryptionUpdateCommand(H323Channel * pH323Channel,int synchFlag=0);		//EncryptionUpdate Command
	void OnRecvEncryptionUpdateRequest(H323Channel * pH323Channel,HCHAN hsChan,int messageId);
	void OnRecvEncryptionUpdateCommand(H323Channel * pH323Channel,HCHAN hsChan,int messageId);
	void OnRecvEncryptionUpdateAck(H323Channel * pH323Channel,HCHAN hsChan,int messageId);
	void SendEncryptionUpdateACK(H323Channel * pH323Channel,int syncFlag=0);			//encryptionUpdateAck Command
	H323Channel* GetRelatedChannel(H323Channel* pH323Channel);
	int & GetMgrH235MediaUse();
	int & GetMgrH235AuthUse();
	int & GetMgrH235MediaAlogrithm();
	int & GetMgrH235MediaEncrypSpecified();
	char * GetMgrH235MediaEncrypSpecifiedPWD();
	void CopyVideoCapOtherParam0(ConfInfo *pConfInfo,int setBitrate,int setFormat);

private:
	int						m_nRemoteMainVideoOLCMaxMBPS;								//����OLC��������ѡ����ֵ ����鴦������
	int						m_nRemoteMainVideoOLCMaxFS;									//����OLC��������ѡ����ֵ ���֡��
	int						m_nRemoteMainVideoOLCMaxDPB;								//����OLC��������ѡ����ֵ ������ͼ�񻺴��С
	int						m_nRemoteMainVideoOLCMaxBR;									//����OLC��������ѡ����ֵ �����Ƶ����
	int						m_nRemoteDualVideoOLCMaxMBPS;								//����OLC��������ѡ����ֵ ����鴦������
	int						m_nRemoteDualVideoOLCMaxFS;									//����OLC��������ѡ����ֵ ���֡��
	int						m_nRemoteDualVideoOLCMaxDPB;								//����OLC��������ѡ����ֵ ������ͼ�񻺴��С
	int						m_nRemoteDualVideoOLCMaxBR;									//����OLC��������ѡ����ֵ �����Ƶ����
	int						m_nMainVideoFrameRate;										//default >0
	int						m_nSecondVideoFrameRate;									//default >0
	unsigned long			m_nLastSendGenIndTimestamp;
	unsigned long			m_nLastSendSecretUpdateTS;									//default 12 hours

public:
	int						m_nAudioUpdatePT;											//default 111
	int						m_nMainVideoUpdatePT;										//default 112
	int						m_nDualVideoUpdatePT;										//default 113
	int						m_nFECCUpdatePT;											//default 114
	int						m_nH235MediaUse;											//0��ʹ�� 1��ѡʹ�� 2ǿ��ʹ��H235ý��������
	int						m_nH235AuthUse;												//0��ʹ�� 1ʹ��
	int						m_nUseAlogrithmId;											//��ѡ�����㷨,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	int						m_nMediaEncrypSpecified;									//0Ϊ��ʹ��ָ������(�Զ�Э��) 1 Ϊʹ��ָ������
	char					m_strMediaEncrypPWD[64];									//ʹ��ָ������ʱ���õļ�������

	int						m_nBandWidth;												//���������� ARQ use

public:
	bool matchRemoteMainVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat);
	bool matchRemoteSecondVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat);

protected:
	// �����ֶ�����flowcontrol���ڶԶ˷���������
	int					m_nMainVideoBitRate;			// ����Ƶ�Ľ�������
	int					m_nMainVideoActualBitRate;		// ����Ƶ��ʵ�ʽ�������
	int					m_nSecondVideoBitRate;			// ����Ƶ�Ľ�������
	int					m_nSecondVideoActualBitRate;	// ����Ƶ��ʵ�ʽ�������
	// ����ͳ��
	unsigned long		m_nMainVideoStartRcvdPacketTimestamp;
	unsigned long		m_nMainVideoStartRcvdPacketSequence;
	unsigned long		m_nMainVideoRcvdPacketCount;
	double				m_dMainVideoRcvLostRate;
	unsigned long		m_nSecondVideoStartRcvdPacketTimestamp;
	unsigned long		m_nSecondVideoStartRcvdPacketSequence;
	unsigned long		m_nSecondVideoRcvdPacketCount;
	double				m_dSecondVideoRcvLostRate;
	// �������ؼ�֡�ļ��
	bool				m_bMainVideoReqKeyFrame;
	bool				m_bSecondVideoReqKeyFrame;
	unsigned long		m_nMainVideoLastReqKeyFrameTimestamp;
	unsigned long		m_nSecondVideoLastReqKeyFrameTimestamp;

	std::string			m_strPeerNodeID;
	std::string			m_strMCUID;

	virtual void OnFlowControlCommand(H323Channel*pH323Channel, unsigned int nBitRate);

	bool IsConfMainVideoSupport(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE videoFormatType);
	void SetVideoBitRate();
	void AjustVideoBitRate();
	void StatisticsMainVideo(unsigned short usSequence);
	void StatisticsSecondVideo(unsigned short usSequence);

	int  m_nRemoteMainVideoFrameRate;
	int  m_nRemoteH239VideoFrameRate;

public:
	void SeRegAccountCallMark(void); //�����ֶ�ע�����ǰ����ARQ��Ϣ���
	void OnCallRecvACF(H323RegUser * pH323RegUser);
	void LevelIncFromMaxFS(int nMaxFS,int & nLevel);

	void AllowToSendSecondVideo(bool bAllow); // ����Զ˷���˫��
	void SendFlowControlCommand(IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate); // ����������

	void GetResolution(int& nRes, int &nFps, int nMaxMBPS, int nMaxFs, int nLevel, int nProfile);
	void SendChannelKeepAlive(void);                //add for 460client
	int					m_nUseFWNAT;				//�Ƿ�����H460 NAT��͸, 0 not use ,1 use,default 0
	int					m_nMasterCall;				//0���� ,1����
	std::string			m_strCalled;				//�Է����� ����
	bool                m_bRegCall; //�ֶ�ע�����ǰ����ARQ��Ϣ���,true��ʾ�ѷ���ARQ,false��ʾδ��ARQ,Ĭ��false

public:
	//notice test , if config,only use the config cap add
	void AddCodecCapFromConfig();
	bool GetRemoteUserIP(std::string & strUserIP);
	int SendRegDRQMessage();
	int SendRegIRRMessage(HRAS hsRas,int type=0); //type=0 is recv IRQ , 1 is recv ACF has irrFrequency

	int                 m_nSendDTMFRTP;
	H323RegUser *       m_pH323RegUser;
	int                 m_nParamCRV;
	char                m_szConferenceID[64];
	char                m_szCallEndpointID[64];

	std::string			m_strHost;                  //MCU��
	int					m_nRegUseGK;				//MCU�� 1 �Զ�����GK������,2 ע�ᵽָ����GK������
	cmAlias				m_GKAlias;                  //MCU��
	char				m_szGKAliasBuf[256];
	cmAlias				m_EPAlias;
	char				m_szEPAliasBuf[256];
	bool                m_bCallState;               //default false,true is Established
	std::string		    m_strE164ID;
	std::string		    m_strH323ID;
	int                 m_nMainVideoSendState;      //test
	int                 m_nSecondVideoSendState;
	int                 m_nAudioSendState;
	int                 m_nLocalHungUP;             //1 hungup call ���������Ҷ� 2 ����Ҷ� 3 Զ�������Ҷ�(���յ�releaseComplete)

	void NETEC_NodeStop();
	void NETEC_NodeStart();
	void SendCallReleaseComplete(cmReasonType nReason);

public:
	//notice netec send ptz 
	virtual int NETECSendCtrl(void*pData,int nLen) {return 0; };
	virtual int NETECSendPTZ(void*pData,int nLen) {return 0; };
	void OnNETECRecvdPTZ(void*pData,int nLen);
	void OnNETECRecvdCtrl(void*pData,int nLen);
	void OnRecvDTMF(char dtmf);//add by Jan for DTMF [20140730]
	bool GetCallSignalAddress(std::string & strIP,int & nPort);
	void AddMediaPacketization(H323Channel * pH323Channel,int nodeID);
	bool GetMediaRtpHandle(void * hRtp);
	int GetVideoCapFrameParam(VIDEC_CODEC_TYPE  nCodecType,VIDEO_FORMAT_TYPE nFormat,int type); //type	1 Enum_Channel_Media_Video,4 Enum_Channel_Media_DualVideo
	int CompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2);
	bool IsConfSecondVideoSupport(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE videoFormatType);

	void                SetCallNum(int num);
	int                 GetCallNum( );
	int                 GetLocalMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);
	int                 GetSecondVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);
	int                 GetRemoteMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo);
	void                CheckGenerateSSRCFlag();

	void                OnRcvH245CapabilitiesSetChange(unsigned int nMediaType);  //�ն���
	void                OnRcvChannelClose(H323Channel* pH323Channel);             //�ն���
	void                SetH239TokenSendReqStatus(int status);
	int                 GetH239TokenSendReqStatus( );
	void                TestConfInfoCapAdd();            //mcu use test

	int                 m_bRoundTripMaxCount;            //4 times ,4*10s >dalay(32s)
	int                 m_nRoundTripSendCount;           //
	int                 m_nNETECNodeCount;               //NETECNode Create Count
	int                 m_nMasterSlaveValue;
	int					m_nCallNum;				         //�к�������Ϊ1 Ĭ��Ϊ0 �ն���
	int 				m_nRemoteCapChaneged;            //�������и��� �ն��� ����л����� 32/4 -1
	int                 m_nForceVideoWid;                //����ǿ���������ֱ���  0��ǿ�� �ն���
	int                 m_nForceVideoHei;
	int                 m_nForceDualVideoWid;            //����ǿ�Ƹ������ֱ���  0��ǿ�� �ն���
	int                 m_nForceDualVideoHei;
	int                 m_nSendH239TokenReq;
	int                 m_nRecvGenericResp;              //�յ�GenericResp��channel active��Ϊtrue,default false
	int                 m_nSendGenericReq;               //Send GenericReq��Ϊtrue,default false
	int                 m_nRecvGenericReqCount;          //�յ� GenericReq�����,default 0
	bool			    m_bFlag;                         //Added by jiangdingfeng 20140929 �����Ƿ���Ҫ��������SSRC����
	unsigned long       m_nForceIP;
	bool                m_bForceIP;
	std::string         m_strMakeDestIP;
	std::string         m_strMakeSourceIP;

	//{{����������������������Э���������ض��崦�� 20141027
public:
	RecvCallCaps        m_RecvCallCaps;
	int					m_nRemoteMainVideoFrame;		//�Է����֡����
	int					m_nRemoteSecondVideoFrame;		//�Է����֡����
	int                 m_nCapDirectionDefault;         //���䷽�� Ĭ��1 ,cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	std::string         m_strQ931CalledNumber;
	std::string         m_strQ931CallerNumber;
	EM_ENTITY_TYPE      m_nEntityType;                  //�Զ�����

	void                SetAudioCapsParam(int nDirection,X_AUDIO_CODEC_ID nAudioCodecID,int entryNum,h235MediaCap& H235Cap);
	void                SetVideoCapsParam(int nDirection,RecvVideoCap& item,int entryNum,h235MediaCap& H235Cap,int nDual); //nDual 0 is main video ,1 is dual video
	void                GetH235MediaCapInfo(h235MediaCap& H235Cap,int entryNum,int& nEncryption,unsigned int& nAlgorithm);
	void                OnRcvH245CapabilitiesTermUse(void* capabilities);

	void                GetVideoAllCaps(HPVT pvtTree,h235MediaCap& remoteH235Cap,int nDirection,char * capAltName,int nCapAltNode,int nEntryNum,int nDual); //nDual 0 is main video ,1 is dual video
	int                 GetH264CapOptionParam(HPVT pvtTree,int nodeId,char * subBuf,int type=0);   //type 0 is main video ,1 is dual video
	bool                H235CapNegotiate(int aEncry,int bEncry,unsigned int remoteAlgo,EncDecAlgorithmID& algoID);
	void                SetTerminalConfInfoCaps(void);    //�����ն������� �ն���
	void                ConfCapabilitisDefault(void);     //��������Ĭ�ϼ��ֱ���ʵ�ʶ�Ӧ��С���㴦��
	void                AddVideoConfInfoCaps(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nFrame,int nDirection,int nBitrate,int nDual); //nDual 0 is main video ,1 is dual video
	void                AddVideoConfInfoBaseCaps2(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nVideoFrameRate,int nDirection,int nVideoBitrate,int nDual);

	void                BuildConfInfoCaps(int nodeId);   //�齨����������
	void                SetMCUConfCapFromProduct(void);  //���ݵ������豸�ͺ���������MCU������ MCU��
	void                AddConfCapFromConfig();          //���ں͵������豸���Ե�����

	int                 CapabilitisNegotiate(int nLocalPriority,bool bEncryOption); //nLocalPriority 1 �������� 2 �ն˶Զ����� 3 A7MCU�������� 4 A7MCU�Զ�����
	bool                CapabilitisAudioItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisDataItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisVideoItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem); //nDual 0 is main video ,1 is dual video

	void                SetTerminalH460NATFwOption(int option);    //�ն���

	int                 GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType);
	float               GetFrameRateFromH264Param(int nMaxFS,int nMBPS);
	VIDEO_FORMAT_TYPE   GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate);
	VIDEO_FORMAT_TYPE   GetCallFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType=0); //nType 0 get min ,1 get max
	VIDEO_FORMAT_TYPE   GetFormatTypeFromLevel(int nLevel);
	void CopyVideoCapOtherParam(ConfInfo *pConfInfo,int setBitrate,int setFormat);
	int  GetVideoCapMaxBitrate(ConfInfo *pConfInfo,int nDual);  //0 is main video ,1 is dual video
	bool GetCallH225LocalSignalAddress(std::string & strIP,int & nPort); //0 src 1 dest
	bool GetCallH245LocalSignalAddress(std::string & strIP,int & nPort);
	void LogRecvCapDapCaps(void);
	void SetCallSourcePhysicalAddress(const char * dest,int type =0);  //type 0 is make call


	//type 0 local ,1 remote , ulTotalPackets -�ܹ������˶��ٸ��� ulLostPackets -�ܹ������˶��ٸ���
	//AvgPacketLossRate -ƽ�������� CurPacketLossRate -��ǰ������ nBitrate -����
	virtual int GetMainVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	virtual int GetDualVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	//���ֱ��ʴ�С��ĳ������������
	void RecvCapDapCapsSort(VIDEC_CODEC_TYPE codecType);
	//��ȡĳЭ��������ʱ������ Negotiate , �� H263P H263PP , H263 H263P
	void CapabilitisProvisionalNeg(VIDEC_CODEC_TYPE localType ,VIDEC_CODEC_TYPE remoteType,int nLocalPriority,bool bEncryOption);
	void CapabilitisNegMainVideoResult(NegMediaCap & NegItem);
	void CapabilitisNegDualVideoResult(NegMediaCap & NegItem);
	void DapChanegCallCaps(ConfInfo *pConfInfo);
	bool CheckSetH264FormatFromProductId(VIDEO_FORMAT_TYPE & nFormat);
	void ConfCapabilitisBitrateDefault(ConfInfo *pConfInfo);
	void SetBitrateFromCaller(int nBitrate);

	void TCSUseTerminalVideoConfCaps(void);   //mcu���������� ʹ���ն���Ƶ����������
	bool CheckConfCapsSupported(ChannelMediaType nMedia,VIDEC_CODEC_TYPE codec,VIDEO_FORMAT_TYPE format);

};

#endif	//#ifndef __H323CALL_H__
