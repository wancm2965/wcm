//NETEC_VideoRTPSession.h
#ifndef __NETEC_VIDEORTPSESSION_H__
#define __NETEC_VIDEORTPSESSION_H__

#include <NETEC/NETEC_Export.h>

class NETEC_VideoRTPSessionCallback
{
public:
	virtual void OnNETEC_VideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int32_t nFrameLen)=0;
	virtual void OnNETEC_VideoRTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short usSequence,uint32_t ulTimestamp)=0;
	virtual void OnNETEC_VideoRTPSessionCallbackSendingPacket(unsigned char*pPacketData,int32_t nPacketLen,const void*pRemoteAddr,int32_t nRemoteAddrLen)=0;
	virtual void OnNETEC_VideoRTPSessionCallbackRequestKeyFrame(void)=0;
};

class NETEC_API NETEC_VideoRTPSession
{
public:
	NETEC_VideoRTPSession(void){};
	virtual~NETEC_VideoRTPSession(void){};
public:
	virtual int32_t Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName)=0;
	virtual void Close(void)=0;
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort)=0;
	virtual void SendFrame(const void*pFrameData,int32_t nFrameLen,int32_t nKeyFrame)=0;
	virtual void SendRTPPacket(unsigned char*pPacketData,int32_t nPacketLen)=0;
	virtual void OnRcvdPacket(unsigned char*pPacketData,int32_t nPacketLen,const void*pRemoteAddr,int32_t nRemoteAddrLen)=0;
	virtual void DoCheck(void)=0;

	static NETEC_VideoRTPSession*Create(NETEC_VideoRTPSessionCallback&rCallback);
	static NETEC_VideoRTPSession*CreateExt(NETEC_VideoRTPSessionCallback&rCallback);
};
#endif
