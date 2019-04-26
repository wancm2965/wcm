// XBitrate.h: interface for the XBitrate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XBITRATE_H__
#define __XBITRATE_H__

class XBitrate
{
public:
	XBitrate();
	virtual ~XBitrate();
public:
	int GetBitrate(void);
	void Calculate(int nLen);
protected:
	unsigned long	m_ulTotalBytes;
	unsigned long	m_ulStartTimeStamp;
	int				m_nLastBitrate;
};

#endif 