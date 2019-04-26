#ifndef __DVCDEV_H__
#define __DVCDEV_H__

#include "DVCAPI.h"
#include <NETEC/NETEC_DVC.h>
#include "XAutoLock.h"


class DVCDev  
	: public NETEC_DVCCallback
{
public:
	DVCDev(void);
	virtual ~DVCDev(void);
public:
	int Open(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData);
	void Close(void);
	int SetParams(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
	int GetStatus(void);
	int RequestKeyFrame(void);
	int SendData(const char*pData,int nLen);
protected:
	virtual void OnNETEC_DVCCallbackVideoFrameData(unsigned char*pFrameData,int nFrameLen);
protected:
	DVC_VIDEO_CB			m_video_cb;
	const void*				m_pUserData;
	XCritSec				m_csDVC;
	NETEC_DVC*				m_pDVC;

	int						m_nFrameRate;
	int						m_nBitrate;
	int						m_nBitrateControlType;
	int						m_nResolution;
};



#endif