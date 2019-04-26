//UDPServerPacket.h

#ifndef __UDPSERVERPACKET_H__
#define __UDPSERVERPACKET_H__

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

class NETEC_API UDPServerPacket
{
public:
	UDPServerPacket(SOCKET hSocket,struct sockaddr*addr,int32_t addrlen,const char*pData,int32_t nDataLen);
	UDPServerPacket(SOCKET hSocket,const int32_t&af,int32_t nBufLen);
	virtual~UDPServerPacket();
public:
	char*GetData(void);
	int32_t GetDataLen(void);
	SOCKET GetSocketHandle(void);

	int32_t getaf(void);
	struct sockaddr*getaddr(void);
	int32_t getaddrlen(void);

	int32_t Send(void);
	int32_t Recv(void);
protected:
	iovec			m_PacketVector;
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
