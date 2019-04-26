//HPMP_Transcode.h
#ifndef __HPMP_TRANSCODE_H__
#define __HPMP_TRANSCODE_H__

#include <HPMP/HPMP_Export.h>

class HPMP_API HPMP_Transcode;
class HPMP_TranscodeCallback
{
public:
	virtual void OnHPMP_TranscodeCallbackPacketData(HPMP_Transcode*pTranscode,void*pPacketData,int nPacketLen){};
};

class HPMP_API HPMP_Transcode
{
public:
	HPMP_Transcode(void){};
	virtual~HPMP_Transcode(void){};
public:
	virtual int Open(int nBitrate,int nBitrateControlType)=0;
	virtual void Close(void)=0;
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual void SetSkipFrameCount(int nSkipFrameCount)=0;//nSkipFrameCount(0:≤ªΩµ÷°£¨1£∫Ωµ1∞Î÷°£¨2£∫Ωµ1/4)

	static HPMP_Transcode*Create(HPMP_TranscodeCallback&rCallback);
};

#endif
