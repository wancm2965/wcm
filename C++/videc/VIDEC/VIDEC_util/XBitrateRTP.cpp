// XBitrateRTP.cpp: implementation of the XBitrateRTP class.
//
//////////////////////////////////////////////////////////////////////

#include "XBitrateRTP.h"
#include "XUtil.h"


#define XBITRATE_RTP_MIN_MS			1
#define XBITRATE_RTP_NORMAL_MS		20
#define XBITRATE_RTP_MAX_MS			40

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XBitrateRTP::XBitrateRTP()
{
	m_nTotalBytes=0;
	m_ulStartTimeStamp=XGetTimestamp();

	m_nLastBitrate=0;
}

XBitrateRTP::~XBitrateRTP()
{
	XAutoLock l(m_csListTimeFrame);
	while (m_listTimeFrame.size())
	{
		PTIME_FRAME pTimeFrame=(PTIME_FRAME)m_listTimeFrame.front();
		m_listTimeFrame.pop_front();

		delete pTimeFrame;
		pTimeFrame=NULL;
	}
}

int XBitrateRTP::GetBitrate(void)
{
	XAutoLock l(m_csListTimeFrame);

	unsigned long nCurTimestamp=XGetTimestamp();
	unsigned long ulDuration=nCurTimestamp-m_ulStartTimeStamp;
	if (ulDuration>=XBITRATE_RTP_MIN_MS)
	{
		m_nLastBitrate=m_nTotalBytes*8/ulDuration;
		while (m_listTimeFrame.size())
		{
			PTIME_FRAME pTimeFrame=(PTIME_FRAME)m_listTimeFrame.front();
			if (nCurTimestamp-pTimeFrame->nFrameTimestamp>=XBITRATE_RTP_NORMAL_MS)
			{
				m_listTimeFrame.pop_front();

				m_nTotalBytes-=pTimeFrame->nFrameLen;
				m_ulStartTimeStamp=pTimeFrame->nFrameTimestamp;

				delete pTimeFrame;
				pTimeFrame=NULL;
			}
			else
			{
				break;
			}
		}
	}

	return m_nLastBitrate;
}

void XBitrateRTP::Calculate(int nLen)
{
	XAutoLock l(m_csListTimeFrame);

	unsigned long nCurTimestamp=XGetTimestamp();

	NEWRECORD3( PTIME_FRAME ,pTimeFrame,new TIME_FRAME )
	NULLRecord( pTimeFrame )  
	pTimeFrame->nFrameLen=nLen;
	pTimeFrame->nFrameTimestamp=nCurTimestamp;
	m_listTimeFrame.push_back(pTimeFrame);

	m_nTotalBytes+=nLen;

	while (m_listTimeFrame.size())
	{
		PTIME_FRAME pTimeFrame=(PTIME_FRAME)m_listTimeFrame.front();
		if (nCurTimestamp-pTimeFrame->nFrameTimestamp>=XBITRATE_RTP_MAX_MS)
		{
			m_listTimeFrame.pop_front();

			m_nTotalBytes-=pTimeFrame->nFrameLen;
			m_ulStartTimeStamp=nCurTimestamp;

			delete pTimeFrame;
			pTimeFrame=NULL;
		}
		else
		{
			break;
		}
	}
}

