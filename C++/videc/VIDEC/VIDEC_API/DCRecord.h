//DCRecord.h
#ifndef __DCRECORD_H__
#define __DCRECORD_H__

#include "VIDEC.h"
#include "VIDEC_DCRecord.h"
#include "VIDEC_DC.h"
#include "AVIWriter.h"

class DCRecord
	: public VIDEC_DCRecord
	, public VIDEC_StreamSink
	, public VIDEC_CapChanCallback
{
public:
	DCRecord(void);
	virtual~DCRecord(void);
public:
	virtual int Open(HDC hDC,int x,int y,int cx,int cy,const char*cszPathName,int nFrameRate=5,int nQuality=3,int nBitrateControlType=VIDEC_NETWORK_LAN);
	virtual void Close(void);
	virtual void CaptureFrame(void);
	virtual bool IsDone(void);
protected:
	void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);


	void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag){nSkipFlag=0;};
protected:
	VIDEC_AVIWriter*		m_pAVIWriter;
	VIDEC_CapChan*			m_pCapChan;
	VIDEC_DC*				m_pDC;
	unsigned long			m_ulFirstVideoTimestamp;
	unsigned long			m_ulFrameInterval;
};

#endif