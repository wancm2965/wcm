//VideoRTPCall.h
#ifndef __VIDEORTPCALL_H__
#define __VIDEORTPCALL_H__

#include <VIDEC/VIDEC_RTPCall.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include "../VIDEC_RTP/VideoRTPSession.h"

class VideoRTPCall
	: public VIDEC_RTPCall
	, public VIDEC_CapChanRTPCallback
	, public VideoRTPSessionCallback
	, public VIDEC_WndCallback
{
public:
	VideoRTPCall(void);
	virtual~VideoRTPCall(void);
public:
	virtual int Open(HWND hLocalWnd,int nDevID,int nLocalWidth,int nLocalHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nCodecType,int nPayloadType,const char*cszLocalIP,unsigned short nLocalPort,HWND hPeerWnd,const char*cszPeerIP,unsigned short nPeerPort);
	virtual void Close(void);

	virtual void SetLocalWndPos(int x,int y,int cx,int cy);
	virtual int GetLocalWidth(void);
	virtual int GetLocalHeight(void);
	virtual HWND GetLocalWnd(void);

	virtual void SetPeerWndPos(int x,int y,int cx,int cy);
	virtual int GetPeerWidth(void);
	virtual int GetPeerHeight(void);
	virtual HWND GetPeerWnd(void);

protected:
	virtual void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
protected:
	VIDEC_Wnd*			m_pLocalWnd;
	VIDEC_CapDev*		m_pDev;
	VIDEC_CapChanRTP*	m_pCapChanRTP;

	VIDEC_Wnd*			m_pPeerWnd;
	VIDEC_PlayChanRTP*	m_pPlayChanRTP;
	VideoRTPSession*	m_pVideoRTPSession;
};

#endif
