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
#include "XCameraCapEx.h"

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

	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");	


	WritePrivateProfileString(_T("VIDEC"), strName , strValue, chIniPath); 	
}

void XCameraCap::GetInput(int nDevice,int&nInput)
{
	char strName[1024];
#ifdef _UNICODE
	wsprintf(strName,_T("dev_%d_input"),nDevice);
#else
	sprintf(strName,_T("dev_%d_input"),nDevice);
#endif

	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");	

	nInput=GetPrivateProfileInt(_T("VIDEC"), strName , -1, chIniPath);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XCameraCap::XCameraCap(VIDEC_CameraCallback&rCallback)
:m_rCallback(rCallback)
{
	m_picgb2=NULL;
	m_pigb=NULL;
	m_pimc=NULL;
	m_pibfCap=NULL;
	m_pXCameraRender=NULL;
	m_bCrossBar=false;

	m_nWidth=0;
	m_nHeight=0;
	m_nDevice=0;

	m_bCapturing=false;
}

XCameraCap::~XCameraCap()
{
}

int XCameraCap::Open(int nDevice,int& nWidth,int& nHeight,int nFrameRate,int nTryMaxSize)
{
	m_nDevice=nDevice;
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

	GetCapDevice(nDevice,&m_pibfCap);
	if (!m_pibfCap)
		return -1;


    hr = m_pigb->AddFilter( m_pibfCap, L"Cap" );
    if( FAILED( hr ) )
	{
        return -1;
	}

	int nTotalFrameRate=30;
	hr=SetVideoProperties(m_pibfCap,nWidth,nHeight,nTotalFrameRate);
	if (FAILED(hr))
	{
        return -1;
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	NEWRECORD( m_pXCameraRender,new XCameraRender(*this,NULL,&hr) )
	NULLReturnRet( m_pXCameraRender , -1 ) 
	m_pXCameraRender->SetFrameRate(nFrameRate,nTotalFrameRate);
	m_pXCameraRender->AddRef();
	hr = m_pigb->AddFilter(m_pXCameraRender, L"XCameraRender");
	if( FAILED( hr ) )
	{
		return -1;
	}


	hr=m_picgb2->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pibfCap, NULL, m_pXCameraRender);
    if( FAILED( hr ) )
	{
        return -1;
	}
	
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
				if (nInput>=0)
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
	HELPER_RELEASE(m_pXCameraRender);
	HELPER_RELEASE(m_pigb);
	HELPER_RELEASE(m_picgb2);
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


HRESULT XCameraCap::SetVideoProperties(IBaseFilter*pCap,int& nWidth,int& nHeight,int&nTotalFrameRate)
{
	if (!pCap)
		return S_FALSE;

    HRESULT hr=0;


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

		GUID OldSubType=pmt->subtype;
		int nOldWidth=pvi->bmiHeader.biWidth;
		int nOldHeight=pvi->bmiHeader.biHeight;
		int nOldBitCount=pvi->bmiHeader.biBitCount;

		pvi->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biWidth=nWidth;
		pvi->bmiHeader.biHeight=nHeight;
		pvi->bmiHeader.biPlanes=1;

#if (0)
		pvi->bmiHeader.biCompression=MEDIASUBTYPE_RGB32.Data1;
		pvi->bmiHeader.biBitCount=32;
		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
		pmt->subtype=MEDIASUBTYPE_RGB32;

		hr = pVSC->SetFormat(pmt);
 		if (FAILED(hr))
#endif
		{
			pvi->bmiHeader.biCompression=MEDIASUBTYPE_YUY2.Data1;
			pvi->bmiHeader.biBitCount=16;
			pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
			pmt->subtype=MEDIASUBTYPE_YUY2;

			hr = pVSC->SetFormat(pmt);
			if (FAILED(hr))
			{
				pvi->bmiHeader.biCompression=MEDIASUBTYPE_YVYU.Data1;
				pvi->bmiHeader.biBitCount=16;
				pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
				pmt->subtype=MEDIASUBTYPE_YVYU;

				hr = pVSC->SetFormat(pmt);
				if (FAILED(hr))
				{
					pvi->bmiHeader.biCompression=MEDIASUBTYPE_UYVY.Data1;
					pvi->bmiHeader.biBitCount=16;
					pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
					pmt->subtype=MEDIASUBTYPE_UYVY;

					hr = pVSC->SetFormat(pmt);
					if (FAILED(hr))
					{
						pvi->bmiHeader.biCompression=MEDIASUBTYPE_I420_VC.Data1;
						pvi->bmiHeader.biBitCount=12;
						pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
						pmt->subtype=MEDIASUBTYPE_I420_VC;

						hr = pVSC->SetFormat(pmt);
						if (FAILED(hr))
						{
							pvi->bmiHeader.biCompression=MEDIASUBTYPE_IYUV.Data1;
							pvi->bmiHeader.biBitCount=12;
							pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
							pmt->subtype=MEDIASUBTYPE_IYUV;//I420

							hr = pVSC->SetFormat(pmt);
							if (FAILED(hr))
							{
								pvi->bmiHeader.biCompression=MEDIASUBTYPE_YV12.Data1;
								pvi->bmiHeader.biBitCount=12;
								pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
								pmt->subtype=MEDIASUBTYPE_YV12;

								hr = pVSC->SetFormat(pmt);
								if (FAILED(hr))
								{
									pvi->bmiHeader.biCompression=MEDIASUBTYPE_RGB24.Data1;
									pvi->bmiHeader.biBitCount=24;
									pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
									pmt->subtype=MEDIASUBTYPE_RGB24;

									hr = pVSC->SetFormat(pmt);
									if (FAILED(hr))
									{
										pvi->bmiHeader.biCompression=MEDIASUBTYPE_RGB32.Data1;
										pvi->bmiHeader.biBitCount=32;
										pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
										pmt->subtype=MEDIASUBTYPE_RGB32;

										hr = pVSC->SetFormat(pmt);
										if (FAILED(hr))
										{
											pvi->bmiHeader.biCompression=OldSubType.Data1;
											pvi->bmiHeader.biBitCount=nOldBitCount;
											pmt->subtype=OldSubType;
										}
									}
								}
							}
						}
					}
				}
			}

			pvi->bmiHeader.biWidth=nWidth;
			pvi->bmiHeader.biHeight=nHeight;
			pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);

			hr = pVSC->SetFormat(pmt);
			if (FAILED(hr))
			{
				nWidth=abs(nOldWidth);
				nHeight=abs(nOldHeight);

				pvi->bmiHeader.biWidth=nWidth;
				pvi->bmiHeader.biHeight=nHeight;
				pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);

				hr = pVSC->SetFormat(pmt);
			}
		}
	}

    DeleteMediaType(pmt);

    hr = pVSC->GetFormat(&pmt);
    if (FAILED(hr))
    {
        return S_FALSE;
    }    
    
	if(pmt->formattype == FORMAT_VideoInfo) 
	{
        VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;

		nTotalFrameRate=30;
		if (pvi->AvgTimePerFrame)
		{
			nTotalFrameRate=(unsigned long)10000000/pvi->AvgTimePerFrame;
		}
	}
    DeleteMediaType(pmt);

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
	//IEnumPins * pEnum;//use 160913 memory leak 
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

	//pEnum->Reset();
	//pEnum->Release();//use 160913 memory leak 

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
