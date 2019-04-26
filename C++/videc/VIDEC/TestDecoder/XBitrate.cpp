// XBitrate.cpp: implementation of the XBitrate class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "XBitrate.h"
#include "XUtil.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XBitrate::XBitrate()
{
	m_ulTotalBytes=0;
	m_ulStartTimeStamp=XGetTimestamp();

	m_nLastBitrate=0;
}

XBitrate::~XBitrate()
{
	m_ulTotalBytes=0;
	m_ulStartTimeStamp=XGetTimestamp();
}

int XBitrate::GetBitrate(void)
{
	unsigned long ulDuration=XGetTimestamp()-m_ulStartTimeStamp;
	if (ulDuration>=2000)
	{
		m_nLastBitrate=m_ulTotalBytes*8/ulDuration;
		m_ulTotalBytes=0;
		m_ulStartTimeStamp=XGetTimestamp();
	}

	return m_nLastBitrate;
}

void XBitrate::Calculate(int nLen)
{
	m_ulTotalBytes+=nLen;
}

