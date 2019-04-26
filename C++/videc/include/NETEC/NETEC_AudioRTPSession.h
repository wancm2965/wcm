//NETEC_AudioRTPSession.h
#ifndef __NETEC_AUDIORTPSESSION_H__
#define __NETEC_AUDIORTPSESSION_H__

#include <NETEC/NETEC_Export.h>

class NETEC_AudioRTPSessionCallback
{
public:
	virtual void OnNETEC_AudioRTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short usSequence,uint32_t ulTimestamp)=0;
	virtual void OnNETEC_AudioRTPSessionCallbackSendingPacket(unsigned char*pPacketData,int32_t nPacketLen,const void*pRemoteAddr,int32_t nRemoteAddrLen)=0;
};

class NETEC_API NETEC_AudioRTPSession
{
public:
	NETEC_AudioRTPSession(void){};
	virtual~NETEC_AudioRTPSession(void){};
public:
	virtual int32_t Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName)=0;
	virtual void Close(void)=0;
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort)=0;
	virtual void SendRTPPacket(unsigned char*pPacketData,int32_t nPacketLen)=0;
	virtual void OnRcvdPacket(unsigned char*pPacketData,int32_t nPacketLen,const void*pRemoteAddr,int32_t nRemoteAddrLen)=0;
	virtual void DoCheck(void)=0;

	static NETEC_AudioRTPSession*Create(NETEC_AudioRTPSessionCallback&rCallback);
	static NETEC_AudioRTPSession*CreateExt(NETEC_AudioRTPSessionCallback&rCallback);
};
#endif
