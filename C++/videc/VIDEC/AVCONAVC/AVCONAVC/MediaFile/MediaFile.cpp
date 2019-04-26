#include "stdafx.h"

#include <d3d9.h>
#include <dxva2api.h>

#include <streams.h>
#include <atlbase.h>
#include <evr.h>
#include <uuids.h>
#include "moreuuids.h"

#include "MediaFile.h"
#include <math.h>
#include <mmsystem.h>
#include "NullRender.h"

#include "DShowUtil.h"

#include "..\..\demuxer\LAVSplitter\LAVSplitter.h"
#include "..\..\decoder\LAVVideo\LAVVideo.h"
#include "growarray.h"
#include "..\..\decoder\LAVAudio\LAVAudio.h"

	
#pragma comment(lib,"winmm.lib")



#define SAFE_RELEASE(x) {if(x) x -> Release(); x = NULL;}

HRESULT InitializeEVR(IBaseFilter *pEVR, HWND hwnd, IMFVideoDisplayControl ** ppWc); 
HRESULT InitWindowlessVMR(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl** ppWc); 

EXTERN_C const CLSID CLSID_NullRenderer;

CComPtr<IPin> GetPin(IBaseFilter*pFilter,LPCOLESTR pinname)
{
	CComPtr<IEnumPins>	pEnum;
	CComPtr<IPin>		pPin;
	HRESULT hr =pFilter->EnumPins(&pEnum);
	if (hr!=S_OK)
	{
		return NULL;
	}
	while (pEnum->Next(1,&pPin,0)==S_OK)
	{
		PIN_INFO pinfo;
		pPin->QueryPinInfo(&pinfo);
		BOOL bFound=!wcsicmp(pinname,pinfo.achName);
		if (bFound)
		{
			return pPin;
		}
		pPin.Release();
	}
	return NULL;
}


AVCONAVC_MediaFile*AVCONAVC_MediaFile::Create(AVCONAVC_MediaFileCallback&rCallback)
{
	//return new MediaFile(rCallback);
	NEWRECORDret3( AVCONAVC_MediaFile*, p, new MediaFile(rCallback) ) 
}

AVCONAVC_MediaFile*AVCONAVC_MediaFile::Create(AVCONAVC_MediaFileCallback&rCallback,bool bAudio,bool bVideo)
{
	//return new MediaFile(rCallback,bAudio,bVideo);
	NEWRECORDret3(AVCONAVC_MediaFile*, p, new MediaFile(rCallback, bAudio, bVideo) ) 
}


MediaFile::MediaFile(AVCONAVC_MediaFileCallback&rCallback,bool bAudio,bool bVideo)
:m_rCallback(rCallback)

,m_nVideoWidth(320)
,m_nVideoHeight(240)
{
	m_bHasVideo=false;
	m_bHasAudio=false;
	m_bVideo=bVideo;
	m_bAudio=bAudio;

	m_hWndVideo=NULL;

	m_strFilePathName="";

	m_pAudioSampleGrabber=NULL;

	m_nVideoRenderType=VRT_NULL;
	m_pVideoDisplay=NULL;
	m_pWindowless=NULL;

	m_dblLastVideoSampleTime=0;

	m_pFileReader=NULL;
	m_pMemStream=NULL;
	m_pMemReader=NULL;

	m_pLAVSplitter=NULL;
	m_pLAVAudio=NULL;
	m_pLAVVideo=NULL;
	m_pVideoNullRenderer=NULL;

	m_bLoadFailed=false;
}

MediaFile::~MediaFile(void)
{
}



int MediaFile::Open(HWND hWndVideo,const char*cszFileName,bool& bTryEVR)
{
	m_hWndVideo=hWndVideo;

	if( cszFileName==NULL )
		return -1 ;

	m_strFilePathName=cszFileName;

	return OpenFile(cszFileName,bTryEVR);
}


void MediaFile::Close(void)
{
	CloseFile();
	{
		XAutoLock l(m_csListXSampleBuffer);
		while (m_ListXSampleBuffer.size())
		{
			XSampleBuffer*pXSampleBuffer=(XSampleBuffer*)m_ListXSampleBuffer.front();
			m_ListXSampleBuffer.pop_front();
			delete pXSampleBuffer;
			pXSampleBuffer=NULL;
		}
	}
}

std::string Replace(std::string&str,const char*string_to_replace,const char *new_string) 
{ 
	//   Find   the   first   string   to   replace 
	int   index   =   str.find(string_to_replace); 
	//   while   there   is   one 
	while(index   !=   std::string::npos) 
	{ 
		//   Replace   it 
		str.replace(index,   strlen(string_to_replace),   new_string); 
		//   Find   the   next   one 
		index   =   str.find(string_to_replace,   index   +   strlen(new_string)); 
	} 
	return   str; 
} 

bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2)
{  
	if (str1.size() != str2.size()) 
	{  
		return false;  
	}  
	for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) 
	{  
		if (tolower(*c1) != tolower(*c2)) 
		{  
			return false;  
		}  
	}  
	return true;  
} 

int MediaFile::OpenFile(const char*cszPathName,bool& bTryEVR)
{
	HRESULT hr=S_OK;

	{
		std::string strPathName=cszPathName;
		if (strPathName.length()>7)
		{
			Replace(strPathName,"\\","/");

			int nIndex=strPathName.find("://");
			if (nIndex!=std::string::npos)
			{
				strPathName=strPathName.substr(nIndex+3,strPathName.length());

				nIndex=strPathName.find("/");
				if (nIndex==std::string::npos)
					return -1;

				std::string strVODServerIP=strPathName.substr(0,nIndex);
				strPathName=strPathName.substr(nIndex+1,strPathName.length());
#ifdef DEBUG1
				NEWRECORD3(  LocalFileReader*,pFileReader,new LocalFileReader() )
				if (pFileReader->Open(strPathName.c_str())==false)
#else
				NEWRECORD3( RemoteFileReader*,pFileReader , new RemoteFileReader() )
				if (pFileReader->Open(strVODServerIP.c_str(),strPathName.c_str())==false)
#endif
				{
					pFileReader->Close();
					delete pFileReader;
					pFileReader=NULL;
					return -1;
				}
				else
				{
					m_pFileReader=pFileReader;
				}

				DWORD dwKBPerSec=INFINITE;

				NEWRECORD(m_pMemStream, new CMemStream(m_pFileReader->GetFileLength(), dwKBPerSec) )
				m_pMemStream->m_pFileReader = m_pFileReader;
				m_pMemStream->SetPointer(0);
				m_pMemStream->m_nRenderBlocks=0;

				CMediaType mt;
				mt.majortype = MEDIATYPE_Stream;
				mt.subtype = MEDIASUBTYPE_NULL;

				NEWRECORD( m_pMemReader , new CMemReader(m_pMemStream, &mt, &hr))
				if(FAILED(hr) || m_pMemReader == NULL)
				{
					return -1;
				}
				m_pMemReader->AddRef();
			}
		}
	}

	hr=m_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (hr!=S_OK)
	{
		return -1;
	}

	CComPtr<ICaptureGraphBuilder2> pBuilder;
	hr=pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	if (hr!=S_OK)
	{
		return -1;
	}
	hr=pBuilder->SetFiltergraph(m_pGraph);
	if (hr!=S_OK)
	{
		return -1;
	}

	if (m_pMemReader)
	{
		hr=m_pGraph->AddFilter(m_pMemReader,L"MemReader Source");
		if (hr!=S_OK)
		{
			return -1;
		}

		NEWRECORD( m_pLAVSplitter , new CLAVSplitter(NULL, &hr) )
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(m_pLAVSplitter,L"LAV Splitter");
		if (hr!=S_OK)
		{
			return -1;
		}

		hr=m_pGraph->ConnectDirect(m_pMemReader->GetPin(0),GetPin(m_pLAVSplitter,L"Input"),NULL);
		if (hr!=S_OK)
		{
			return -1;
		}
	}
	else
	{
		NEWRECORD( m_pLAVSplitter , new CLAVSplitterSource(NULL, &hr) )
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(m_pLAVSplitter,L"LAV Splitter Source");
		if (hr!=S_OK)
		{
			return -1;
		}

		CComQIPtr<IFileSourceFilter> pFileSourceFilter=m_pLAVSplitter;
		if (!pFileSourceFilter)
		{
			return -1;
		}

		{
			USES_CONVERSION;
			hr=pFileSourceFilter->Load(A2W(cszPathName),NULL);
			if (hr!=S_OK)
			{
				m_bLoadFailed=true;

				return -1;
			}
		}
	}

	CComPtr<IPin> pAudioPin=GetPin(m_pLAVSplitter,L"Audio");
	CComPtr<IBaseFilter> pAudioRender;
	if (pAudioPin && m_bAudio)
	{
		bool bNullRenderer=false;
		hr=pAudioRender.CoCreateInstance(CLSID_DSoundRender);
		if (hr!=S_OK)
		{
			hr=pAudioRender.CoCreateInstance(CLSID_NullRenderer);
			if (hr!=S_OK)
			{
				return -1;
			}
			bNullRenderer=true;
		}
		hr=m_pGraph->AddFilter(pAudioRender,L"Audio Renerer");
		if (hr!=S_OK)
		{
			return -1;
		}

		NEWRECORD( m_pLAVAudio , new CLAVAudio(NULL, &hr) )
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(m_pLAVAudio,L"LAV Audio Decoder");
		if (hr!=S_OK)
		{
			return -1;
		}

		m_pAudioSampleGrabber=(AudioSampleGrabber*)AudioSampleGrabber::CreateInstance(NULL,&hr,*this);
		if (m_pAudioSampleGrabber==NULL)
		{
			return -1;
		}
		m_pAudioSampleGrabber->AddRef();
		hr=m_pGraph->AddFilter(m_pAudioSampleGrabber,L"Sample Grabber Audio");
		if (hr!=S_OK)
		{
			return -1;
		}


		hr=m_pGraph->ConnectDirect(pAudioPin,GetPin(m_pLAVAudio,L"Input"),NULL);
		if (hr==S_OK)
		{
			{
				hr=m_pGraph->Connect(GetPin(m_pLAVAudio,L"Output"),m_pAudioSampleGrabber->GetPin(0));
				if (hr!=S_OK)
				{
					return -1;
				}

				hr=m_pGraph->Connect(m_pAudioSampleGrabber->GetPin(1),GetPin(pAudioRender,bNullRenderer?L"In":L"Audio Input pin (rendered)"));
				if (hr!=S_OK)
				{
					return -1;
				}
			}		
		}
		else
		{
			{
				hr=m_pGraph->Connect(pAudioPin,m_pAudioSampleGrabber->GetPin(0));
				if (hr!=S_OK)
				{
					return -1;
				}
				hr=m_pGraph->Connect(m_pAudioSampleGrabber->GetPin(1),GetPin(pAudioRender,bNullRenderer?L"In":L"Audio Input pin (rendered)"));
				if (hr!=S_OK)
				{
					return -1;
				}
			}
		}

		m_bHasAudio=true;
	}

	CComPtr<IPin> pVideoPin=GetPin(m_pLAVSplitter,L"Video");
	if (pVideoPin && m_bVideo)
	{
		NEWRECORD( m_pLAVVideo , new CLAVVideo(NULL, &hr) )
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(m_pLAVVideo,L"LAV Video Decoder");
		if (hr!=S_OK)
		{
			return -1;
		}

		hr=m_pGraph->ConnectDirect(pVideoPin,GetPin(m_pLAVVideo,L"Input"),NULL);
		if (hr!=S_OK)
		{
			return -1;
		}

		if (m_hWndVideo==NULL)
		{
			NEWRECORD( m_pVideoNullRenderer , new NullRenderer(NULL, &hr, *this) )
			if (hr!=S_OK)
			{
				return -1;
			}

			m_nVideoRenderType=VRT_NULL;

			hr=m_pGraph->AddFilter(m_pVideoNullRenderer,L"Video Null Renerer");
			if (hr!=S_OK)
			{
				return -1;
			}

			hr=m_pGraph->ConnectDirect(GetPin(m_pLAVVideo,L"Output"),GetPin(m_pVideoNullRenderer,L"In"),NULL);
			if (hr!=S_OK)
			{
				return -1;
			}
		}
		else
		{

			while (1)
			{
				if (bTryEVR)
				{
					CComPtr<IBaseFilter> pVideoRender;

					hr=pVideoRender.CoCreateInstance(CLSID_EnhancedVideoRenderer);
					if (hr==S_OK)
					{
						hr = InitializeEVR(pVideoRender, m_hWndVideo, &m_pVideoDisplay);
						if (hr==S_OK)
						{
							m_nVideoRenderType=VRT_EVR;

							hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (EVR)");
							if (hr!=S_OK)
							{
								bTryEVR=false;
								return -1;
							}

							hr=m_pGraph->ConnectDirect(GetPin(m_pLAVVideo,L"Output"),GetPin(pVideoRender,L"EVR Input0"),NULL);
							if (hr!=S_OK)
							{
								bTryEVR=false;
								return -1;
							}
							break;
						}
					}
					bTryEVR=false;
				}

				{
					CComPtr<IBaseFilter> pVideoRender;

					hr=pVideoRender.CoCreateInstance(CLSID_VideoMixingRenderer);
					if (hr==S_OK)
					{
						hr = InitWindowlessVMR(pVideoRender, m_hWndVideo, &m_pWindowless);
						if (hr==S_OK)
						{
							m_nVideoRenderType=VRT_VMR;

							hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (VMR)");
							if (hr!=S_OK)
							{
								return -1;
							}

							hr=m_pGraph->ConnectDirect(GetPin(m_pLAVVideo,L"Output"),GetPin(pVideoRender,L"VMR Input0"),NULL);
							if (hr!=S_OK)
							{
								return -1;
							}
							break;
						}
					}
				}

				{
					CComPtr<IBaseFilter> pVideoRender;

					hr=pVideoRender.CoCreateInstance(CLSID_VideoRenderer);
					if (hr!=S_OK)
					{
						return -1;
					}
					m_nVideoRenderType=VRT_SDR;

					hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (SDR)");
					if (hr!=S_OK)
					{
						return -1;
					}

					hr=m_pGraph->ConnectDirect(GetPin(m_pLAVVideo,L"Output"),GetPin(pVideoRender,L"Input"),NULL);
					if (hr!=S_OK)
					{
						return -1;
					}
				}
				break;
			}
		}

		m_bHasVideo=true;
				
		CComQIPtr<IVideoDecoder,&IID_IVideoDecoder> pVideoDecoder(m_pLAVVideo);
		if (pVideoDecoder)
		{
			m_nVideoWidth=pVideoDecoder->GetWidth();
			m_nVideoHeight=pVideoDecoder->GetHeight();

			if (m_nVideoWidth==1920 && m_nVideoHeight==540)
			{
				m_nVideoHeight=m_nVideoHeight*2;
			}
		}
	}


	if (HasAudio()==false && HasVideo()==false)
	{
		return -1;
	}

	RECT rc;
	::GetClientRect(m_hWndVideo,&rc);
	switch (m_nVideoRenderType)
	{
	case VRT_EVR:
		m_pVideoDisplay->SetVideoPosition(NULL, &rc);
		break;
	case VRT_VMR:
		m_pWindowless->SetVideoPosition(NULL, &rc);
		m_pWindowless->DisplayModeChanged();
		break;
	case VRT_SDR:
		CComQIPtr<IVideoWindow,&IID_IVideoWindow> pVideoWindow(m_pGraph);
		if (pVideoWindow)
		{
			pVideoWindow -> put_Owner((OAHWND)m_hWndVideo);
			pVideoWindow -> put_MessageDrain((OAHWND)m_hWndVideo);
			pVideoWindow -> put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			pVideoWindow -> put_Visible(OATRUE);

			pVideoWindow -> SetWindowPosition(0, 0, rc.right-rc.left, rc.bottom-rc.top);
		}
		break;
	}

	Play();

	return 0;
}


void MediaFile::CloseFile(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	if (pMediaControl)
	{
		pMediaControl->Stop();
	}

	if (m_pLAVSplitter)
	{
		if (m_pGraph)
		{
			m_pGraph->RemoveFilter(m_pLAVSplitter);
		}

		if (!m_bLoadFailed)
		{
			delete m_pLAVSplitter;
		}
		m_pLAVSplitter=NULL;
	}

	if (m_pLAVAudio)
	{
		if (m_pGraph)
		{
			m_pGraph->RemoveFilter(m_pLAVAudio);
		}
		delete m_pLAVAudio;
		m_pLAVAudio=NULL;
	}

	if (m_pLAVVideo)
	{
		if (m_pGraph)
		{
			m_pGraph->RemoveFilter(m_pLAVVideo);
		}
		delete m_pLAVVideo;
		m_pLAVVideo=NULL;
	}

	if (m_pVideoNullRenderer)
	{
		if (m_pGraph)
		{
			m_pGraph->RemoveFilter(m_pVideoNullRenderer);
		}
		delete m_pVideoNullRenderer;
		m_pVideoNullRenderer=NULL;
	}
	SAFE_RELEASE(m_pWindowless);
	SAFE_RELEASE(m_pVideoDisplay);
	SAFE_RELEASE(m_pAudioSampleGrabber);

	SAFE_RELEASE(m_pMemReader);

	if(m_pMemStream)
	{
		delete m_pMemStream;
		m_pMemStream = NULL;
	}
	
	if (m_pFileReader)
	{
		m_pFileReader->Close();
		delete m_pFileReader;
		m_pFileReader=NULL;
	}
}

void MediaFile::SetWndPos(int x,int y,int cx,int cy)
{
	RECT rc;
	rc.left=x;
	rc.right=x+cx;
	rc.top=y;
	rc.bottom=y+cy;

	switch (m_nVideoRenderType)
	{
	case VRT_EVR:
		m_pVideoDisplay->SetVideoPosition(NULL, &rc);
		break;
	case VRT_VMR:
		m_pWindowless->SetVideoPosition(NULL, &rc);
		m_pWindowless->DisplayModeChanged();
		break;
	case VRT_SDR:
		CComQIPtr<IVideoWindow,&IID_IVideoWindow> pVideoWindow(m_pGraph);
		if (pVideoWindow)
		{
			pVideoWindow -> SetWindowPosition(0, 0,rc.right-rc.left, rc.bottom-rc.top);
		}
		break;
	}
}

void MediaFile::Repaint(void)
{
	switch (m_nVideoRenderType)
	{
	case VRT_EVR:
		m_pVideoDisplay->RepaintVideo();
		break;
	case VRT_VMR:
		{
			HDC hDC=::GetDC(m_hWndVideo);
			m_pWindowless->RepaintVideo(m_hWndVideo, hDC);
			::ReleaseDC(m_hWndVideo,hDC);
		}
		break;
	}
}

unsigned long MediaFile::GetDuration(void)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	double ulDuration=0;
	if (pMediaPosition==NULL)
		return (unsigned long)ulDuration;

	HRESULT hr=pMediaPosition->get_Duration(&ulDuration);

	return (unsigned long)ulDuration;
}

unsigned long MediaFile::GetPosition(void)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	double ulCurrentPosition=0;
	if (pMediaPosition==NULL)
		return (unsigned long)ulCurrentPosition;

	HRESULT hr=pMediaPosition->get_CurrentPosition(&ulCurrentPosition);

	return (unsigned long)ulCurrentPosition;
}

int MediaFile::Seek(unsigned long ulSeconds)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	if (pMediaPosition==NULL)
		return -1;

	pMediaPosition->put_CurrentPosition(ulSeconds);

	return 0;
}

int MediaFile::Pause(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	if (pMediaControl!=NULL)
	{
		pMediaControl -> Pause ();
	}

	return 0;
}

int MediaFile::Play(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	HRESULT hr;
	if (pMediaControl!=NULL)
	{
		hr=pMediaControl -> Run ();
	}


	{
		XAutoLock l(m_csListXSampleBuffer);
		while (m_ListXSampleBuffer.size())
		{
			XSampleBuffer*pXSampleBuffer=(XSampleBuffer*)m_ListXSampleBuffer.front();
			m_ListXSampleBuffer.pop_front();
			delete pXSampleBuffer;
			pXSampleBuffer=NULL;
		}
	}

	return 0;
}


bool MediaFile::HasAudio(void)
{
	return  m_bHasAudio;
}

int MediaFile::SetVolume(int nVolume)
{
	if (HasAudio() && m_pGraph!=NULL)
	{
		CComQIPtr<IBasicAudio,&IID_IBasicAudio> pBasicAudio(m_pGraph);
		if ( pBasicAudio ==NULL ) 
			return -1;

		long lNewValue;
		if (nVolume<1)
			nVolume=1;
		else if (nVolume>100)
			nVolume=100;

		lNewValue=(long)(log((double(nVolume))/100.0) /log(1.0001)/4.6);

		if (lNewValue < -10000)
			lNewValue=-10000;


		pBasicAudio->put_Volume(lNewValue);
	}

	return 0;
}

bool MediaFile::HasVideo(void)
{
	return m_bHasVideo;
}

int MediaFile::GetVideoWidth(void)
{
	return m_nVideoWidth;
}

int MediaFile::GetVideoHeight(void)
{
	return m_nVideoHeight;
}

void MediaFile::OnNullRendererYV12Buffer(unsigned char**ppBuffer,int nBufferLen)
{
	m_rCallback.OnAVCONAVC_MediaFileCallbackYV12Buffer(ppBuffer,nBufferLen);
}

void MediaFile::OnNullRendererCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,double dblSampleTime)
{
	//char szDebug[1024];
	//sprintf(szDebug,"V:%.2f %u ",dblSampleTime,timeGetTime());
	//OutputDebugStringA(szDebug);

	m_dblLastVideoSampleTime=dblSampleTime;
	m_rCallback.OnAVCONAVC_MediaFileCallbackYV12Data(this,pData,nLen,nWidth,nHeight,nFrameRate,timeGetTime());


	DoCheckAudioSample();
}

void MediaFile::OnAudioSampleGrabberCallback(unsigned char*pData,int nLen,int nSampleRate,double dblSampleTime)
{
	if (m_bHasVideo)
	{
		double dblDelta=dblSampleTime-m_dblLastVideoSampleTime;
		if (dblDelta<0.0)
		{
			XSampleBuffer*pXSampleBuffer=NULL;
			while (true)
			{
				{
					XAutoLock l(m_csListXSampleBuffer);
					if (m_ListXSampleBuffer.size())
					{
						pXSampleBuffer=(XSampleBuffer*)m_ListXSampleBuffer.front();
						m_ListXSampleBuffer.pop_front();
					}
					else
					{
						break;
					}
				}

				if (pXSampleBuffer!=NULL)
				{
					OnCaputuredPCM(pXSampleBuffer->GetBuffer(),pXSampleBuffer->GetBufferLen(),pXSampleBuffer->GetSampleRate(),pXSampleBuffer->GetSampleTime());

					delete pXSampleBuffer;
					pXSampleBuffer=NULL;
				}
				else
				{
					break;
				}
			}

			OnCaputuredPCM(pData,nLen,nSampleRate,dblSampleTime);
		}
		else
		{
			NEWRECORD3( XSampleBuffer* ,pXSampleBuffer , new XSampleBuffer() )
			if (pXSampleBuffer!=NULL)
			{
				if (pXSampleBuffer->SetSampleBuffer(dblSampleTime,pData,nLen,nSampleRate))
				{
					XAutoLock l(m_csListXSampleBuffer);
					m_ListXSampleBuffer.push_back(pXSampleBuffer);
				}
				else
				{
					delete pXSampleBuffer;
					pXSampleBuffer=NULL;
				}
			}

			DoCheckAudioSample();
		}
	}
	else
	{
		OnCaputuredPCM(pData,nLen,nSampleRate,dblSampleTime);
	}
}


void MediaFile::DoCheckAudioSample(void)
{
	XSampleBuffer*pXSampleBuffer=NULL;
	while (true)
	{
		{
			XAutoLock l(m_csListXSampleBuffer);
			if (m_ListXSampleBuffer.size())
			{
				pXSampleBuffer=(XSampleBuffer*)m_ListXSampleBuffer.front();
				double dblDelta=pXSampleBuffer->GetSampleTime()-m_dblLastVideoSampleTime;
				if (dblDelta<0.0)
				{
					m_ListXSampleBuffer.pop_front();
				}
				else
				{
					pXSampleBuffer=NULL;
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (pXSampleBuffer!=NULL)
		{
			OnCaputuredPCM(pXSampleBuffer->GetBuffer(),pXSampleBuffer->GetBufferLen(),pXSampleBuffer->GetSampleRate(),pXSampleBuffer->GetSampleTime());

			delete pXSampleBuffer;
			pXSampleBuffer=NULL;
		}
		else
		{
			break;
		}
	}

}

void MediaFile::OnCaputuredPCM(unsigned char*pPCMData,int nLen,int nSampleRate,double dblSampleTime)
{
	//char szDebug[1024];
	//sprintf(szDebug,"A:%.2f %u\n",dblSampleTime,m_ListXSampleBuffer.size());
	//OutputDebugStringA(szDebug);

	m_rCallback.OnAVCONAVC_MediaFileCallbackPCMData(this,pPCMData,nLen,nSampleRate,timeGetTime());
}

//-----------------------------------------------------------------------------
// InitializeEVR
// Initialize the EVR filter. 
//-----------------------------------------------------------------------------

HRESULT InitializeEVR( 
					  IBaseFilter *pEVR,				// Pointer to the EVR
					  HWND hwnd,						// Clipping window
					  IMFVideoDisplayControl** ppDisplayControl
					  ) 
{ 

	HRESULT hr = S_OK;

	IMFGetService *pService = NULL;
	IMFVideoDisplayControl *pDisplay = NULL;

	hr = pEVR->QueryInterface(__uuidof(IMFGetService), (void**)&pService); 
	if (SUCCEEDED(hr)) 
	{ 
		hr = pService->GetService(
			MR_VIDEO_RENDER_SERVICE, 
			__uuidof(IMFVideoDisplayControl),
			(void**)&pDisplay
			);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pDisplay->SetVideoWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pDisplay->SetAspectRatioMode(MFVideoARMode_None);
	}

	// Return the IMFVideoDisplayControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppDisplayControl = pDisplay;
		(*ppDisplayControl)->AddRef();
	}

	SAFE_RELEASE(pService);
	SAFE_RELEASE(pDisplay);

	return hr; 
} 

//-----------------------------------------------------------------------------
// InitWindowlessVMR
// Initialize the VMR-7 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT InitWindowlessVMR( 
						  IBaseFilter *pVMR,				// Pointer to the VMR
						  HWND hwnd,						// Clipping window
						  IVMRWindowlessControl** ppWC	// Receives a pointer to the VMR.
						  ) 
{ 

	IVMRFilterConfig* pConfig = NULL; 
	IVMRWindowlessControl *pWC = NULL;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
	if (SUCCEEDED(hr)) 
	{ 
		hr = pConfig->SetRenderingMode(VMRMode_Windowless); 

		DWORD dwPrefs=0;
		hr = pConfig->GetRenderingPrefs(&dwPrefs); 
		dwPrefs|=RenderPrefs_ForceOffscreen;
		hr = pConfig->SetRenderingPrefs(dwPrefs); 
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR_ARMODE_NONE);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr; 
} 