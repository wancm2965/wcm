//IMXEC_MixAVDispatch.h 
#ifndef __IMXEC_MIXAVDISPATCH_H__
#define __IMXEC_MIXAVDISPATCH_H__

#include <AVCONMP/IMXEC_Export.h>
#include <AVCONMP/IMXEC_VideoSetting.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <AUDEC/AUDEC_CodecID.h>

class IMXEC_MixAVDispatchCallback
{
public:
	IMXEC_MixAVDispatchCallback(void){};
	virtual~IMXEC_MixAVDispatchCallback(void){};
public:
	virtual void OnIMXEC_RequestMPChannel(unsigned int nChannelType,unsigned long ulSelfChannelID,VIDEC_CODEC_TYPE emVidEcCodecType,VIDEO_FORMAT_TYPE emVidFormatType,X_AUDIO_CODEC_ID emAudioCodecID)=0;
};

class IMXEC_API IMXEC_MixAVDispatch
{
public:
	IMXEC_MixAVDispatch(void){};
	virtual~IMXEC_MixAVDispatch(void){};

public:
	virtual void Init(IMXEC_MixAVDispatchCallback *pcallback)=0;
	virtual int Open(unsigned int nChannelType,unsigned long ulChannelID,unsigned long ul323EpAudChanID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,VIDEC_CODEC_TYPE emVidEcCodecType,VIDEO_FORMAT_TYPE emVidFormatType,X_AUDIO_CODEC_ID emAudioCodecID)=0;
	virtual void Close(unsigned long ulChannelID)=0;
	virtual void CloseAll(void)=0;

	static IMXEC_MixAVDispatch& Instance(void);
};

#endif