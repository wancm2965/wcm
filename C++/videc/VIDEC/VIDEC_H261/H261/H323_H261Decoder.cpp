// H323_H261Decoder.cpp: implementation of the H323_H261Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"

#include "p64.h"
#include "H323_H261Decoder.h"
#include "..\colorspace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

H323_H261Decoder::H323_H261Decoder():m_pDecoder(NULL),m_pConvertBuf(NULL),m_pBitBuf(NULL),m_cPacketSpan(0),
									 m_nBitBufSize(0),m_nCovBufSize(0),m_colorspace(ENC_CSP_I420)
{
	m_nMode=UDEC_OPT_PACKET;
	End();
}

H323_H261Decoder::~H323_H261Decoder()
{
	End();
}

long H323_H261Decoder::Initialize(void *pParam1, void *pParam2)
{
	int iResult=UDEC_OK;
	if (pParam1==NULL) 
	{
		iResult=UDEC_BAD_PARAM;
		return iResult;
	}
	
	End();
	
	UBitmapInfoHeader* pin=(UBitmapInfoHeader*)pParam1;
	
	if((pin->biWidth==352 && pin->biHeight==288 )||
		(pin->biWidth==176 && pin->biHeight==144)) 
	{
		m_BitmapHeader.biWidth=pin->biWidth;
		m_BitmapHeader.biHeight=pin->biHeight;
	}
	else
	{
		iResult=UDEC_BAD_FORMAT;
		return iResult;
	}
	
	if (pin->biCompression==0 && pin->biBitCount==24) 
	{
		m_colorspace=ENC_CSP_RGB24;
	}
	else if (pin->biCompression==MAKEFOURCC('I','4','2','0')) 
	{
		m_colorspace=ENC_CSP_I420;
	}
	else
	{
		iResult=UDEC_BAD_FORMAT;
		return iResult;
	}
	
	
	m_pDecoder=(P64Decoder*)new FullP64Decoder();
	
	//	m_pDecoder->marks();
	
	memcpy(&m_BitmapHeader,pin,sizeof(UBitmapInfoHeader));
	
	long nConvBufSize=m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*m_BitmapHeader.biBitCount/8;
	
	if (m_nCovBufSize<nConvBufSize)
	{	
		if (m_pConvertBuf)
		{
			delete[] m_pConvertBuf;
			m_pConvertBuf=NULL;
			m_nCovBufSize=0;
		}
		m_nCovBufSize=nConvBufSize;
		m_pConvertBuf=new BYTE[m_nCovBufSize];
	}
	UINT nBitBufSize=m_BitmapHeader.biWidth*m_BitmapHeader.biHeight;
	if(m_nBitBufSize<nBitBufSize)
	{
		if(m_pBitBuf)
		{
			delete[] m_pBitBuf;
			m_pBitBuf=NULL;
			m_nBitBufSize=0;
		}
		m_nBitBufSize=nBitBufSize;
		m_pBitBuf=new BYTE[m_nBitBufSize];

	}
	m_pBitPos=m_pBitBuf;	
	return iResult;
}
long H323_H261Decoder::Decode(LPBYTE lpData,UINT iInLength, LPBYTE lpOut,BOOL bLostPreviousPacket,BOOL& bRenderFlag)
{
	BYTE * pOutImage=lpOut;
	if (m_colorspace!=ENC_CSP_I420)	
		pOutImage=m_pConvertBuf;
	
	BOOL ok = m_pDecoder->decode(lpData, iInLength, bLostPreviousPacket,pOutImage,bRenderFlag);
	if (!ok) 
		return -1;
 // If the incoming video stream changes size, resize the rendering device.
	Resize(m_pDecoder->width(), m_pDecoder->height());
	if ((TRUE==bRenderFlag)&&(ENC_CSP_RGB24==m_colorspace))
	{
	 	uint8_t * x_ptr=lpOut;
		int x_stride=m_BitmapHeader.biWidth*3;
		uint8_t * y_src=m_pConvertBuf;
		uint8_t * u_src=m_pConvertBuf+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight;
		uint8_t * v_src=m_pConvertBuf+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*5/4;
		int y_stride=m_BitmapHeader.biWidth;
		int uv_stride=m_BitmapHeader.biWidth/2;
		int width=m_BitmapHeader.biWidth;
		int height=m_BitmapHeader.biHeight;
		int vflip=1;
		yv12_to_bgr_mmx(
			x_ptr, x_stride, y_src, u_src,v_src ,
			y_stride,
			uv_stride,
			width,
			height,
			vflip);
		
	}
	return 0;
}

long H323_H261Decoder::PreDecode(LPBYTE lpData,UINT iInLength, LPBYTE lpOut,BOOL bLostPreviousPacket,BOOL bLastPacket,BOOL& bRenderFlag)
{
	long nResult=0;
	if((!m_pDecoder)||(iInLength<=4))
	{
		nResult=-1;
		return nResult;
	}
//	m_pDecoder->mark(now);
	
	h261hdr_t h261hdr;
	
	// get 32 bit H261 header
	h261hdr = ntohl(*(u_int*)lpData);
	// decode values we need from the RTP H261 header
	int sbit  = (h261hdr >> 29) & 0x07;
	int ebit  = (h261hdr >> 26) & 0x07;
	int gob   = (h261hdr >> 20) & 0xf;
	unsigned char value;
	bRenderFlag=FALSE;
	int nRenderNum=0;

	UINT nTotalLen=(UINT)m_pBitPos-(UINT)m_pBitBuf;
	if((nTotalLen>0)&&(bLostPreviousPacket||((nTotalLen+iInLength+128)>m_nBitBufSize)))
	{
		nResult=Decode(m_pBitBuf, nTotalLen, lpOut,bLostPreviousPacket,bRenderFlag);
		if((0==nResult)&&(bRenderFlag))
			nRenderNum++;
		m_pBitPos=m_pBitBuf;
	}	
	
	if(m_pBitPos==m_pBitBuf)//first packet
	{
		memcpy(m_pBitPos,lpData,iInLength);
		m_pBitPos+=iInLength;
	}
	else
	{
		
		UINT nBitLen=iInLength-sizeof(h261hdr_t);
		UINT nInOff=0;
		if(sbit)
		{
			value=*(lpData+sizeof(h261hdr_t));
			value=value<<sbit;
			value=value>>sbit;
			m_cPacketSpan|=value;

			*(m_pBitPos-1)=m_cPacketSpan;
			nBitLen--;
			nInOff++;
		}
		memcpy(m_pBitPos,lpData+sizeof(h261hdr_t)+nInOff,nBitLen);
		m_pBitPos+=nBitLen;
		//修正EBIT
		h261hdr = ntohl(*(u_int*)m_pBitBuf);
		h261hdr&=0xe3ffffff;//zero ebit
		h261hdr |= ((ebit) & 7) << 26; //EBIT = 8 - SBIT for next packet
		h261hdr = htonl(h261hdr);
		*(h261hdr_t*)m_pBitBuf = h261hdr;
	}
	if(0==ebit)
	{
		m_cPacketSpan=0;
	}
	else
	{
		value=lpData[iInLength-1];
		value=value>>ebit;
		value=value<<ebit;
		m_cPacketSpan=value;
	}

	if(bLastPacket)
	{
		nTotalLen=(UINT)m_pBitPos-(UINT)m_pBitBuf;
		nResult=Decode(m_pBitBuf, nTotalLen,lpOut, bLostPreviousPacket,bRenderFlag);
		if((0==nResult)&&(bRenderFlag))
			nRenderNum++;
		m_pBitPos=m_pBitBuf;

	}
	if(nRenderNum>0)
	{
		bRenderFlag=TRUE;
		nResult=0;
	}
	return nResult;
	
}

long H323_H261Decoder::DeCompress(long dec_opt, void *pParam1, void *pParam2)
{
	long iResult=UDEC_OK;

	if (dec_opt!=UDEC_OPT_PACKET||pParam1==NULL) 
	{
		iResult=UDEC_BAD_PARAM;
		return iResult;
	}
	UDEC_FRAME* pFrame=(UDEC_FRAME*)pParam1;
	BOOL bLostPreviousPacket=FALSE;
	if (pFrame->iLostPacket>0) 
		bLostPreviousPacket=TRUE;
	//先重新组包再进行解码
	if (0!=PreDecode((LPBYTE)pFrame->bitstream,pFrame->iLength,(LPBYTE)pFrame->bmp, \
		bLostPreviousPacket,pFrame->iLastPacket,pFrame->iRenderFlag)) 
	{
		iResult=UDEC_FAIL;
		return iResult;
			
	}
	UDEC_RESULT* pResult=(UDEC_RESULT*)pParam2;
	if(pResult)
	{
		pResult->iWidth=m_pDecoder->width();
		pResult->iHeight=m_pDecoder->height();
	
	}

	return iResult;
}


BOOL H323_H261Decoder::Resize(int _width, int _height)
{
	if ((m_BitmapHeader.biWidth != _width) || (m_BitmapHeader.biHeight != _height) )
	{
		
		m_BitmapHeader.biWidth=_width;
		m_BitmapHeader.biHeight=_height;
		
		long nConvBufSize=m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*m_BitmapHeader.biBitCount/8;
		
		if (m_nCovBufSize<nConvBufSize)
		{	
			if (m_pConvertBuf)
			{
				delete[] m_pConvertBuf;
				m_pConvertBuf=NULL;
				m_nCovBufSize=0;
			}
			m_nCovBufSize=nConvBufSize;
			m_pConvertBuf=new BYTE[m_nCovBufSize];
		}
	}
	return TRUE;
}

long H323_H261Decoder::End()
{

	if (m_pDecoder) 
	{
		delete m_pDecoder;
		m_pDecoder=NULL;
	}
	if (m_pConvertBuf)
	{
		delete[] m_pConvertBuf;
		m_pConvertBuf=NULL;
		m_nCovBufSize=0;
	}
	memset(&m_BitmapHeader,0,sizeof(UBitmapInfoHeader));
	if(m_pBitBuf)
	{
		delete[] m_pBitBuf;
		m_pBitBuf=NULL;
		m_nBitBufSize=0;
	}
	m_pBitPos=m_pBitBuf;
	return UDEC_OK;
}






