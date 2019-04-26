/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "DirectXWebcamDriver.h"

#include "DirectXWebcamUtils.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <initguid.h>


using namespace std;

/* I420 isn't defined in any of the headers but everyone seems to use it.... */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_I420 =
	{0x30323449,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};

/* IYUV */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_IYUV =
	{0x56555949,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};

/* Y444 */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y444 =
	{0x34343459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

/* Y800 */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y800 =
	{0x30303859, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

/* Y422 */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y422 =
	{0x32323459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

/* NV12 */
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_NV12 =
	{0x3231564E, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};


typedef struct {
	GUID pix_ms;
	pixosi pix_osi;
} pixms_pixosi;

pixms_pixosi _ms_table[] = {
	{ MEDIASUBTYPE_YUYV, PIX_OSI_YUYV },
	{ MEDIASUBTYPE_Y411, PIX_OSI_YUV411 },
	{ MEDIASUBTYPE_Y41P, PIX_OSI_YUV41P },
	{ MEDIASUBTYPE_YUY2, PIX_OSI_YUY2 },
	{ MEDIASUBTYPE_YVYU, PIX_OSI_YVYU },
	{ MEDIASUBTYPE_UYVY, PIX_OSI_UYVY },
	{ MEDIASUBTYPE_YV12, PIX_OSI_YV12 },
	{ MEDIASUBTYPE_RGB1, PIX_OSI_RGB1 },
	{ MEDIASUBTYPE_RGB4, PIX_OSI_RGB4 },
	{ MEDIASUBTYPE_RGB8, PIX_OSI_RGB8 },
	{ MEDIASUBTYPE_RGB565, PIX_OSI_RGB565 },
	{ MEDIASUBTYPE_RGB555, PIX_OSI_RGB555 },
	{ MEDIASUBTYPE_RGB24, PIX_OSI_RGB24 },
	{ MEDIASUBTYPE_RGB32, PIX_OSI_RGB32 },
	{ MEDIASUBTYPE_ARGB32, PIX_OSI_ARGB32 },
	{ OUR_MEDIASUBTYPE_I420, PIX_OSI_YUV420P }, //FIXME: OUR_MEDIASUBTYPE_I420 is actually OUR_MEDIASUBTYPE_IYUV
	{ OUR_MEDIASUBTYPE_Y422, PIX_OSI_YUV422 },
	{ OUR_MEDIASUBTYPE_IYUV, PIX_OSI_YUV420P },
	{ OUR_MEDIASUBTYPE_NV12, PIX_OSI_NV12 }
};

pixosi pix_directx_to_pix_osi(GUID pix) {
	register unsigned i;
	pixosi palette = PIX_OSI_UNSUPPORTED;

	for (i = 0; i < sizeof(_ms_table) / sizeof(pixms_pixosi); i++) {
		if (memcmp(&_ms_table[i].pix_ms, &pix, sizeof(GUID)) == 0) {
			palette = _ms_table[i].pix_osi;
			break;
		}
	}
	return palette;
}

GUID pix_directx_from_pix_osi(pixosi pix) {
	register unsigned i;
	GUID palette = MEDIASUBTYPE_NULL;

	for (i = 0; i < sizeof(_ms_table) / sizeof(pixms_pixosi); i++) {
		if (_ms_table[i].pix_osi == pix) {
			return _ms_table[i].pix_ms;
		}
	}
	return palette;
}


#define SAFE_RELEASE_POINTER(x) { if (x) x->Release(); x = NULL; }
#define SAFE_RELEASE(x) { if (x) x.Release(); x = NULL; }


//
//VIDEC_Camera*VIDEC_Camera::Create(VIDEC_CameraCallback&rCallback)
//{
//	DirectXWebcamDriver*pDirectXWebcamDriver=new DirectXWebcamDriver(rCallback);
//	return pDirectXWebcamDriver;
//}

DirectXWebcamDriver::DirectXWebcamDriver(VIDEC_CameraCallback&rCallback)
:m_rCallback(rCallback)
{
	_pGrabberF = NULL;
	_pGrabber = NULL;
	_pNull = NULL;
	_iam = NULL;
	_pGraph = NULL;
	_pControl = NULL;
	_pCap = NULL;
	_pBuild = NULL;

	m_bCrossBar=false;
	m_nDevice=0;
}

DirectXWebcamDriver::~DirectXWebcamDriver() 
{

}

int DirectXWebcamDriver::Open(int nDevice,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,bool bAnyFormat)
{
	m_nDevice=nDevice;
	_cachedFPS = nFrameRate;
	if (_cachedFPS<1)
		_cachedFPS=1;
	else if (_cachedFPS>30)
		_cachedFPS=30;

	//nWidth=704;
	//nHeight=480;

	_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (!_pGraph) {
		return -1;
	}

	_pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	if (!_pBuild) {
		return -1;
	}

	_pBuild->SetFiltergraph(_pGraph);
	//Create the Sample Grabber
	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(_pGrabberF));
	if (hr != S_OK) {
		return -1;
	}

	//Add the filter to the graph
	hr = (_pGraph)->AddFilter(_pGrabberF, L"Sample Grabber");
	if (hr != S_OK) {
		return -1;
	}

	//Query the Sample Grabber for the ISampleGrabber interface.
	_pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&_pGrabber);
	hr = _pGrabber->SetBufferSamples(FALSE);
	hr = _pGrabber->SetOneShot(FALSE);

	//Set the Sample Grabber callback
	//0: SampleCB (the buffer is the original buffer, not a copy)
	//1: BufferCB (the buffer is a copy of the original buffer)
	if (_pGrabber->SetCallback(this, 0) != S_OK) {
		return -1;
	}

	GetCapDevice(nDevice,&_pCap);
	if (!_pCap)
		return -1;

	//initialize IAMStreamConfig
	_iam = GetIAMStreamConfig(_pCap);
	if (!_iam) {
		return -1;
	}

	//add the capture filter to the graph
	hr = (_pGraph)->AddFilter(_pCap, L"");
	if (hr != S_OK) {
		return S_FALSE;
	}

	//Add a null renderer filter
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&_pNull);
	hr = (_pGraph)->AddFilter(_pNull, L"NullRender");


	_pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, NULL, _pCap, NULL, _pGrabberF);

	//try to assign some palette until the webcam supports it
	if (setCaps(PIX_OSI_YUV420P, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_I420, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_RGB32, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_RGB24, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_YUV422, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_RGB565, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_RGB555, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_YUY2, _cachedFPS, nWidth,nHeight) != S_OK) {
	if (setCaps(PIX_OSI_UYVY, _cachedFPS, nWidth,nHeight) != S_OK) {
	}}}}}}}}}

	readCaps();

	if (getPalette() == PIX_OSI_UNSUPPORTED) {
		return -1;
	}

	_isOpen = true;

	hr = _pGraph->QueryInterface(IID_IMediaControl, (void **)&_pControl);
	if (hr != S_OK) {
		return -1;
	}

	hr = _pControl->Run();
	if (hr != S_OK) {
		SAFE_RELEASE(_pControl);
		return -1;
	}

	SAFE_RELEASE(_pControl);
	return 0;
}

void DirectXWebcamDriver::Close(void)
{
	stopCapture();

	_isOpen = false;
	_cachedWidth = 0;
	_cachedHeight = 0;
	_cachedFPS = 15;
	_cachedPalette = PIX_OSI_UNSUPPORTED;
	SAFE_RELEASE_POINTER(_pGrabberF);
	SAFE_RELEASE_POINTER(_pGrabber);
	SAFE_RELEASE_POINTER(_pNull);
	SAFE_RELEASE_POINTER(_iam);
	SAFE_RELEASE(_pControl);
	SAFE_RELEASE(_pGraph);
	SAFE_RELEASE(_pCap);
	SAFE_RELEASE(_pBuild);
}


bool DirectXWebcamDriver::isOpen() const {
	return _isOpen;
}

void DirectXWebcamDriver::startCapture() {
	HRESULT hr;

}


void DirectXWebcamDriver::stopCapture() {
	HRESULT hr;

	if (!_pGraph) {
		return;
	}

	hr = _pGraph->QueryInterface(IID_IMediaControl, (void **)&_pControl);
	if (hr != S_OK) {
	}

	hr = _pControl->StopWhenReady();
	if (hr != S_OK) {
	}

	SAFE_RELEASE(_pControl);
}

HRESULT DirectXWebcamDriver::setPalette(pixosi palette) {
	setCaps(palette, getFPS(), getWidth(), getHeight());
	if (_cachedPalette != palette) {
		return S_FALSE;
	} else {
		return S_OK;
	}
}

pixosi DirectXWebcamDriver::getPalette() const {
	return _cachedPalette;
}

unsigned DirectXWebcamDriver::getFPS() const {
	return _cachedFPS;
}

HRESULT DirectXWebcamDriver::setResolution(unsigned width, unsigned height) {
	setCaps(getPalette(), getFPS(), width, height);
	if ((_cachedWidth != width) || (_cachedHeight != height)) {
		return S_FALSE;
	} else {
		return S_OK;
	}
}


unsigned DirectXWebcamDriver::getWidth() const {
	return _cachedWidth;
}

unsigned DirectXWebcamDriver::getHeight() const {
	return _cachedHeight;
}

void DirectXWebcamDriver::setBrightness(int brightness) {
}

int DirectXWebcamDriver::getBrightness() const {
	return 0;
}

void DirectXWebcamDriver::setContrast(int contrast) {
}

int DirectXWebcamDriver::getContrast() const {
	return 0;
}


HRESULT DirectXWebcamDriver::setCaps(pixosi palette, unsigned fps, unsigned resolutionWidth, unsigned resolutionHeight) {
	_cachedFPS = fps;

	if (!isOpen()) {
		return S_FALSE;
	}

	if (!_pCap) {
	}
	if (!_iam) {
	}

	int iCount, iSize;
	HRESULT hr = _iam->GetNumberOfCapabilities(&iCount, &iSize);

	VIDEO_STREAM_CONFIG_CAPS scc;
	if (sizeof(scc) != iSize) {
		return S_FALSE;
	}

	int i =0;
	for ( i = 0; i < iCount; i++) {
		AM_MEDIA_TYPE * pmt = NULL;
		hr = _iam->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE *>(&scc));
		if (hr == S_OK) {
			pixosi wc_palette = pix_directx_to_pix_osi(pmt->subtype);
			if (wc_palette != palette) {
				hr = E_FAIL;
				continue;
			}
			VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *) pmt->pbFormat;
			if (pvi->bmiHeader.biWidth != resolutionWidth ||
				pvi->bmiHeader.biHeight != resolutionHeight)
			{
				hr = E_FAIL;
				continue;
			}

			REFERENCE_TIME avgTimePerFrame=pvi->AvgTimePerFrame;
			if (avgTimePerFrame!=400000 && avgTimePerFrame!=333333 && avgTimePerFrame!=333667)
			{
				pvi->AvgTimePerFrame = 333333;
				hr = _iam->SetFormat(pmt);
				if (hr != S_OK) 
				{
					pvi->AvgTimePerFrame = 333667;
					hr = _iam->SetFormat(pmt);
					if (hr != S_OK) 
					{
						pvi->AvgTimePerFrame = 400000;
						hr = _iam->SetFormat(pmt);
						if (hr != S_OK) 
						{
							pvi->AvgTimePerFrame = avgTimePerFrame;
							hr = _iam->SetFormat(pmt);
						}
					}
				}
			}
				
			hr = S_OK;
			break;
		}
	}

	//for ( i = 0; i < iCount; i++) {
	//	AM_MEDIA_TYPE * pmt = NULL;
	//	hr = _iam->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE *>(&scc));
	//	if (hr == S_OK) {
	//		pixosi wc_palette = pix_directx_to_pix_osi(pmt->subtype);
	//		if (wc_palette != palette) {
	//			hr = E_FAIL;
	//			continue;
	//		}
	//		VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *) pmt->pbFormat;
	//		pvi->bmiHeader.biWidth = resolutionWidth;
	//		pvi->bmiHeader.biHeight = resolutionHeight;
	//		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	//		//pvi->AvgTimePerFrame = (LONGLONG) (10000000. / (double)fps);
	//		hr = _iam->SetFormat(pmt);
	//		if (hr != S_OK) {
	//			hr = E_FAIL;
	//			continue;
	//		} else {
	//			break;
	//		}
	//	}
	//}

	readCaps();

	if (hr == S_OK) {
		return S_OK;
	}

	return S_FALSE;
}

void DirectXWebcamDriver::readCaps() {
	VIDEOINFOHEADER * pvi;
	pixosi palette;

	AM_MEDIA_TYPE * pmt = NULL;
	HRESULT hr = _iam->GetFormat(&pmt);

	if (pmt->formattype == FORMAT_VideoInfo) {
		pvi = (VIDEOINFOHEADER *) pmt->pbFormat;
		palette = pix_directx_to_pix_osi(pmt->subtype);
	}

	_cachedPalette = palette;
	_cachedWidth = pvi->bmiHeader.biWidth;
	_cachedHeight = pvi->bmiHeader.biHeight;
}

STDMETHODIMP DirectXWebcamDriver::QueryInterface(REFIID riid, void ** ppv) {

	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
		*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP DirectXWebcamDriver::BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize) {
	//This method is not used but must be implemented
	return 0;
}

STDMETHODIMP DirectXWebcamDriver::SampleCB(double SampleTime, IMediaSample * pSample) {
	if (!pSample) {
		return E_POINTER;
	} else {
		BYTE * pBuffer;
		pSample->GetPointer(&pBuffer);
		long lBufferSize = pSample->GetSize();

		if (!pBuffer) {
			return E_POINTER;
		}

		//_capturedImage.data = (uint8_t *) pBuffer;
		//_capturedImage.width = _cachedWidth;
		//_capturedImage.height = _cachedHeight;
		//_capturedImage.palette = _cachedPalette;

		char szDebug[1024];
		sprintf(szDebug,"SampleTime=%f\n",SampleTime);
		OutputDebugString(szDebug);
	}
	return 0;
}

/***********************************************************************/
/*功能：显示视频调整窗口
/*参数：
/*返回：
/*备注：如果没有打开视频设备，则不显示任何窗口
/***********************************************************************/
void DirectXWebcamDriver::ShowAdjustVideoWnd(HWND hWnd)
{
	if (_pCap)
		ShowPropertyPage(hWnd,(IUnknown*)_pCap,NULL);//NAME("视频调整")
}


bool DirectXWebcamDriver::HasVideoInputWnd(void)
{
	return m_bCrossBar;
}

void DirectXWebcamDriver::ShowVideoInputWnd(HWND hWnd)
{
	if (_pBuild==NULL || _pCap==NULL || !HasVideoInputWnd())
		return ;

	HRESULT hr; 

    CComPtr< IAMCrossbar > pCrossbar;
    hr = _pBuild->FindInterface(&PIN_CATEGORY_CAPTURE,
												&MEDIATYPE_Video, _pCap,
												IID_IAMCrossbar, (void **)&pCrossbar);
    if (FAILED(hr))
    {
        return ;
    }    
    
	ShowPropertyPage(hWnd,(IUnknown*)pCrossbar,NULL);//NAME("视频调整")

	LONG lRouted=0;
	hr=pCrossbar->get_IsRoutedTo(0, &lRouted);
	if (!FAILED(hr))
	{
		int nInput=-1;
		DirectXWebcamDriver::GetInput(m_nDevice,nInput);
		if (lRouted!=nInput)
		{
			nInput=lRouted;	
			DirectXWebcamDriver::SetInput(m_nDevice,nInput);
		}
	}
}


bool DirectXWebcamDriver::TVTunerPutChannel(long lChannel)
{
	if (_pBuild==NULL ||
		_pCap==NULL )
	{
		return false;
	}

	HRESULT hr;
    CComPtr< IAMTVTuner > pTVTuner;
	 
	hr = _pBuild->FindInterface(&PIN_CATEGORY_CAPTURE,
										&MEDIATYPE_Interleaved, _pCap,
										IID_IAMTVTuner, (void **)&pTVTuner);
	if (FAILED(hr))
	{
		hr = _pBuild->FindInterface(&PIN_CATEGORY_CAPTURE,
											&MEDIATYPE_Video, _pCap,
											IID_IAMTVTuner, (void **)&pTVTuner);
		if (FAILED(hr))
		{
			return false;
		}
	}

	hr=pTVTuner->put_Channel(lChannel,AMTUNER_SUBCHAN_DEFAULT,AMTUNER_SUBCHAN_DEFAULT);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////
// showPropertyPage: Display the property page of a COM object
// Auguments:
//   pIU:    A interface of the COM object
//   name:   the name of the dialog box of the property page
// return value: error code
///////////////////////////////////////////////////////////////////////
HRESULT DirectXWebcamDriver::ShowPropertyPage(HWND hwndParent,IUnknown* pIU, const WCHAR* name)
{
    HRESULT hr;
    if (pIU) {
        ISpecifyPropertyPages* pispp = 0;
        hr=pIU->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pispp);
		if (FAILED(hr))
			return hr;

        CAUUID caGUID;
        if (SUCCEEDED(pispp->GetPages(&caGUID))) {
            OleCreatePropertyFrame(hwndParent, 0, 0,
                name,     // Caption for the dialog box
                1,        // Number of filters
                (IUnknown**)&pIU,
                caGUID.cElems,
                caGUID.pElems,
                0, 0, 0);
            // Release the memory
            CoTaskMemFree(caGUID.pElems);
        }
        SAFE_RELEASE_POINTER(pispp);
    }
    return hr;
}

void DirectXWebcamDriver::GetCapDevice(INT nCapDevice,IBaseFilter**ppCap)
{
    HRESULT hr;

    *ppCap = NULL;

    // create an enumerator
    //
    CComPtr< ICreateDevEnum > pCreateDevEnum;
    pCreateDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum );
    if( !pCreateDevEnum )
        return;

    // enumerate video capture devices
    //
    CComPtr< IEnumMoniker > pEm;
    pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEm, 0 );
    if( !pEm )
        return;

    pEm->Reset( );
 
	INT nIndex=0;
    // go through and find first video capture device
    //
    while( 1 )
    {
        ULONG ulFetched = 0;
        CComPtr< IMoniker > pM;
        hr = pEm->Next( 1, &pM, &ulFetched );
        if( hr != S_OK )
            break;

		if (nIndex++==nCapDevice)
		{
			// ask for the actual filter
			//
			hr = pM->BindToObject( 0, 0, IID_IBaseFilter, (void**) ppCap );
			if( *ppCap )
				break;

			break;
		}
    }

    return;
}

HRESULT DirectXWebcamDriver::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    CComPtr< IEnumPins > pEnum;
    *ppPin = NULL;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;
                // Found requested pin, so clear error
                hr = S_OK;
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}

IPin * DirectXWebcamDriver::GetInPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
    return pComPin;
}


IPin * DirectXWebcamDriver::GetOutPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
    return pComPin;
}

void DirectXWebcamDriver::SetInput(int nDevice,int nInput)
{
	char strName[1024];
	char strValue[1024];
#ifdef _UNICODE
	wsprintf(strName,_T("dev_%d_input"),nDevice);
	wsprintf(strValue,_T("%d"),nInput);
#else
	sprintf(strName,_T("dev_%d_input"),nDevice);
	sprintf(strValue,_T("%d"),nInput);
#endif
	WritePrivateProfileString(_T("VIDEC"), strName , strValue, _T("VIDEC.ini")); 	
}

void DirectXWebcamDriver::GetInput(int nDevice,int&nInput)
{
	char strName[1024];
#ifdef _UNICODE
	wsprintf(strName,_T("dev_%d_input"),nDevice);
#else
	sprintf(strName,_T("dev_%d_input"),nDevice);
#endif
	nInput=GetPrivateProfileInt(_T("VIDEC"), strName , -1, _T("VIDEC.ini"));
}
