//SDDevCap.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDDevCap.h"
#include "SDDevMgr.h"
#include "VIDEC_CameraDev.h"

SDDevCap::SDDevCap(VIDEC_SDDevCapCallback&rCallback)
:m_rCallback(rCallback)

,m_nDevIndex(-1)

,m_nFrameRate(15)
,m_nBitrate(0)
,m_nQuant(0)
,m_nQMin(0)
,m_nQMax(0)

,m_CodecType(VIDEC_CODEC_H264)
{

}

SDDevCap::~SDDevCap(void)
{

}

int SDDevCap::Open(int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,int nQuality,bool bCBR,VIDEC_CODEC_TYPE CodecType)
{
	if (nDevIndex<0)
		return -1;
	if (nDevIndex>=VIDEC_CameraDev::GetDevCount())
		return -1;

	m_nDevIndex=nDevIndex;

	if (nFrameRate<1 || nFrameRate>30)
		nFrameRate=15;

	m_nFrameRate=nFrameRate;

	if (CodecType!=VIDEC_CODEC_H264 && 
		CodecType!=VIDEC_CODEC_H263P &&
		CodecType!=VIDEC_CODEC_H263PP)
	{
		CodecType=(VIDEC_CODEC_H264);
	}
	m_CodecType=CodecType;

	if (nQuality<0 || nQuality>5)
		nQuality=2;

	if (m_CodecType==VIDEC_CODEC_H264)
	{
		m_nQMin=1;
		m_nQMax=51;
	}
	else
	{
		m_nQMin=1;
		m_nQMax=31;
	}

	switch (nQuality)
	{
	case 0:
		if (bCBR)
		{
			m_nBitrate=64;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=36;
			}
			else
			{
				m_nQuant=16;
			}
		}
		break;
	case 1:
		if (bCBR)
		{
			m_nBitrate=128;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=32;
			}
			else
			{
				m_nQuant=12;
			}
		}
		break;
	case 2:
		if (bCBR)
		{
			m_nBitrate=384;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=28;
			}
			else
			{
				m_nQuant=8;
			}
		}
		break;
	case 3:
		if (bCBR)
		{
			m_nBitrate=768;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=24;
			}
			else
			{
				m_nQuant=6;
			}
		}
		break;
	case 4:
		if (bCBR)
		{
			m_nBitrate=2048;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=20;
			}
			else
			{
				m_nQuant=3;
			}
		}
		break;
	case 5:
		if (bCBR)
		{
			m_nBitrate=8192;
		}
		else
		{
			if (m_CodecType==VIDEC_CODEC_H264)
			{
				m_nQuant=8;
			}
			else
			{
				m_nQuant=1;
			}
		}
		break;
	};


	if (0!=SDDevMgr::SetDevCap(m_nDevIndex,nWidth,nHeight,m_nFrameRate,bAutoFrameRate))
		return -1;

	if (0!=SDDevMgr::AddOutputImage(m_nDevIndex,this))
		return -1;

	if (0!=SDDevMgr::AddOutputStream(m_nDevIndex,this))
		return -1;

	return 0;
}

void SDDevCap::Close(void)
{
	if (m_nDevIndex>=0)
	{
		SDDevMgr::RemoveOutputImage(m_nDevIndex,this);
		SDDevMgr::RemoveOutputStream(m_nDevIndex,this);

		m_nDevIndex=-1;
	}
}

void SDDevCap::OnVIDEC_OutputImageOutputData(void*pData,int nLen,int nWidth,int nHeight)
{
	m_rCallback.OnVIDEC_SDDevCapCallbackImageData((unsigned char*)pData,nLen,nWidth,nHeight);
}

int SDDevCap::OnVIDEC_OutputImageGetFrameRate(void)
{
	return m_nFrameRate;
}


void SDDevCap::OnVIDEC_OutputStreamOutputData(void*pData,int nLen)
{
	m_rCallback.OnVIDEC_SDDevCapCallbackPacketData((unsigned char*)pData,nLen);
}

int SDDevCap::OnVIDEC_OutputStreamGetBitrate(void)
{
	return m_nBitrate;
}

int SDDevCap::OnVIDEC_OutputStreamGetMaxBitrate(void)
{
	return m_nBitrate;
}

int SDDevCap::OnVIDEC_OutputStreamGetFrameRate(void)
{
	return m_nFrameRate;
}

int SDDevCap::OnVIDEC_OutputStreamGetQuant(void)
{
	return m_nQuant;
}

int SDDevCap::OnVIDEC_OutputStreamGetQMin(void)
{
	return m_nQMin;
}

int SDDevCap::OnVIDEC_OutputStreamGetQMax(void)
{
	return m_nQMax;
}


VIDEC_CODEC_TYPE SDDevCap::OnVIDEC_OutputStreamGetCodecType(void)
{
	return m_CodecType;
}


//取设备个数
int VIDEC_CameraDev::GetDevCount(void)
{
	return SDDevMgr::GetDevCount();
}

//取设备名称
int VIDEC_CameraDev::GetDevName(unsigned int nDevIndex,char*szName)
{
	return SDDevMgr::GetDevName(nDevIndex,szName);
}


//列出可裁剪的格式的个数
int VIDEC_CameraDev::GetFormatCount(void)
{
	return SDDevMgr::GetFormatCount();
}

//取格式名称
int VIDEC_CameraDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return SDDevMgr::GetFormatName(nIndex,szName,nMaxCount);
}

//取格式大小
int VIDEC_CameraDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return SDDevMgr::GetFormatSize(nIndex,nWidth,nHeight);
}

VIDEC_SDDevCap*VIDEC_SDDevCap::Create(VIDEC_SDDevCapCallback&rCallback)
{
	return new SDDevCap(rCallback);
}

