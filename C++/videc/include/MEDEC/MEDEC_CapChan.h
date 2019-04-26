//MEDEC_CapChan.h
#ifndef __MEDEC_CAPCHAN_H__
#define __MEDEC_CAPCHAN_H__

#include <MEDEC/MEDEC_Export.h>


class MEDEC_CapChanCallback
{
public:
	MEDEC_CapChanCallback(void){};
	virtual~MEDEC_CapChanCallback(void){};
public:
	virtual void OnMEDEC_CapChanCallbackRecvdRTP(void*pPacketData,int nPacketLen)=0;
	virtual void OnMEDEC_CapChanCallbackSamples(short*pSamples,int nSamples)=0;
};

class MEDEC_API MEDEC_CapChan
{
public:
	MEDEC_CapChan(void){};
	virtual~MEDEC_CapChan(void){};
public:
	virtual int Open(const char*cszCodecName,int nSampleRate,int nBitrate,int nFramesPerPacket)=0;
	virtual void Close(void)=0;
	virtual int GetPayloadType(void)=0;
	virtual int GetLevel(void)=0;
	virtual void SetVolume(int nVolume)=0;//0-10-20
	virtual void EnableVAD(int nVAD)=0;
	virtual unsigned int GetConferenceID(void)=0;
	virtual int StartMixingWithWaveFile(const char*cszPathName)=0;
	virtual void StopMixingWithWaveFile(void)=0;

	static MEDEC_CapChan*Create(MEDEC_CapChanCallback&rCalllback);

	virtual unsigned int GetRecordConferenceID(void)=0;
};

#endif