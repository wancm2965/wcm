// AVSCTPSessionReceiver.h: interface for the AVSCTPSessionReceiver class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVSCTPSSSSIONRECEIVER_H__
#define __AVSCTPSSSSIONRECEIVER_H__

#include "AVSCTPHeader.h"
#include "ListPtr.h"
#include "AVAutoLock.h"

class AFX_EXT_CLASS AVSCTPSession;
class AFX_EXT_CLASS AVSCTPSessionReceiver
{
public:
	AVSCTPSessionReceiver(AVSCTPSession&rAVSCTPSession);
	virtual~AVSCTPSessionReceiver(void);
public:
	bool Connect(void);
	void ReleaseConnections(void); 
	void ParseStreamPacket(AVSCTPPacket tAVSCTPPacket);
protected:
	AVSCTPSession&					m_rAVSCTPSession;
	ListPtr							m_listAVSCTPPacket;
	AVCritSec						m_AVCritSecListAVSCTPPacket;
	unsigned short					m_usGoodSequence;
	bool							m_bGotFirstPacket;
};


#endif 
