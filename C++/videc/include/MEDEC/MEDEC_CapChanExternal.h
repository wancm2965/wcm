//MEDEC_CapChanExternal.h
#ifndef __MEDEC_CAPCHANEXTERNAL_H__
#define __MEDEC_CAPCHANEXTERNAL_H__

#include <MEDEC/MEDEC_Export.h>

class MEDEC_CapChanExternalCallback
{
public:
	MEDEC_CapChanExternalCallback(void){};
	virtual~MEDEC_CapChanExternalCallback(void){};
public:
	virtual void OnMEDEC_CapChanExternalCallbackRecvdRTP(void*pPacketData,int nPacketLen)=0;
	virtual int OnMEDEC_CapChanExternalCallbackReadSamples(short*pSamples,int nSamples)=0;

	virtual int OnMEDEC_CapChanExternalGetFrameSamples( short*pSamples )=0; //liurui
};

class MEDEC_API MEDEC_CapChanExternal
{
public:
	MEDEC_CapChanExternal(void){};
	virtual~MEDEC_CapChanExternal(void){};
public:
	virtual int Open(const char*cszCodecName,int nSampleRate,int nBitrate)=0;
	virtual void Close(void)=0;
	virtual int GetPayloadType(void)=0;
	virtual int GetLevel(void)=0;
	virtual void SetVolume(int nVolume)=0;//0-10-20
	virtual void EnableVAD(int nVAD)=0;
	virtual unsigned int GetConferenceID(void)=0;
	virtual int JoinConference(unsigned int nConferenceID)=0;
	virtual void LeaveConference(unsigned int nConferenceID)=0;
	virtual int StartMixingWithWaveFile(const char*cszPathName)=0;
	virtual void StopMixingWithWaveFile(void)=0;

	static MEDEC_CapChanExternal*Create(MEDEC_CapChanExternalCallback&rCalllback);
};

#endif