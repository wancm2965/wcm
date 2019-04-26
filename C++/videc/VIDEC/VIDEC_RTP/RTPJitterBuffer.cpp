#include "RTPJitterBuffer.h"
#include "RTPBuffer.h"
#include "XUtil.h"


//Kbps
RTPJitterBufferOut::RTPJitterBufferOut(void)
:m_nLastSendingDataTimestamp(0)
,m_nLeftBytes(0)
{
	SetMaxBitrate(0);
}

RTPJitterBufferOut::~RTPJitterBufferOut(void)
{
	{
		XAutoLock l(m_csListRTPBuffer);
		while (m_listRTPBuffer.size()>0)
		{
			RTPBuffer* pRTPBuffer=(RTPBuffer*)m_listRTPBuffer.front();
			m_listRTPBuffer.pop_front();

			pRTPBuffer->Release();
			pRTPBuffer=NULL;
		}
	}
}

int RTPJitterBufferOut::Open(void)
{
	return RTPTick::Open();
}

void RTPJitterBufferOut::Close(void)
{
	RTPTick::Close();

	{
		XAutoLock l(m_csListRTPBuffer);
		while (m_listRTPBuffer.size()>0)
		{
			RTPBuffer* pRTPBuffer=(RTPBuffer*)m_listRTPBuffer.front();
			m_listRTPBuffer.pop_front();

			pRTPBuffer->Release();
			pRTPBuffer=NULL;
		}
	}
}

void RTPJitterBufferOut::SetMaxBitrate(int nMaxBitrate)
{
	if (nMaxBitrate<=0)
	{
		nMaxBitrate=10240;
	}

	m_nMaxBitrate=nMaxBitrate;//(nMaxBitrate>>3);
}

bool RTPJitterBufferOut::DoTick(void)
{
	bool bRet=false;

	unsigned long nTimestamp=XGetTimestamp();
	unsigned long nDuration=nTimestamp-m_nLastSendingDataTimestamp;
	if (nDuration>0)
	{

#ifdef _DEBUG
		if (nDuration>20)
			nDuration=20;
#else
		if (nDuration>4)
			nDuration=4;
#endif

		int nLeftBytes=nDuration*m_nMaxBitrate;
		nLeftBytes>>=3;
		if (nLeftBytes+m_nLeftBytes<=0)
		{
			nLeftBytes+=m_nLeftBytes;
			m_nLeftBytes=nLeftBytes;

			if (m_nLeftBytes<=-1500)
			{
				m_nLeftBytes=0;
			}

			m_nLastSendingDataTimestamp=nTimestamp;

			return bRet;
		}


		//char szDebug[1024];
		//sprintf(szDebug,"nLeftBytes=%d nDuration=%u m_nMaxBitrate=%d\n",nLeftBytes,nDuration,m_nMaxBitrate);
		//DebugOutput(szDebug);
		int nCount=0;

		RTPBuffer* pRTPBuffer=GetRTPBuffer();
		while (pRTPBuffer!=NULL)
		{
			nCount++;

			if (!bRet)
			{
				bRet=true;
				nLeftBytes+=m_nLeftBytes;
			}

			int nLen=pRTPBuffer->GetLen();
			OnRTPPacketOut(pRTPBuffer);

			nLeftBytes-=nLen;

			pRTPBuffer->Release();
			pRTPBuffer=NULL;

			if (nLeftBytes<=0)
			{
				break;
			}
			pRTPBuffer=GetRTPBuffer();
		}

		if (bRet)
		{
			m_nLeftBytes=nLeftBytes;
			if (m_nLeftBytes<=-1500)
			{
				m_nLeftBytes=0;
			}

			m_nLastSendingDataTimestamp=nTimestamp;

			//char szDebug[1024];
			//sprintf(szDebug,"nCount=%d\n",nCount);
			//DebugOutput(szDebug);
		}
	}

	return bRet;
}

RTPBuffer*RTPJitterBufferOut::GetRTPBuffer(void)
{
	XAutoLock l(m_csListRTPBuffer);
	if (m_listRTPBuffer.size()>0)
	{
		RTPBuffer* pRTPBuffer=(RTPBuffer*)m_listRTPBuffer.front();
		m_listRTPBuffer.pop_front();
		return pRTPBuffer;
	}
	return NULL;
}

int RTPJitterBufferOut::OnRTPPacketIn(RTPBuffer*pRTPBuffer)
{
	if (pRTPBuffer==NULL)
	{
		return -1;
	}
	pRTPBuffer->AddRef();

	XAutoLock l(m_csListRTPBuffer);
	m_listRTPBuffer.push_back(pRTPBuffer);


	return 0;
}

int RTPJitterBufferOut::GetRTPBufferCount(void)
{
	XAutoLock l(m_csListRTPBuffer);
	return m_listRTPBuffer.size();
}

