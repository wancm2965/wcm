#include <stdlib.h>
#include <memory.h>

#include "XDecoderFrameH263.h"

//#include "ravevc.h"
#include <VIDEC/VIDEC_Header.h>
#include "XUtil.h"
//#include "MPVIDEO_avc.h"
#include "avc.h"


XDecoderFrameH263::XDecoderFrameH263(MPVideo_DecoderCallback&rCallback,int nH263Plus)
:XDecoderFrame(rCallback)
	
,m_pHandle(NULL)
,m_pRTPFrame(NULL)

,m_nH263Plus(nH263Plus)

,m_nLastFrameIsKeyFrame(0)
,m_bGotFirstKeyFrame(false)
,m_usSeqNo(0)
{
}

XDecoderFrameH263::~XDecoderFrameH263(void)
{
}

int XDecoderFrameH263::Open(int nWidth,int nHeight)
{
	//m_nWidth=nWidth;
	//m_nHeight=nHeight;
	m_pHandle=avc_decoder_open(AVC_CODEC_H263,nWidth,nHeight);
	if (!m_pHandle)
	{
		return -1;
	}

	if (m_nH263Plus)
	{
		m_pRTPFrame=new H263PRTPFrame(*this);
	}
	else
	{
		m_pRTPFrame=new H263RTPFrame(*this);
	}
	m_pRTPFrame->Open(97,1400,VIDEC_HEADER_EXT_KEY_FRAME_LEN);

	return 0;
}

void XDecoderFrameH263::Close(void)
{
	if (m_pHandle!=NULL)
	{
		avc_decoder_close(m_pHandle);
		m_pHandle=NULL;
	}

	if (m_pRTPFrame!=NULL)
	{
		m_pRTPFrame->Close();
		delete m_pRTPFrame;
		m_pRTPFrame=NULL;
	}

	XDecoderFrame::Close();
}


int XDecoderFrameH263::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (!m_pHandle)
		return -1;

	unsigned char*pFrameData[3];
	int nLineSize[3];
	char*qscale_table=NULL;
	int qstride=0;
	int nGotPicture=0;

	int nUsedBytes=0;
	int nWidth=0;
	int nHeight=0;
	int nH263DeblockingFilter=0;

	int nLeftLen=nPacketSize;
	unsigned char*pLeftData=(unsigned char*)pPacketData;
	while (nLeftLen>0)
	{
		try
		{
			nGotPicture=avc_decoder_decode(m_pHandle,(unsigned char*)pLeftData,nLeftLen,pFrameData,nLineSize,&qscale_table,&qstride,&nWidth,&nHeight,&nUsedBytes,&m_nLastFrameIsKeyFrame);
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
				OnResized(nWidth,nHeight);
			}
			PostProcess(pFrameData,nLineSize,qscale_table,qstride);
		}
	}


	return 0;
}

int XDecoderFrameH263::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	if (m_pRTPFrame!=NULL)
	{
		m_pRTPFrame->OnRecvdRTPPacket(pPacketData,nPacketSize);
	}

	return 0;
}

void XDecoderFrameH263::OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame)
{
	DecodeFramePacket(pPacketData,nPacketLen);

	//char szDebug[1024]="";
	//sprintf(szDebug,"XDecoderFrameH263 nPacketLen=%d m_nLastFrameIsKeyFrame=%d\n",nPacketLen,m_nLastFrameIsKeyFrame);
	//OutputDebugString(szDebug);

	if (m_bGotFirstKeyFrame)
	{
		int nKeyFrame=m_nLastFrameIsKeyFrame;
		unsigned char*pFramePacketData=(unsigned char*)pPacketData;
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pFramePacketData-=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pFramePacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pFramePacketData,(m_nH263Plus?VIDEC_CODEC_H263P:VIDEC_CODEC_H263));
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pFramePacketData,1);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pFramePacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pFramePacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pFramePacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pFramePacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pFramePacketData,m_usSeqNo++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pFramePacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pFramePacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pFramePacketData,m_nHeight);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pFramePacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pFramePacketData,m_nHeight);
		}
		
		m_rCallback.OnMPVideo_DecoderCallbackFramePacket(this,pFramePacketData,nHeaderLen+nPacketLen,m_nWidth,m_nHeight);
	}
}