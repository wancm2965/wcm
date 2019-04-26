#include "IntelVideoDecoder.h"
#include "IntelCodecWrapper.h"

void IntelDecoderCB(const void*pUserData,unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride)
{
	IntelVideoDecoder*pIntelVideoDecoder=(IntelVideoDecoder*)pUserData;
	if (pIntelVideoDecoder)
	{
		pIntelVideoDecoder->OnVideoDecoderCB(pData,nLen,nWidth,nHeight,nStride);
	}
}


IntelVideoDecoder::IntelVideoDecoder(HWVideoDecoderCallback&rCallback)
:m_rCallback(rCallback)
,m_pDecoder(NULL)
{
	m_nWidth=0;
	m_nHeight=0;
}

IntelVideoDecoder::~IntelVideoDecoder(void)
{

}


int IntelVideoDecoder::Open(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_pDecoder=IntelDecoderOpen(nWidth,nHeight,IntelDecoderCB,this);
	if (m_pDecoder==NULL)
	{
		return -1;
	}

	return 0;
}

void IntelVideoDecoder::Close(void)
{
	if (m_pDecoder)
	{
		IntelDecoderClose(m_pDecoder);
		m_pDecoder=NULL;
	}
}

int IntelVideoDecoder::Decode(const unsigned char *buffer, int buflen)
{
	if (m_pDecoder)
	{
		return IntelDecoderDecode(m_pDecoder,buffer,buflen);
	}
	return -1;
}

int IntelVideoDecoder::GetWidth(void)
{
	return m_nWidth;
}

int IntelVideoDecoder::GetHeight(void)
{
	return m_nHeight;
}

void IntelVideoDecoder::OnVideoDecoderCB(unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride)
{
	m_rCallback.OnHWVideoDecoderCallback(pData,nLen,nWidth,nHeight,nStride);
}