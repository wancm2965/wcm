//NETEC_Frame2RTP.h
#ifndef __NETEC_FRAME2RTP_H__
#define __NETEC_FRAME2RTP_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_Frame2RTP;
class NETEC_Frame2RTPCallback
{
public:
	NETEC_Frame2RTPCallback(void){};
	virtual~NETEC_Frame2RTPCallback(void){};
public:
	virtual void OnNETEC_Frame2RTPCallbackRTPPacketData(NETEC_Frame2RTP*pFrame2RTP,unsigned char*pRTPPacketData,int32_t nRTPPacketLen)=0;
};

class NETEC_API NETEC_Frame2RTP
{
public:
	NETEC_Frame2RTP(void){};
	virtual~NETEC_Frame2RTP(void){};
public:
	virtual int32_t Open(void)=0;
	virtual void Close(void)=0;
	virtual void SetRTPPayloadType(int32_t nPayloadType)=0;
	virtual int32_t GetRTPPayloadType(void)=0;
	virtual void SetMaxBitrate(int32_t nMaxBitrate)=0;
	virtual void SetFrameRate(int32_t nFrameRate)=0;
	virtual int32_t SetFrameData(const void*pData,int32_t nLen)=0;

	static NETEC_Frame2RTP*Create(NETEC_Frame2RTPCallback&rCallback,const char*cszName);
	static NETEC_Frame2RTP*CreateJitter(NETEC_Frame2RTPCallback&rCallback,const char*cszName);
};

#endif
