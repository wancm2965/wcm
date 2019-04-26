#include "H264Frame2RTP.h"
#include "RTPBuffer.h"
#include "RTPHeader.h"
#include <NETEC/XSocket.h>


#ifdef _WIN32_WCE
#ifndef WIN64
typedef signed int intptr_t;
typedef unsigned int uintptr_t;
#endif
#endif


static const unsigned char *h264_find_startcode_internal(const unsigned char *p, const unsigned char *end)
{
    const unsigned char *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    for (end -= 3; p < end; p += 4) {
        unsigned int x = *(const unsigned int*)p;
//      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
//      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
        if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                    return p;
                if (p[2] == 0 && p[3] == 1)
                    return p+1;
            }
            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                    return p+2;
                if (p[4] == 0 && p[5] == 1)
                    return p+3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    return end + 3;
}

const unsigned char *h264_find_startcode(const unsigned char *p, const unsigned char *end)
{
    const unsigned char *out= h264_find_startcode_internal(p, end);
    if(p<out && out<end && !out[-1]) out--;
    return out;
}

IH264Frame2RTP*IH264Frame2RTP::CreateSimple(IH264Frame2RTPCallback&rCallback)
{
	//return new H264Frame2RTPSimple(rCallback);
	NEWRECORDret3(  IH264Frame2RTP* ,p , new H264Frame2RTPSimple( rCallback) )   
}

IH264Frame2RTP*IH264Frame2RTP::CreateJitter(IH264Frame2RTPCallback&rCallback)
{
	//return new H264Frame2RTPJitter(rCallback);
	NEWRECORDret3(  IH264Frame2RTP* ,p , new H264Frame2RTPJitter( rCallback) )   
}


H264Frame2RTPSimple::H264Frame2RTPSimple(IH264Frame2RTPCallback&rCallback)
:m_nRTPPayloadType(97)
,m_rCallback(rCallback)
,m_nMaxRTPPayloadLen(1400)
,m_nRTPTimestampInc(3003)
{

}

H264Frame2RTPSimple::~H264Frame2RTPSimple(void)
{

}

int H264Frame2RTPSimple::Open(void)
{
	return 0;
}

void H264Frame2RTPSimple::Close(void)
{

}


void H264Frame2RTPSimple::SetRTPPayloadType(int nPayloadType)
{
	m_nRTPPayloadType=nPayloadType;
}

int H264Frame2RTPSimple::GetRTPPayloadType(void)
{
	return m_nRTPPayloadType;
}
	
void H264Frame2RTPSimple::SetMaxBitrate(int nMaxBitrate)
{

}

void H264Frame2RTPSimple::SetFrameRate(int nFrameRate)
{
	if (nFrameRate>=27)
	{
		m_nRTPTimestampInc=3003;//30
	}
	else if (nFrameRate>=17)
	{
		m_nRTPTimestampInc=3600;//25
	}
	else if (nFrameRate>=13)
	{
		m_nRTPTimestampInc=6006;//15
	}
	else if (nFrameRate>=7)
	{
		m_nRTPTimestampInc=7500;//10
	}
	else
	{
		m_nRTPTimestampInc=15000;//5
	}
}

int H264Frame2RTPSimple::SetFrameData(void*pData,int nLen,int nKeyFrame)
{
	const unsigned char *pTempBuf=(unsigned char*)pData;
	int size=nLen;
    const unsigned char *r;
    r = h264_find_startcode(pTempBuf, pTempBuf + size);
    while (r < pTempBuf + size) 
	{
        const unsigned char *r1;

        while(!*(r++));
        r1 = h264_find_startcode(r, pTempBuf + size);
        nal_send(r, r1 - r, (r1 == pTempBuf + size));
        r = r1;
    }

	return 0;
}

void H264Frame2RTPSimple::nal_send(const unsigned char *buf, int size, int last)
{
	if (size<=2)
	{
		return ;
	}

	RTPBuffer*pRTPBuffer=GetFreeRTPBuffer();
	if (pRTPBuffer==NULL)
	{
		return;
	}
	pRTPBuffer->AddRef();
	
	unsigned char*pRTPPacketBuf=(unsigned char*)pRTPBuffer->GetData();

    if (size <= m_nMaxRTPPayloadLen) 
	{
		pRTPBuffer->SetLen(RTP_HEADER_LEN+size);
		memcpy(pRTPPacketBuf+RTP_HEADER_LEN,buf,size);

		set_rtp_header(pRTPPacketBuf,last!=0);

		m_rCallback.OnIH264Frame2RTPCallbackRTPPacketData(pRTPBuffer);

		pRTPBuffer->Release();
		pRTPBuffer=NULL;
		return ;
	} 

	int nRTPPacketDataLen=RTP_HEADER_LEN+m_nMaxRTPPayloadLen;

	pRTPBuffer->SetLen(nRTPPacketDataLen);
	unsigned char*rtp_payload_buf=pRTPPacketBuf+RTP_HEADER_LEN;

	unsigned char type = buf[0] & 0x1F;
	unsigned char nri = buf[0] & 0x60;

	rtp_payload_buf[0] = 28;        
	rtp_payload_buf[0] |= nri;
	rtp_payload_buf[1] = type;
	rtp_payload_buf[1] |= 1 << 7;
	buf += 1;
	size -= 1;
	while (size + 2 > m_nMaxRTPPayloadLen) 
	{
		memcpy(&rtp_payload_buf[2], buf, m_nMaxRTPPayloadLen - 2);

		{
			set_rtp_header(pRTPPacketBuf,0);

			m_rCallback.OnIH264Frame2RTPCallbackRTPPacketData(pRTPBuffer);

			pRTPBuffer->Release();
			pRTPBuffer=NULL;
		}

		pRTPBuffer=GetFreeRTPBuffer();
		if (pRTPBuffer==NULL)
		{
			return;
		}
		pRTPBuffer->AddRef();
		
		pRTPPacketBuf=(unsigned char*)pRTPBuffer->GetData();
		pRTPBuffer->SetLen(nRTPPacketDataLen);
		rtp_payload_buf=pRTPPacketBuf+RTP_HEADER_LEN;

		rtp_payload_buf[0] = 28;        
		rtp_payload_buf[0] |= nri;
		rtp_payload_buf[1] = type;
		rtp_payload_buf[1] |= 1 << 7;

		buf += m_nMaxRTPPayloadLen - 2;
		size -= m_nMaxRTPPayloadLen - 2;
		rtp_payload_buf[1] &= ~(1 << 7);
	}
	rtp_payload_buf[1] |= 1 << 6;
	memcpy(&rtp_payload_buf[2], buf, size);

	pRTPBuffer->SetLen(RTP_HEADER_LEN + size + 2);	
	set_rtp_header(pRTPPacketBuf,last!=0);
	m_rCallback.OnIH264Frame2RTPCallbackRTPPacketData(pRTPBuffer);

	pRTPBuffer->Release();
	pRTPBuffer=NULL;
}

void H264Frame2RTPSimple::set_rtp_header(unsigned char*pHeader,bool bMarker)
{
	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pHeader;
	memset(pRTPHeader,0,RTP_HEADER_LEN);
	pRTPHeader->version=RTP_VERSION;
	pRTPHeader->marker=(bMarker?1:0);
	pRTPHeader->type=m_nRTPPayloadType;
	pRTPHeader->sequence=htons(m_nSeqNo++);
	pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
	pRTPHeader->ssrc=htonl(m_nSSRC);

	if (bMarker)
	{
		m_nRTPTimestamp+=7000;
	}
}

H264Frame2RTPJitter::H264Frame2RTPJitter(IH264Frame2RTPCallback&rCallback)
:m_nRTPPayloadType(97)
,m_rCallback(rCallback)
,m_nMaxRTPPayloadLen(1400)
,m_nRTPTimestampInc(3003)
{

}

H264Frame2RTPJitter::~H264Frame2RTPJitter(void)
{

}

int H264Frame2RTPJitter::Open(void)
{
	return RTPJitterBufferOut::Open();
}

void H264Frame2RTPJitter::Close(void)
{
	RTPJitterBufferOut::Close();
}

void H264Frame2RTPJitter::SetRTPPayloadType(int nPayloadType)
{
	m_nRTPPayloadType=nPayloadType;
}

int H264Frame2RTPJitter::GetRTPPayloadType(void)
{
	return m_nRTPPayloadType;
}
	
void H264Frame2RTPJitter::SetMaxBitrate(int nMaxBitrate)
{
	RTPJitterBufferOut::SetMaxBitrate(nMaxBitrate);
}

void H264Frame2RTPJitter::SetFrameRate(int nFrameRate)
{
	if (nFrameRate>=27)
	{
		m_nRTPTimestampInc=3003;//30
	}
	else if (nFrameRate>=17)
	{
		m_nRTPTimestampInc=3600;//25
	}
	else if (nFrameRate>=13)
	{
		m_nRTPTimestampInc=6006;//15
	}
	else if (nFrameRate>=7)
	{
		m_nRTPTimestampInc=7500;//10
	}
	else
	{
		m_nRTPTimestampInc=15000;//5
	}
}

int H264Frame2RTPJitter::SetFrameData(void*pData,int nLen,int nKeyFrame)
{
	const unsigned char *pTempBuf=(unsigned char*)pData;
	int size=nLen;
    const unsigned char *r;
    r = h264_find_startcode(pTempBuf, pTempBuf + size);
    while (r < pTempBuf + size) 
	{
        const unsigned char *r1;

        while(!*(r++));
        r1 = h264_find_startcode(r, pTempBuf + size);
        nal_send(r, r1 - r, (r1 == pTempBuf + size));
        r = r1;
    }

	return 0;
}

void H264Frame2RTPJitter::nal_send(const unsigned char *buf, int size, int last)
{
	if (size<=2)
	{
		return ;
	}

	RTPBuffer*pRTPBuffer=GetFreeRTPBuffer();
	if (pRTPBuffer==NULL)
	{
		return;
	}
	pRTPBuffer->AddRef();
	
	unsigned char*pRTPPacketBuf=(unsigned char*)pRTPBuffer->GetData();

    if (size <= m_nMaxRTPPayloadLen) 
	{
		pRTPBuffer->SetLen(RTP_HEADER_LEN+size);
		memcpy(pRTPPacketBuf+RTP_HEADER_LEN,buf,size);

		set_rtp_header(pRTPPacketBuf,last!=0);

		OnRTPPacketIn(pRTPBuffer);

		pRTPBuffer->Release();
		pRTPBuffer=NULL;

		return ;
	} 

	int nRTPPacketDataLen=RTP_HEADER_LEN+m_nMaxRTPPayloadLen;

	pRTPBuffer->SetLen(nRTPPacketDataLen);
	unsigned char*rtp_payload_buf=pRTPPacketBuf+RTP_HEADER_LEN;

	unsigned char type = buf[0] & 0x1F;
	unsigned char nri = buf[0] & 0x60;

	rtp_payload_buf[0] = 28;        
	rtp_payload_buf[0] |= nri;
	rtp_payload_buf[1] = type;
	rtp_payload_buf[1] |= 1 << 7;
	buf += 1;
	size -= 1;
	while (size + 2 > m_nMaxRTPPayloadLen) 
	{
		memcpy(&rtp_payload_buf[2], buf, m_nMaxRTPPayloadLen - 2);

		{
			set_rtp_header(pRTPPacketBuf,0);

			OnRTPPacketIn(pRTPBuffer);

			pRTPBuffer->Release();
			pRTPBuffer=NULL;
		}

		pRTPBuffer=GetFreeRTPBuffer();
		if (pRTPBuffer==NULL)
		{
			return;
		}
		pRTPBuffer->AddRef();
		
		pRTPPacketBuf=(unsigned char*)pRTPBuffer->GetData();
		pRTPBuffer->SetLen(nRTPPacketDataLen);
		rtp_payload_buf=pRTPPacketBuf+RTP_HEADER_LEN;

		rtp_payload_buf[0] = 28;        
		rtp_payload_buf[0] |= nri;
		rtp_payload_buf[1] = type;
		rtp_payload_buf[1] |= 1 << 7;

		buf += m_nMaxRTPPayloadLen - 2;
		size -= m_nMaxRTPPayloadLen - 2;
		rtp_payload_buf[1] &= ~(1 << 7);
	}
	rtp_payload_buf[1] |= 1 << 6;
	memcpy(&rtp_payload_buf[2], buf, size);

	pRTPBuffer->SetLen(RTP_HEADER_LEN + size + 2);	
	set_rtp_header(pRTPPacketBuf,last!=0);
	
	OnRTPPacketIn(pRTPBuffer);

	pRTPBuffer->Release();
	pRTPBuffer=NULL;
}

void H264Frame2RTPJitter::set_rtp_header(unsigned char*pHeader,bool bMarker)
{
	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pHeader;
	memset(pRTPHeader,0,RTP_HEADER_LEN);
	pRTPHeader->version=RTP_VERSION;
	pRTPHeader->marker=(bMarker?1:0);
	pRTPHeader->type=m_nRTPPayloadType;
	pRTPHeader->sequence=htons(m_nSeqNo++);
	pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
	pRTPHeader->ssrc=htonl(m_nSSRC);

	if (bMarker)
	{
		m_nRTPTimestamp+=m_nRTPTimestampInc;
	}

	//7000:30 7007:15 7010:10
}

void H264Frame2RTPJitter::OnRTPPacketOut(RTPBuffer*pRTPBuffer)
{
	m_rCallback.OnIH264Frame2RTPCallbackRTPPacketData(pRTPBuffer);
}

int H264Frame2RTPJitter::GetRTPBufferCount(void)
{
	return RTPJitterBufferOut::GetRTPBufferCount();

}

