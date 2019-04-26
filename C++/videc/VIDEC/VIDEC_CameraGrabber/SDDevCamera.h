//SDDevCamera.h
#ifndef __SDDEVCAMERA_H__
#define __SDDEVCAMERA_H__


#include <winsock2.h>
#include <windows.h>

#include "SDDev.h"
#include "VIDEC_Camera.h"
#include "libXPreProc.h"

class SDDevCamera
	: public SDDev
	, public VIDEC_CameraCallback
	, public IXPreProcessNotify
{
public:
	SDDevCamera(unsigned int nDevIndex);
	virtual~SDDevCamera(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate);
	void Close(void);
	int SetDevCap(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate);
	
	void OnVIDEC_CameraCallbackImageData(unsigned char*pData,int nLen,int nWidth,int nHeight);

	void OnIXPreProcessNotifyOutputData(unsigned char*pData,int nLen,int nWidth,int nHeight);
protected:
	VIDEC_Camera*	m_pCamera;
	IXPreProcess*	m_pIXPreProcess;
	unsigned long	m_ulLastImageTimestamp;
};

#endif