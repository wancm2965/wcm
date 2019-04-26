#include "stdafx.h"

#include <d3d9.h>
#include <dxva2api.h>

#include <streams.h>
#include <atlbase.h>
#include <evr.h>
#include <uuids.h>
#include "moreuuids.h"

#include "AudioFile.h"
#include <math.h>
#include <mmsystem.h>

#include "DShowUtil.h"

#include "..\..\demuxer\LAVSplitter\LAVSplitter.h"
#include "growarray.h"
#include "..\..\decoder\LAVAudio\LAVAudio.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"
	
#pragma comment(lib,"winmm.lib")



#define SAFE_RELEASE(x) {if(x) x -> Release(); x = NULL;}


extern CComPtr<IPin> GetPin(IBaseFilter*pFilter,LPCOLESTR pinname);


AVCONAVC_AudioFile*AVCONAVC_AudioFile::Create(AVCONAVC_AudioFileCallback&rCallback)
{
	//return new AudioFile(rCallback);
	NEWRECORDret3(AVCONAVC_AudioFile*, p, new AudioFile(rCallback) ) 
}


AudioFile::AudioFile(AVCONAVC_AudioFileCallback&rCallback)
:m_rCallback(rCallback)
{
	m_strFilePathName="";

	m_pAudioSampleGrabber=NULL;
	m_pLAVSplitter=NULL;
	m_pLAVAudio=NULL;
}

AudioFile::~AudioFile(void)
{
}



int AudioFile::Open(const char*cszFileName)
{
	if( cszFileName==NULL )
		return -1 ;

	m_strFilePathName=cszFileName;

	return OpenFile(cszFileName);
}


void AudioFile::Close(void)
{
	CloseFile();
}

int AudioFile::OpenFile(const char*cszPathName)
{
	HRESULT hr=S_OK;

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
			return -1;
		}
	}

	CComPtr<IPin> pAudioPin=GetPin(m_pLAVSplitter,L"Audio");
	if (pAudioPin)
	{
		NEWRECORD( m_pLAVAudio ,  new CLAVAudio(NULL, &hr) )
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(m_pLAVAudio,L"LAV Audio Decoder");
		if (hr!=S_OK)
		{
			return -1;
		}

		CComPtr<IBaseFilter> pAudioRender;
		{
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

			hr=pAudioRender.CoCreateInstance(CLSID_DSoundRender);
		}
		if (hr!=S_OK)
		{
			return -1;
		}
		hr=m_pGraph->AddFilter(pAudioRender,L"Audio Renerer");
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

				hr=m_pGraph->Connect(m_pAudioSampleGrabber->GetPin(1),GetPin(pAudioRender,L"Audio Input pin (rendered)"));
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
				hr=m_pGraph->Connect(m_pAudioSampleGrabber->GetPin(1),GetPin(pAudioRender,L"Audio Input pin (rendered)"));
				if (hr!=S_OK)
				{
					return -1;
				}
			}
		}
	}

	Play();

	return 0;
}


void AudioFile::CloseFile(void)
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
		delete m_pLAVSplitter;
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

	SAFE_RELEASE(m_pAudioSampleGrabber);
}


unsigned long AudioFile::GetDuration(void)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	double ulDuration=0;
	if (pMediaPosition==NULL)
		return (unsigned long)ulDuration;

	HRESULT hr=pMediaPosition->get_Duration(&ulDuration);

	return (unsigned long)ulDuration;
}

unsigned long AudioFile::GetPosition(void)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	double ulCurrentPosition=0;
	if (pMediaPosition==NULL)
		return (unsigned long)ulCurrentPosition;

	HRESULT hr=pMediaPosition->get_CurrentPosition(&ulCurrentPosition);

	return (unsigned long)ulCurrentPosition;
}

int AudioFile::Seek(unsigned long ulSeconds)
{
	CComQIPtr<IMediaPosition,&IID_IMediaPosition> pMediaPosition(m_pGraph);

	if (pMediaPosition==NULL)
		return -1;

	pMediaPosition->put_CurrentPosition(ulSeconds);

	return 0;
}

int AudioFile::Pause(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	if (pMediaControl!=NULL)
	{
		pMediaControl -> Pause ();
	}

	return 0;
}

int AudioFile::Play(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	HRESULT hr;
	if (pMediaControl!=NULL)
	{
		hr=pMediaControl -> Run ();
	}

	return 0;
}


int AudioFile::SetVolume(int nVolume)
{
	if (m_pGraph!=NULL)
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

void AudioFile::OnAudioSampleGrabberCallback(unsigned char*pData,int nLen,int nSampleRate,double dblSampleTime)
{
	m_rCallback.OnAVCONAVC_AudioFileCallbackPCMData(this,pData,nLen,nSampleRate,timeGetTime());
}
