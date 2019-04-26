//DVCCap.h
#ifndef __DVCCAP_H__
#define __DVCCAP_H__

#include <VIDEC/VIDEC_DVC.h>
#include "DVCWrapper.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_convert.h"
#include "XAutoLock.h"
#include <VIDEC/VIDEC_Decoder.h>

class DVCCap
	: public VIDEC_DVC
	, public VIDEC_DecoderStreamCallback
{
public:
	DVCCap(VIDEC_DVCCallback&rCallback);
	virtual~DVCCap(void);
public:
	virtual int Open(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
	virtual void Close(void);
	virtual int RequestKeyFrame(void);
	virtual int SetParams(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
	virtual int SendData(const char*pData,int nLen);

	void OnVideoCallback(unsigned char*pFrameData,int nFrameLen);
	void OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage);
protected:
	VIDEC_DVCCallback&	m_rCallback;

	void*				m_pHandle;
	VIDEC_DecoderStream*m_pDecoder;

};

#endif
