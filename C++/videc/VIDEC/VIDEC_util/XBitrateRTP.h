// XBitrateRTP.h: interface for the XBitrateRTP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XBITRATERTP_H__
#define __XBITRATERTP_H__

#include "XAutoLock.h"
#include "XListPtr.h"

class XBitrateRTP
{
public:
	XBitrateRTP();
	virtual ~XBitrateRTP();
public:
	int GetBitrate(void);
	void Calculate(int nLen);
protected:
	typedef struct tagTIME_FRAME
	{
		unsigned long	nFrameTimestamp;
		long			nFrameLen;
	}TIME_FRAME,*PTIME_FRAME;

	XCritSec		m_csListTimeFrame;
	XListPtr		m_listTimeFrame;

	long			m_nTotalBytes;
	unsigned long	m_ulStartTimeStamp;
	int				m_nLastBitrate;
};

#endif 