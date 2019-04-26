#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "XEncoderFrameH263.h"

#include "XUtil.h"
//#include "VIDEC_avc.h"
#include "avc.h"

#include "avbitstream.h"

#define MAX_PAYLOAD_LEN	988


void rtp_callback(void * user_data,PH263RTPCONTEXT pH263RTPContext,void *data, 
									int size,int keyframe,int gob_number,int mb_count)
{
	XEncoderFrameH263* pXEncoderFrameH263=reinterpret_cast<XEncoderFrameH263*>(user_data);
	pXEncoderFrameH263->OnRTPCallback((void*)pH263RTPContext,data,size,keyframe,gob_number,mb_count);
}

void XEncoderFrameH263::OnRTPCallback(void* pRTPContext,void *data,int size,int keyframe,int gob_number,int mb_count)
{
	int nMark=0;
	int i,psize,mb_w,mb_h;
	int bits,lastsentbit,lastgobind,lastind,len,payloadhdr_len;
	int gobn,mba, sbit,ebit,gob_index;
	int * mba_index,*mb_quant;
	short*mx,*my;
	char *pdata;

	unsigned char*pHeader;

	PH263RTPCONTEXT pH263RTPContext=reinterpret_cast<PH263RTPCONTEXT>(pRTPContext);
	gob_index=((m_nFormat==4)? 2:1);
	PutBitContext PutBitContext;
	
	
	int nFormat=m_nFormat;
	
	gobn=gob_number*gob_index;
	pdata=(char *)pH263RTPContext->pbuf;
	
	nMark=(m_nHeight==(gobn+gob_index)*16);
	
	mb_w=pH263RTPContext->mb_w;
	mb_h=pH263RTPContext->mb_h;
	mba_index=&pH263RTPContext->mba_index[gobn*mb_w];
	mx=&pH263RTPContext->mx[gobn*mb_w];
	my=&pH263RTPContext->my[gobn*mb_w];
	mb_quant=&pH263RTPContext->mb_quant[gobn*mb_w];
	if(gob_number==0)
		pH263RTPContext->mba_index[0]=0;
	lastind=0;

	bits=mba_index[mb_count];
	lastsentbit=mba_index[0];
	len=(bits-lastsentbit+7)>>3 ;
	assert(len==size);
	assert((pdata+(lastsentbit>>3)) == data);


	payloadhdr_len=4;
	while (lastsentbit<bits)
	{
		for (i=lastind;(i<=mb_count && (mba_index[i]<lastsentbit-8+(MAX_PAYLOAD_LEN<<3)));i++)
			lastgobind = mba_index[i];	
		
		mba=gobn*mb_w+lastind;
		sbit=lastsentbit&7;
		ebit=7-((lastgobind-1) &7);
		psize=((lastgobind+7)>>3)-(lastsentbit>>3);
		
		pHeader=m_pPayloadBuffer;

		init_put_bits(&PutBitContext, (PBYTE)(pHeader), payloadhdr_len, NULL, NULL);
		put_bits(&PutBitContext, 1, 0);	//only mode a
		put_bits(&PutBitContext, 1, 0);		//no PutBitContext frames
		put_bits(&PutBitContext, 3, sbit);		//sbit
		put_bits(&PutBitContext, 3, ebit);		//ebit
		put_bits(&PutBitContext, 3, m_nFormat);//src
		put_bits(&PutBitContext, 1,(keyframe==0));//Intra 0 :Inter 1
		put_bits(&PutBitContext, 1, 0);		//u
		put_bits(&PutBitContext, 1, 0);		//s
		put_bits(&PutBitContext, 1, 0);		//a
		put_bits(&PutBitContext, 4, 0);		//r
		put_bits(&PutBitContext, 2, 0);		//dbq
		put_bits(&PutBitContext, 3, 0);		//trb
		put_bits(&PutBitContext, 8, 0);		//tr
		flush_put_bits(&PutBitContext);
		assert(psize<2*MAX_PAYLOAD_LEN);

		memcpy(m_pPayloadBuffer+payloadhdr_len,pdata+(lastsentbit>>3),psize);
		PushBack(m_pRTPPacketBuffer,RTP_HEADER_LEN+payloadhdr_len+psize);

		lastsentbit=lastgobind;
		lastind=i;
	}
}

XEncoderFrameH263::XEncoderFrameH263(VIDEC_EncoderCallback&rCallback,bool bRTPPacket)
:XEncoderFrame(rCallback,bRTPPacket)
,m_hEncoder(NULL)
{
	m_nMaxPacketsToSend=1;
	m_nLastSentPacketTimestamp=0;
	
	m_nRTPTimestamp=0;
	m_nRTPSeqNo=0;
	m_bEncoding=false;
	m_nFormat=(1);
	m_pPayloadBuffer=(NULL);
	m_pRTPPacketBuffer=(NULL);

	m_nLastKeyFrameCount=0;
	m_nMaxBitrate=6144;
}

XEncoderFrameH263::~XEncoderFrameH263(void)
{
}

int XEncoderFrameH263::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
{
	int nFormat=1;
    if (nWidth == 128 && nHeight == 96)
		nFormat = 1;
    else if (nWidth == 176 && nHeight == 144)
		nFormat = 2;
    else if (nWidth == 352 && nHeight == 288)
		nFormat = 3;
    else if (nWidth == 704 && nHeight == 576)
		nFormat = 4;
    else if (nWidth == 1408 && nHeight == 1152)
		nFormat = 5;
	else
		return -1;

	m_nFormat=nFormat;

	if (nBitrateControlType==VIDEC_NETWORK_PRIVATE ||
		nBitrateControlType==VIDEC_NETWORK_SATELLITE ||
		nBitrateControlType==VIDEC_NETWORK_3G)
	{
		nMinQP=2;
	}
	else if (nBitrateControlType==VIDEC_NETWORK_LAN)
	{
		//nBitrate=0;
		//nMaxQP=nMinQP;
		nBitrate+=(nBitrate>>1);
		if (m_nWidth>704)
		{
			if (nMinQP<nMaxQP)
			{
				nMinQP+=1;
			}
		}
	}
	else //VIDEC_NETWORK_INTERNET
	{
		//nBitrate+=(nBitrate>>1);
		if (m_nWidth>704)
		{
			if (nMinQP<nMaxQP)
			{
				nMinQP+=1;
			}
		}
	}

	int nRet=XEncoderFrame::Open(nWidth,nHeight,nFrameRate,bDoubleField,nBitrate,nMinQP,nMaxQP,nBitrateControlType);
	if (nRet!=0)
	{
		return nRet;
	}

	if (m_bRTPPacket)
	{
		m_pRTPPacketBuffer=(unsigned char*)MALLOCMSG(MAX_PAYLOAD_LEN*2);
		if (m_pRTPPacketBuffer==NULL)
		{
			return -1;
		}
		m_pPayloadBuffer=m_pRTPPacketBuffer+RTP_HEADER_LEN;

		m_hEncoder=avc_encoder_open(AVC_CODEC_H263,m_nWidth,m_nHeight,MAX_PAYLOAD_LEN,rtp_callback,this);

		m_nMaxBitrate=(nBitrate>0?nBitrate:6144);
		m_nMaxBitrate+=(m_nMaxBitrate>>2);
	}
	else
	{
		m_hEncoder=avc_encoder_open(AVC_CODEC_H263,m_nWidth,m_nHeight,0,NULL,0);
	}

	if (m_hEncoder==NULL)
	{
		return -1;
	}

	m_VBRControl.Reset(nMinQP,nMaxQP,nBitrate);

	return 0;
}

void XEncoderFrameH263::Close(void)
{
	if (m_hEncoder!=NULL)
	{
		avc_encoder_close(m_hEncoder);
		m_hEncoder=NULL;
	}

	if (m_pRTPPacketBuffer!=NULL)
	{
		FREEMSG(m_pRTPPacketBuffer);
		m_pRTPPacketBuffer=NULL;
	}
	m_pPayloadBuffer=NULL;

	XEncoderFrame::Close();
}

bool XEncoderFrameH263::DoRTPPacket(void)
{
	bool bRet=false;
	int nSentPackets=0;
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp==m_nLastSentPacketTimestamp)
	{
		return bRet;
	}

	int nTotalLen=0;
	unsigned long nDuration=ulTimestamp-m_nLastSentPacketTimestamp;

	int nMaxPacketsToSend=m_nMaxPacketsToSend;
	while (nSentPackets<nMaxPacketsToSend)
	{
		void*pData=NULL;
		int nLen=0;
		
		LockPool();

		bool bRet=get_front(&pData,nLen);
		if (bRet && pData && nLen>0)
		{
			nTotalLen+=nLen;

// 			int nBitrate=nTotalLen*8/nDuration;
// 			if (nBitrate>m_nMaxBitrate)
// 			{
// 				UnlockPool();
// 				return bRet;
// 			}

			bool bMarkerFlag=(GetDataCount()==1);
			PRTP_HEADER pRTPHeader=(PRTP_HEADER)pData;
			memset(pRTPHeader,0,RTP_HEADER_LEN);
			pRTPHeader->version=RTP_VERSION;
			pRTPHeader->marker=bMarkerFlag?1:0;
			pRTPHeader->type=34;
			pRTPHeader->sequence=htons(m_nRTPSeqNo++);
			pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
			pRTPHeader->ssrc=htonl((unsigned long)this);
			if (bMarkerFlag)
			{
				m_nRTPTimestamp+=3003;
			}

			OnBaseRTPFrameCallbackRTPPacket(pData,nLen);
			m_nLastSentPacketTimestamp=ulTimestamp;

			UnlockPool();

			PopFront();
		}
		else
		{
			UnlockPool();
		}


		nSentPackets++;

		bRet=true;
	}

	bRet=false;

	return bRet;
}

int XEncoderFrameH263::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_hEncoder==NULL)
	{
		return -1;
	}

// 	if (GetDataCount()>0)
// 	{
// 		return -1;
// 	}

	int nCanEncThisFrame=1;
	int nQuant=m_nMinQP;
	if (m_nBitrateControlType!=VIDEC_NETWORK_LAN)
	{
		nQuant=m_VBRControl.GetNextQP(nCanEncThisFrame);
// 		if (!nCanEncThisFrame)
// 			return -1; 
	}

	//if (m_bRTPPacket && m_nLastKeyFrameCount++>15)
	//{
	//	if (GetDataCount()>15)
	//	{
	//		return -1;
	//	}
	//}

	int nTempBufLen=nPacketLen;
	try
	{
		m_bEncoding=true;

		nPacketLen=avc_encoder_encode(m_hEncoder,pData,pPacketData,nPacketLen,nQuant,&nKeyFrame);

		m_bEncoding=false;

		if (nKeyFrame!=0)
		{
			nKeyFrame=-1;
			m_nLastKeyFrameCount=0;
		}
	}
	catch (...)
	{
		return -1;
	}

	if (nPacketLen<0)
	{
		nPacketLen=0;
		return -1;
	}
	else if (nPacketLen>0)
	{
		m_VBRControl.Calculate(nPacketLen);
	}

	return 0;
}


unsigned char XEncoderFrameH263::GetCodecType(void)
{
	return VIDEC_CODEC_H263;
}

void XEncoderFrameH263::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnVIDEC_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}
