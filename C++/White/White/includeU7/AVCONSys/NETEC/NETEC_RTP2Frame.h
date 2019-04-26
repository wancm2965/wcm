//NETEC_RTP2Frame.h
#ifndef __NETEC_RTP2FRAME_H__
#define __NETEC_RTP2FRAME_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_RTP2Frame;
class NETEC_RTP2FrameCallback
{
public:
	NETEC_RTP2FrameCallback(void){};
	virtual~NETEC_RTP2FrameCallback(void){};
public:
	virtual void OnNETEC_RTP2FrameCallbackFramePacket(NETEC_RTP2Frame*pRTPFrame,unsigned char*pPacketData,int32_t nPacketLen,int32_t nKeyFrame)=0;
};

class NETEC_API NETEC_RTP2Frame
{
public:
	NETEC_RTP2Frame(void){};
	virtual~NETEC_RTP2Frame(void){};
public:
	virtual int32_t Open(int32_t nHeaderMargin=0)=0;
	virtual void Close(void)=0;
	virtual int32_t GetFrame(unsigned char*pFrameBuf,int32_t nBufLen)=0;
	virtual void OnRecvdRTPPacket(void*pPacketData,int32_t nPacketLen)=0;

	static NETEC_RTP2Frame*Create(NETEC_RTP2FrameCallback&rCallback,const char*cszName);
};

#endif