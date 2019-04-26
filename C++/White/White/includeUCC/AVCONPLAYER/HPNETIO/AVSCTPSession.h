// AVSCTPSession.h: interface for the AVSCTPSession class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XMCPIPE_H__
#define __XMCPIPE_H__

#include "AVBufferPool.h"
#include "AVSCTPHeader.h"
#include "AVSCTPSessionSender.h"
#include "AVSCTPSessionReceiver.h"

class AFX_EXT_CLASS AVSCTPSessionNotify
{
public:
	AVSCTPSessionNotify(void){};
	virtual~AVSCTPSessionNotify(void){};
public:
	virtual void OnAVSCTPSessionNotifyReceivedFrame(const char*pData,int nLen)=0;
	virtual void OnAVSCTPSessionNotifySendData(const char*pData,int nLen)=0;
};

class AFX_EXT_CLASS AVSCTPSession  
{
public:
	AVSCTPSession(AVSCTPSessionNotify&rAVSCTPSessionNotify,bool bFixedMTU=false);
	virtual ~AVSCTPSession(void);
public:
	bool Connect(void);
	void ReleaseConnections(void);
	bool SendFrameData(const char*pData,int nLen);
	bool CanSendFrameData(void);
	bool HasSendData(void);
	bool DoSendData(void);
	void SetRTT(unsigned long ulRTT);
	void SetMTU(unsigned long ulMTU);
	void SetReceiverWindowSize(unsigned char ucWindowSize);
	unsigned char GetReceiverWindowSize(void);
	void OnReceivedPacket(const char*pData,int nLen);

	//called by Sender
	AVSCTPPacket*GetAVSCTPPacket(void);
	void OnSendingPacketData(AVSCTPPacket*pAVSCTPPacket);

	void OnReceivedOrderedPacketData(AVSCTPPacket*pAVSCTPPacket);
	void SendPacketACK(bool bFlag,unsigned short usSequence);
protected:
	void OnReceivedStreamPacket(AVSCTPPacket tAVSCTPPacket);
	void OnReceivedStreamACK(AVSCTPPacket tAVSCTPPacket);
	void SendOutData(char*pData,int nDataLen);
protected:
	AVSCTPSessionNotify&	m_rAVSCTPSessionNotify;

	ListPtr					m_listAVSCTPPacket;
	AVCritSec				m_AVCritSecListAVSCTPPacket;

	AVSCTPSessionSender*	m_pAVSCTPSessionSender;
	AVSCTPSessionReceiver*	m_pAVSCTPSessionReceiver;

	AVSCTPPacket			m_ackAVSCTPPacket;

	//slice to frame
	char*					m_pStreamDataBuffer;
	int						m_nStreamDataBufferLength;
	int						m_nStreamDataSize;

	unsigned long			m_ulRTT;
	unsigned long			m_ulMTU;
	bool					m_bFixedMTU;

	unsigned char			m_ucReceiverWindowSize;
};

#endif