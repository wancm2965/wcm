#ifndef __AVSYN_H__
#define __AVSYN_H__

class AVCONEC40_AFX_EXT_CLASS AVSyn
{
public:
	AVSyn(void);
	virtual ~AVSyn(void);
public:
	void SetTimestamp(unsigned long ulTimestamp);
	unsigned long GetTimestamp(void);
	bool HasUpdateTimestamp(void);
	void Reset(void);
protected:
	unsigned long				m_ulTimestamp;
	unsigned long				m_ulUpdateTimestamp;
	AVCritSec					m_AVCritSec;
};

#endif