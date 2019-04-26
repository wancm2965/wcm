// AVSCTPSessionSender.h: interface for the AVSCTPSessionSender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVSCTPSESSIONSENDER_H__
#define __AVSCTPSESSIONSENDER_H__

#define XMC_UDP_MAX_WINDOW_SIZE			96
#define XMC_UDP_MIN_WINDOW_SIZE			4

#include "AVSCTPHeader.h"
#include "ListPtr.h"
#include "AVAutoLock.h"

class AFX_EXT_CLASS AVSCTPSessionSenderNotify
{
public:
	AVSCTPSessionSenderNotify(void){};
	virtual~AVSCTPSessionSenderNotify(void){};
public:
	virtual void OnSendOutStreamPacket(char*pStreamPacketData,int nStreamPacketLen)=0;//send out data with socket
	virtual AVSCTPPacket*GetAVSCTPPacket(void)=0;
};

class AFX_EXT_CLASS AVSCTPSession;
class AFX_EXT_CLASS AVSCTPSessionSender
{
public:
	AVSCTPSessionSender(AVSCTPSession&rAVSCTPSession);
	virtual~AVSCTPSessionSender(void);
public:
	bool Connect(void);
	void ReleaseConnections(void);
	void ProcessACK(bool bFlag,unsigned short usSequence,unsigned char ucWindowSize);
	bool DoSendData(unsigned long ulRTT);
	bool HasSendData(void);
protected:
	AVSCTPSession&					m_rAVSCTPSession;
	ListPtr							m_listAVSCTPPacket;
	AVCritSec						m_AVCritSecListAVSCTPPacket;
	unsigned short					m_usSequence;
	unsigned long					m_ulCurrentWindowSize;

	unsigned long					m_ulTotalResentCount;
	unsigned long					m_ulResentCountIndex;
};

#endif 
