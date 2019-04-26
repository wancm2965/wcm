//NETEC_MCUAudioReceiver.h
#ifndef __NETEC_MCUAUDIORECEIVER_H__
#define __NETEC_MCUAUDIORECEIVER_H__

#include <NETEC/NETEC_Export.h>
#include <NETEC/NETEC.h>

class NETEC_API NETEC_MCUAudioReceiver;
class NETEC_MCUAudioReceiverCallback
{
public:
	virtual void OnNETEC_MCUAudioReceiverCallbackPacket(NETEC_MCUAudioReceiver*pReceiver,unsigned char*pPacketData,int32_t nPacketLen)=0;
};

class NETEC_API NETEC_MCUAudioReceiver
{
public:
	NETEC_MCUAudioReceiver(void){};
	virtual~NETEC_MCUAudioReceiver(void){};
public:
	virtual void Close(void)=0;
	virtual int32_t EnableAudio(int32_t nEnable)=0;
	virtual int32_t IsAudioEnable(void)=0;

	static NETEC_MCUAudioReceiver*Create(NETEC_MCUAudioReceiverCallback&rCallback,uint32_t nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
};


#endif
