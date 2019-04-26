//ScreenCapChanRTP.h
#ifndef __SCREENCAPCHANRTP_H__
#define __SCREENCAPCHANRTP_H__

#include <string>
#include <map>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include "RTPHeader.h"
#include "../IMXEC_Util/XAutoLock.h"
#include "MediaCall.h"


class ScreenCapChanRTP
	: public VIDEC_CapChanRTPCallback
	, public VIDEC_StreamSink
{
public:
	ScreenCapChanRTP(void);
	virtual~ScreenCapChanRTP(void);
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate);
	virtual void Close(void);
	virtual void SetScreenIndex(int nScreenIndex);
	virtual void AddMediaCall(MediaCall*pMediaCall);
	virtual bool RemoveMediaCall(MediaCall*pMediaCall);
	virtual int GetMediaCallCount(void);
	virtual bool RequestKeyFrame(MediaCall*pMediaCall);
protected:
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0){};
protected:
	typedef std::map<void*,MediaCall*>				MapMediaCall;
protected:
	XCritSec				m_csMapMediaCall;
	MapMediaCall			m_MapMediaCall;

	VIDEC_CapChanRTP*		m_pCapChan;
	VIDEC_CODEC_TYPE		m_nCodecType;
	int						m_nScreenIndex;
	VIDEC_MPScreen*			m_pMPScreen;
};

class ScreenCapChanRTPMain
	: public ScreenCapChanRTP
{
public:
	ScreenCapChanRTPMain(void){};
	virtual~ScreenCapChanRTPMain(void){};
protected:
	virtual void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
};

class ScreenCapChanRTPSecond
	: public ScreenCapChanRTP
{
public:
	ScreenCapChanRTPSecond(void){};
	virtual~ScreenCapChanRTPSecond(void){};
protected:
	virtual void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
};


#endif
