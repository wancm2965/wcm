//PJSIP_RTPSession.h
#ifndef __PJSIP_RTPSESSION_H__
#define __PJSIP_RTPSESSION_H__

#include "PJSIP_Export.h"
#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjnath/ice_strans.h>
#include <pjmedia/sdp.h>
#include <pjmedia/transport.h>

class PJSIP_RTPSessionCallback
{
public:
	virtual void OnPJSIP_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
	//virtual void OnPJSIP_RTPSessionCallbackSendingPacket(unsigned char*pPacketData,int nPacketLen,const void*pRemoteAddr,int nRemoteAddrLen)=0;
};

class PJSIP_API PJSIP_RTPSession
{
public:
	PJSIP_RTPSession(void){};
	virtual~PJSIP_RTPSession(void){};
public:
	virtual int Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName)=0;
	virtual int PJSIPICEOpen(pjmedia_transport * pMediaTransport)=0;
	//after open is call the function
	virtual int PJSIPEncodeInOfferSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp,int nMediaIndex,int nCmpCount)=0;
	virtual int PJSIPEncodeInAnswerSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp,int nMediaIndex,int nCmpCount)=0;
	virtual void AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame)=0;
	virtual void Close(void)=0;
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
		pj_pool_t *tmp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_remote,int media_index,int nCmpCount)=0;
	virtual void SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount=0)=0;
	virtual void OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;

	static PJSIP_RTPSession*Create(PJSIP_RTPSessionCallback&rCallback,pjmedia_endpt * pMediaEndpoint
		,unsigned nClockRate,int nSamplePerFrame,int nUseICE);

};
#endif//__PJSIP_RTPSESSION_H__
