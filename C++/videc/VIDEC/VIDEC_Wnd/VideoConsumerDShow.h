// VideoConsumerDShow.h: interface for the DSVideoConsumer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEOCONSUMERDSHOW_H__
#define __VIDEOCONSUMERDSHOW_H__


#include "VideoConsumerFilter.h"
#include "VideoConsumer.h"

class VideoConsumerDShow 
	: public VideoConsumer
{
public:
	VideoConsumerDShow(VideoConsumerCallback&rCallback);
	virtual ~VideoConsumerDShow(void);
public:
	virtual int Open(long nHandle,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType);
	virtual void Close(void);
	
	virtual void SetWndPos(int x,int y,int cx,int cy);
	virtual void Repaint(void);
	

	virtual int GetWidth(void){return m_nWidth;};
	virtual int GetHeight(void){return m_nHeight;};
	virtual int GetFrameRate(void){return m_nFrameRate;};

	virtual bool GetNV12(void){return m_bNV12;};
	virtual bool GetSupportScreenChanged(void){return true;};
protected:
	VideoConsumerCallback&	m_rCallback;

	typedef enum {
		VR_EVR=0,
		VR_VMR,
		VR_SDR,
	}VIDEO_RENDER_TYPE;

	IFilterGraph*			m_pFilterGraph;
	IBaseFilter*			m_pRenderFilter;
	IBaseFilter*			m_pColourFilter;
	VideoConsumerSource*	m_pVideoConsumerFilter;
	IMFVideoDisplayControl*	m_pVideoDisplay;
	IVMRWindowlessControl*	m_pWindowless;
	IVideoWindow*			m_pVideoWindow;
	IMediaControl*			m_pMediaControl;

	int						m_nWidth;
	int						m_nHeight;
	int						m_nFrameRate;
	HWND					m_hWnd;

	bool					m_bNV12;

	VIDEO_RENDER_TYPE		m_nVideoRenderType;
};	


#endif 