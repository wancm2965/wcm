//MPScreen.h
#ifndef __MPSCREEN_H__
#define __MPSCREEN_H__

#include <map>
#include <MPVideo/MPVideo.h>
#include "../MPVIDEO_API/XImageSource.h"
#include "XThreadBase.h"
#include "MPVIDEO_convert.h"

class MPScreen
	: public MPVideo_MPScreen
	, public XImageSource
{
public:
	MPScreen(void);
	virtual~MPScreen(void);
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate);
	virtual void Close(void);
	virtual int SetScreenIndex(int nScreenIndex);
	virtual int GetScreenIndex(void);
	virtual int GetWidth(void);
	virtual int GetHeight(void);
	virtual int GetFrameRate(void);

	virtual int AddImageSink(MPVideo_ImageSink*pImageSink);
	virtual void RemoveImageSink(MPVideo_ImageSink*pImageSink);

	virtual void OnCaptureImage(MPVideo_Image*pImage);
protected:
	int			m_nWidth;
	int			m_nHeight;
	int			m_nFrameRate;
	int			m_nScreenIndex;
	int			m_nFrameRateCount;

	MPVideo_Resize*			m_pResize;
	int						m_nDstBufLen;
	unsigned char*			m_pDstBuf;
	unsigned char*			m_dst[3];
};


#endif
