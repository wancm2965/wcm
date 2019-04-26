//SDDevCap.h
#ifndef __SDDEVCAP_H__
#define __SDDEVCAP_H__

#include "VIDEC_SDDevCap.h"
#include "VIDEC_OutputImage.h"
#include "VIDEC_OutputStream.h"

class SDDevCap
	: public VIDEC_SDDevCap
	, public VIDEC_OutputImage
	, public VIDEC_OutputStream
{
public:
	SDDevCap(VIDEC_SDDevCapCallback&rCallback);
	virtual~SDDevCap(void);
public:
	int Open(int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,int nQuality,bool bCBR,VIDEC_CODEC_TYPE CodecType);
	void Close(void);

	void OnVIDEC_OutputImageOutputData(void*pData,int nLen,int nWidth,int nHeight);
	int OnVIDEC_OutputImageGetFrameRate(void);
	
	//输出AVCON的视频包
	void OnVIDEC_OutputStreamOutputData(void*pData,int nLen);

	int OnVIDEC_OutputStreamGetBitrate(void);
	int OnVIDEC_OutputStreamGetMaxBitrate(void);
	int OnVIDEC_OutputStreamGetFrameRate(void);
	int OnVIDEC_OutputStreamGetQuant(void);
	int OnVIDEC_OutputStreamGetQMin(void);
	int OnVIDEC_OutputStreamGetQMax(void);

	VIDEC_CODEC_TYPE OnVIDEC_OutputStreamGetCodecType(void);
protected:
	VIDEC_SDDevCapCallback&	m_rCallback;

	int						m_nDevIndex;

	int						m_nFrameRate;
	int						m_nBitrate;
	int						m_nQuant;
	int						m_nQMin;
	int						m_nQMax;

	VIDEC_CODEC_TYPE		m_CodecType;
};

#endif