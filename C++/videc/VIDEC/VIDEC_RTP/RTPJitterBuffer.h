//RTPJitterBuffer.h
#ifndef __RTPJITTERBUFFER_H__
#define __RTPJITTERBUFFER_H__

#include "XListPtr.h"
#include "XAutoLock.h"
#include "RTPTick.h"
#include "RTPBuffer.h"

class RTPJitterBufferOut
	: public RTPTick
{
public:
	RTPJitterBufferOut(void);
	virtual ~RTPJitterBufferOut(void);
public:
	virtual int Open(void);
	virtual void Close(void);

	virtual void SetMaxBitrate(int nMaxBitrate);
	virtual int OnRTPPacketIn(RTPBuffer*pRTPBuffer);
	virtual int GetRTPBufferCount(void);

	virtual void OnRTPPacketOut(RTPBuffer*pRTPBuffer)=0;
protected:
	virtual bool DoTick(void);
protected:
	XListPtr	m_listRTPBuffer;
	XCritSec	m_csListRTPBuffer;

	int			m_nMaxBitrate;
	int			m_nLeftBytes;

	unsigned long	m_nLastSendingDataTimestamp;

protected:
	virtual RTPBuffer* GetRTPBuffer(void);
};

#endif