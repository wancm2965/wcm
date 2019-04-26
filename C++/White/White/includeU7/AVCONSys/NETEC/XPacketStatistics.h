//XPacketStatistics.h
#ifndef __XPACKETSTATISTICS_H__
#define __XPACKETSTATISTICS_H__

#include <NETEC/XAutoLock.h>

class NETEC_API XPacketStatistics
{
public:
	XPacketStatistics(void);
	virtual~XPacketStatistics(void);
public:
	void GetStatistics(uint32_t&ulTotalPackets,uint32_t&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate);
	
	void IncreaseTotalPackets(uint32_t ulPackets);
	void IncreaseLostPackets(uint32_t ulPackets);

	uint32_t GetLostPackets(void);
protected:
	typedef struct NETEC_API PACKET_ITEM
	{
		PACKET_ITEM(void)
		:m_ulTotalPackets(0)
		,m_ulLostPackets(0)
		{
		};
		
		uint32_t	m_ulTotalPackets;
		uint32_t	m_ulLostPackets;
	}PACKET_ITEM;
	
	PACKET_ITEM			m_PacketItem;		
	PACKET_ITEM			m_PacketItems[2];
	uint32_t		m_ulCurIndex;
	uint32_t		m_ulNextIndex;
	uint32_t		m_ulLastIndexTimestamp;

	XCritSec			m_csPacketItem;
};	

#endif