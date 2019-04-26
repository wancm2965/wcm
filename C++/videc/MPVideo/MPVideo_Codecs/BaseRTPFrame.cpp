
#include <map>
#include "BaseRTPFrame.h"
#include "XUtil.h"
#include "XThreadBase.h"

class RTPFrameThreadCallback
{
public:
	virtual int OnRTPFrameThreadCallback(void)=0;
};

class RTPFrameThread
	: public XThreadBase
{
public:
	RTPFrameThread(RTPFrameThreadCallback&rCallback)
		: m_rCallback(rCallback)
		, m_bRunning(false)
	{

	}
	virtual~RTPFrameThread(void)
	{

	}
public:
	int Start(void)
	{
		m_bRunning=true;
		StartThread();
		return 0;
	}
	void Stop(void)
	{
		m_bRunning=false;
		WaitForStop();
	}
protected:
	void ThreadProcMain(void)
	{
		while (m_bRunning)
		{
			if (0==m_rCallback.OnRTPFrameThreadCallback())
			{
				XSleep(1);
			}
		}
	}
protected:
	RTPFrameThreadCallback&	m_rCallback;
	bool					m_bRunning;
};

class RTPFrameMgr
	: public RTPFrameThreadCallback
{
public:
	RTPFrameMgr(void)
	{
		m_pRTPFrameThread=NULL;
	}
	virtual~RTPFrameMgr(void)
	{

	}
public:
	void AddBaseRTPFrame(BaseRTPFrame*pBaseRTPFrame)
	{
		{
			XAutoLock l(m_csMapBaseRTPFrame);
			m_MapBaseRTPFrame[pBaseRTPFrame]=pBaseRTPFrame;
		}

		{
			XAutoLock l(m_csRTPFrameThread);
			if (m_MapBaseRTPFrame.size()>0)
			{
				if (m_pRTPFrameThread==NULL)
				{
					m_pRTPFrameThread=new RTPFrameThread(*this);
					m_pRTPFrameThread->Start();
				}
			}
		}
	}

	void RemoveBaseRTPFrame(BaseRTPFrame*pBaseRTPFrame)
	{
		{
			XAutoLock l(m_csMapBaseRTPFrame);
			m_MapBaseRTPFrame.erase(pBaseRTPFrame);
		}
		{
			XAutoLock l(m_csRTPFrameThread);
			if (m_MapBaseRTPFrame.size()==0)
			{
				if (m_pRTPFrameThread)
				{
					m_pRTPFrameThread->Stop();
					delete m_pRTPFrameThread;
					m_pRTPFrameThread=NULL;
				}
			}
		}
	}
protected:
	virtual int OnRTPFrameThreadCallback(void)
	{
		int nRet=0;
		XAutoLock l(m_csMapBaseRTPFrame);
		MapBaseRTPFrame::iterator iter;
		for (iter=m_MapBaseRTPFrame.begin();iter!=m_MapBaseRTPFrame.end();++iter)
		{
			BaseRTPFrame*pBaseRTPFrame=iter->second;
			nRet|=(pBaseRTPFrame->DoSendData()?1:0);
		}
		return nRet;
	}
protected:
	typedef std::map<void*,BaseRTPFrame*>	MapBaseRTPFrame;

	MapBaseRTPFrame		m_MapBaseRTPFrame;
	XCritSec			m_csMapBaseRTPFrame;
	RTPFrameThread*		m_pRTPFrameThread;
	XCritSec			m_csRTPFrameThread;
};

static RTPFrameMgr g_RTPFrameMgr;

BaseRTPFrame::BaseRTPFrame(BaseRTPFrameCallback&rCallback,int nMaxBitrate)
:m_nPayloadType(34)
,m_nMaxRTPFrameSize(1400)
,m_nMaxRTPPayloadBufLen(m_nMaxRTPFrameSize-RTP_HEADER_LEN)

,m_nRTPTimestamp(0)
,m_ulSSRC(XGenerateSSRC())
,m_usSeqNo(0)

,m_rCallback(rCallback)
,m_nKeyFrame(0)
,m_nMaxBitrate(nMaxBitrate+(nMaxBitrate>>1))
{
	m_pFrameBuf=NULL;
	m_nFrameBufLen=0;

	m_nHeaderMargin=0;

	m_nLastRecvdRTPTime=0;
	m_bGotFrame=false;

	m_nBufferedFrameCount=0;
	m_nMaxPacketsToSend=1;
	m_nLastSentPacketTimestamp=0;
}

BaseRTPFrame::~BaseRTPFrame(void)
{

}

int BaseRTPFrame::Open(int nPayloadType,int nMaxRTPFrameSize,int nHeaderMargin)
{
	m_nHeaderMargin=nHeaderMargin;
	m_nPayloadType=nPayloadType;
	m_nMaxRTPFrameSize=nMaxRTPFrameSize;
	m_nMaxRTPPayloadBufLen=m_nMaxRTPFrameSize-RTP_HEADER_LEN-get_codec_header_len();
	return 0;
}

void BaseRTPFrame::Close(void)
{
	g_RTPFrameMgr.RemoveBaseRTPFrame(this);

	FlushRTPPackets();

	if (m_pFrameBuf!=NULL)
	{
		free(m_pFrameBuf);
		m_pFrameBuf=NULL;
	}
	m_nFrameBufLen=0;
}

void BaseRTPFrame::RunRTPThread(void)
{
	g_RTPFrameMgr.AddBaseRTPFrame(this);
}

void BaseRTPFrame::FlushRTPPackets(void)
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


int BaseRTPFrame::CalculateFrameSize(void)
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

int BaseRTPFrame::GetFrame(unsigned char*pFrameBuf,int nBufLen)
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


void BaseRTPFrame::SendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp)
{
	{
		XAutoLock l(m_csListRTPPacket);
		if (m_ListRTPPacket.size()>512)
		{
			XListPtr listRTPPackets;
			bool bFound=false;
			while (m_ListRTPPacket.size())
			{
				RTPFrame*pRTPFrame=(RTPFrame*)m_ListRTPPacket.front();
				m_ListRTPPacket.pop_front();
				if (!bFound)
				{
					PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPFrame->pBuffer;
					if (pRTPHeader->marker)
					{
						bFound=true;
						listRTPPackets.push_back(pRTPFrame);
					}
				}
				else
				{
					pRTPFrame->Release();
					pRTPFrame=NULL;
				}
			}

			while (listRTPPackets.size())
			{
				RTPFrame*pRTPFrame=(RTPFrame*)listRTPPackets.front();
				listRTPPackets.pop_front();
				m_ListRTPPacket.push_back(pRTPFrame);
			}
		}
	}

	m_nRTPTimestamp=ulTimestamp;

	DoSendFrame(pFrameData,nFrameLen,nWidth,nHeight,nKeyFrame,ulTimestamp);
}

void BaseRTPFrame::SendFrameHeader(unsigned char*pRTPPacket,int nPacketLen)
{
	//m_rCallback.OnBaseRTPFrameCallbackRTPPacket(pRTPPacket,nPacketLen);

	RTPFrame*pRTPFrame=new RTPFrame;
	if (pRTPFrame==NULL)
	{
		return;
	}
	pRTPFrame->pBuffer=malloc(nPacketLen);
	if (pRTPFrame->pBuffer!=NULL)
	{
		memcpy(pRTPFrame->pBuffer,pRTPPacket,nPacketLen);
		pRTPFrame->nLen=nPacketLen;

		XAutoLock l(m_csListRTPPacket);

		pRTPFrame->bFirstPacket=true;

		m_nBufferedFrameCount++;

		m_ListRTPPacket.push_back(pRTPFrame);
	}
	else
	{
		pRTPFrame->Release();
		pRTPFrame=NULL;
		return;
	}
}

void BaseRTPFrame::set_rtp_header(unsigned char*pHeader,bool bMarker)
{
	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pHeader;
	memset(pRTPHeader,0,RTP_HEADER_LEN);
	pRTPHeader->version=RTP_VERSION;
	pRTPHeader->marker=(bMarker?1:0);
	pRTPHeader->type=m_nPayloadType;
	pRTPHeader->sequence=htons(m_usSeqNo++);
	pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
	pRTPHeader->ssrc=htonl(m_ulSSRC);

	//if (bMarker)
	//{
		//m_nRTPTimestamp+=3003;
	//}
	//char szDebug[1024];
	//sprintf(szDebug,"nRTPTime=%u m_nLastRecvdRTPTime=%u\n",nRTPTime,m_nLastRecvdRTPTime);
	//OutputDebugString(szDebug);
}

void BaseRTPFrame::SendRTPPacket(RTPFrame*pRTPFrame)
{
	//m_rCallback.OnBaseRTPFrameCallbackRTPPacket(pRTPFrame->pBuffer,pRTPFrame->nLen);

	//pRTPFrame->Release();
	//pRTPFrame=NULL;

	XAutoLock l(m_csListRTPPacket);
	m_ListRTPPacket.push_back(pRTPFrame);
}


void BaseRTPFrame::OnRecvdRTPPacket(void*pPacketData,int nPacketLen)
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

int BaseRTPFrame::GetFrameTestOnly(unsigned char*pFrameBuf,int nBufLen)
{
	int nFrameLen=CalculateFrameSize();
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
			return 0;
		}

		nFrameLen=GetFrame(m_pFrameBuf+m_nHeaderMargin,m_nFrameBufLen-m_nHeaderMargin);

		memcpy(pFrameBuf,m_pFrameBuf+m_nHeaderMargin,nFrameLen);

		return nFrameLen;
	}
	return 0;
}

void BaseRTPFrame::PacketsToFrame(void)
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

		//char szDebug[1024]="";
		//sprintf(szDebug,"nKeyFrame=%d nFrameLen=%d\n",nKeyFrame,nFrameLen);
		//OutputDebugString(szDebug);

		m_rCallback.OnBaseRTPFrameCallbackFramePacket(this,m_pFrameBuf+m_nHeaderMargin,nFrameLen,nKeyFrame);
	}
}

bool BaseRTPFrame::DoSendData(void)
{
	bool bRet=false;
	int nSentPackets=0;
	unsigned long ulTimestamp=XGetTimestamp();

	if (m_nMaxBitrate<=1200)
	{
		if (ulTimestamp==m_nLastSentPacketTimestamp ||
			(ulTimestamp==(unsigned long)(m_nLastSentPacketTimestamp+1)) ||
			(ulTimestamp==(unsigned long)(m_nLastSentPacketTimestamp+2)))
		{
			return bRet;
		}
	}
	else if (m_nMaxBitrate<=2400)
	{
		if (ulTimestamp==m_nLastSentPacketTimestamp ||
			(ulTimestamp==(unsigned long)(m_nLastSentPacketTimestamp+1)))
		{
			return bRet;
		}
	}
	else
	{
		if (ulTimestamp==m_nLastSentPacketTimestamp)
		{
			return bRet;
		}
	}

	unsigned long nDuration=ulTimestamp-m_nLastSentPacketTimestamp;

	//char szDebug[1024];
	//sprintf(szDebug,"nBitrate=%d\n",nBitrate);
	//OutputDebugString(szDebug);

	int nMaxPacketsToSend=m_nMaxPacketsToSend;
	if (m_nMaxPacketsToSend==1)
		m_nMaxPacketsToSend=2;
	else
		m_nMaxPacketsToSend=1;


	RTPFrame*pRTPFrame=NULL;
	int nTotalLen=0;
	while (nSentPackets<nMaxPacketsToSend)
	{
		{
			XAutoLock l(m_csListRTPPacket);
			if (m_ListRTPPacket.size()>0)
			{
				pRTPFrame=(RTPFrame*)m_ListRTPPacket.front();

				//nTotalLen+=pRTPFrame->nLen;

				//int nBitrate=nTotalLen*8/nDuration;
				//if (nBitrate>m_nMaxBitrate)
				//{
				//	//char szDebug[1024];
				//	//sprintf(szDebug,"nBitrate=%d m_nMaxBitrate=%d nDuration=%u nTotalLen=%d nPackets=%d\n",nBitrate,m_nMaxBitrate,nDuration,nTotalLen,m_ListRTPPacket.size());
				//	//OutputDebugString(szDebug);
				//	return bRet;
				//}

				m_ListRTPPacket.pop_front();

				if (pRTPFrame->bFirstPacket)
				{
					m_nBufferedFrameCount--;
				}

				bRet=true;
			}
			else
			{
				break;
			}
		}

		m_rCallback.OnBaseRTPFrameCallbackRTPPacket(pRTPFrame->pBuffer,pRTPFrame->nLen);
		m_nLastSentPacketTimestamp=ulTimestamp;


		nSentPackets++;

		pRTPFrame->Release();
		pRTPFrame=NULL;

		bRet=true;
	}


	return bRet;
}

int BaseRTPFrame::GetPacketCount(void)
{
	XAutoLock l(m_csListRTPPacket);
	return m_ListRTPPacket.size();

}
