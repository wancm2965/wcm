

#include "BaseRTP2Frame.h"
#include "XUtil.h"

#define RTP_MAX_BITRATE		65535


BaseRTP2Frame::BaseRTP2Frame(BaseRTPFrameCallback&rCallback)
:m_rCallback(rCallback)
,m_nKeyFrame(0)
{
	m_pFrameBuf=NULL;
	m_nFrameBufLen=0;

	m_nHeaderMargin=0;

	m_nLastRecvdRTPTime=0;
	m_bGotFrame=false;
}

BaseRTP2Frame::~BaseRTP2Frame(void)
{

}

int BaseRTP2Frame::Open(int nHeaderMargin)
{
	m_nHeaderMargin=nHeaderMargin;
	return 0;
}

void BaseRTP2Frame::Close(void)
{
	FlushRTPPackets();

	if (m_pFrameBuf!=NULL)
	{
		free(m_pFrameBuf);
		m_pFrameBuf=NULL;
	}
	m_nFrameBufLen=0;
}

void BaseRTP2Frame::FlushRTPPackets(void)
{
	XAutoLock l(m_csListRTPPacket);
	while (m_ListRTPPacket.size()>0)
	{
		RTPFrame*pRTPFrame=(RTPFrame*)m_ListRTPPacket.front();
		m_ListRTPPacket.pop_front();
		pRTPFrame->Release();
		pRTPFrame=NULL;
	}
}


int BaseRTP2Frame::CalculateFrameSize(void)
{
	int nFrameLen=0;
	XAutoLock l(m_csListRTPPacket);
	XListPtr::iterator iter=m_ListRTPPacket.begin();
	while (iter!=m_ListRTPPacket.end())
	{
		RTPFrame*pRTPFrame=(RTPFrame*)*iter;
		nFrameLen+=pRTPFrame->nLen;
		++iter;
	}
	return nFrameLen;
}

int BaseRTP2Frame::GetFrame(unsigned char*pFrameBuf,int nBufLen)
{
	int nFrameLen=0;
	XAutoLock l(m_csListRTPPacket);
	while (m_ListRTPPacket.size()>0)
	{
		RTPFrame*pRTPFrame=(RTPFrame*)m_ListRTPPacket.front();
		m_ListRTPPacket.pop_front();

		int nTemp=handle_rtp_packet((unsigned char*)pRTPFrame->pBuffer+RTP_HEADER_LEN,pRTPFrame->nLen-RTP_HEADER_LEN,pFrameBuf,nBufLen);
		nFrameLen+=nTemp;

		pFrameBuf+=nTemp;
		nBufLen-=nBufLen;

		pRTPFrame->Release();
		pRTPFrame=NULL;
	}

	return nFrameLen;
}


void BaseRTP2Frame::OnRecvdRTPPacket(void*pPacketData,int nPacketLen)
{
	unsigned char*pFrameData=(unsigned char*)pPacketData;
	if (pFrameData!=NULL && nPacketLen>=12)
	{
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pFrameData;
		unsigned long nRTPTime=ntohl(pRTPHeader->timestamp);

		//char szDebug[1024];
		//sprintf(szDebug,"nRTPTime=%u m_nLastRecvdRTPTime=%u\n",nRTPTime,m_nLastRecvdRTPTime);
		//OutputDebugString(szDebug);

		if (m_bGotFrame && m_nLastRecvdRTPTime!=nRTPTime)
		{
			PacketsToFrame();
			m_bGotFrame=false;
		}

		RTPFrame*pRTPFrame=new RTPFrame;
		if (pRTPFrame==NULL)
		{
			return;
		}

		pRTPFrame->pBuffer=malloc(nPacketLen);
		if (pRTPFrame->pBuffer!=NULL)
		{
			memcpy(pRTPFrame->pBuffer,pPacketData,nPacketLen);
			pRTPFrame->nLen=nPacketLen;

			XAutoLock l(m_csListRTPPacket);
			m_ListRTPPacket.push_back(pRTPFrame);
		}
		else
		{
			pRTPFrame->Release();
			pRTPFrame=NULL;
			return;
		}

		m_bGotFrame=true;
		m_nLastRecvdRTPTime=nRTPTime;

		if (pRTPHeader->marker)//Marker
		{
			PacketsToFrame();
			m_bGotFrame=false;
		}
	}
}

void BaseRTP2Frame::PacketsToFrame(void)
{
	int nFrameLen=CalculateFrameSize()*2;
	if (nFrameLen>0)
	{
		if (m_pFrameBuf==NULL || m_nFrameBufLen<nFrameLen)
		{
			if (m_pFrameBuf!=NULL)
			{
				free(m_pFrameBuf);
				m_pFrameBuf=NULL;
			}
			m_nFrameBufLen=m_nHeaderMargin+nFrameLen;
			m_pFrameBuf=(unsigned char*)malloc(m_nFrameBufLen);
		}

		if (m_pFrameBuf==NULL)
		{
			FlushRTPPackets();
			return;
		}

		m_nKeyFrame=0;
		nFrameLen=GetFrame(m_pFrameBuf+m_nHeaderMargin,m_nFrameBufLen-m_nHeaderMargin);
		int nKeyFrame=m_nKeyFrame;
		m_nKeyFrame=0;
		m_rCallback.OnBaseRTPFrameCallbackFramePacket(this,m_pFrameBuf+m_nHeaderMargin,nFrameLen,nKeyFrame);
	}
}
