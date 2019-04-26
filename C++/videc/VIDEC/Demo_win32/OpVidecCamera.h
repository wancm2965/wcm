#pragma once
#include "VIDEC_Camera.h"

#include "XThreadBase.h"
#include "XCritSec.h"
#include "XListPtr.h"
#include "XAutoLock.h"

class COpVidecCameraCallback
{
public:
	virtual void OnOpVidecCameraaCallbackImageData(VIDEC_Image*pImage)=0;                                                                         
};

class COpVidecCamera
	: public VIDEC_CameraCallback
	, public  XThreadBase
{
public:
	virtual ~COpVidecCamera(void);

	static COpVidecCamera* Create(COpVidecCameraCallback* pCallback);
	virtual int Open(int nDevIndex,int& nWidth,int& nHeight,int nFrameRate,int nTryMaxSize);
	virtual void Close(void);

	virtual void ShowAdjustVideoWnd(HWND hWnd);
	virtual void ShowVideoInputWnd(HWND hWnd);
	virtual bool HasVideoInputWnd(void);
	virtual bool TVTunerPutChannel(long lChannel);

protected:
	COpVidecCamera(void);
	virtual void ThreadProcMain(void);
	virtual void OnVIDEC_CameraCallbackImageData(VIDEC_Image*pImage);

	typedef std::map<COpVidecCameraCallback*,int> MAP_OPCAMERA_CB;	
	static MAP_OPCAMERA_CB m_mapCallback;

	COpVidecCameraCallback* m_pCB;
	VIDEC_Camera*			m_pVidecCamera;
	int						m_nDevIndex;

	XListPtr				m_listImage;
	XCritSec				m_csListImage;
	bool					m_bWaitStop;
};
