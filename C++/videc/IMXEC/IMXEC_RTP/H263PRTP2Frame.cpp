
#include "H263PRTP2Frame.h"
#include "XUtil.h"


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


H263PRTP2Frame::H263PRTP2Frame(BaseRTPFrameCallback&rCallback)
:BaseRTP2Frame(rCallback)
,m_nLastRequestKeyFrameTimestamp(0)
{
}

H263PRTP2Frame::~H263PRTP2Frame(void)
{

}

int H263PRTP2Frame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
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

int H263PRTP2Frame::GetFrame(unsigned char*pFrameBuf,int nBufLen)
{
	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nLastRequestKeyFrameTimestamp>=4000)
	{
		m_nLastRequestKeyFrameTimestamp=nTimestamp;
		m_nKeyFrame=1;
	}
	return BaseRTP2Frame::GetFrame(pFrameBuf,nBufLen);
}
