//AH2KCap.h
#ifndef __AH2KCAP_H__
#define __AH2KCAP_H__

#include <VIDEC/VIDEC_AH2K.h>
#include "AH2KWrapper.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_convert.h"
#include "XAutoLock.h"
#include "libxpostproc.h"

class AH2KCap
	: public VIDEC_AH2K
{
public:
	AH2KCap(VIDEC_AH2KCallback&rCallback);
	virtual~AH2KCap(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate);
	virtual void Close(void);
	virtual int StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax){return -1;};
	virtual int RequestKeyFrame(void){return -1;};
	virtual int StopVideoStream(void){return -1;};

	void* OnVideoCallback(unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType);
	void OnAudioCallback(short*pSamples,int nSamples,int nSampleRate,unsigned long nTimestamp);

	void*OnVideoFrameData(unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType);
protected:
	VIDEC_AH2KCallback&	m_rCallback;

	void*				m_pHandle;
	int					m_nScaleType;
	int					m_nActualWidth;
	int					m_nActualHeight;
	int					m_nVirtualWidth;
	int					m_nVirtualHeight;
	int					m_nFrameRate;
	int					m_nImageSize;

	XImage*				m_pXImage;
	PBYTE				m_pTempBuf;
	int					m_nBufLen;
	VIDEC_Resize*		m_pResize;
	unsigned short		m_usDstWidth;
	unsigned short		m_usDstHeight;

	unsigned long		m_nFrameCount;
	unsigned long		m_nLastTS;

	int					m_nDevID;
};

#endif
