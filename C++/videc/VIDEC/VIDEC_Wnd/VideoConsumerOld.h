//VideoConsumerOld.h
#ifndef __VIDEOCONSUMSEROLD_H__
#define __VIDEOCONSUMSEROLD_H__

#include "VideoConsumer.h"
#include "IYUVDraw.h"
#include "XThreadBase.h"

class VideoConsumerOld
	: public VideoConsumer
	, public XThreadBase
{
public:
	VideoConsumerOld(VideoConsumerCallback&rCallback);
	virtual~VideoConsumerOld(void);
public:
	virtual int Open(long nHandle,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType);
	virtual void Close(void);
	virtual int GetWidth(void);
	virtual int GetHeight(void);
	virtual int GetFrameRate(void);
	virtual void SetWndPos(int x,int y,int cx,int cy);
	virtual void Repaint(void);
	virtual bool GetNV12(void){return m_bNV12;};
	virtual bool GetSupportScreenChanged(void){return true;};
protected:
	virtual bool IsPending(void);
	virtual bool Running(void);

	virtual void ThreadProcMain(void);
protected:
	VideoConsumerCallback&	m_rCallback;
	bool					m_bRunning;

	HWND					m_hVideoWnd;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nFrameRate;

	IYUVDraw*				m_pDraw;

	unsigned long			m_nLastDrawTimestamp;
	unsigned long			m_nNextFrameInterval;

	bool					m_bNV12;
};

#endif