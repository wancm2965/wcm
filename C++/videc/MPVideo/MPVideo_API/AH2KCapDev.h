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
	//���豸����ʼ�ɼ���������ƵԴ��ʱ��Ż�ɼ���ͼ�񣬳ɹ�����0��ʧ�ܷ���-1
	//nDevID���豸��ţ���0��ʼ
	//nFrameRate��֡�ʣ�1-30
	virtual int Open(int nDevID,int nFrameRate);
	//�ر��豸
	virtual void Close(void);
	//����ͼ�����
	//nBrightness:����
	//nContrast:�Աȶ�
	//nSaturation:���Ͷ�
	//nHue:ɫ��
	virtual void SetImageParam(int nBrightness=128/*����ֵ0-255*/, int nContrast=64/*�Աȶ�0-127*/, int nSaturation=64/*���Ͷ�0-127*/, int nHue=0/*ɫ��0-255*/,int nGamma=100/*٤��1-400*/,int nSharpen=0/*��0-127*/);
protected:
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource){};
	//pData:IYUV��ʽ������,Y+U+V
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