#include "stdafx.h"
#include "AudioFileCapture.h"
#include <math.h>
#include "XUtil.h"


VIDEC_AudioFile*VIDEC_AudioFile::Create(VIDEC_AudioFileCallback&rCallback)
{
	//return new AudioFileCapture(rCallback);
	NEWRECORDret3(  VIDEC_AudioFile* ,p , new AudioFileCapture( rCallback )  )   
}

IMPLEMENT_DYNAMIC(AudioFileCapture, CWnd)

AudioFileCapture::AudioFileCapture(VIDEC_AudioFileCallback&rCallback)
:m_rCallback(rCallback)
,m_pAudioFile(NULL)
,m_nPlayState(PLAY_STATE_STOP)
{
	m_bMute=false;
	m_nVolume=100;

	m_bLoop=true;

	m_bPlaying=false;


	m_bSeeking=false;
	m_strFilePathName="";

	m_nSampleTimestamp=0;
	m_pLeftSampleBuffer=NULL;
	m_nLeftSampleBufferLen=0;
	m_nLeftSampleDataLen=0;
	m_nSampleRate=0;
	m_nFrameLen=0;

	m_nEndCount=0;
}

AudioFileCapture::~AudioFileCapture(void)
{
}


BEGIN_MESSAGE_MAP(AudioFileCapture, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void AudioFileCapture::OnTimer(UINT_PTR nIDEvent)
{
	CWnd::OnTimer(nIDEvent);

	switch (nIDEvent)
	{
	case 500:
		{
			unsigned long ulDuration=GetDuration();
			unsigned long ulPosition=GetPosition();
			m_rCallback.OnVIDEC_AudioFileCallbackPositionChanged(this,ulPosition);
			

			if (ulPosition>=ulDuration)
			{
				m_nEndCount++;
				if (m_nEndCount>=4)
				{
					m_nEndCount=0;

					if (m_bLoop)
					{
						Seek(0);
					}
					else
					{
						Pause();
						Seek(0);
						m_rCallback.OnVIDEC_AudioFileCallbackEnd(this);
					}
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
				Play();
			}
		}
		break;
	}
}


int AudioFileCapture::Open(const char*cszPathName,bool bLoop)
{
	if (!CreateEx(WS_EX_CLIENTEDGE,AfxRegisterWndClass(CS_CLASSDC), _T("AudioFileCapture"),WS_POPUP,0,0,1,1,NULL,NULL))
	{
		return -1;
	}

	m_bLoop=bLoop;
	if( cszPathName==NULL )
		return -1 ;

	m_strFilePathName=cszPathName;

	m_nLeftSampleBufferLen=2560;
	m_pLeftSampleBuffer=(unsigned char*)MALLOCMSG(m_nLeftSampleBufferLen);
	if (m_pLeftSampleBuffer==NULL)
	{
		return -1;
	}

	if (0!=OpenFile(cszPathName))
	{
		return -1;
	}

	return XWorkThread::Start();
}


void AudioFileCapture::Close(void)
{
	CloseFile();

	XWorkThread::Stop();

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

	if (m_pLeftSampleBuffer)
	{
		FREEMSG(m_pLeftSampleBuffer);
		m_pLeftSampleBuffer=NULL;
	}

	DestroyWindow();
}

int AudioFileCapture::OpenFile(const char*cszPathName)
{
	m_pAudioFile=AVCONAVC_AudioFile::Create(*this);
	if (m_pAudioFile==NULL)
	{
		return -1;
	}

	if (0!=m_pAudioFile->Open(cszPathName))
	{
		m_pAudioFile->Close();
		delete m_pAudioFile;
		m_pAudioFile=NULL;

		return -1;
	}


	Play();

	SetTimer(500,500,NULL);

	return 0;
}

void AudioFileCapture::CloseFile(void)
{
	KillTimer(500);

	if (m_pAudioFile)
	{
		m_pAudioFile->Close();
		delete m_pAudioFile;
		m_pAudioFile=NULL;
	}


	m_nPlayState=PLAY_STATE_STOP;
}


unsigned long AudioFileCapture::GetDuration(void)
{
	if (m_pAudioFile==NULL)
		return 0;

	return m_pAudioFile->GetDuration();
}

unsigned long AudioFileCapture::GetPosition(void)
{
	if (m_pAudioFile==NULL)
		return 0;

	return m_pAudioFile->GetPosition();
}

int AudioFileCapture::Seek(unsigned long ulSeconds)
{
	if (m_pAudioFile==NULL)
		return -1;

	if (m_nPlayState==PLAY_STATE_PLAY)
	{
		Pause();

		m_bSeeking=true;
		m_bPlaying=true;
	}

	m_pAudioFile->Seek(ulSeconds);

	if (m_bPlaying)
	{
		SetTimer(2008,2008,NULL);
	}

	return 0;
}

int AudioFileCapture::Pause(void)
{
	if (m_pAudioFile!=NULL && m_nPlayState!=PLAY_STATE_PAUSE)
	{
		m_nPlayState=PLAY_STATE_PAUSE;

		m_pAudioFile -> Pause ();

		m_bPlaying=false;
	}

	return 0;
}

int AudioFileCapture::Play(void)
{
	if (m_pAudioFile!=NULL && m_nPlayState!=PLAY_STATE_PLAY)
	{
		m_nPlayState=PLAY_STATE_PLAY;

		m_pAudioFile -> Play ();
	}

	return 0;
}

bool AudioFileCapture::IsPlaying(void)
{
	return m_nPlayState==PLAY_STATE_PLAY;
}

int AudioFileCapture::SetVolume(int nVolume)
{
	if (m_pAudioFile!=NULL)
	{
		m_pAudioFile->SetVolume(nVolume);
	}

	return 0;
}

int AudioFileCapture::SetMute(int nMute)
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

void AudioFileCapture::OnAVCONAVC_AudioFileCallbackPCMData(AVCONAVC_AudioFile*pAudioFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)
{
	if (m_nSampleRate!=nSampleRate)
	{
		m_nSampleRate=nSampleRate;
		m_nFrameLen=m_nSampleRate/100*4;
	}

	NEWRECORD3( XSampleBuffer*,pXSampleBuffer,new XSampleBuffer() )
	//NULLRecord( pXSampleBuffer )  
	if (pXSampleBuffer!=NULL)
	{
		if (pXSampleBuffer->SetSampleBuffer(pPCMData,nLen,nSampleRate))
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
}

bool AudioFileCapture::IsPending(void)
{
	XAutoLock l(m_csListXSampleBuffer);
	return (m_ListXSampleBuffer.size()>0);
}

_inline void swap_bytes(unsigned short *buf, unsigned int count)
{
	unsigned short *end = buf + count;
	while (buf != end) 
	{
		*buf = (unsigned short)((*buf << 8) | (*buf >> 8));
		++buf;
	}
}

bool AudioFileCapture::Running(void)
{
	XAutoLock l(m_csListXSampleBuffer);
	while (m_ListXSampleBuffer.size())
	{
		XSampleBuffer*pXSampleBuffer=(XSampleBuffer*)m_ListXSampleBuffer.front();
		m_ListXSampleBuffer.pop_front();

		int nLeftDataLen=pXSampleBuffer->GetBufferLen();
		unsigned char*pLeftData=pXSampleBuffer->GetBuffer();
		if (m_nLeftSampleDataLen)
		{
			if (m_nLeftSampleDataLen+nLeftDataLen>=m_nFrameLen)
			{
				int nOffset=m_nFrameLen-m_nLeftSampleDataLen;
				memcpy(m_pLeftSampleBuffer+m_nLeftSampleDataLen,pLeftData,nOffset);

				swap_bytes((unsigned short*)m_pLeftSampleBuffer,m_nFrameLen>>1);

				m_rCallback.OnVIDEC_AudioFileCallbackPCMData(this,m_pLeftSampleBuffer,m_nFrameLen,m_nSampleRate,m_nSampleTimestamp);
				m_nLeftSampleDataLen=0;
				m_nSampleTimestamp+=20;

				pLeftData+=nOffset;
				nLeftDataLen-=nOffset;
			}
			else
			{
				memcpy(m_pLeftSampleBuffer+m_nLeftSampleDataLen,pLeftData,nLeftDataLen);
				m_nLeftSampleDataLen+=nLeftDataLen;
				nLeftDataLen=0;
			}
		}

		while (nLeftDataLen>=m_nFrameLen)
		{
			swap_bytes((unsigned short*)pLeftData,m_nFrameLen>>1);

			m_rCallback.OnVIDEC_AudioFileCallbackPCMData(this,pLeftData,m_nFrameLen,m_nSampleRate,m_nSampleTimestamp);
			m_nSampleTimestamp+=20;
			pLeftData+=m_nFrameLen;
			nLeftDataLen-=m_nFrameLen;
		}
		if (nLeftDataLen)
		{
			memcpy(m_pLeftSampleBuffer,pLeftData,nLeftDataLen);
			m_nLeftSampleDataLen=nLeftDataLen;
		}

		delete pXSampleBuffer;
		pXSampleBuffer=NULL;
	}

	return false;
}
