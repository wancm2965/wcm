// XBitrateExt.cpp: implementation of the XBitrateExt class.
//
//////////////////////////////////////////////////////////////////////

#include "XBitrateExt.h"
#include "XUtil.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XBitrateExt::XBitrateExt()
{
	m_nTotalBytes=0;
	m_ulStartTimeStamp=XGetTimestamp();

	m_nLastBitrate=0;
}

XBitrateExt::~XBitrateExt()
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

int XBitrateExt::GetBitrate(void)
{
	XAutoLock l(m_csListTimeFrame);

	unsigned long nCurTimestamp=XGetTimestamp();
	unsigned long ulDuration=nCurTimestamp-m_ulStartTimeStamp;
	if (ulDuration>=200)
	{
		m_nLastBitrate=m_nTotalBytes*8/ulDuration;
		while (m_listTimeFrame.size())
		{
			PTIME_FRAME pTimeFrame=(PTIME_FRAME)m_listTimeFrame.front();
			if (nCurTimestamp-pTimeFrame->nFrameTimestamp>=1000)
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

void XBitrateExt::Calculate(int nLen)
{
	XAutoLock l(m_csListTimeFrame);

	unsigned long nCurTimestamp=XGetTimestamp();

	NEWRECORD3( PTIME_FRAME ,pTimeFrame ,new TIME_FRAME )
		NULLRecord( pTimeFrame )  
	pTimeFrame->nFrameLen=nLen;
	pTimeFrame->nFrameTimestamp=nCurTimestamp;
	m_listTimeFrame.push_back(pTimeFrame);

	m_nTotalBytes+=nLen;

	while (m_listTimeFrame.size())
	{
		PTIME_FRAME pTimeFrame=(PTIME_FRAME)m_listTimeFrame.front();
		if (nCurTimestamp-pTimeFrame->nFrameTimestamp>=2000)
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

