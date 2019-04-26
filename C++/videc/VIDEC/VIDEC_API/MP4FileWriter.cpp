//
// MP4FileWriter
//
#include "MP4FileWriter.h"
#include <VIDEC/VIDEC_Header.h>
#include <stdio.h>
#include <stdlib.h>
#include "../VIDECMP4/VIDEC_MP4.h"


#ifdef _WIN64

#pragma comment(lib,"VIDECMP4.lib")
#pragma comment(lib,"HPATC.lib")

#else
#pragma comment(lib,"../VIDECMP4/VIDECMP4.lib")
#pragma comment(lib,"../VIDECMP4/HPATC.lib")
#endif


MP4FileWriter::MP4FileWriter(void) 
:m_pMP4Handle(NULL)
,m_bOpenVideoStream(false)
,m_bOpenAudioStream(false)

, m_nSampleRate(0)

, m_nWidth(0)
, m_nHeight(0)

, m_nVideoType(VIDEO_H264)

, m_strPathName("")
{
	m_pPCM2AAC=NULL;
}

MP4FileWriter::~MP4FileWriter(void)
{
}

int MP4FileWriter::Open(const char* cszPathName)
{
	if (m_pMP4Handle!=NULL)
	{
		return -1;
	}

	std::string strPathName=cszPathName;
	strPathName=strPathName.substr(0,strPathName.length()-3);
	strPathName+="mp4";

	::DeleteFile( strPathName.c_str() );

	m_pMP4Handle=VIDEC_MP4_Open(strPathName.c_str());
	if (m_pMP4Handle==NULL)
	{
		return -1;
	}

	m_strPathName=strPathName;

	return	0;
}

void	MP4FileWriter::Close()
{
	void* pMP4Handle = NULL;
	HPATC_Transcode* pPCM2AAC=NULL;
	{
		XAutoLock l(m_csMP4Handle);
		pMP4Handle=m_pMP4Handle;
		m_pMP4Handle=NULL;

		pPCM2AAC = m_pPCM2AAC;
		m_pPCM2AAC = NULL;
	}

	if (pPCM2AAC)
	{
		pPCM2AAC->Close();
		delete pPCM2AAC;
		pPCM2AAC=NULL;
	}

	if (pMP4Handle!=NULL)
	{
		VIDEC_MP4_Close(pMP4Handle);
		pMP4Handle=NULL;
	}

	if (m_strPathName.size()>0)
	{
		FILE*pFile=fopen(m_strPathName.c_str(),"rb");
		if (pFile!=NULL)
		{
			fseek(pFile,0L,SEEK_END);
			long nLen=ftell(pFile);
			fclose(pFile);
			pFile=NULL;
			if (nLen<=32*1024)
			{
				::DeleteFile( m_strPathName.c_str() );
			}
		}
	}
}

bool MP4FileWriter::WriteVideo( unsigned char* pVideoData, int nVideoLen )
{
	XAutoLock l(m_csMP4Handle);

	unsigned short nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pVideoData);
	unsigned long ulTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pVideoData);
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pVideoData);

	if (m_pMP4Handle)
	{
		VIDEC_MP4_WriteVideo(m_pMP4Handle,pVideoData+nHeaderLen,nVideoLen-nHeaderLen,ulTimestamp,nKeyFrame,m_nWidth,m_nHeight,(VIDEO_TYPE)m_nVideoType,0);
	}

	return 0;
}



bool MP4FileWriter::WriteAudio( unsigned char* pData, const int nLen ,unsigned long ulTimestamp,int&nBytes)
{
	if (!m_bOpenVideoStream)
	{
		return false;
	}

	{
		XAutoLock l(m_csMP4Handle);
		if (m_pMP4Handle==NULL)
			return false;

		if (m_pPCM2AAC==NULL)
		{
			return	false;
		}
	}

	short*pSamples=(short*)pData;
	int nSamples=(nLen>>1);

	m_pPCM2AAC->TranscodePCMData(pSamples,nSamples,m_nSampleRate,ulTimestamp);

	nBytes=nLen;

	return	true;
}


int MP4FileWriter::OpenAudioStream(int nSampleRate)
{
	{
		XAutoLock l(m_csMP4Handle);
		if (m_pMP4Handle==NULL)
		{
			return -1;
		}
		if (m_pPCM2AAC)
		{
			return -1;
		}
	}

	m_nSampleRate = nSampleRate;

	if (m_pPCM2AAC==NULL)
	{
		m_pPCM2AAC=HPATC_Transcode::Create(this);
		if (m_pPCM2AAC==NULL)
		{
			return -1;
		}
		if (0!=m_pPCM2AAC->Open())
		{
			m_pPCM2AAC->Close();
			delete m_pPCM2AAC;
			m_pPCM2AAC=NULL;
			return -1;
		}
	}

	m_bOpenAudioStream=true;

	return 0;
}

int MP4FileWriter::OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC)
{
	{
		XAutoLock l(m_csMP4Handle);
		if (m_pMP4Handle==NULL)
		{
			return -1;
		}
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_nVideoType=VIDEO_H264;
	if (FourCC==mmioFOURCC('H','2','6','4') || FourCC==mmioFOURCC('A','V','C','1'))
	{
		m_nVideoType=VIDEO_H264;
	}
	else if (FourCC==mmioFOURCC('H','2','6','3'))
	{
		m_nVideoType=VIDEO_H263;
	}

	if (m_bOpenAudioStream==false)
	{
		m_bOpenAudioStream=true;
	}

	m_bOpenVideoStream=true;

	return 0;
}

void MP4FileWriter::OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*‘≠“Ù∆µ±‡¬Î ±º‰*/)
{
	XAutoLock l(m_csMP4Handle);

	if (m_pMP4Handle)
	{
		VIDEC_MP4_WriteAudio(m_pMP4Handle,pPacketData,nPacketLen,ulTimestamp,0);
	}
}
