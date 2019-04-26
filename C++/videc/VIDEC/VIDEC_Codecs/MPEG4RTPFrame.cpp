
#include "MPEG4RTPFrame.h"



MPEG4RTPFrame::MPEG4RTPFrame(BaseRTPFrameCallback&rCallback)
:BaseRTPFrame(rCallback)
{
}

MPEG4RTPFrame::~MPEG4RTPFrame(void)
{

}

int MPEG4RTPFrame::get_codec_header_len(void)
{
	return 0;
}


int MPEG4RTPFrame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
{
    memcpy(pFrameBuf, buf, len);
    return len;
}

void MPEG4RTPFrame::DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp)
{
	int nLeft=nFrameLen;
	unsigned char*pRTPPacketBuf=NULL;

	while (nLeft>0)
	{
		int nPayloadLen=(nLeft>m_nMaxRTPPayloadBufLen?m_nMaxRTPPayloadBufLen:nLeft);

		RTPFrame*pRTPFrame=new RTPFrame;
		if (pRTPFrame==NULL)
		{
			return;
		}

		pRTPFrame->nLen=RTP_HEADER_LEN+nPayloadLen;
		pRTPPacketBuf=(unsigned char*)malloc(pRTPFrame->nLen);
		if (pRTPPacketBuf==NULL)
		{
			pRTPFrame->Release();
			pRTPFrame=NULL;
			return;
		}

		pRTPFrame->pBuffer=pRTPPacketBuf;
		memcpy(pRTPPacketBuf+RTP_HEADER_LEN,pFrameData,nPayloadLen);

		set_rtp_header(pRTPPacketBuf,(nLeft==nPayloadLen?1:0));
		SendRTPPacket(pRTPFrame);

		pFrameData+=nPayloadLen;
		nLeft-=nPayloadLen;
	}
}
