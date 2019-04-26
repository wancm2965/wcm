
#include "H263RTP2Frame.h"


#define RFC2190_HEADER_LEN	0x04

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

H263RTP2Frame::H263RTP2Frame(BaseRTPFrameCallback&rCallback)
:BaseRTP2Frame(rCallback)
,m_cPacketSpan(0)
{
}

H263RTP2Frame::~H263RTP2Frame(void)
{

}

int H263RTP2Frame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
{
	int result=0;
	unsigned char value=0;

	int nHeaderLen=skip_rfc2190_header((unsigned char*)buf,len);


	if (len>nHeaderLen)
	{
		int sbit = (buf[0] >> 3) & 0x07;
		int ebit = buf[0] & 0x07;

		if (nHeaderLen==4)
		{
			int nKeyFrame = (buf[1] >> 4) & 0x01;
			if (nKeyFrame==0)
			{
				m_nKeyFrame=1;
			}
		}
		else if (nHeaderLen==8 || nHeaderLen==12)
		{
			int nKeyFrame = (buf[4] >> 7) & 0x01;
			if (nKeyFrame==0)
			{
				m_nKeyFrame=1;
			}
		}

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

			//ÐÞÕýEBIT
			unsigned char cRtp=m_pFrameBuf[0];
			cRtp=cRtp&0xf8;
			cRtp=cRtp|(ebit&0x7);
			m_pFrameBuf[0]=cRtp;
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
