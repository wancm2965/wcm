//VideoDevice.h
#ifndef __VIDEODEVICE_H__
#define __VIDEODEVICE_H__

#include <VIDEC/VIDEC_Device.h>
#include <VIDEC/VIDEC.h>

class VideoDevice
	: public VIDEC_Device
	, public VIDEC_ImageSink
{
public:
	VideoDevice(VIDEC_DeviceCallback&rCallback);
	virtual~VideoDevice(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate,int nFrameType);
	virtual void Close(void);
	virtual void ShowInputWnd(HWND hParentWnd);
	virtual void ShowPropertyWnd(HWND hParentWnd);
protected:
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnReceivedImage(VIDEC_Image*pImage);
	virtual void OnReceivedPreSend(int& nSend);
protected:
	VIDEC_DeviceCallback&		m_rCallback;
	int							m_nDevID;
	int							m_nWidth;
	int							m_nHeight;
	int							m_nFrameRate;
	int							m_nFrameType;
	VIDEC_CapDev*				m_pCapDev;
};

#endif
