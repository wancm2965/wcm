//VideoRTPSession.h
#ifndef __VIDEORTPSESSION_H__
#define __VIDEORTPSESSION_H__

#include "../jrtplib/src/rtpsession.h"
#include "../jrtplib/src/rtppacket.h"
#include "../jrtplib/src/rtpudpv4transmitter.h"
#include "../jrtplib/src/rtpipv4address.h"
#include "../jrtplib/src/rtpsessionparams.h"
#include "../jrtplib/src/rtperrors.h"
#ifndef WIN32
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else
	#include <winsock2.h>
#endif // WIN32
#include "../jrtplib/src/rtpsourcedata.h"

#include <NETEC/NETEC_VideoRTPSession.h>
//add
#include "PJSIP_RTPSession.h"
#include "MediaNet_RTPSession.h"

class VideoRTPSession;
class VideoRTPSessionCallback
{
public:
	VideoRTPSessionCallback(void){};
	virtual~VideoRTPSessionCallback(void){};
public:
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
	virtual void OnVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen)=0;
	virtual void OnVideoRTPSessionCallbackRequestKeyFrame(void)=0;
};

class VideoRTPSession
	: public RTPSession
	, public NETEC_VideoRTPSessionCallback
	, public PJSIP_RTPSessionCallback //add
	, public MediaNet_RTPSessionCallback //add
{
public:
	VideoRTPSession(VideoRTPSessionCallback&rCallback);
	virtual~VideoRTPSession(void);
public:
	//notice if pMediaEndpoint is not NULL use pjsip rtp
	int Open(const char*cszLocalIP,unsigned short nLocalPort,int nFlagReliable,const std::string&strCodecName,int nRTPPayloadType 
		,pjmedia_endpt * pMediaEndpoint=NULL,unsigned nClockRate=0,int nSamplePerFrame=0);
	// use ice nUseICE !=0 notice pMediaEndpoint and pMediaTransport is not NULL pMediaTransport 是调pjmedia_ice_create3创建的参数
	int PJSIPICEOpen(pjmedia_endpt * pMediaEndpoint=NULL,int nUseICE=0,pjmedia_transport * pMediaTransport=NULL);
	//after open is call the function,return 0 on success, or the appropriate error code
	int PJSIPEncodeInOfferSDP(pj_pool_t *sdp_pool,int nMediaIndex,pjmedia_sdp_session *sdp=NULL,int nCmpCount=1);
	int PJSIPEncodeInAnswerSDP(pj_pool_t *sdp_pool,int nMediaIndex,pjmedia_sdp_session * localSdp=NULL,pjmedia_sdp_session * remSdp=NULL,int nCmpCount=1);
	void AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame);
	void Close(void);
	//void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
	//	pj_pool_t *tmp_pool=NULL,pjmedia_sdp_session *sdp_local=NULL,pjmedia_sdp_session *sdp_remote=NULL,int media_index=0,int nCmpCount=1);

	void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
		pj_pool_t *tmp_pool=NULL,pjmedia_sdp_session *sdp_local=NULL,pjmedia_sdp_session *sdp_remote=NULL,int media_index=0,int nCmpCount=1,
        int nFlagReliable=0);

	void SendRTPPacket(const void *data,size_t len,uint8_t pt,bool mark,uint32_t timestampinc,unsigned char*pPacketData,int nPacketLen);
	//1 audio 2 video 3 H239 4 Data
	void InitRtpParam(int nMaxFrameSize, int nNetBand, int nFrameRate,int nMediaType,int nCodecType,int nSSRC=0,unsigned nClockRate=0 ,int nSamplePerFrame=0,int nFlagReliable=0);

protected:
	virtual void OnPollThreadStep();
	virtual void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
	virtual void OnNETEC_VideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen);
	virtual void OnNETEC_VideoRTPSessionCallbackRequestKeyFrame(void);
	virtual void OnNETEC_VideoRTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short usSequence,uint32_t ulTimestamp);
	virtual void OnNETEC_VideoRTPSessionCallbackSendingPacket(unsigned char*pPacketData,int32_t nPacketLen,const void*pRemoteAddr,int32_t nRemoteAddrLen){};
protected:
	VideoRTPSessionCallback&	m_rCallback;
	NETEC_VideoRTPSession*		m_pSession;
public:
	//add
	virtual void OnPJSIP_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	bool m_bUsePJSIPRTP;
	PJSIP_RTPSession* m_pPJSIPRTPSession;
	virtual void OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	bool m_bUseMediaNetRTP;
	MediaNet_RTPSession* m_pMediaNetRTPSession;

	bool m_bRecvPause;
	void PacketRecvPause();
	void PacketRecvRestart();

};

#endif