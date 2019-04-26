//XPacketUDP.h

#ifndef __XPACKETUDP_H__
#define __XPACKETUDP_H__

#ifndef _WIN32
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#else
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef NETEC_ARM

#else
#include <wspiapi.h>
#endif

#ifndef IOVEC
#define IOVEC
#include "NETEC_TYPES.h"
struct iovec
{
	int32_t		iov_len;
	char*	iov_base;
};
#endif

#endif

#include <NETEC/XSocket.h>

class XPacketUDP
{
public:
	XPacketUDP(SOCKET hSocket,struct sockaddr*addr,int32_t addrlen,unsigned char nAppType,unsigned char nCmd,unsigned short nSeqNo,uint32_t nDstID,const char*pData,int32_t nDataLen);
	XPacketUDP(SOCKET hSocket,const int32_t&af,int32_t nBufLen);
	virtual~XPacketUDP();
public:
	unsigned char GetAppType(void);
	unsigned char GetCmd(void);
	unsigned short GetSeqNo(void);
	uint32_t GetDstID(void);
	char*GetData(void);
	int32_t GetDataLen(void);
	SOCKET GetSocketHandle(void);

	int32_t getaf(void);
	struct sockaddr*getaddr(void);
	int32_t getaddrlen(void);

	void GetDstIP(char*szIP,int32_t nLen);
	void GetLocalIPAddress(char*szIP,int32_t nLen);

	int32_t Send(void);
	int32_t Recv(void);
protected:
#define UDPHEADER_LEN	8
	struct UDPHEADER
	{
		unsigned short	nAppType:8;
		unsigned short	nCmd:8;
		unsigned short	nSeqNo;
		uint32_t	nDstID;
	};

	UDPHEADER		m_UDPHeader;
	iovec			m_PacketVector[2];             // The 2-demension vector of UDP packet [header, data]
	char*			m_pData;
	int32_t				m_nDataLen;
	int32_t				m_nBufLen;

	SOCKET			m_hSocket;
	
	struct sockaddr*m_pAddr;
	int32_t				m_nAddrLen;

	struct sockaddr_in m_saIPV4;
	struct sockaddr_in6 m_saIPV6;

	int32_t					m_af;
};



#endif
