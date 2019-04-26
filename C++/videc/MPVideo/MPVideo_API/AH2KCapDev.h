//AH2KCapDev.h
#ifndef __AH2KCAPDEV_H__
#define __AH2KCAPDEV_H__

#include "VIDEC_AH2KCapDev.h"
#include "VIDEC_AH2KDev.h"
#include "VIDEC_convert.h"

class AH2KCapDev
	: public VIDEC_AH2KCapDev
	, public VIDEC_ImageSink
{
public:
	AH2KCapDev(VIDEC_AH2KCapDevCallback&rCallback);
	virtual~AH2KCapDev(void);
public:
	//打开设备，开始采集，当有视频源的时候才会采集到图像，成功返回0，失败返回-1
	//nDevID：设备序号，从0开始
	//nFrameRate：帧率，1-30
	virtual int Open(int nDevID,int nFrameRate);
	//关闭设备
	virtual void Close(void);
	//设置图像参数
	//nBrightness:亮度
	//nContrast:对比度
	//nSaturation:饱和度
	//nHue:色调
	virtual void SetImageParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/,int nGamma=100/*伽马1-400*/,int nSharpen=0/*锐化0-127*/);
protected:
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource){};
	//pData:IYUV格式的数据,Y+U+V
	virtual void OnReceivedImage(VIDEC_Image*pImage);
protected:
	VIDEC_AH2KCapDevCallback&	m_rCallback;
	VIDEC_AH2KDev*				m_pDev;
	VIDEC_Resize*				m_pResize;
	int							m_nDstBufLen;
	unsigned char*				m_pDstBuf;
	unsigned char*				m_dst[3];
};

#endif