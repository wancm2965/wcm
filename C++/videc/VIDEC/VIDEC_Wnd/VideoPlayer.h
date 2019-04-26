//VideoPlayer.h
#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__

#include "VIDEC_Codec.h"
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "VideoPlayerWnd.h"
#include <VIDEC/VIDEC_Player.h>
#include <VIDEC/VIDEC_VQD.h>
#include "../VIDEC_Codecs/XDecoderStreamBase.h"

class VideoPlayer 
	: public XDecoderStreamBase
	, public VIDEC_Player
	, public VIDEC_VQDCallback
{
public:
	VideoPlayer(VIDEC_PlayerCallback&rCallback);
	virtual ~VideoPlayer(void);
public:
	int Open(HWND hWnd, unsigned long*lpnVideoTimestamp, unsigned long*lpnAudioTimestamp, char* pBgBmp = NULL, int bgcolr = -1, int bgcolg = -1, int bgcolb = -1);
	void Close(void);

#ifdef _TIANZHU_CACHE_

	//天竺项目：添加一个接口，运行时设置视频缓冲
	void SetCacheFrameCount(int nCacheFrameCount);

#endif

	int GetVideoWidth(void);
	int GetVideoHeight(void);

	HWND GetWnd(void);
	bool IsWindowVisible(void);
	bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags);

	double GetAverageFPS(void);
	
	void SetSynFlag(int nSynFlag);
	bool HasVideo(void);

	int PlayData(void*pPacketData,int nPacketSize);
	int GetDecoderType(void);
	int GetVideoShowFrameRate(void);
	int GetVideoDecodeFrameRate(void);
	int GetVideoRecvFrameRate(void);
	int GetVideoShowType(void);
	virtual void SetVideoDelay(int nMilliSecond);
	virtual int SetVideoSkipFrame(int nInterval,int nSkip);
	bool CaptureToBMPFile(const char*cszFile);
	virtual void ReDraw(void);
	//设置视频属性
	int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);
	int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	virtual int GetSupportPreview(void);
	VIDEC_Image*GetImage(void);
	void ResetImage(VIDEC_Image*pImage);

	virtual void SetEnableVideo(int nEnable);
	virtual int GetEnableVideo(void);

	virtual int StartVQD(unsigned long nDetectFlags);
	virtual void StopVQD(void);
	void OnCallbackDrawD3D9DC(void* hDC);

	void ReSetFrameRate(void);
protected:
	virtual void OnDecoderStreamCallbackFrame(VIDEC_Image*pImage);
	bool IsPending(void);
	virtual void OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp);
	virtual int DoDecodePacket(void*pPacketData,int nPacketSize,int nFrameRate);
	
	virtual void OnVIDEC_VQDCallbackResult(unsigned long nDetectResult);
protected:
	VIDEC_PlayerCallback&			m_rCallback;
	VideoPlayerWnd*					m_pVideoPlayerWnd;
	unsigned long*					m_lpnVideoTimestamp;
	unsigned long*					m_lpnAudioTimestamp;
	int								m_nSynFlag;
	unsigned long					m_nLastAudioTimestamp;
	unsigned long					m_nLastUpdateAudioTimestamp;
	unsigned long					m_nNewFrameTimestamp;

	unsigned long					m_nLastPaintedTimestamp;
	unsigned long					m_nLastFrameTimestamp;

	XCritSec						m_csVQD;
	VIDEC_VQD*						m_pVQD;

	unsigned long					m_nLastCountTimestamp;
	unsigned long					m_nTotalCount;
	unsigned long					m_nCountIndex;

	unsigned long					m_nTotalLoopCount;
	unsigned long					m_nLoopCountIndex;

	int								m_nNextSkipCount;
	int								m_nDecoderType;

	int								m_nFrameCount;
	unsigned long					m_nLastTS;

	int								m_nDelayInterval;
	int								m_nDelaySkip;
	bool							m_bPausePlay;

	int								m_nCurDelayInterval;
	int								m_nCurDelaySkip;

	int								m_nRecvFrameRate;
	unsigned long					m_ulLastCountFrameTimestamp;
	unsigned long					m_nRecvFrameRateCount;
};

#endif
