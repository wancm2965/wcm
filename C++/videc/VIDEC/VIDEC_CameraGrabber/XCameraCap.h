#ifndef __XCAMERACAP_H__
#define __XCAMERACAP_H__

#include "VIDEC_Camera.h"
#include "XCameraRender.h"

class XCameraCap 
	: public VIDEC_Camera
	, public ISampleGrabberCB
{
public:
	XCameraCap(VIDEC_CameraCallback&rCallback);
	~XCameraCap(void);
public:
	static void SetInput(int nDevice,int nInput);
	static void GetInput(int nDevice,int&nInput);
public:
	virtual int Open(int nDevice,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,bool bAnyFormat);
	virtual void Close(void);

	virtual void ShowAdjustVideoWnd(HWND hWnd);
	virtual void ShowVideoInputWnd(HWND hWnd);
	virtual bool HasVideoInputWnd(void);


	virtual int GetWidth(void);
	virtual int GetHeight(void);

	virtual void OnCaptureVideoFrame(VIDEC_Image*pImage);

	bool TVTunerPutChannel(long lChannel);
public:
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }

	// fake out any COM QI'ing
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	// callback to access the buffer - the original buffer is passed
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample * pSample);

	// callback to access the buffer - a copy is passed
	STDMETHODIMP BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize);
protected:
	HRESULT ShowPropertyPage(HWND hwndParent,IUnknown* pIU, const WCHAR* name);
	HRESULT SetVideoProperties(IBaseFilter*pCap,int& nWidth,int& nHeight,int nFrameRate);
	HRESULT SetCap(IBaseFilter*pCap,GUID subType,int nWidth,int nHeight);

	void GetCapDevice(INT nCapDevice,IBaseFilter**ppCap);
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	IPin * GetInPin ( IBaseFilter *pFilter, int Num );
	IPin * GetOutPin( IBaseFilter *pFilter, int Num );
protected:
	ICaptureGraphBuilder2*		m_picgb2;
	IGraphBuilder*				m_pigb;
	IMediaControl*				m_pimc;
	IBaseFilter*				m_pibfCap;
	IBaseFilter*				m_pibfGrabber;
	ISampleGrabber*				m_piSampleGrabber;
	IBaseFilter*				m_pibfNullRender;

	bool						m_bCrossBar;

	int							m_nWidth;
	int							m_nHeight;
	int							m_nDevice;
	bool						m_bAutoFrameRate;

	VIDEC_CameraCallback&		m_rCallback;

	bool						m_bCapturing;

	XCameraRender*				m_pXCameraRender;
	GUID						m_subType;

	unsigned short				m_nTotalFrameRate;
};

#endif 