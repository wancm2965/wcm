
#include "H263RTPFrame.h"

#define RFC2190_HEADER_LEN	0x08

static int skip_rfc2190_header(unsigned char*pPayloadData,int nPayloadLen)
{
	int nHeaderLen=0;
	if (nPayloadLen >= 4) 
	{
		unsigned char *ph = pPayloadData;
		int F = (ph[0]>>7) & 0x1;
		int P = (ph[0]>>6) & 0x1;
		if (F == 0) nHeaderLen = 4;  // mode A
		else if (P == 0) nHeaderLen = 8; // mode B
		else nHeaderLen = 12;   // mode C
	}

	return nHeaderLen;
}

H263RTPFrame::H263RTPFrame(BaseRTPFrameCallback&rCallback,int nMaxBitrate)
:BaseRTPFrame(rCallback,nMaxBitrate)
,m_cPacketSpan(0)
{
}

H263RTPFrame::~H263RTPFrame(void)
{
}

int H263RTPFrame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
{
	int result=0;
	unsigned char value=0;

	int nHeaderLen=skip_rfc2190_header((unsigned char*)buf,len);

	//if (nHeaderLen>4)
	//{
	//	char szDebug[1024];
	//	sprintf(szDebug,"nHeaderLen=%d\n",nHeaderLen);
	//	DebugOutput(szDebug);
	//}

	if (len>nHeaderLen)
	{
		int sbit = (buf[0] >> 3) & 0x07;
		int ebit = buf[0] & 0x07;

		result=len-nHeaderLen;
		if (m_pFrameBuf==pFrameBuf)
		{
			memcpy(pFrameBuf,buf+nHeaderLen,result);
		}
		else
		{
			int nBitLen=len-nHeaderLen;
			int nOffset=0;
			if(sbit)
			{
				value=*(buf+nHeaderLen);
				value=value<<sbit;
				value=value>>sbit;
				m_cPacketSpan|=value;
				*(pFrameBuf-1)=m_cPacketSpan;
				nBitLen--;
				nOffset++;
			}
			memcpy(pFrameBuf,buf+nHeaderLen+nOffset,nBitLen);
			result=nBitLen;
		}

		if(0==ebit)
		{
			m_cPacketSpan=0;
		}
		else
		{
			value=buf[len-1];
			value=value>>ebit;
			value=value<<ebit;
			m_cPacketSpan=value;
		}
	}

	return result;
}

int H263RTPFrame::get_codec_header_len(void)
{
	return RFC2190_HEADER_LEN;
}

void H263RTPFrame::DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp)
{
}
