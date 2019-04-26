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

//用宏得到某数的某位
#define BitGet(Number,pos) ((Number) >> (pos) & 1) 
//把某位置1
#define BitSet(Number,pos) ((Number) |= 1<<(pos))
//把某位置0
#define BitClr(Number,pos) ((Number) &= ~(1<<(pos)))
//把Number的POS位取反
#define BitRes(Number,pos) ((Number) ^= 1<<(pos))


#define MaxPathValue 160  /*存储最多PATH VALUE数*/

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
	int	m_nNum;	//有效数据个数
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
	unsigned int	m_nCapNum;	//有效数据能力集个数
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
	int	            m_nCount;	//有效数据个数
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

	//便捷函数
	void OpenAudioOutChannel(void);
	void OpenVideoOutChannel(void);
	void OpenH239VideoOutChannel(void);
	void OpenFECCOutChannel(void);

	int  SetLocalCapabilitis(int nodeId);

	int SendMainVideo(char* RTPPacketData, int PacketDataLen);
	int SendAudio(char* RTPPacketData, int PacketDataLen);
	//发送DTMF
	int SendDTMF(const char dtmf,int nType=1);

	int SendSecondVideo(char* RTPPacketData, int PacketDataLen);

	void OnRcvH245CapabilitiesSet(void* capabilities);
	void OnRcvH245OpenChannel(HANDLE hsChannel, HANDLE* lphaChannel);

	//远遥事件
	void OnRemoteCameraLeft();//远遥向左
	void OnRemoteCameraStopLeft();//远遥停止向左
	void OnRemoteCameraRight();//远遥向右
	void OnRemoteCameraStopRight();//远遥停止向右
	void OnRemoteCameraUp();//远遥向上
	void OnRemoteCameraStopUp();//远遥停止向上
	void OnRemoteCameraDown();//远遥向下
	void OnRemoteCameraStopDown();//远遥停止向下
	void OnRemoteCameraZoomIn();//远遥拉近（放大）
	void OnRemoteCameraStopZoomIn();//远遥停止拉近
	void OnRemoteCameraZoomOut();//远遥拉远（缩小）
	void OnRemoteCameraStopZoomOut();//远遥停止拉远
	
	void OnRemoteSourceSelect(int vsID);//远遥选择视频源
	void OnRemoteSourceSwitched(int vsID);//远遥通知它的视频源切换了
	void OnRemoteCameraPresetStore(int position);//远遥摄像头预置设置
	void OnRemoteCameraPresetActive(int position);//远遥摄像头预置激活

	virtual void OnCallRingIn(void);
	virtual void OnCallRingOut(void);

	//媒体接收
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
	*描述：发送PTZ【发送云台控制命令】
	*输入：	nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendPTZ (PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*描述：开始辅流
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartSecondVideo (void);

	/******************************************************************************
	* StopSecondVideo
	*描述：停止辅流
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StopSecondVideo (void);

	/******************************************************************************
	* GetSecondVideoDirection
	*描述：获取辅流的方向
	*输入：	cszCallID		-呼叫标识
	*输出： nDir			-辅流的方向
	*返回值：成功返回0，失败返回-1
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
	int					m_nRemoteMainVideoMaxBitrate;		//对方最大速率
	VIDEO_FORMAT_TYPE	m_nRemoteMainVideoFormatType;
	int					m_nMainVideoQCIF;
	int					m_nMainVideoCIF;
	int					m_nMainVideo4CIF;
	int					m_nMainVideoH264Level;

	bool				m_bHasRemoteH239Video;
	VIDEC_CODEC_TYPE	m_nRemoteH239VideoCodecType;
	int					m_nRemoteH239VideoMaxBitrate;		//对方最大速率
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
	MapH323Channel	m_MapH323Channel;					// 信道列表

	H323CallMgr&		m_rH323CallMgr;					// 所属的H323实例
	std::string			m_strCallID;


	//===双流
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
	bool m_bSecondSendCap;										//true 修改能力集重发 false 不发
	int m_nRemoteTerminalType;									//1 一般 2 MCU  注意 用于本端发起主叫  

	bool SetG729ChannelCapability(char * pName,int type=2) ;	//1=G729 2=G729A 3=G729B
	int m_nRemoteG729Type;										//1=G729 2=G729A 3=G729B
	bool SetG719ChannelCapability(char * pName,int nMaxBitrate);

public:
	std::string m_strPrintPathName;								//用于H235Media OLC
	bool				m_bHasRemoteAudioFirst;					//用于H235强制加密
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


	char				m_strSharedSecret[256];		//双方协商的共享密钥
	char				m_strSecVector[64];			//随机向量 OLC 及OLC ACK使用
	int					m_nSecVector;				//7为DES随机向量长度  21为3des随机向量长度
	H235HCALL			m_hSecCall;					//H235 Call 句柄
	bool				m_bUseDiffieHellman;		//DiffieHellman状态
	entityId_t			m_remoteEndpoint;			//对端ENTITY ID
	bool				m_bShareSecret;				//媒体流加密共享密钥协商结果 false失败 true成功
	int					m_nH239SessionId;

	//取出对端的所有H235加密能力项参数
	void GetH235MediaCapSet(h235MediaCap * pRemoteH235Cap,void* capabilities);

	//查找加密能力项 取与本端协商结果 取出加解密算法 return >0 - successful, =0 - failure.
	int GetH235MediaCapNegotiate(h235MediaCap * pRemoteH235Cap,cmCapStruct* pCapAlt,int codecType,bool h239,int nLevel);
	
	//媒体流加密状态通知
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
	bool					m_bTokenOwnState;							//令牌拥有状态 true为拥有，false为没有拥有

	//检查令牌拥有状态，传入CALL指针参数 返回true为拥有，false为没有拥有
	bool CheckH239TokenOwnState();

	//置令牌使用状态，传入CALL指针参数，bState true为拥有，false为没有拥有
	void SetH239TokenOwnState(bool bState);

	std::string				m_strConferenceID;
	void SetConferenceID(std::string strConferenceID);
	std::string & GetConferenceID(void);

	bool					m_bVirtualMark;			//true 表示为虚拟终端用于MP自身发辅流
	void SetCallID(std::string strCallID,H323Call * pCall);
	void SetVitualMark(bool mark);
	H323Call *				m_pVirtualCall;			//虚拟终端用于MP自身发辅流

	CHANNEL_INFO			m_struChannelInfo;		//虚拟终端用
	int GetChannelInfo(CHANNEL_INFO& rChannelInfo); //虚拟终端用
	int SetSendSecondVideoParam(int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nPayloadType=0);


#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1

	int						m_nRecvH239TokenReq;	//0 not recv ,1 is recved
	void SetH239TokenRequestStatus(int status);
	int GetH239TokenRequestStatus(void);


public:

	/******************************************************************************
	* SetLocalMainVideoH264Level
	*描述：设置本地主流视频H264能力使用的最大Level值 如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	
			nLevel			-本地主视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoH264Level(int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264Level
	*描述：设置本地辅流视频H264能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	
			nLevel			-本地辅流视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalSecondVideoH264Level(int nLevel=0);

	/******************************************************************************
	* SetLocalMainVideoH264HPLevel
	*描述：设置本地主流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	
			nLevel			-本地主视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoH264HPLevel(int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264HPLevel
	*描述：设置本地辅流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	
			nLevel			-本地辅流视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalSecondVideoH264HPLevel(int nLevel=0);

	/******************************************************************************
	* SetRTPMediaMulticast 
	*描述：设置RTP 媒体流组播 如有设置组播IP则会启用组播
	*输入： cszMulticastIP	-组播IP
			ttl				-组播数据包的TTL(生存时间)
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRTPMediaMulticast(const char*cszMulticastIP,int ttl);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	/******************************************************************************
	* AddCapAudioAll
	*描述：添加音频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioAll(void);

	/******************************************************************************
	* RemoveCapAudioAll
	*描述：移除音频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapAudioAll(void);

	/******************************************************************************
	* AddCapG711A_64K
	*描述：添加音频G711A能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG711A_64K(void);

	/******************************************************************************
	* AddCapG711U_64K
	*描述：添加音频G711U能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG711U_64K(void);

	/******************************************************************************
	* AddCapAudioG722_64K
	*描述：添加音频G722-64K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG722_64K(void);

	/******************************************************************************
	* AddCapAudioG7221_24K
	*描述：添加音频G7221-24K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221_24K(void);

	/******************************************************************************
	* AddCapAudioG7221_32K
	*描述：添加音频G7221-32K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221_32K(void);

	/******************************************************************************
	* AddCapAudioG7221C_24K
	*描述：添加音频G7221C-24K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_24K(void);

	/******************************************************************************
	* AddCapAudioG7221C_32K
	*描述：添加音频G7221C-32K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_32K(void);

	/******************************************************************************
	* AddCapAudioG7221C_48K
	*描述：添加音频G7221C-48K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_48K(void);

	/******************************************************************************
	* AddCapAudioG729A
	*描述：添加音频G729A能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG729A(void);

	/******************************************************************************
	* AddCapAudioG719_32K
	*描述：添加音频G719-32K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_32K(void);

	/******************************************************************************
	* AddCapAudioG719_48K
	*描述：添加音频G719-48K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_48K(void);

	/******************************************************************************
	* AddCapAudioG719_64K
	*描述：添加音频G719_64K能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_64K(void);

	/******************************************************************************
	* AddCapVideoAll
	*描述：添加主流视频的所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoAll(void);

	/******************************************************************************
	* RemoveCapVideoAll
	*描述：移除主流视频的所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapVideoAll(void);

	/******************************************************************************
	* AddCapH263
	*描述：添加主流视频H263能力集
	*输入：	
			nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
			nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
			nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263(int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapVideoH263Plus
	*描述：添加主流视频H263+能力集
	*输入：	
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263Plus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH263PlusPlus
	*描述：添加主流视频H263++能力集
	*输入：	
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263PlusPlus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH264QCIF
	*描述：添加主流视频的H264QCIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264QCIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264CIF
	*描述：添加主流视频的H264CIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264CIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2644CIF
	*描述：添加主流视频的H264 4CIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH2644CIF(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264720P
	*描述：添加主流视频的H264720P能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264720P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2641080P
	*描述：添加主流视频的H2641080P能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH2641080P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP720P
	*描述：添加主流视频的H264HighProfile能力集,默认最高720P
	*输入：	
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264HP720P(int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP1080P
	*描述：添加主流视频的H264HighProfile能力集,默认最高1080P
	*输入：	
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264HP1080P(int nLevel=0);


	/******************************************************************************
	* AddCapDualVideoAll
	*描述：添加辅流视频的所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoAll(void);

	/******************************************************************************
	* RemoveCapDualVideoAll
	*描述：移除辅流视频的所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapDualVideoAll(void);

	/******************************************************************************
	* AddCapDualVideoH263
	*描述：添加辅流视频H263能力集
	*输入：	
			nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
			nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
			nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263(int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapDualVideoH263Plus
	*描述：添加辅流视频H263+能力集
	*输入：	
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263Plus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH263PlusPlus
	*描述：添加辅流视频H263++能力集
	*输入：	
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263PlusPlus(int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH264QCIF
	*描述：添加辅流视频的H264QCIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264QCIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264CIF
	*描述：添加辅流视频的H264CIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264CIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2644CIF
	*描述：添加辅流视频的H264 4CIF能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH2644CIF(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264720P
	*描述：添加辅流视频的H264720P能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264720P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2641080P
	*描述：添加辅流视频的H2641080P能力集
	*输入：	
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH2641080P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP720P
	*描述：添加辅流视频的H264HighProfile能力集,默认最高720P
	*输入：	
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264HP720P(int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP1080P
	*描述：添加辅流视频的H264HighProfile能力集,默认最高1080P
	*输入：	
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
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
	*描述：添加FECC控制能力集
	*输入：
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapFECCData(void);

	/******************************************************************************
	* RemoveCapAll
	*描述：移除所有媒体类型的所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapAll(void);

	/******************************************************************************
	* RemoveCapFECCDataAll
	*描述：移除FECC控制所有能力集
	*输入：	
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapFECCDataAll(void);

	int DAPSetLocalCapabilitis(void);

	int						AddCapAudioItem(X_AUDIO_CODEC_ID codecID);
	int						AddCapVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);
	int						AddCapDualVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel);

#endif

	/******************************************************************************
	* BuildLocalCapabilitisHeader
	*描述：组建CALL能力集头部树结点
	*输入：	
	*输出：无
	*返回值：成功返回非负值，失败返回-1
	*****************************************************************************/
	int BuildLocalCapabilitisHeader();

	/******************************************************************************
	* BuildSendLocalCapabilitis
	*描述：组建CALL能力集树结点并发送
	*输入：	
	*输出：无
	*返回值：成功返回非负值，失败返回-1
	*****************************************************************************/
	int BuildSendLocalCapabilitis();

	/******************************************************************************
	* BuildSendVideoOLCCapabilitis
	*描述：组建CALL视频通道能力集树结点并发送
	*输入：	
	*输出：无
	*返回值：成功返回非负值，失败返回-1
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
	bool					m_bAudioCapItemSetFirst;								//能力集项优先设置状态 控制先添加者优先
	bool					m_bVideoCapItemSetFirst;								//能力集项优先设置状态 控制先添加者优先
	bool					m_bDualVideoCapItemSetFirst;							//能力集项优先设置状态 控制先添加者优先
	int						m_nCapabilityNodeID;									//CALL能力集树结点
	VIDEO_FORMAT_TYPE		m_nMainVideoFormatType;									//本端主流视频设置的最大分辨率
	VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;								//本端辅流视频设置的最大分辨率
	std::string				m_strMediaMulticastIP;									//媒体流组播地址
	bool					m_bMediaMulticastEnable;								//媒体流启用组播 true启用 false不启用 默认false
	int						m_nMediaMulticastTTL;									//组播报文的数据包的TTL(生存时间) 0..255
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
	*描述：发送H245控制消息 包含flowControlIndication H239令牌相关 会控消息
	*输入：	
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendCallH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	void RecvH245ControlMessage(RvPvtNodeId message,cmH245MessageType messageType);
	void OnRecvH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);
	void RecvH245TerminalNumberAssign(int mcuNumber, int terminalNumber);
	void RecvGenericMessage(cmH245MessageType messageType,int nBitrate, int terLabel, int chID, int symBreaking=0,int response=0,int respValue=0);
	void RecvFlowControlCommand(H323Channel* pH323Channel, HCHAN hsChan, RvUint32 rate);
	int  GetH323ChannelLogicalNumber(int type,std::string & strMediaName);						//type=0 取发送通道的,其他值取接收通道的,媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	void GetH323ChannelLogicalNumMediaName(int type,int nLogicalNum,std::string &strMediaName);	//type=0 取发送通道的,其他值取接收通道的

	void OnAdapCallRecvChannelOpen(H323Channel * pH323Channel);
	void OnAdapCallChannelConnected(H323Channel * pH323Channel);
	void OnAdapCallChannelDisConnected(H323Channel * pH323Channel);
	virtual void OnA7ChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void OnAdapChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void OnRemoteCameraControl(H323Channel * pH323Channel,std::string strAction,int nValue);//远遥摄像头控制 //nValue 视频源ID或预置点
	int	 SendCameraControl(std::string strAction,int nValue);								//nValue 视频源ID或预置点

public:
	IMXEC_CALL_DIRECTION	m_nCallDir;
	std::string				m_strTelNum;
	std::string				m_strRemoteUserNonstandardInfo;
	std::string				m_strDisconnectReason;
	ConfInfo				m_ConfInfo;							//用于保存MCU会议能力集信息
	ConfInfo				m_TCSTerminalConfInfo;				//使用终端视频能力集配置
	int                     m_nTCSUseTerminal;                  //0不使用 1使用

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

	//H.235v3.6 8.6.2 改进的密钥更新
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
	int						m_nRemoteMainVideoOLCMaxMBPS;								//用于OLC能力集可选参数值 最大宏块处理速率
	int						m_nRemoteMainVideoOLCMaxFS;									//用于OLC能力集可选参数值 最大帧长
	int						m_nRemoteMainVideoOLCMaxDPB;								//用于OLC能力集可选参数值 最大解码图像缓存大小
	int						m_nRemoteMainVideoOLCMaxBR;									//用于OLC能力集可选参数值 最大视频码率
	int						m_nRemoteDualVideoOLCMaxMBPS;								//用于OLC能力集可选参数值 最大宏块处理速率
	int						m_nRemoteDualVideoOLCMaxFS;									//用于OLC能力集可选参数值 最大帧长
	int						m_nRemoteDualVideoOLCMaxDPB;								//用于OLC能力集可选参数值 最大解码图像缓存大小
	int						m_nRemoteDualVideoOLCMaxBR;									//用于OLC能力集可选参数值 最大视频码率
	int						m_nMainVideoFrameRate;										//default >0
	int						m_nSecondVideoFrameRate;									//default >0
	unsigned long			m_nLastSendGenIndTimestamp;
	unsigned long			m_nLastSendSecretUpdateTS;									//default 12 hours

public:
	int						m_nAudioUpdatePT;											//default 111
	int						m_nMainVideoUpdatePT;										//default 112
	int						m_nDualVideoUpdatePT;										//default 113
	int						m_nFECCUpdatePT;											//default 114
	int						m_nH235MediaUse;											//0不使用 1可选使用 2强制使用H235媒体流加密
	int						m_nH235AuthUse;												//0不使用 1使用
	int						m_nUseAlogrithmId;											//首选加密算法,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	int						m_nMediaEncrypSpecified;									//0为不使用指定密码(自动协商) 1 为使用指定密码
	char					m_strMediaEncrypPWD[64];									//使用指定密码时设置的加密密码

	int						m_nBandWidth;												//呼叫最大带宽 ARQ use

public:
	bool matchRemoteMainVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat);
	bool matchRemoteSecondVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat);

protected:
	// 以下字段用于flowcontrol调节对端发来的码率
	int					m_nMainVideoBitRate;			// 主视频的接收码率
	int					m_nMainVideoActualBitRate;		// 主视频的实际接收码率
	int					m_nSecondVideoBitRate;			// 辅视频的接受码率
	int					m_nSecondVideoActualBitRate;	// 辅视频的实际接受码率
	// 丢包统计
	unsigned long		m_nMainVideoStartRcvdPacketTimestamp;
	unsigned long		m_nMainVideoStartRcvdPacketSequence;
	unsigned long		m_nMainVideoRcvdPacketCount;
	double				m_dMainVideoRcvLostRate;
	unsigned long		m_nSecondVideoStartRcvdPacketTimestamp;
	unsigned long		m_nSecondVideoStartRcvdPacketSequence;
	unsigned long		m_nSecondVideoRcvdPacketCount;
	double				m_dSecondVideoRcvLostRate;
	// 最近请求关键帧的间隔
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
	void SeRegAccountCallMark(void); //设置手动注册呼叫前发送ARQ消息标记
	void OnCallRecvACF(H323RegUser * pH323RegUser);
	void LevelIncFromMaxFS(int nMaxFS,int & nLevel);

	void AllowToSendSecondVideo(bool bAllow); // 允许对端发送双流
	void SendFlowControlCommand(IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate); // 发流控命令

	void GetResolution(int& nRes, int &nFps, int nMaxMBPS, int nMaxFs, int nLevel, int nProfile);
	void SendChannelKeepAlive(void);                //add for 460client
	int					m_nUseFWNAT;				//是否启用H460 NAT穿透, 0 not use ,1 use,default 0
	int					m_nMasterCall;				//0主叫 ,1被叫
	std::string			m_strCalled;				//对方号码 被叫
	bool                m_bRegCall; //手动注册呼叫前发送ARQ消息标记,true表示已发送ARQ,false表示未发ARQ,默认false

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

	std::string			m_strHost;                  //MCU用
	int					m_nRegUseGK;				//MCU用 1 自动查找GK服务器,2 注册到指定的GK服务器
	cmAlias				m_GKAlias;                  //MCU用
	char				m_szGKAliasBuf[256];
	cmAlias				m_EPAlias;
	char				m_szEPAliasBuf[256];
	bool                m_bCallState;               //default false,true is Established
	std::string		    m_strE164ID;
	std::string		    m_strH323ID;
	int                 m_nMainVideoSendState;      //test
	int                 m_nSecondVideoSendState;
	int                 m_nAudioSendState;
	int                 m_nLocalHungUP;             //1 hungup call 本端主动挂断 2 网络挂断 3 远端正常挂断(接收到releaseComplete)

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

	void                OnRcvH245CapabilitiesSetChange(unsigned int nMediaType);  //终端用
	void                OnRcvChannelClose(H323Channel* pH323Channel);             //终端用
	void                SetH239TokenSendReqStatus(int status);
	int                 GetH239TokenSendReqStatus( );
	void                TestConfInfoCapAdd();            //mcu use test

	int                 m_bRoundTripMaxCount;            //4 times ,4*10s >dalay(32s)
	int                 m_nRoundTripSendCount;           //
	int                 m_nNETECNodeCount;               //NETECNode Create Count
	int                 m_nMasterSlaveValue;
	int					m_nCallNum;				         //有呼入或呼出为1 默认为0 终端用
	int 				m_nRemoteCapChaneged;            //能力集有更新 终端用 最大切换次数 32/4 -1
	int                 m_nForceVideoWid;                //设置强制主流发分辨率  0不强制 终端用
	int                 m_nForceVideoHei;
	int                 m_nForceDualVideoWid;            //设置强制辅流发分辨率  0不强制 终端用
	int                 m_nForceDualVideoHei;
	int                 m_nSendH239TokenReq;
	int                 m_nRecvGenericResp;              //收到GenericResp或channel active后为true,default false
	int                 m_nSendGenericReq;               //Send GenericReq后为true,default false
	int                 m_nRecvGenericReqCount;          //收到 GenericReq后计数,default 0
	bool			    m_bFlag;                         //Added by jiangdingfeng 20140929 增加是否需要重新生成SSRC参数
	unsigned long       m_nForceIP;
	bool                m_bForceIP;
	std::string         m_strMakeDestIP;
	std::string         m_strMakeSourceIP;

	//{{发送能力集接收能力集及协商能力集重定义处理 20141027
public:
	RecvCallCaps        m_RecvCallCaps;
	int					m_nRemoteMainVideoFrame;		//对方最大帧速率
	int					m_nRemoteSecondVideoFrame;		//对方最大帧速率
	int                 m_nCapDirectionDefault;         //传输方向 默认1 ,cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	std::string         m_strQ931CalledNumber;
	std::string         m_strQ931CallerNumber;
	EM_ENTITY_TYPE      m_nEntityType;                  //对端类型

	void                SetAudioCapsParam(int nDirection,X_AUDIO_CODEC_ID nAudioCodecID,int entryNum,h235MediaCap& H235Cap);
	void                SetVideoCapsParam(int nDirection,RecvVideoCap& item,int entryNum,h235MediaCap& H235Cap,int nDual); //nDual 0 is main video ,1 is dual video
	void                GetH235MediaCapInfo(h235MediaCap& H235Cap,int entryNum,int& nEncryption,unsigned int& nAlgorithm);
	void                OnRcvH245CapabilitiesTermUse(void* capabilities);

	void                GetVideoAllCaps(HPVT pvtTree,h235MediaCap& remoteH235Cap,int nDirection,char * capAltName,int nCapAltNode,int nEntryNum,int nDual); //nDual 0 is main video ,1 is dual video
	int                 GetH264CapOptionParam(HPVT pvtTree,int nodeId,char * subBuf,int type=0);   //type 0 is main video ,1 is dual video
	bool                H235CapNegotiate(int aEncry,int bEncry,unsigned int remoteAlgo,EncDecAlgorithmID& algoID);
	void                SetTerminalConfInfoCaps(void);    //配置终端能力集 终端用
	void                ConfCapabilitisDefault(void);     //能力方向默认及分辨率实际对应大小计算处理
	void                AddVideoConfInfoCaps(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nFrame,int nDirection,int nBitrate,int nDual); //nDual 0 is main video ,1 is dual video
	void                AddVideoConfInfoBaseCaps2(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nVideoFrameRate,int nDirection,int nVideoBitrate,int nDual);

	void                BuildConfInfoCaps(int nodeId);   //组建能力集信令
	void                SetMCUConfCapFromProduct(void);  //根据第三方设备型号重新配置MCU能力集 MCU用
	void                AddConfCapFromConfig();          //用于和第三方设备测试单能力

	int                 CapabilitisNegotiate(int nLocalPriority,bool bEncryOption); //nLocalPriority 1 本地优先 2 终端对端优先 3 A7MCU本地优先 4 A7MCU对端优先
	bool                CapabilitisAudioItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisDataItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem);
	bool                CapabilitisVideoItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem); //nDual 0 is main video ,1 is dual video

	void                SetTerminalH460NATFwOption(int option);    //终端用

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


	//type 0 local ,1 remote , ulTotalPackets -总共发送了多少个包 ulLostPackets -总共丢弃了多少个包
	//AvgPacketLossRate -平均丢包率 CurPacketLossRate -当前丢包率 nBitrate -码流
	virtual int GetMainVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	virtual int GetDualVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	//按分辨率大小对某类型能力排序
	void RecvCapDapCapsSort(VIDEC_CODEC_TYPE codecType);
	//提取某协议类型临时能力集 Negotiate , 如 H263P H263PP , H263 H263P
	void CapabilitisProvisionalNeg(VIDEC_CODEC_TYPE localType ,VIDEC_CODEC_TYPE remoteType,int nLocalPriority,bool bEncryOption);
	void CapabilitisNegMainVideoResult(NegMediaCap & NegItem);
	void CapabilitisNegDualVideoResult(NegMediaCap & NegItem);
	void DapChanegCallCaps(ConfInfo *pConfInfo);
	bool CheckSetH264FormatFromProductId(VIDEO_FORMAT_TYPE & nFormat);
	void ConfCapabilitisBitrateDefault(ConfInfo *pConfInfo);
	void SetBitrateFromCaller(int nBitrate);

	void TCSUseTerminalVideoConfCaps(void);   //mcu级联测试用 使用终端视频能力集配置
	bool CheckConfCapsSupported(ChannelMediaType nMedia,VIDEC_CODEC_TYPE codec,VIDEO_FORMAT_TYPE format);

};

#endif	//#ifndef __H323CALL_H__
