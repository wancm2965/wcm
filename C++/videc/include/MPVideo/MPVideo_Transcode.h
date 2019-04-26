//MPVideo_Transcode.h
#ifndef __MPVIDEO_TRANSCODE_H__
#define __MPVIDEO_TRANSCODE_H__

#include <MPVideo/MPVideo_Export.h>

class MPVIDEO_API MPVideo_Transcode;
class MPVideo_TranscodeCallback
{
public:
	virtual void OnMPVideo_TranscodeCallbackPacketData(MPVideo_Transcode*pTranscode,void*pPacketData,int nPacketLen){};
};

class MPVIDEO_API MPVideo_Transcode
{
public:
	MPVideo_Transcode(void){};
	virtual~MPVideo_Transcode(void){};
public:
	virtual int Open(int nBitrate,int nBitrateControlType)=0;
	virtual void Close(void)=0;
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual void SetSkipFrameCount(int nSkipFrameCount)=0;//nSkipFrameCount(0:≤ªΩµ÷°£¨1£∫Ωµ1∞Î÷°£¨2£∫Ωµ1/4)

	static MPVideo_Transcode*Create(MPVideo_TranscodeCallback&rCallback);
};

#endif
