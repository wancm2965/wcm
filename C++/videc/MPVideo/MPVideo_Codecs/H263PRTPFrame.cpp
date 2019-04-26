
#include "H263PRTPFrame.h"


#define RFC2429_HEADER_LEN	0x02

#define MAKE_MASK(bits)		( (1<<(bits)) -1 )

static inline unsigned int rfc2429_get_P(const unsigned char *header){
	return (header[0]>>2) & 0x1;
}

static inline void rfc2429_set_P(unsigned char *header, unsigned int val){
	header[0]=header[0] | ( (val&0x1)<<2);
}

static inline unsigned int rfc2429_get_V(const unsigned char *header){
	return (header[0]>>1) & 0x1;
}

static inline unsigned int rfc2429_get_PLEN(const unsigned char *header){
	unsigned short *p=(unsigned short*)header;
	return (ntohs(p[0])>>3) & MAKE_MASK(6);
}

static inline unsigned int rfc2429_get_PEBIT(const unsigned char *header){
	unsigned short *p=(unsigned short*)header;
	return ntohs(p[0]) & MAKE_MASK(3);
}



/* returns the last psc position just below packet_size */
static unsigned char *get_psc(unsigned char *begin,unsigned char *end, int packet_size)
{
	int i;
	unsigned char *ret=NULL;
	unsigned char *p;
	if (begin==end) return NULL;
	for(i=1,p=begin+1;p<end && i<packet_size;++i,++p)
	{
		if (p[-1]==0 && p[0]==0)
		{
			ret=p-1;
		}
		p++;/* to skip possible 0 after the PSC that would make a double detection */
	}
	return ret;
}

static int skip_rfc2429_header(unsigned char*pPayloadData,int nPayloadLen)
{
	int nHeaderLen=0;
	if (nPayloadLen >= RFC2429_HEADER_LEN)
	{
		unsigned long *p = (unsigned long*)pPayloadData;
		unsigned char *ph=pPayloadData;
		int PLEN;
		unsigned int P;
		unsigned int V;

		nHeaderLen=RFC2429_HEADER_LEN;
		
		P=rfc2429_get_P(ph);
		V=rfc2429_get_V(ph);
		PLEN=rfc2429_get_PLEN(ph);

		if (V)
		{
			++nHeaderLen;
		}

		if (PLEN>0)
		{
			nHeaderLen+=PLEN;

			if (nHeaderLen>nPayloadLen)
			{
				return nHeaderLen;
			}
		}

		if (P)
		{
			nHeaderLen-=2;

			pPayloadData[nHeaderLen]=pPayloadData[nHeaderLen+1]=0;
		}
	}

	return nHeaderLen;
}


H263PRTPFrame::H263PRTPFrame(BaseRTPFrameCallback&rCallback,int nMaxBitrate)
:BaseRTPFrame(rCallback,nMaxBitrate)
{
}

H263PRTPFrame::~H263PRTPFrame(void)
{

}

int H263PRTPFrame::get_codec_header_len(void)
{
	return RFC2429_HEADER_LEN;
}

int H263PRTPFrame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
{
	int result=0;
	int nHeaderLen=skip_rfc2429_header((unsigned char*)buf,len);
	if (len>nHeaderLen)
	{
		result=len-nHeaderLen;
		memcpy(pFrameBuf,buf+nHeaderLen,result);
	}

	return result;
}

void H263PRTPFrame::DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp)
{
	unsigned char *lastpsc;
	unsigned char *psc;

	lastpsc=(unsigned char*)pFrameData;
	unsigned char *pEndOfFrameData=lastpsc+nFrameLen;

	while(1)
	{
		psc=get_psc(lastpsc+2,pEndOfFrameData,m_nMaxRTPPayloadBufLen);
		if (psc!=NULL)
		{
			rfc4629_generate_follow_on_packets(lastpsc,psc-lastpsc,0);
			lastpsc=psc;
		}
		else 
		{
			break;
		}
	}

	rfc4629_generate_follow_on_packets(lastpsc,pEndOfFrameData-lastpsc,1);
}

void H263PRTPFrame::rfc4629_generate_follow_on_packets(unsigned char *psc, int len, int last_packet)
{
	unsigned char*pTempFrame=(unsigned char*)psc;
	int nLeftLen=len;
	bool bFirstPacket=true;
	unsigned char*pRTPPacketBuf=NULL;

	while (nLeftLen>0)
	{
		int nThisFrameLen=nLeftLen>m_nMaxRTPPayloadBufLen?m_nMaxRTPPayloadBufLen:nLeftLen;
	
		{
			RTPFrame*pRTPFrame=new RTPFrame;
			if (pRTPFrame==NULL)
			{
				return;
			}

			pRTPFrame->nLen=RTP_HEADER_LEN+nThisFrameLen;
			pRTPPacketBuf=(unsigned char*)malloc(pRTPFrame->nLen+RFC2429_HEADER_LEN);
			if (pRTPPacketBuf==NULL)
			{
				pRTPFrame->Release();
				pRTPFrame=NULL;
				return;
			}

			pRTPFrame->pBuffer=pRTPPacketBuf;
			if (bFirstPacket)
			{
				bFirstPacket=false;

				memcpy(pRTPPacketBuf+RTP_HEADER_LEN,pTempFrame,nThisFrameLen);

				rfc2429_set_P(pRTPPacketBuf+RTP_HEADER_LEN,1);
			}
			else
			{
				(pRTPPacketBuf+RTP_HEADER_LEN)[0]=0;
				(pRTPPacketBuf+RTP_HEADER_LEN)[1]=0;
				memcpy(pRTPPacketBuf+RTP_HEADER_LEN+RFC2429_HEADER_LEN,pTempFrame,nThisFrameLen);
				pRTPFrame->nLen+=RFC2429_HEADER_LEN;
			}

			set_rtp_header(pRTPPacketBuf,(last_packet && nThisFrameLen==nLeftLen)?1:0);

			SendRTPPacket(pRTPFrame);

			//char szDebug[1024]="";
			//sprintf(szDebug,"nThisFrameLen=%d nThisFrameLen=%d nLeftLen=%d\n",nThisFrameLen,nThisFrameLen,nLeftLen);
			//OutputDebugString(szDebug);
		}

		nLeftLen-=nThisFrameLen;
		pTempFrame+=nThisFrameLen;
	}
}
