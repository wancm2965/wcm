
#include "DataUdp.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

/*------------------------------------------------------
*Define IP header
*-----------------------------------------------------*/
typedef struct ip_hdr
{
    unsigned char  ip_verlen;        // IP version & length
    unsigned char  ip_tos;           // IP type of service
    unsigned short ip_totallength;   // Total length
    unsigned short ip_id;            // Unique identifier 
    unsigned short ip_offset;        // Fragment offset field
    unsigned char  ip_ttl;           // Time to live
    unsigned char  ip_protocol;      // Protocol(TCP,UDP etc)
    unsigned short ip_checksum;      // IP checksum
    unsigned int   ip_srcaddr;       // Source address
    unsigned int   ip_destaddr;      // Destination address
}IP_HDR,*PIP_HDR,* LPIP_HDR;

// Define UDP header 
//
typedef struct udp_hdr
{
    unsigned short src_portno;       // Source port number
    unsigned short dst_portno;       // Destination port number
    unsigned short udp_length;       // UDP packet length
    unsigned short udp_checksum;     // UDP checksum (optional)
} UDP_HDR, *PUDP_HDR;



bool CDataUDP::SocketCreate(unsigned int dwProtocol)
{
	m_hSocket = socket(AF_INET, SOCK_RAW, dwProtocol);
	if( m_hSocket == INVALID_SOCKET ) return false;
	//Enable the IP header include option 
	BOOL bOpt = TRUE;
	int nRet = setsockopt(m_hSocket, IPPROTO_IP, IP_HDRINCL, (char *)&bOpt, sizeof(bOpt));
	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		return false;
	}
	return true;
}

unsigned short CDataUDP::checksum(unsigned short *pwbuffer, unsigned int size)
{
	unsigned long cksum=0;

	while (size > 1)
	{
		cksum += *pwbuffer++;
		size  -= sizeof(u16);   
	}
	if (size)
	{
		cksum += *(u8*)pwbuffer;   
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16); 

	return (u16)(~cksum); 
}

unsigned int CDataUDP::SendTo(unsigned int dwLocalIpAddr, unsigned short wLocalPort, unsigned int dwRemoteIpAddr, unsigned short wRemotePort, unsigned char *pbyBuf, unsigned int dwBufLen)
{
	IP_HDR m_tipHdr;
	UDP_HDR m_tudpHdr;
	sockaddr_in m_tAddrIn;

	int iIPVersion = 4;    
	int iIPSize = sizeof(m_tipHdr) / sizeof(unsigned long);
	int iUdpSize = sizeof(m_tudpHdr) + dwBufLen;
	int iTotalSize = sizeof(m_tipHdr) + sizeof(m_tudpHdr) + dwBufLen;

	// Initalize the IP header
	// IP version goes in the high order 4 bits of ip_verlen. The
	// IP header length (in 32-bit words) goes in the lower 4 bits.

	m_tipHdr.ip_verlen = (iIPVersion << 4) | iIPSize;
	m_tipHdr.ip_tos = 0;                         // IP type of service
	m_tipHdr.ip_totallength = htons(iTotalSize); // Total packet len
	m_tipHdr.ip_id = 0;                 // Unique identifier: set to 0
	m_tipHdr.ip_offset = 0;             // Fragment offset field
	m_tipHdr.ip_ttl = 128;              // Time to live
	m_tipHdr.ip_protocol = 0x11;        // Protocol(UDP) 
	m_tipHdr.ip_checksum = 0 ;          // IP checksum
	m_tipHdr.ip_srcaddr = dwLocalIpAddr;     // Source address ????
	m_tipHdr.ip_destaddr = dwRemoteIpAddr;      // Destination address

	// Initalize the UDP header

	m_tudpHdr.src_portno = htons(wLocalPort) ;
	m_tudpHdr.dst_portno = htons(wRemotePort) ;
	m_tudpHdr.udp_length = htons(iUdpSize) ;
	m_tudpHdr.udp_checksum = 0 ;

	char *pRawBuf = (char *)&m_RawBuf;
	int  iUdpChecksumSize = 0;

	memset(pRawBuf, 0 , sizeof(m_RawBuf));

	memcpy(pRawBuf, &m_tipHdr.ip_srcaddr,  sizeof(m_tipHdr.ip_srcaddr));  
	pRawBuf += sizeof(m_tipHdr.ip_srcaddr);
	iUdpChecksumSize += sizeof(m_tipHdr.ip_srcaddr);

	memcpy(pRawBuf, &m_tipHdr.ip_destaddr, sizeof(m_tipHdr.ip_destaddr)); 
	pRawBuf += sizeof(m_tipHdr.ip_destaddr);
	iUdpChecksumSize += sizeof(m_tipHdr.ip_destaddr);

	pRawBuf++;
	iUdpChecksumSize += 1;

	memcpy(pRawBuf, &m_tipHdr.ip_protocol, sizeof(m_tipHdr.ip_protocol)); 
	pRawBuf += sizeof(m_tipHdr.ip_protocol);
	iUdpChecksumSize += sizeof(m_tipHdr.ip_protocol);

	memcpy(pRawBuf, &m_tudpHdr.udp_length, sizeof(m_tudpHdr.udp_length)); 
	pRawBuf += sizeof(m_tudpHdr.udp_length);
	iUdpChecksumSize += sizeof(m_tudpHdr.udp_length);

	memcpy(pRawBuf, &m_tudpHdr, sizeof(m_tudpHdr)); 
	pRawBuf += sizeof(m_tudpHdr);
	iUdpChecksumSize += sizeof(m_tudpHdr);

	memcpy(pRawBuf, pbyBuf, dwBufLen);
	iUdpChecksumSize += dwBufLen;


	u16 cksum = checksum((u16 *)&m_RawBuf, iUdpChecksumSize);
	m_tudpHdr.udp_checksum = cksum;

	//
	// Now assemble the IP and UDP headers along with the data
	//  so we can send it

	pRawBuf = (char *)&m_RawBuf;
	memset(pRawBuf, 0 , sizeof(m_RawBuf));

	memcpy(pRawBuf, &m_tipHdr, sizeof(m_tipHdr));   
	pRawBuf += sizeof(m_tipHdr);
	memcpy(pRawBuf, &m_tudpHdr, sizeof(m_tudpHdr)); 
	pRawBuf += sizeof(m_tudpHdr);
	memcpy(pRawBuf, pbyBuf, dwBufLen);

	memset( &m_tAddrIn, 0, sizeof(m_tAddrIn));
	m_tAddrIn.sin_family	  = AF_INET; 
	m_tAddrIn.sin_addr.s_addr = dwRemoteIpAddr;
	m_tAddrIn.sin_port	      = htons(wRemotePort);

	int nRet = sendto( m_hSocket, (char*)&m_RawBuf, iTotalSize, 0,
		(sockaddr *)&m_tAddrIn, sizeof(SOCKADDR_IN) );

	return nRet;
}


CDataUDP::CDataUDP()
{	
	m_hSocket=INVALID_SOCKET;

	memset(m_szRemoteIP,0,64);
	m_nRemotePort=0;
	memset(m_szLocalIP,0,64);
	m_nLocalPort=0;

	memset(m_RawBuf,0,sizeof(m_RawBuf));
	m_nLocalIP =0;
	m_nRemoteIP =0;

}


bool CDataUDP::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	strcpy(m_szLocalIP,cszLocalIP);
	strcpy(m_szRemoteIP,cszRemoteIP);
	m_nLocalPort=nLocalPort;
	m_nRemotePort=nRemotePort;
	m_nLocalIP =inet_addr(m_szLocalIP);
	m_nRemoteIP =inet_addr(m_szRemoteIP);
	SocketCreate(0);
	return (m_hSocket != INVALID_SOCKET);
}

void CDataUDP::Close(void)
{
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}
}

int CDataUDP::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	if(m_hSocket != INVALID_SOCKET)
	{
		nSendlen=SendTo(m_nLocalIP,m_nLocalPort,m_nRemoteIP,m_nRemotePort,(u8 *)pData,nLen);
	}
	return nSendlen;
}
