#ifndef __XCAMERACAP_H__
#define __XCAMERACAP_H__

#include "XCameraRender.h"
#include "VIDEC_Camera.h"

class XCameraCap 
	: public VIDEC_Camera
{
public:
	XCameraCap(VIDEC_CameraCallback&rCallback);
	~XCameraCap(void);
public:
	static void SetInput(int nDevice,int nInput);
	static void GetInput(int nDevice,int&nInput);
public:
	virtual int Open(int nDevice,int& nWidth,int& nHeight,int nFrameRate,int nTryMaxSize);
	virtual void Close(void);

	virtual void ShowAdjustVideoWnd(HWND hWnd);
	virtual void ShowVideoInputWnd(HWND hWnd);
	virtual bool HasVideoInputWnd(void);


	virtual int GetWidth(void);
	virtual int GetHeight(void);

	virtual void OnCaptureVideoFrame(VIDEC_Image*pImage);

	bool TVTunerPutChannel(long lChannel);
protected:
	HRESULT ShowPropertyPage(HWND hwndParent,IUnknown* pIU, const WCHAR* name);
	HRESULT SetVideoProperties(IBaseFilter*pCap,int& nWidth,int& nHeight,int&nTotalFrameRate);
	void GetCapDevice(INT nCapDevice,IBaseFilter**ppCap);
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	IPin * GetInPin ( IBaseFilter *pFilter, int Num );
	IPin * GetOutPin( IBaseFilter *pFilter, int Num );
protected:
	ICaptureGraphBuilder2*		m_picgb2;
	IGraphBuilder*				m_pigb;
	IMediaControl*				m_pimc;
	IBaseFilter*				m_pibfCap;
	XCameraRender*				m_pXCameraRender;
	bool						m_bCrossBar;

	int							m_nWidth;
	int							m_nHeight;
	int							m_nDevice;

	VIDEC_CameraCallback&		m_rCallback;

	bool						m_bCapturing;
};

#endif 