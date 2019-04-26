//MEDEC_PlayChan.h
#ifndef __MEDEC_PLAYCHAN_H__
#define __MEDEC_PLAYCHAN_H__

#include <MEDEC/MEDEC_Export.h>

class MEDEC_PlayChanCallback
{
public:
	MEDEC_PlayChanCallback(void){};
	virtual~MEDEC_PlayChanCallback(void){};
public:
	virtual void OnMEDEC_PlayChanCallbackSamples(short*pSamples,int nSamples)=0;
	virtual void OnRecvDTMFValue(char dtmf)=0;
};

class MEDEC_API MEDEC_PlayChan
{
public:
	MEDEC_PlayChan(void){};
	virtual~MEDEC_PlayChan(void){};
public:
	virtual int Open(const char*cszCodecName,int nSampleRate,int nBitrate)=0;
	virtual void Close(void)=0;
	virtual int GetPayloadType(void)=0;
	virtual int GetLevel(void)=0;
	virtual void SetVolume(int nVolume)=0;//0-10-20
	virtual int JoinConference(unsigned int nConferenceID)=0;
	virtual void LeaveConference(unsigned int nConferenceID)=0;

	virtual int OnRecvdRTP(const void*pPacketData,int nPacketLen)=0;

	static MEDEC_PlayChan*Create(MEDEC_PlayChanCallback&rCallback);
};

#endif
