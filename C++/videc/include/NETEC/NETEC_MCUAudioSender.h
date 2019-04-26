//NETEC_MCUAudioSender.h
#ifndef __NETEC_MCUAUDIOSENDER_H__
#define __NETEC_MCUAUDIOSENDER_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_MCUAudioSender
{
public:
	NETEC_MCUAudioSender(void){};
	virtual~NETEC_MCUAudioSender(void){};
public:
	virtual int32_t Open(void)=0;
	virtual void Close(void)=0;
	virtual int32_t SendAudio(unsigned char*pData,int32_t nLen)=0;
	virtual uint32_t GetAudioID(void)=0;
	virtual int32_t IsEnable(void)=0;

	static NETEC_MCUAudioSender*Create(uint32_t nAudioID);
};

#endif
