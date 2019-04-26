#ifndef __XCAMERACAPEX_H__
#define __XCAMERACAPEX_H__

#include "XCameraRenderEx.h"
#include <VIDEC/VIDEC_Camera.h>

class XCameraCapEx 
	: public VIDEC_Camera
{
public:
	XCameraCapEx(VIDEC_CameraCallback&rCallback);
	~XCameraCapEx(void);
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

	HRESULT SetVideoProperties(IBaseFilter*pCap,int& nWidth,int& nHeight,int& nTotalFrameRate);
	HRESULT SetCap(IBaseFilter*pCap,GUID subType,int nWidth,int nHeight,int nTotalFrameRate);

	void GetCapDevice(INT nCapDevice,IBaseFilter**ppCap);
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	IPin * GetInPin ( IBaseFilter *pFilter, int Num );
	IPin * GetOutPin( IBaseFilter *pFilter, int Num );
protected:
	ICaptureGraphBuilder2*		m_picgb2;
	IGraphBuilder*				m_pigb;
	IMediaControl*				m_pimc;
	IBaseFilter*				m_pibfCap;
	XCameraRenderEx*			m_pXCameraRender;
	bool						m_bCrossBar;

	int							m_nWidth;
	int							m_nHeight;
	int							m_nDevice;

	VIDEC_CameraCallback&		m_rCallback;

	bool						m_bCapturing;
};

#endif 