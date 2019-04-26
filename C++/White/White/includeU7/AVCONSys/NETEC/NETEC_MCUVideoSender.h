//NETEC_MCUVideoSender.h
#ifndef __NETEC_MCUVIDEOSENDER_H__
#define __NETEC_MCUVIDEOSENDER_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_MCUVideoSender
{
public:
	NETEC_MCUVideoSender(void){};
	virtual~NETEC_MCUVideoSender(void){};
public:
	virtual int32_t Open(void)=0;
	virtual void Close(void)=0;
	virtual int32_t SendVideo(unsigned char*pData,int32_t nLen)=0;
	virtual uint32_t GetVideoID(void)=0;
	virtual int32_t IsEnable(int32_t nFrameType)=0;
	virtual int32_t IsRequestKeyFrame(int32_t nFrameType)=0;
	virtual void SetMaxBitrate(int32_t nMaxBitrate)=0;

	static NETEC_MCUVideoSender*Create(uint32_t nVideoID);
};

#endif
