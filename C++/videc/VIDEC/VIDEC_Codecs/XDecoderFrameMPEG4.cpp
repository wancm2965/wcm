#include <stdlib.h>
#include <memory.h>

#include "XDecoderFrameMPEG4.h"

#include "VIDEC_Header.h"
#include "XUtil.h"


XDecoderFrameMPEG4::XDecoderFrameMPEG4(VIDEC_DecoderCallback&rCallback)
:XDecoderFrame(rCallback)
	
,m_pHandle(NULL)
,m_nLastFrameIsKeyFrame(0)
,m_bGotFirstKeyFrame(false)
,m_usSeqNo(0)
{
}

XDecoderFrameMPEG4::~XDecoderFrameMPEG4(void)
{
}

int XDecoderFrameMPEG4::Open(int nWidth,int nHeight)
{
	m_pHandle=AVCONAVC_DecoderOpen(nWidth,nHeight,AVC_CODEC_MPEG4);
	if (!m_pHandle)
	{
		return -1;
	}

	return 0;
}

void XDecoderFrameMPEG4::Close(void)
{
	if (m_pHandle!=NULL)
	{
		AVCONAVC_DecoderClose(m_pHandle);
		m_pHandle=NULL;
	}

	XDecoderFrame::Close();
}


int XDecoderFrameMPEG4::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (!m_pHandle)
		return -1;

	unsigned char*pFrameData[3];
	int nLineSize[3];
	char*qscale_table=NULL;
	int qstride=0;
	int nGotPicture=0;
	int pix_fmt = -1;

	int nUsedBytes=0;
	int nWidth=0;
	int nHeight=0;
	int nMPEG4DeblockingFilter=0;

	int nLeftLen=nPacketSize;
	unsigned char*pLeftData=(unsigned char*)pPacketData;
	while (nLeftLen>0)
	{
		try
		{
			nGotPicture=AVCONAVC_DecoderDecode(m_pHandle,(unsigned char*)pLeftData,nLeftLen,pFrameData,nLineSize,&nWidth,&nHeight,&nUsedBytes,&m_nLastFrameIsKeyFrame,&qscale_table,&qstride,&pix_fmt);

			if (!m_bGotFirstKeyFrame)
			{
				if (m_nLastFrameIsKeyFrame)
				{
					m_bGotFirstKeyFrame=true;
				}
			}
		}
		catch(...)
		{
			return -1;
		}

		if (nUsedBytes<=0)
		{
			return -1;
		}

		nLeftLen-=nUsedBytes;
		pLeftData+=nUsedBytes;

		if (nGotPicture>0 && m_bGotFirstKeyFrame)
		{
			if (m_nWidth!=nWidth ||
				m_nHeight!=nHeight)
			{
				if (nWidth==720 && nHeight==576)
				{
					m_bDoubleField=true;
				}
				else
				{
					m_bDoubleField=false;
				}

				if ((nWidth==176 && nHeight==144) ||
					(nWidth==320 && nHeight==240) ||
					(nWidth==352 && nHeight==288) ||
					(nWidth==352 && nHeight==240) ||
					(nWidth==640 && nHeight==480) ||
					(nWidth==704 && nHeight==240) ||
					(nWidth==704 && nHeight==288) ||
					(nWidth==704 && nHeight==576) ||
					(nWidth==704 && nHeight==480) ||
					(nWidth==720 && nHeight==240) ||
					(nWidth==720 && nHeight==288) ||
					(nWidth==720 && nHeight==576) ||
					(nWidth==720 && nHeight==480) ||
					(nWidth==1280 && nHeight==720) ||
					(nWidth==1920 && nHeight==1080))
				{

				}
				else
				{
					m_bGotFirstKeyFrame=false;
					return -1;
				}

				OnResized(nWidth,nHeight);
			}
			PostProcess(pFrameData,nLineSize,qscale_table,qstride);
		}
	}


	return 0;
}

int XDecoderFrameMPEG4::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	return 0;
}
