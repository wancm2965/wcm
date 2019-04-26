//VideoRTPCall.h
#ifndef __VIDEORTPCALL_H__
#define __VIDEORTPCALL_H__

#include <MPVideo/MPVIDEO_RTPCall.h>
#include <MPVideo/MPVIDEO_CameraDev.h>
#include <MPVideo/MPVIDEO_AH2KDev.h>
#include <MPVideo/MPVIDEO_AH400Dev.h>
#include "../MPVIDEO_RTP/VideoRTPSession.h"

class VideoRTPCall
	: public MPVIDEO_RTPCall
	, public MPVIDEO_CapChanRTPCallback
	, public VideoRTPSessionCallback
{
public:
	VideoRTPCall(void);
	virtual~VideoRTPCall(void);
public:
	virtual int Open(HWND hLocalWnd,int nDevID,int nLocalWidth,int nLocalHeight,int nFrameRate,int nBitrate,MPVIDEO_CODEC_TYPE nCodecType,int nPayloadType,const char*cszLocalIP,unsigned short nLocalPort,HWND hPeerWnd,const char*cszPeerIP,unsigned short nPeerPort);
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
	virtual void OnMPVIDEO_CapChanRTPCallbackPacketData(MPVIDEO_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
protected:
	MPVIDEO_Wnd*			m_pLocalWnd;
	MPVIDEO_CapDev*		m_pDev;
	MPVIDEO_CapChanRTP*	m_pCapChanRTP;

	MPVIDEO_Wnd*			m_pPeerWnd;
	MPVIDEO_PlayChanRTP*	m_pPlayChanRTP;
	VideoRTPSession*	m_pVideoRTPSession;
};

#endif
