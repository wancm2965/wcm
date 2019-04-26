#include "DVCCap.h"
#include "XUtil.h"
#include <VIDEC/VIDEC_Header.h>


VIDEC_DVC*VIDEC_DVC::Create(VIDEC_DVCCallback&rCallback)
{
	//return new DVCCap(rCallback);
	NEWRECORDret3(  VIDEC_DVC* ,p , new DVCCap(rCallback)  )    
}


void DVC_VideoCallback(const void*pUserData,unsigned char*pFrameData,int nFrameLen)
{
	DVCCap*pDVCCap=(DVCCap*)pUserData;
	if (pDVCCap)
	{
		pDVCCap->OnVideoCallback(pFrameData,nFrameLen);
	}

}

DVCCap::DVCCap(VIDEC_DVCCallback&rCallback)
:m_rCallback(rCallback)
,m_pHandle(NULL)
,m_pDecoder(NULL)
{
}

DVCCap::~DVCCap(void)
{

}

int DVCCap::Open(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	m_pDecoder=VIDEC_DecoderStreamCreate(*this);
	if (m_pDecoder==NULL)
	{
		return -1;
	}

	m_pHandle=DVCOpen(nDevID,DVC_VideoCallback,this);
	if (m_pHandle==NULL)
	{
		return -1;
	}

	DVCSetParams(m_pHandle,nFrameRate,nBitrate,nBitrateControlType,nResolution);

	return 0;
}

void DVCCap::Close(void)
{
	if (m_pHandle)
	{
		DVCClose(m_pHandle);

		m_pHandle=NULL;
	}

	if (m_pDecoder != NULL)
	{
		m_pDecoder->Close();
		delete m_pDecoder;
		m_pDecoder = NULL;
	}
}

int DVCCap::RequestKeyFrame(void)
{
	if (m_pHandle)
	{
		DVCRequestKeyFrame(m_pHandle);
	}
	return 0;
}

int DVCCap::SetParams(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	if (m_pHandle)
	{
		DVCSetParams(m_pHandle,nFrameRate,nBitrate,nBitrateControlType,nResolution);
	}
	return 0;
}

int DVCCap::SendData(const char*pData,int nLen)
{
	if (m_pHandle)
	{
		DVCSendData(m_pHandle,pData,nLen);
	}
	return 0;
}

void DVCCap::OnVideoCallback(unsigned char*pFrameData,int nFrameLen)
{
	VIDEC_HEADER_EXT_SET_TIMESTAMP(pFrameData,XGetTimestamp());

	if (m_pDecoder)
	{
		m_pDecoder->DecodeFramePacket(pFrameData,nFrameLen);
	}

	m_rCallback.OnVIDEC_DVCCallbackVideoStreamData(pFrameData,nFrameLen,VIDEC_HEADER_EXT_GET_KEYFRAME(pFrameData),VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pFrameData),VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pFrameData));
}

void DVCCap::OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	m_rCallback.OnVIDEC_DVCCallbackImage(pImage);
}


int VIDEC_DVC::Init(void)
{
	DVC_init();

	return 0;
}

void VIDEC_DVC::Terminate(void)
{
	DVC_terminate();
}

int VIDEC_DVC::GetDevCount(void)
{
	return DVCGetDevCount();
}

int VIDEC_DVC::GetDevName(int nDevID,char*szName,int nMaxCount)
{
	return DVCGetDevName(nDevID,szName,nMaxCount);
}

int VIDEC_DVC::GetDevType(int nDevID,VIDEC_DEV_TYPE&nDevType)
{
	nDevType=VIDEC_DEV_SD;

	return 0;
}

bool VIDEC_DVC::HasAudioInput(int nDevID)
{
	return 0;
}

bool VIDEC_DVC::SupportHardwareEncoding(int nDevID)
{
	return true;
}
