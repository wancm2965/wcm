#include "stdafx.h"
#include <WtsApi32.h>
#include "MediaFileCapture.h"
#include <math.h>
#include "XUtil.h"
#include "../VIDEC_API/XImage.h"
#include "../VIDEC_csp/fastmemcpy.h"
#include "VIDEC_csp.h"


VIDEC_MediaFile*VIDEC_MediaFile::Create(VIDEC_MediaFileCallback&rCallback)
{
	//return new MediaFileCapture(rCallback);
	NEWRECORDret3(  VIDEC_MediaFile* ,p , new MediaFileCapture( rCallback )  )   
}

IMPLEMENT_DYNAMIC(MediaFileCapture, CWnd)

MediaFileCapture::MediaFileCapture(VIDEC_MediaFileCallback&rCallback)
:XImageSource(this)
,m_rCallback(rCallback)
,m_pMediaFile(NULL)
,m_nPlayState(PLAY_STATE_STOP)

,m_pIXResample(NULL)
,m_nSampleRateResample(32000)
,m_pResampleBuf(NULL)
,m_nResampleBufLen(0)
{
	m_bMute=false;
	m_nVolume=100;

	m_bLoop=true;

	m_bPlaying=false;


	m_nLastPlayPosition=0;
	m_bSeeking=false;
	m_nLastCapturedFrameTimestamp=0;
	m_strFilePathName="";

	m_hParentWnd=NULL;

	m_nDetectingError=0;
}

MediaFileCapture::~MediaFileCapture(void)
{
}


BEGIN_MESSAGE_MAP(MediaFileCapture, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void MediaFileCapture::OnTimer(UINT_PTR nIDEvent)
{
	CWnd::OnTimer(nIDEvent);

	switch (nIDEvent)
	{
	case 500:
		{
			unsigned long ulDuration=GetDuration();
			unsigned long ulPosition=GetPosition();
			m_rCallback.OnVIDEC_MediaFileCallbackPositionChanged(this,ulPosition);
			
			if (IsPlaying() && m_nLastCapturedFrameTimestamp==0 && m_nDetectingError>0 && ulPosition==0)
			{
				m_nDetectingError--;
				if (m_nDetectingError<=0)
				{
					if (m_pMediaFile)
					{
						m_pMediaFile->Close();
						delete m_pMediaFile;
						m_pMediaFile=NULL;
					}
					
					m_pMediaFile=AVCONAVC_MediaFile::Create(*this,false,true);
					if (m_pMediaFile)
					{
						bool bTryingEVR=false;
						if (0!=m_pMediaFile->Open(NULL,m_strFilePathName.c_str(),bTryingEVR))
						{
							m_pMediaFile->Close();
							delete m_pMediaFile;
							m_pMediaFile=NULL;
						}
						else
						{
							m_nPlayState=PLAY_STATE_STOP;
							Play();
							::PostMessage(m_hParentWnd,WM_SIZE,0,0);
						}
					}
				}
			}

			m_nLastPlayPosition=ulPosition;

			if (ulPosition>=(ulDuration-1))
			{
				if (m_bLoop)
				{
					if (m_pMediaFile)
					{
						m_pMediaFile->Seek(0);
						if (m_nPlayState==PLAY_STATE_PLAY)
						{
							m_pMediaFile->Play();
						}
					}
				}
				else
				{
					Pause();
					Seek(0);
					m_rCallback.OnVIDEC_MediaFileCallbackEnd(this);
				}
			}
		}
		break;
	case 2008:
		{
			m_bSeeking=false;
			KillTimer(nIDEvent);
			if (m_bPlaying)
			{
				m_bPlaying=false;
				if (m_pMediaFile)
				{
					m_pMediaFile -> Play ();
					m_nPlayState=PLAY_STATE_PLAY;
				}
			}
		}
		break;
	}
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


int MediaFileCapture::Open(HWND hParentWnd,const char*cszPathName,bool bLoop)
{
	m_hParentWnd=hParentWnd;
	if (!CreateEx(0,AfxRegisterWndClass(CS_CLASSDC), _T("MediaFileCapture"),WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_DISABLED ,-1,-1,1,1,hParentWnd,NULL))
	{
		return -1;
	}

	m_bLoop=bLoop;
	if( cszPathName==NULL )
		return -1 ;

	m_strFilePathName=cszPathName;

	if (m_strFilePathName.size()>5)
	{
		std::string strFile=m_strFilePathName;
		strFile=strFile.substr(strFile.size()-5,5);
		if (caseInsensitiveStringCompare(strFile,".avms"))
		{
			NEWRECORD( m_pMediaFile,new AVMSMediaFile(*this) )
			//NULLReturnRet( m_pMediaFile , -1 )  
			if (m_pMediaFile==NULL)
			{
				return -1;
			}

			bool bTryingEVR=false;
			if (0!=m_pMediaFile->Open(m_hWnd,cszPathName,bTryingEVR))
			{
				m_pMediaFile->Close();
				delete m_pMediaFile;
				m_pMediaFile=NULL;

				return -1;
			}

			if (HasAudio()==false && HasVideo()==false)
			{
				return -1;
			}

			Play();

			SetTimer(500,500,NULL);

			::PostMessage(m_hParentWnd,WM_SIZE,0,0);

			return 0;
		}
	}


	return OpenFile(cszPathName);
}


void MediaFileCapture::Close(void)
{
	XImageSource::Close();

	CloseFile();

	DestroyWindow();

	if (m_pIXResample!=NULL)
	{
		m_pIXResample->ReleaseConnections();
		delete m_pIXResample;
		m_pIXResample=NULL;
	}

	if (m_pResampleBuf!=NULL)
	{
		FREEMSG(m_pResampleBuf);
		m_pResampleBuf=NULL;
	}
}

int MediaFileCapture::OpenFile(const char*cszPathName)
{
	m_nDetectingError=10;
	m_pMediaFile=AVCONAVC_MediaFile::Create(*this);
	if (m_pMediaFile==NULL)
	{
		return -1;
	}

	bool bTryingEVR=false;
	if (0!=m_pMediaFile->Open(NULL,cszPathName,bTryingEVR))
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;

		return -1;
	}

	if (HasAudio()==false && HasVideo()==false)
	{
		return -1;
	}

	Play();

	SetTimer(500,500,NULL);

	::PostMessage(m_hParentWnd,WM_SIZE,0,0);

	return 0;
}

void MediaFileCapture::CloseFile(void)
{
	KillTimer(500);

	if (m_pMediaFile)
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;
	}


	m_nPlayState=PLAY_STATE_STOP;
}


unsigned long MediaFileCapture::GetDuration(void)
{
	if (m_pMediaFile==NULL)
		return 0;

	return m_pMediaFile->GetDuration();
}

unsigned long MediaFileCapture::GetPosition(void)
{
	if (m_pMediaFile==NULL)
		return 0;

	return m_pMediaFile->GetPosition();
}

int MediaFileCapture::Seek(unsigned long ulSeconds)
{
	if (m_pMediaFile==NULL)
		return -1;

	if (m_nPlayState==PLAY_STATE_PLAY)
	{
		if (m_pMediaFile)
		{
			m_pMediaFile -> Pause ();
		}

		m_bSeeking=true;
		m_bPlaying=true;
	}

	m_pMediaFile->Seek(ulSeconds);

	if (m_bPlaying)
	{
		SetTimer(2008,2008,NULL);
	}

	return 0;
}

int MediaFileCapture::Pause(void)
{
	if (m_pMediaFile!=NULL && m_nPlayState!=PLAY_STATE_PAUSE)
	{
		m_nPlayState=PLAY_STATE_PAUSE;

		m_pMediaFile -> Pause ();

		KillTimer(2008);

		m_bSeeking=false;
		m_bPlaying=false;
	}

	return 0;
}

int MediaFileCapture::Play(void)
{
	if (m_pMediaFile!=NULL && m_nPlayState!=PLAY_STATE_PLAY)
	{
		m_nPlayState=PLAY_STATE_PLAY;

		m_pMediaFile -> Play ();
	}

	return 0;
}

bool MediaFileCapture::IsPlaying(void)
{
	return m_nPlayState==PLAY_STATE_PLAY;
}

bool MediaFileCapture::HasAudio(void)
{
	if (m_pMediaFile==NULL)
	{
		return false;
	}
	return  m_pMediaFile->HasAudio();
}

int MediaFileCapture::SetVolume(int nVolume)
{
	if (HasAudio() && m_pMediaFile!=NULL)
	{
		m_pMediaFile->SetVolume(nVolume);
	}

	return 0;
}

int MediaFileCapture::SetMute(int nMute)
{
	m_bMute=nMute!=0;
	if (m_bMute)
	{
		int nVolume=m_nVolume;
		SetVolume(1);
		m_nVolume=nVolume;
	}
	else
	{
		SetVolume(m_nVolume);
	}

	return 0;
}


bool MediaFileCapture::HasVideo(void)
{
	if (m_pMediaFile==NULL)
		return false;

	return m_pMediaFile->HasVideo();
}

int MediaFileCapture::GetWidth(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetVideoWidth();
	}

	return 0;
}

int MediaFileCapture::GetHeight(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetVideoHeight();
	}

	return 0;
}


void MediaFileCapture::OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)
{
	if (m_nPlayState!=PLAY_STATE_PLAY)
	{
		return ;
	}

	m_nLastCapturedFrameTimestamp=XGetTimestamp();
	if (nSampleRate!=m_nSampleRateResample)
	{
		if (m_pIXResample==NULL)
		{
			m_pIXResample=CreateIXResample();
			if (m_pIXResample!=NULL)
			{
				if (0!=m_pIXResample->Connect(nSampleRate,m_nSampleRateResample))
				{
					m_pIXResample->ReleaseConnections();
					delete m_pIXResample;
					m_pIXResample=NULL;
				}
			}
		}

		if (m_pIXResample!=NULL)
		{
			int nResampleBufLen=(nLen*m_nSampleRateResample/nSampleRate)*2;
			if (nResampleBufLen>m_nResampleBufLen || m_pResampleBuf==NULL)
			{
				m_nResampleBufLen=nResampleBufLen;
				if (m_pResampleBuf!=NULL)
				{
					FREEMSG(m_pResampleBuf);
					m_pResampleBuf=NULL;
				}

				m_pResampleBuf=(unsigned char*)MALLOCMSG(m_nResampleBufLen);
			}

			if (m_pResampleBuf!=NULL)
			{
				short*pOutSamples=(short*)m_pResampleBuf;
				int nOutSamples=0;
				if(0==m_pIXResample->Convert((short*)pPCMData,nLen>>1,pOutSamples,nOutSamples))
				{
					pPCMData=(unsigned char*)pOutSamples;
					nLen=(nOutSamples<<1);
					nSampleRate=m_nSampleRateResample;
				}
			}
		}
	}
	m_rCallback.OnVIDEC_MediaFileCallbackPCMData(this,(unsigned char*)pPCMData,nLen,XGetTimestamp());
}

void MediaFileCapture::OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen)
{
	*ppBuffer=(unsigned char*)MALLOCMSG(nBufferLen+nBufferLen/4);
}

void MediaFileCapture::OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp)
{
	m_nLastCapturedFrameTimestamp=XGetTimestamp();


	NEWRECORD3( XImage*,pXImage,new XImage(pData,nLen) )
	NULLReturn( pXImage )  
	pXImage->AddRef();

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(nHeight);
	pXImage->SetVirtualWidth(nWidth);
	pXImage->SetVirtualHeight(nHeight);
	pXImage->SetDoubleField(false);
	pXImage->SetEvenField(false);
	pXImage->SetOddField(false);
	pXImage->SetTimestamp(XGetTimestamp());
	pXImage->SetFrameRate(nFrameRate);

	if (pXImage!=NULL)
	{
		SendImage(pXImage);

		pXImage->Release();
		pXImage=NULL;
	}
}

void MediaFileCapture::OnAVMSMediaFileCallbackFrame(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage)
	{
		pXImage->SetTimestamp(XGetTimestamp());
	}

	SendImage(pImage);
}

int MediaFileCapture::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void MediaFileCapture::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void MediaFileCapture::EnableOSD(bool bEnable)
{
}

bool MediaFileCapture::IsOSDEnable(void)
{
	return false;
}

void MediaFileCapture::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
}

void MediaFileCapture::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
}

