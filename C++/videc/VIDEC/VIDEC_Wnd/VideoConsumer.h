//VideoConsumer.h
#ifndef __VIDEOCONSUMER_H__
#define __VIDEOCONSUMER_H__

#include <VIDEC/VIDEC.h>

typedef enum 
{
	VC_CSP_YV12=0,
	VC_CSP_NV12,
	VC_CSP_RGB
}VC_CSP_TYPE;

class VideoConsumerCallback
{
public:
	virtual void OnVideoConsumerCallbackPull(unsigned char*pData,int nWidth,int nHeight,int nStride,int& nFrameRate,int nBitCount,int nFlip)=0;
	virtual void OnVideoConsumerCallbackPull(unsigned char**ppData,int nWidth,int nHeight,int& nStride,int& nFrameRate,VIDEC_Image**ppImage)=0;
	virtual void OnVideoConsumerCallbackDrawFailed(void)=0;
};

class VideoConsumer
{
public:
public:
	virtual int Open(long nHandle,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType=VC_CSP_YV12)=0;
	virtual void Close(void)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual int GetFrameRate(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual void Repaint(void)=0;
	virtual bool GetNV12(void)=0;
	virtual bool GetSupportScreenChanged(void)=0;

	static int GetSupportEVR(void);
};

VideoConsumer*CreateVideoConsumerDShow(VideoConsumerCallback&rCallback);

#endif

