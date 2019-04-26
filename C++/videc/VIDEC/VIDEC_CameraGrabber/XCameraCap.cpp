// XCameraCap.cpp: implementation of the XCameraCap class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <initguid.h>
#include <qedit.h>
#include <stdio.h>
#include <stdlib.h>
#include "XCameraCap.h"
#include "XUtil.h"

#define HELPER_RELEASE(x) {if(x) x -> Release(); x = NULL;}

// 30323449-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_I420 
DEFINE_GUID(MEDIASUBTYPE_I420_VC, 
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 

void XCameraCap::SetInput(int nDevice,int nInput)
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

void XCameraCap::GetInput(int nDevice,int&nInput)
{
	char strName[1024];
#ifdef _UNICODE
	wsprintf(strName,_T("dev_%d_input"),nDevice);
#else
	sprintf(strName,_T("dev_%d_input"),nDevice);
#endif
	nInput=GetPrivateProfileInt(_T("VIDEC"), strName , -1, _T("VIDEC.ini"));
}



VIDEC_Camera*VIDEC_Camera::Create(VIDEC_CameraCallback&rCallback)
{
	XCameraCap*pXCameraCap=new XCameraCap(rCallback);
	return pXCameraCap;
}

int VIDEC_Camera::GetDeviceCount(void)
{
	int nCount=0;
    HRESULT hr;

    // create an enumerator
    //
    CComPtr< ICreateDevEnum > pCreateDevEnum;
    pCreateDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum );
    if( !pCreateDevEnum )
        return nCount;

    // enumerate video capture devices
    //
    CComPtr< IEnumMoniker > pEm;
    pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEm, 0 );
    if( !pEm )
        return nCount;

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

		nCount++;
    }

	return nCount;
}

int VIDEC_Camera::GetDeviceName(int nDevice,char*szDeviceName)
{
	int nCount=0;

    HRESULT hr;

	int nRet=-1;

    // create an enumerator
    //
    CComPtr< ICreateDevEnum > pCreateDevEnum;
    pCreateDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum );
    if( !pCreateDevEnum )
        return nRet;

    // enumerate video capture devices
    //
    CComPtr< IEnumMoniker > pEm;
    pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEm, 0 );
    if( !pEm )
        return nRet;

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

		if (nCount++==nDevice)
		{
			CComPtr< IPropertyBag > pBag;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if(SUCCEEDED(hr)) 
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if (hr == NOERROR) {
//#ifdef _UNICODE
//					wcscpy(szDeviceName,var.bstrVal);
//#else
					USES_CONVERSION;
					strcpy(szDeviceName,OLE2T(var.bstrVal));
//#endif
					SysFreeString(var.bstrVal);
					nRet=0;
				}
			}
			break;
		}
    }

	return nRet;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XCameraCap::XCameraCap(VIDEC_CameraCallback&rCallback)
:m_rCallback(rCallback)
,m_bAutoFrameRate(false)
{
	m_picgb2=NULL;
	m_pigb=NULL;
	m_pimc=NULL;
	m_pibfCap=NULL;
	m_pibfGrabber=NULL;
	m_piSampleGrabber=NULL;
	m_pibfNullRender=NULL;
	m_bCrossBar=false;

	m_nWidth=0;
	m_nHeight=0;
	m_nDevice=0;

	m_bCapturing=false;

	m_pXCameraRender=NULL;
	m_nTotalFrameRate=25;
}

XCameraCap::~XCameraCap()
{
}

int XCameraCap::Open(int nDevice,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,bool bAnyFormat)
{
	m_nDevice=nDevice;
	m_bAutoFrameRate=bAutoFrameRate;
	m_nWidth=nWidth;
	m_nHeight=nHeight;

    HRESULT hr;

	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
							 IID_IGraphBuilder, (void **)&m_pigb);
	if (FAILED(hr))
		return -1;

    hr=CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
        IID_ICaptureGraphBuilder2, (void **) &m_picgb2);

	if (FAILED(hr))
		return -1;

    hr=m_picgb2->SetFiltergraph(m_pigb);


	//Create the Sample Grabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(m_pibfGrabber));
	if (hr != S_OK) {
		return -1;
	}

	//Add the filter to the graph
	hr = (m_pigb)->AddFilter(m_pibfGrabber, L"Sample Grabber");
	if (hr != S_OK) {
		return -1;
	}

	//Query the Sample Grabber for the ISampleGrabber interface.
	m_pibfGrabber->QueryInterface(IID_ISampleGrabber, (void**)&m_piSampleGrabber);
	hr = m_piSampleGrabber->SetBufferSamples(FALSE);
	hr = m_piSampleGrabber->SetOneShot(FALSE);

	//Set the Sample Grabber callback
	//0: SampleCB (the buffer is the original buffer, not a copy)
	//1: BufferCB (the buffer is a copy of the original buffer)
	if (m_piSampleGrabber->SetCallback(this, 0) != S_OK) {
		return -1;
	}


	GetCapDevice(nDevice,&m_pibfCap);
	if (!m_pibfCap)
		return -1;


    hr = m_pigb->AddFilter( m_pibfCap, L"Cap" );
    if( FAILED( hr ) )
	{
        return -1;
	}

	//Add a null renderer filter
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pibfNullRender);
	hr = (m_pigb)->AddFilter(m_pibfNullRender, L"NullRender");


	hr=m_picgb2->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pibfCap, NULL, m_pibfGrabber);
    if( FAILED( hr ) )
	{
        return -1;
	}

	//hr=SetVideoProperties(m_pibfCap,nWidth,nHeight,nFrameRate);
	hr=SetVideoProperties(m_pibfCap,nWidth,nHeight,30);
	if (FAILED(hr))
	{
        return -1;
	}

	if (!bAnyFormat)
	{
		if (m_nWidth!=nWidth || m_nHeight!=nHeight)
		{
			return -1;
		}
	}
	else
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
	}

	m_pXCameraRender=new XCameraRender(*this);
	m_pXCameraRender->SetFrameRate(nFrameRate,m_nTotalFrameRate);
	m_pXCameraRender->CheckMediaType(m_subType);
	
	hr=m_pigb -> QueryInterface(IID_IMediaControl, (void **)&m_pimc);
	if (FAILED(hr))
	{
        return -1;
	}

	m_bCrossBar=true;
    CComPtr< IAMCrossbar > pCrossbar;
    hr = m_picgb2->FindInterface(&PIN_CATEGORY_CAPTURE,
												&MEDIATYPE_Video, m_pibfCap,
												IID_IAMCrossbar, (void **)&pCrossbar);
    if (FAILED(hr))
    {
 		m_bCrossBar=false;
    } 
	else if (pCrossbar!=NULL)
	{
		LONG lRouted=0;
		hr=pCrossbar->get_IsRoutedTo(0, &lRouted);
		if (!FAILED(hr))
		{
			int nInput=-1;
			XCameraCap::GetInput(m_nDevice,nInput);
			if (lRouted!=nInput)
			{
				if (nInput>0)
				{
					hr=pCrossbar->Route(0,nInput);
					if (FAILED(hr))
					{
						nInput=lRouted;	
						XCameraCap::SetInput(m_nDevice,nInput);
					}
				}
				else
				{
					nInput=lRouted;	
					XCameraCap::SetInput(m_nDevice,nInput);
				}
			}
		}
	}	

	m_pimc->Run();

	return 0;
}

void XCameraCap::Close(void)
{
	if (m_pimc)
	{
		m_pimc->Stop();
	}

	HELPER_RELEASE(m_pimc);
	HELPER_RELEASE(m_pibfCap);
	HELPER_RELEASE(m_pibfGrabber);
	HELPER_RELEASE(m_piSampleGrabber);
	HELPER_RELEASE(m_pibfNullRender);
	HELPER_RELEASE(m_pigb);
	HELPER_RELEASE(m_picgb2);

	if (m_pXCameraRender!=NULL)
	{
		delete m_pXCameraRender;
		m_pXCameraRender=NULL;
	}
}


STDMETHODIMP XCameraCap::QueryInterface(REFIID riid, void ** ppv) 
{
	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP XCameraCap::BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize) 
{
	//This method is not used but must be implemented
	return 0;
}

STDMETHODIMP XCameraCap::SampleCB(double SampleTime, IMediaSample * pSample) 
{
	if (!pSample) 
	{
		return E_POINTER;
	} 
	else 
	{
		if (m_pXCameraRender!=NULL)
		{
			m_pXCameraRender->DoRenderSample(pSample);
		}
	}
	return 0;
}


void XCameraCap::OnCaptureVideoFrame(VIDEC_Image*pImage)
{
	m_rCallback.OnVIDEC_CameraCallbackImageData(pImage);
}

/***********************************************************************/
/*功能：显示视频调整窗口
/*参数：
/*返回：
/*备注：如果没有打开视频设备，则不显示任何窗口
/***********************************************************************/
void XCameraCap::ShowAdjustVideoWnd(HWND hWnd)
{
	if (m_pibfCap)
		ShowPropertyPage(hWnd,(IUnknown*)m_pibfCap,NULL);//NAME("视频调整")
}


HRESULT XCameraCap::SetVideoProperties(IBaseFilter*pCap,int& nWidth,int& nHeight,int nFrameRate)
{
	if (!pCap)
		return S_FALSE;

    HRESULT hr=0;
	if (SetCap(pCap,MEDIASUBTYPE_YUY2,nWidth,nHeight)!=S_OK){
	if (SetCap(pCap,MEDIASUBTYPE_YVYU,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_UYVY,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_I420_VC,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_IYUV,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_YV12,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_RGB24,nWidth,nHeight)!=S_OK){	
	if (SetCap(pCap,MEDIASUBTYPE_RGB32,nWidth,nHeight)!=S_OK){	
	}}}}}}}}


    CComPtr< IPin > pPin0;
	pPin0=GetOutPin(pCap,0);
	if (!pPin0)
		return S_FALSE;

    CComPtr< IAMStreamConfig > pVSC;
    hr = pPin0->QueryInterface(IID_IAMStreamConfig, (void **)&pVSC);
    if (FAILED(hr))
    {
        return S_FALSE;
    }            
	
    AM_MEDIA_TYPE *pmt;
    hr = pVSC->GetFormat(&pmt);
    if (FAILED(hr))
    {
        return S_FALSE;
    }    
    
	hr=E_FAIL;
	if(pmt->formattype == FORMAT_VideoInfo) 
	{
        VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
		m_subType=pmt->subtype;
		nWidth=pvi->bmiHeader.biWidth;
		nHeight=pvi->bmiHeader.biHeight;
		if (m_subType!=MEDIASUBTYPE_YUY2 &&
			m_subType!=MEDIASUBTYPE_YVYU &&
			m_subType!=MEDIASUBTYPE_UYVY &&
			m_subType!=MEDIASUBTYPE_I420_VC &&
			m_subType!=MEDIASUBTYPE_IYUV &&
			m_subType!=MEDIASUBTYPE_YV12 &&
			m_subType!=MEDIASUBTYPE_RGB24 &&
			m_subType!=MEDIASUBTYPE_RGB32)
		{
			hr=E_FAIL;
		}
		else
		{
			hr=S_OK;

			REFERENCE_TIME avgTimePerFrame=pvi->AvgTimePerFrame;
			if (avgTimePerFrame!=0)
			{
				m_nTotalFrameRate=10000000/avgTimePerFrame;
				if (m_nTotalFrameRate==29)
				{
					m_nTotalFrameRate=30;
				}
			}
		}
	}

    DeleteMediaType(pmt);

    return hr;
}

HRESULT XCameraCap::SetCap(IBaseFilter*pCap,GUID subType,int nWidth,int nHeight)
{
	if (!pCap)
		return S_FALSE;


    CComPtr< IPin > pPin0;
	pPin0=GetOutPin(pCap,0);
	if (!pPin0)
		return S_FALSE;

    CComPtr< IAMStreamConfig > pVSC;
    HRESULT hr = pPin0->QueryInterface(IID_IAMStreamConfig, (void **)&pVSC);
    if (FAILED(hr))
    {
        return S_FALSE;
    }            

	int iCount, iSize;
	hr = pVSC->GetNumberOfCapabilities(&iCount, &iSize);

	VIDEO_STREAM_CONFIG_CAPS scc;
	if (sizeof(scc) != iSize) {
		return S_FALSE;
	}

	int i =0;
	for ( i = 0; i < iCount; i++) {
		AM_MEDIA_TYPE * pmt = NULL;
		hr = pVSC->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE *>(&scc));
		if (hr == S_OK) {
			if (subType != pmt->subtype) 
			{
				hr = E_FAIL;
				continue;
			}
			VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *) pmt->pbFormat;
			if (pvi->bmiHeader.biWidth != nWidth ||
				pvi->bmiHeader.biHeight != nHeight)
			{
				hr = E_FAIL;
				continue;
			}

			REFERENCE_TIME avgTimePerFrame=pvi->AvgTimePerFrame;
			if (avgTimePerFrame!=400000 && avgTimePerFrame!=333333 && avgTimePerFrame!=333667)
			{
				pvi->AvgTimePerFrame = 333333;
				hr = pVSC->SetFormat(pmt);
				if (hr != S_OK) 
				{
					pvi->AvgTimePerFrame = 333667;
					hr = pVSC->SetFormat(pmt);
					if (hr != S_OK) 
					{
						pvi->AvgTimePerFrame = 400000;
						hr = pVSC->SetFormat(pmt);
						if (hr != S_OK) 
						{
							pvi->AvgTimePerFrame = avgTimePerFrame;
							hr = pVSC->SetFormat(pmt);
						}
					}
				}
			}
			else
			{
				pvi->AvgTimePerFrame = avgTimePerFrame;
				hr = pVSC->SetFormat(pmt);
			}
				
			hr = S_OK;
			break;
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////
// showPropertyPage: Display the property page of a COM object
// Auguments:
//   pIU:    A interface of the COM object
//   name:   the name of the dialog box of the property page
// return value: error code
///////////////////////////////////////////////////////////////////////
HRESULT XCameraCap::ShowPropertyPage(HWND hwndParent,IUnknown* pIU, const WCHAR* name)
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
        HELPER_RELEASE(pispp);
    }
    return hr;
}

void XCameraCap::GetCapDevice(INT nCapDevice,IBaseFilter**ppCap)
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

HRESULT XCameraCap::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
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

IPin * XCameraCap::GetInPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
    return pComPin;
}


IPin * XCameraCap::GetOutPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
    return pComPin;
}

bool XCameraCap::HasVideoInputWnd(void)
{
	return m_bCrossBar;
}

void XCameraCap::ShowVideoInputWnd(HWND hWnd)
{
	if (m_picgb2==NULL || m_pibfCap==NULL || !HasVideoInputWnd())
		return ;

	HRESULT hr; 

    CComPtr< IAMCrossbar > pCrossbar;
    hr = m_picgb2->FindInterface(&PIN_CATEGORY_CAPTURE,
												&MEDIATYPE_Video, m_pibfCap,
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
		XCameraCap::GetInput(m_nDevice,nInput);
		if (lRouted!=nInput)
		{
			nInput=lRouted;	
			XCameraCap::SetInput(m_nDevice,nInput);
		}
	}
}


int XCameraCap::GetWidth(void)
{
	return m_nWidth;
}

int XCameraCap::GetHeight(void)
{
	return m_nHeight;
}

bool XCameraCap::TVTunerPutChannel(long lChannel)
{
	if (m_picgb2==NULL ||
		m_pibfCap==NULL )
	{
		return false;
	}

	HRESULT hr;
    CComPtr< IAMTVTuner > pTVTuner;
	 
	hr = m_picgb2->FindInterface(&PIN_CATEGORY_CAPTURE,
										&MEDIATYPE_Interleaved, m_pibfCap,
										IID_IAMTVTuner, (void **)&pTVTuner);
	if (FAILED(hr))
	{
		hr = m_picgb2->FindInterface(&PIN_CATEGORY_CAPTURE,
											&MEDIATYPE_Video, m_pibfCap,
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
