// JitterBuffer.h: interface for the JBBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __JITTERBUFFER_H__
#define __JITTERBUFFER_H__

#include <NETEC/XAutoLock.h>
#include <NETEC/XListPtr.h>
#include <NETEC/XPacketStatistics.h>

#define JB_MAX_DELAY_VARIATION		2560
#define JB_MIN_DELAY_VARIATION		60//120
		
class NETEC_API JBBase
	: public XPacketStatistics
{
public:
	JBBase(void);
	virtual~JBBase(void);
public:
	virtual int32_t OnRecvdPacket(unsigned short usSeq,uint32_t ulRTPTimeMS,const unsigned char*pPacketData,int32_t nPacketLen);

	virtual bool DoTick(void);

	virtual void SetDelayVariation(uint32_t nDelayVariation);

	virtual void OnLostPackets(unsigned short usLostPackets,unsigned short usFromSeqNo,unsigned short usToSeqNo);

	virtual void OnJBFilteredPacket(const unsigned char*pPacketData,int32_t nPacketLen){};
	virtual bool IsPacketJB(void)=0;
	
protected:
	virtual void ClearPackets(void); 

	typedef struct NETEC_API tagPACKET
	{
		bool			bDiscontinous;
		unsigned short	usSeq;
		uint32_t	ulRTPTimeMS;
		uint32_t	ulTimestampNow;
		unsigned char*	pPacketData;
		int32_t				nPacketLen;
	}PACKET,*PPACKET;

	virtual void OnStreamPacket(PPACKET pPacket){};
protected:
	uint32_t	m_nDelayVariation;
	uint32_t	m_nLastSentPacketRTPTimeMS;
	uint32_t	m_nNewRecvdPacketRTPTimeMS;


	XListPtr		m_listPacket;
	XCritSec		m_csListPacket;

	unsigned short	m_usLastGoodSequence;
	bool			m_bGotFirstFrame;

	uint32_t	m_nCurMaxDelayVariation;
	uint32_t	m_ulLastCurMaxDelayVariationTimestamp;
	bool			m_bResetDelayVariation;
	int32_t				m_nCurMaxDelayVariationPacketCount;

	uint32_t	m_ulLastRecvdPacketTimestamp;
	uint32_t	m_ulLastRecvdPacketRTPTimeMS;
};


class NETEC_API JBFrame
	: public JBBase
{
public:
	JBFrame(void);
	virtual~JBFrame(void);
public:
	virtual int32_t OnRecvdRepairedPacket(unsigned short usFromSeqNo,unsigned short usToSeqNo,const unsigned char*pPacketData,int32_t nPacketLen);
	virtual void OnJBAckPackets(unsigned short usFromSeqNo,unsigned short usToSeqNo)=0;
	virtual void OnJBFrameData(unsigned char*pData,int32_t nLen)=0;
protected:
	virtual bool IsPacketJB(void);
	virtual void OnStreamPacket(PPACKET pPacket);
protected:
	XListPtr		m_listStreamPackets;
	XCritSec		m_csListStreamPackets;
	unsigned char*	m_pFrameBuf;
	int32_t				m_nFrameBufLen;

};

		
class NETEC_API JBPacket
	: public JBBase
{
public:
	JBPacket(void);
	virtual~JBPacket(void);
public:
	virtual int32_t OnRecvdPacket(unsigned short usSeq,uint32_t ulRTPTimeMS,const unsigned char*pPacketData,int32_t nPacketLen);
	virtual void ResetJBPacket(void);
protected:
	virtual bool IsPacketJB(void);
};

#endif