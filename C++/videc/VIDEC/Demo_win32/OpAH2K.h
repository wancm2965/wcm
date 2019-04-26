#pragma once

#include "XThreadBase.h"
#include "XCritSec.h"
#include "XListPtr.h"
#include "XAutoLock.h"


class COpAH2KCallback
{
public:
	virtual void OnOpAH2KCallbackImage(VIDEC_Image*pImage) = 0;
	virtual void OnOpAH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate) = 0;
};

class COpAH2K
	: public VIDEC_AH2KCallback
	, public  XThreadBase
{
public:
	virtual ~COpAH2K(void);
	static COpAH2K* Create(COpAH2KCallback* pCb);

	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate);
	virtual void Close(void);
	virtual int StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax);
	virtual int RequestKeyFrame(void);
	virtual int StopVideoStream(void);

	virtual void ThreadProcMain(void);

protected:
	COpAH2K(void);

	virtual void OnVIDEC_AH2KCallbackPreSendImage(int&nSend);
	virtual void OnVIDEC_AH2KCallbackImage(VIDEC_Image*pImage);
	virtual void OnVIDEC_AH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate);

	typedef std::map<COpAH2KCallback*,int> MAP_OPAH2K_CB;	
	static MAP_OPAH2K_CB m_mapCallback;

	COpAH2KCallback* m_pCB;
	VIDEC_AH2K* m_pAH2k;
	int m_nDevIndex;

	XListPtr				m_listImage;
	XCritSec				m_csListImage;
	bool					m_bWaitStop;


};
