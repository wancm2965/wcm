#include "BackgroundMusic.h"
#include "AudioCapChanMgr.h"
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>

IMXEC_BackgroundMusic*IMXEC_BackgroundMusic::Create(IMXEC_BackgroundMusicCallback&rCallback)
{
	return AudioCapChanMgr::Instance().CreateBackgroundMusic(rCallback);
}


BackgroundMusic::BackgroundMusic(IMXEC_BackgroundMusicCallback&rCallback)
:m_rCallback(rCallback)
,m_pAudioFile(NULL)
,m_pPlayChan(NULL)
{
	m_pAudioBuffer=NULL;
	m_nAudioBufferLen=0;
	m_nSequence=0;
}

BackgroundMusic::~BackgroundMusic(void)
{

}

int BackgroundMusic::Open(const char*cszPathName,bool bLoop)
{
	AUDEC_GetEngine().AUDEC_CreatePlayChan(&m_pPlayChan);
	if (m_pPlayChan==NULL)
	{
		return -1;
	}
	if (0!=m_pPlayChan->Connect())
	{
		m_pPlayChan->ReleaseConnections();
		delete m_pPlayChan;
		m_pPlayChan=NULL;
		return -1;
	}

	m_pAudioFile=VIDEC_AudioFile::Create(*this);
	if (0!=m_pAudioFile->Open(cszPathName,bLoop))
	{
		m_pAudioFile->Close();
		delete m_pAudioFile;
		m_pAudioFile=NULL;

		return -1;
	}
	else
	{
		m_pAudioFile->SetVolume(0);
	}

	AudioCapChanMgr::Instance().OnBackgroundMusicOpen(this);

	return 0;
}

void BackgroundMusic::Close(void)
{
	AudioCapChanMgr::Instance().OnBackgroundMusicClose(this);

	if (m_pAudioFile)
	{
		m_pAudioFile->Close();
		delete m_pAudioFile;
		m_pAudioFile=NULL;
	}

	if (m_pPlayChan)
	{
		m_pPlayChan->ReleaseConnections();
		delete m_pPlayChan;
		m_pPlayChan=NULL;
	}

	if (m_pAudioBuffer)
	{
		free(m_pAudioBuffer);
		m_pAudioBuffer=NULL;
	}
}

unsigned long BackgroundMusic::GetDuration(void)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->GetDuration();
	}

	return 0;
}

unsigned long BackgroundMusic::GetPosition(void)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->GetPosition();
	}

	return 0;
}

int BackgroundMusic::Seek(unsigned long ulSeconds)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->Seek(ulSeconds);
	}

	return -1;
}

int BackgroundMusic::Pause(void)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->Pause();
	}

	return -1;
}

int BackgroundMusic::Play(void)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->Play();
	}

	return -1;
}

bool BackgroundMusic::IsPlaying(void)
{
	if (m_pAudioFile)
	{
		return m_pAudioFile->IsPlaying();
	}

	return false;
}

int BackgroundMusic::SetVolume(int nVolume)
{
	if (m_pPlayChan)
	{
		return m_pPlayChan->SetVolume(nVolume);
	}

	return -1;
}

int BackgroundMusic::SetMute(int nMute)
{
	if (m_pPlayChan)
	{
		return m_pPlayChan->SetMute(nMute);
	}

	return -1;
}

void BackgroundMusic::OnVIDEC_AudioFileCallbackPositionChanged(VIDEC_AudioFile*pAudioFile,unsigned long ulSeconds)
{
	m_rCallback.OnIMXEC_BackgroundMusicCallbackPositionChanged(ulSeconds);
}

void BackgroundMusic::OnVIDEC_AudioFileCallbackEnd(VIDEC_AudioFile*pAudioFile)
{
	m_rCallback.OnIMXEC_BackgroundMusicCallbackEnd();
}

void BackgroundMusic::OnVIDEC_AudioFileCallbackPCMData(VIDEC_AudioFile*pAudioFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long ulTimestamp)
{
	if (m_pPlayChan==NULL)
	{
		return ;
	}

	int nPacketLen=AUDEC_HEADER_VER_01_LEN+nLen;
	if (m_pAudioBuffer==NULL || m_nAudioBufferLen<nPacketLen)
	{
		if (m_pAudioBuffer)
		{
			free(m_pAudioBuffer);
			m_pAudioBuffer=NULL;
		}

		m_nAudioBufferLen=nPacketLen*2;
		m_pAudioBuffer=(unsigned char*)malloc(m_nAudioBufferLen);
		if (m_pAudioBuffer==NULL)
		{
			return ;
		}
	}

	X_AUDIO_CODEC_ID nCodecID=X_AUDIO_CODEC_L16_32KHZ;
	switch (nSampleRate)
	{
	case 8000:
		nCodecID=X_AUDIO_CODEC_L16_8KHZ;
		break;
	case 16000:
		nCodecID=X_AUDIO_CODEC_L16_16KHZ;
		break;
	case 32000:
		nCodecID=X_AUDIO_CODEC_L16_32KHZ;
		break;
	case 48000:
		nCodecID=X_AUDIO_CODEC_L16_48KHZ;
		break;
	default:
		return ;
	}


	AUDEC_HEADER_RESET(m_pAudioBuffer);
	AUDEC_HEADER_SET_SEQUENCE(m_pAudioBuffer,m_nSequence++);
	AUDEC_HEADER_SET_TIMESTAMP(m_pAudioBuffer,ulTimestamp);
	AUDEC_HEADER_SET_CODEC_ID(m_pAudioBuffer,nCodecID);

	memcpy(m_pAudioBuffer+AUDEC_HEADER_VER_01_LEN,pPCMData,nLen);
	m_pPlayChan->Write(m_pAudioBuffer,AUDEC_HEADER_VER_01_LEN+nLen);
}
