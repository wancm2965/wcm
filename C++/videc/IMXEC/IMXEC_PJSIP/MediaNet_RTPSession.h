//MediaNet_RTPSession.h
#ifndef __MediaNet_RTPSESSION_H__
#define __MediaNet_RTPSESSION_H__

#include "MediaNet_Export.h"
//#include <pjsip.h>
//#include <pjlib-util.h>
//#include <pjlib.h>
//#include <pjnath/ice_strans.h>
//#include <pjmedia/sdp.h>
//#include <pjmedia/transport.h>

//use medianetrtp
#include <platform/ipc.h>
#include <protocol/hpdatatrans.h>

//class MediaNet_RTPSessionCallback
//{
//public:
//	virtual void OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
//	//virtual void OnMediaNet_RTPSessionCallbackSendingPacket(unsigned char*pPacketData,int nPacketLen,const void*pRemoteAddr,int nRemoteAddrLen)=0;
//};

class MediaNet_API MediaNet_RTPSession
{
public:
	MediaNet_RTPSession(void){};
	virtual~MediaNet_RTPSession(void){};
public:
	virtual int Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName,int nDir=3,char * szKeyBuf=NULL,int nAlgoId=0)=0;
	//virtual int MediaNetICEOpen(pjmedia_transport * pMediaTransport)=0;
	////after open is call the function
	//virtual int MediaNetEncodeInOfferSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp,int nMediaIndex,int nCmpCount)=0;
	//virtual int MediaNetEncodeInAnswerSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp,int nMediaIndex,int nCmpCount)=0;
	virtual void AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame)=0;
	virtual void Close(void)=0;
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,void *tmp_pool,void *sdp_local,void *sdp_remote,int media_index,int nCmpCount, int nFlagReliable=0,unsigned short m_nRemoteRTCPPort=0,int nDir =3) =0; //nDir 1 recv , 2 send , 3 sendrecv
	virtual void SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount=0)=0;
	virtual void OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
	virtual void InitRtpParam2(int nMaxFrameSize, int nNetBand, int nFrameRate,int nMedia,int nCodecType,int nSSRC ,int nDir =3)=0;//1 audio 2 video 3 H239 4 Data
	virtual void SendRecvPause(int nDir =3) =0;//nDir 1 recv , 2 send , 3 sendrecv
	virtual void SendRecvStart(int nDir =3,void * pCallBack=NULL) =0;//nDir 1 recv , 2 send , 3 sendrecv ,pCallBack h323 Use

	static MediaNet_RTPSession*Create(MediaNet_RTPSessionCallback&rCallback,void * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE);

};
#endif//__MediaNet_RTPSESSION_H__
