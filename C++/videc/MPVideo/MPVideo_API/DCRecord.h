//DCRecord.h
#ifndef __DCRECORD_H__
#define __DCRECORD_H__

#include "MPVideo.h"
#include "MPVIDEO_DCRecord.h"
#include "MPVIDEO_DC.h"
#include "AVIWriter.h"

class DCRecord
	: public MPVIDEO_DCRecord
	, public MPVIDEO_StreamSink
	, public MPVIDEO_CapChanCallback
{
public:
	DCRecord(void);
	virtual~DCRecord(void);
public:
	virtual int Open(HDC hDC,int x,int y,int cx,int cy,const char*cszPathName,int nFrameRate=5,int nQuality=3,int nBitrateControlType=MPVIDEO_NETWORK_LAN);
	virtual void Close(void);
	virtual void CaptureFrame(void);
	virtual bool IsDone(void);
protected:
	void OnAttachedToStreamSource(MPVIDEO_StreamSource*pStreamSource);
	void OnDetachedFromStreamSource(MPVIDEO_StreamSource*pStreamSource);
	void OnReceivedStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);


	void OnMPVIDEO_CapChanCallbackPreEncode(MPVIDEO_CapChan*pCapChan,int&nSkipFlag){nSkipFlag=0;};
protected:
	MPVIDEO_AVIWriter*		m_pAVIWriter;
	MPVIDEO_CapChan*			m_pCapChan;
	MPVIDEO_DC*				m_pDC;
	unsigned long			m_ulFirstVideoTimestamp;
	unsigned long			m_ulFrameInterval;
};

#endif