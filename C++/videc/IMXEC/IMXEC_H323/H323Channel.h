#ifndef __H323CHANNEL_H__
#define __H323CHANNEL_H__

#include "H323APIType.h"
#include <RVINC/cm.h>
#include <RVINC/rtp.h>
#include <RVINC/rtcp.h>
#include "../DefineHeader/H323DefineHeader.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>

#else

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPH323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#endif

#include <string>

//use h235 Media
#include <RVINC/rvrtpinterface.h>
#include "deseccryption.h"

////add medianetrtp channel 0615
#include "../IMXEC_PJSIP/MediaNet_Export.h"
class MediaNet_RTP;

#define BYTE	unsigned char
#define LPBYTE   BYTE*
#define USHORT	unsigned short

#define MAX_CAMERAINFO_LENGTH 256
///////////////////////////////////////////////////////////////////////////////////
#define CMAERA_DELAYTIME		500
#define UNIT_H281_TIME			50

#define H281_START				0x01
#define H281_CONTINUE			0x02
#define H281_STOP				0x03
#define H281_SELECT_VIDEOSRC	0x04
#define H281_SWITCH_VIDEOSRC	0x05
#define H281_STORE_PRESET       0x06
#define H281_ACTIVE_PRESET      0x07

#define H281_UP					0x30
#define H281_DOWN				0x20
#define H281_LEFT				0x80
#define H281_RIGHT				0xc0
#define H281_ZOOMIN				0x0c
#define H281_ZOOMOUT			0x08
#define H281_FOCUSNEAR			0x03
#define H281_FOCUSFAR			0x02

#define TYPE_CONTROL			0
#define TYPE_CAPABILITY			1
#define TYPE_CAMERA_POSITION_STATUS   2
#define LOW_PRIORITY			0
#define HIGH_PRIORITY			1

//recv and send local rtp port set same 20120229 add
enum ChannelMediaType{
	Enum_Channel_Media_Invalid=0,
	Enum_Channel_Media_Video,
	Enum_Channel_Media_Audio,		
	Enum_Channel_Media_H224,
	Enum_Channel_Media_DualVideo,
};

typedef struct RCMINFO
{
	int DataLen;
	BYTE m_bData[256];
	int m_nPriority;
	int m_nDstMcuNo;
	int m_nSrcMcuNo;
	int m_nDstTerNo;
	int m_nSrcTerNo;
	int m_nType;
}RCMInfoTag;

#define   MAX_LSD_LEN  256
typedef struct tagLSDStruct
{
	unsigned char LSDLen;
	unsigned char LSDChar[MAX_LSD_LEN];	
}LSDStruct;

void MakeReverseTable(LPBYTE lpRevTable, int nNum);

class CDataUDP;

class H323Call;
/////////////////////////////////////////////////////////////////////////////////
//add medianetrtp channel 0615
class H323Channel : public MediaNet_RTPSessionCallback
{
	friend class H323Call;
public:
	H323Channel(H323Call& rH323Call,HCHAN hsChan);
	~H323Channel();
public:
	X_AUDIO_CODEC_ID& AudioCodecID(void);
	VIDEC_CODEC_TYPE& VideoCodecType(void);
	VIDEO_FORMAT_TYPE& VideoFormatType(void);
	std::string&ChannelName(void);

	unsigned int& Direction(void);		// cmCapReceive/cmCapTransmit/cmCapReceiveAndTransmit

	int& PayloadType(void);			// G711A/G711U/G729/H261/H263/H224


	int& MaxBitrate(void);

	int  GetChannelInfo(CHANNEL_INFO& rChannelInfo);

	void SetLocalRTPAddress(unsigned long ip, unsigned short port);
	void SetRemoteRTPAddress(unsigned long ip, unsigned short port);
	void SetLocalRTCPAddress(unsigned long ip, unsigned short port);
	void SetRemoteRTCPAddress(unsigned long ip, unsigned short port);


	void SetLocalAddress(unsigned long& ulRtpIP, unsigned short& usRtpPort, unsigned long& ulRtcpIP, unsigned short& usRtcpPort);

	int RequestKeyFrame(void);
	int SendChannelOn(void);
	int SendChannelOff(void);

	virtual void OnChannelOffering(void);
	virtual void OnChannelConnected(cmChannelState_e state,cmChannelStateMode_e stateMode);
	virtual void OnChannelDisconnected(void);
	virtual void OnChannelRequestKeyFrame(void);
	//add VCF
	void OnChannelRequestFreeze(void);

	//
	void OnRecvdRTPPacket(char* pData, rtpParam& param);
	//Added by jiangdingfeng 20140929 ��������SSRC�ӿ�
	void GenerateSSRC();
	void SendData(char* buff, int datalen);
	void SendDataEx(char* buff, int datalen, rtpParam* p);

	//===AnnexQ
	int  AddAnnexQRtpHead(BYTE *tmp);
	void SendAnnexQInfo(RCMInfoTag rcmInfo);
	void RemoteCameraLeft();
	void RemoteCameraStopLeft();
	void RemoteCameraRight();
	void RemoteCameraStopRight();
	void RemoteCameraUp();
	void RemoteCameraStopUp();
	void RemoteCameraDown();
	void RemoteCameraStopDown();
	void RemoteCameraZoomIn();
	void RemoteCameraStopZoomIn();
	void RemoteCameraZoomOut();
	void RemoteCameraStopZoomOut();

	void RemoteSourceSelect(int vsID);
	void RemoteSourceSwitched(int vsID);
	void RemoteCameraPresetStore(int position);
	void RemoteCameraPresetActive(int position);

	USHORT crchware(USHORT data, USHORT genploy, USHORT accum);
	void EncodeBufferH281ToH224(RCMInfoTag rcmInfo, LSDStruct *plsd);
	int Q922Encode(BYTE *lpSendData, int nTotalNum, int nUseStart, int nUseEnd, BYTE *lpRevTable, int nCrcCheckInd);
	int Add0FromHDLC( BYTE *lpH224HDLC, int nTotalNum, int nStartPos, int nNum, BYTE *lpH224HDLCOut );
	void ReverseByte(BYTE *lpByte,int nNum, BYTE *lpRevTable);
	void MoveToPos(LPBYTE lpForeByte, LPBYTE lpBackByte, int nPos);

	void OnRecvAnnexQDataProc(unsigned char *pData, int nLen);
	void DealwithCameraInfoH224ToH281(const RCMInfoTag&rcmInfoTag);
	void DealWithCameraPositionStatus(const RCMInfoTag&rcmInfoTag);
	void DealWithCameraCapability(const RCMInfoTag&rcmInfoTag);
	void DealWithCameraControl(const RCMInfoTag&rcmInfoTag);
	void OnH281Start(BYTE bDirection, int nPeriod);
	void OnH281Continue(BYTE bDirection);
	void OnH281Stop(BYTE bDirection);
	void OnSelectVideoSrc(BYTE bDirection);
	void OnSwitchVideoSrc(BYTE bDirection, BYTE bMcuNo, BYTE bTerNo);
	void OnCameraPresetStore(BYTE bDirection);
	void OnCameraPresetActive(BYTE bDirection);

	int SearchDataBetween7E(BYTE* lpData, int nNum, BYTE* lpDataBuff, BYTE* lpRevTableTemp, BYTE* lpOutputData, int* nOutputNum);
	int Find7EInData(BYTE bInPut, BYTE* bKeep);
	int Del0FromHDLC(LPBYTE lpH224HDLC, int nNum);

public:
    H323Call&			m_rH323Call;		// �����ĺ��� 

    HCHAN				m_hsChan;			// �ײ��ŵ����

	cmChannelState_e	m_nChannelState;
	

	unsigned long	m_ulRTPLocalIP;			// �ŵ�����RTP��ַ
	unsigned short	m_nLocalRTPPort;		// �ŵ�����RTP�˿ں�
	unsigned long	m_ulRTPRemoteIP;		// �ŵ��Զ�RTP��ַ
	unsigned short	m_nRTPRemotePort;		// �ŵ��Զ�RTP�˿ں�

	unsigned long	m_ulRTCPLocalIP;		// �ŵ�����RTCP��ַ
	unsigned short	m_nLocalRTCPPort;		// �ŵ�����RTCP�˿ں�
	unsigned long	m_ulRTCPRemoteIP;		// �ŵ��Զ�RTCP��ַ
	unsigned short	m_nRTCPRemotePort;		// �ŵ��Զ�RTCP�˿ں�


	//==Fecc
	BYTE			m_CurVideoID;			//���ص�ǰ��ƵԴID
	unsigned short	m_nAnnexQSequence;
	unsigned int	m_nAnnexQTimeStamp;
	unsigned int	m_nAnnexQSSRC;

	char*			m_pBuf;
	int				m_nBufLen;
	unsigned int	m_nTime;

protected:

	HRTPSESSION			m_hRTP;
    HRTCPSESSION		m_hRTCP;

	std::string			m_strChannelName;
	X_AUDIO_CODEC_ID	m_AudioCodecID;
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	VIDEO_FORMAT_TYPE	m_VideoFormatType;
	int					m_nMaxBitrate;				// �������ʣ�Kbit/s��
	int 				m_nPayloadType;				// G711A/G711U/G729/H261/H263/H224

public:
	unsigned int        m_nAlgorithmId;				//�㷨ID
	bool                m_bEncryptionStatus;		//����״̬ true �м���, false �޼���
	bool				m_bEncryptionSecret;		//OLC��OLC ACKȡ���������Կ״̬ false û��ȡ��,true ȡ����Կ,Ĭ��false
	DesEncryption       m_DesEncryption;
	ChannelMediaType	m_nMediaType;
	unsigned int		m_nDirection;				// �ŵ����䷽�� 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit

	int					m_nH264Level;				// H264���level
	int					m_nWidth;					// �ֱ��ʿ�
	int					m_nHeight;					// �ֱ��ʸ�
	int					m_nLastRTPSeq;
	int					m_nTimestamp;
	int					m_nLastRTPTimestamp;
	int					m_nSeqCount;

public:
	unsigned int&		AlgorithmId(void);
	bool &				EncryptionStatus(void);		
	int&				H264Level(void);
	void				GetVideoFormatType(int & w,int & h);
	HCHAN				GetChannelHsChan(void);
	void				CloseChannel(void);

	char *				UpdateChannelSpecifiedKey(void);
	void				SetChannelUpdateSharedSecret(unsigned char * secret,int len);
	void				ChangeChannelPayloadType();
	int					GetChannelPayloadType();

	void				SetChannelUpdatePayloadType(int updatePT,int type=0);
	void				AddChannelEncryption();
	void				AddChannelEncryptionAlgoChange();	//�㷨�б������´���
	bool &				EncryptionSecretStatus(void);

	void                SetKeepAliveAddress(unsigned long ip, unsigned short port);//add  for 460client
	void                SendChannelKeepAlive(void);			// add for 460client
	void                SendAnnexQDataNETEC(char* buff, int datalen);
	void                SetH264CapOptionParam(int type=0); //0 MainVideo 1 DualVideo
	void                SetVideoFormatTypeFromMaxFSBitrate(int nType=0); //0 terminal use 1 mp mcu use
	VIDEO_FORMAT_TYPE   GetVideoFormatTypeFromMaxBitrate();
	VIDEO_FORMAT_TYPE   GetCompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2,int nType=0); //0 get max 1 get min

	char				m_strSharedSecretOLC[64];			//˫��Э�̵Ĺ�����Կ
	char				m_strSharedSecretUpdate[64];		//���µĹ�����Կ
	char				m_strSpecifiedSecretOLC[64];		//ָ���ı�ͨ����Կ
	char				m_strSpecifiedSecretUpdate[64];		//ָ���ı�ͨ��������Կ
	int 				m_nPayloadTypeUpdate;				//���µ�
	unsigned int        m_nAlgorithmIdUpdate;				//���µ��㷨ID
	bool				m_bSecretUpdate;					//��Կ���±�� false �и�����Ϊtrue
	DesEncryption       m_DecDesEncryption;
	int					m_nSecretUpdateTimes;
	int					m_nSecretUpdateMaxTimes;			//��Ϊż��
	int					m_nMsdStatus;

	unsigned long		m_ulKeepAliveIP;		// �ŵ��Զ˴򶴰����յ�ַ
	unsigned short		m_nKeepAlivePort;		// �ŵ��Զ˴򶴰����ն˿ں�
	bool				m_bGetKeepAlive;		// ȡ���򶴰���ַ���Ϊtrue,Ĭ��Ϊfalse
	CDataUDP *			m_pFWNAT_UDP;			// add for 460client
	int                 m_nRFCNumber;
	bool				m_bH235OLCCap;		    //OLC H235 Capȡ�������״̬ false û��,true ��,Ĭ��false
	bool                m_bH235Cap;             //H235 Cap Э�̳ɹ���Ϊtrue,Э��ʧ����Ϊfalse,Ĭ��false
	int                 m_nFlowCCBitRate;       //�յ����������� ��λ100 bit/s Ĭ��Ϊ1 flowControlCommand add

	int					m_nRemoteOLCMaxMBPS;	//����OLC��������ѡ����ֵ ����鴦������
	int					m_nRemoteOLCMaxFS;		//����OLC��������ѡ����ֵ ���֡��
	int					m_nRemoteOLCMaxDPB;		//����OLC��������ѡ����ֵ ������ͼ�񻺴��С
	int					m_nRemoteOLCMaxBR;		//����OLC��������ѡ����ֵ �����Ƶ����

	//add medianetrtp channel 0615
public:

	virtual void        OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void                MediaNetRtpChannelConnected(cmChannelState_e state,cmChannelStateMode_e stateMode);
	void                MediaNetRtpChannelDisconnected(void);
	void                MediaNetRtpSendData(char* buff, int datalen);
	void                OpenKeepAliveChannel(void);          //�������ʹ򶴰�ͨ��
	void                MediaNetRtpChannelOpen(int nDir);
	void                ChannelRTPClose(void * hRtp);
	void                ChannelRTPPause(void);
	int&                VideoFrameRate(void);


	//ulTotalPackets �ܹ������˶��ٸ��� ulLostPackets �ܹ������˶��ٸ���AvgPacketLossRate ƽ�������� CurPacketLossRate ��ǰ������
	void GetRecvStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	//ulTotalPackets �ܹ������˶��ٸ��� ulLostPackets �ܹ������˶��ٸ���AvgPacketLossRate ƽ�������� CurPacketLossRate ��ǰ������
	void GetSendStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	unsigned int GetCalcTimeStamp();

public:
	int                 m_nVideoFrameRate;
	bool                m_bUseMediaNetRTP;
	MediaNet_RTP *      m_pMediaNet_RTP;
	int                 m_nEndpontType;                     //0 terminal use 1 mcu mp use
	unsigned int        m_nRTPTime;
	unsigned int        m_nIncRTPTime;

};

typedef std::map<void*,H323Channel*>	MapH323Channel;

#endif	//#ifndef __H323CHANNEL_H__
