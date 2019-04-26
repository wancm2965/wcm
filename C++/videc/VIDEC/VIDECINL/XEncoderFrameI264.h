//XEncoderFrameI264.h
#ifndef __XENCODERFRAMEI264_H__
#define __XENCODERFRAMEI264_H__

#include "pipeline_encode.h"
#include "VIDECINLAPI.h"

class XEncoderFrameI264 
	: public IntelH264EncoderCallback
{
public:
	XEncoderFrameI264(void);
	virtual ~XEncoderFrameI264(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData);
	void Close(void);
	int Encode(unsigned char*pData,int nLen,int nRequestKeyFrame);
protected:
	void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame);
protected:
	CEncodingPipeline*	m_pEncodingPipeLine;
	VIDECINL_ENCODER_CB m_encoder_cb;
	void*				m_pUserData;
	int					m_nWidth;
	int					m_nHeight;
	int					m_nY;
	int					m_nUV;

	unsigned long m_nLastResetIMSDKTimestamp;
};

void IntelH264GetEncoderCount(int& nCurCount,int& nMaxCount);

extern int IntelH264HardwareEncoderTry(unsigned long nWidth,unsigned long nHeight);
extern void IntelH264HardwareEncoderSetMaxCount(unsigned int nMaxCount);

#endif