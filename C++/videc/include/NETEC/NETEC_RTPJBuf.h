//NETEC_RTPJBuf.h
#ifndef __NETEC_RTPJBUF_H__
#define __NETEC_RTPJBUF_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_RTPJBuf;
class NETEC_RTPJBufCallback
{
public:
	virtual void OnNETEC_RTPJBufCallbackRTPPacket(NETEC_RTPJBuf*pRTPJBuf,unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short nSeqNo,uint32_t nTimestamp)=0;
};

class NETEC_API NETEC_RTPJBuf
{
public:
	NETEC_RTPJBuf(void){};
	virtual~NETEC_RTPJBuf(void){};
public:
	virtual int32_t Open(void)=0;
	virtual void Close(void)=0;

	virtual int32_t OnRecvdPacket(unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short nSeqNo,uint32_t nTimestamp)=0;

	static NETEC_RTPJBuf*Create(NETEC_RTPJBufCallback&rCallback);
};

#endif
