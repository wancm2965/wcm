
/************************************************************************/
//DataUdp.h
//Copyright(c):AVCON
//Author:Gao
//Notice:Send Data RAW UDP Net Interface
//Date:2014-01-13
/************************************************************************/

#ifndef __SIP_RAW_UDP_H__
#define __SIP_RAW_UDP_H__

#include <winsock2.h>
#include <WS2TCPIP.H>

/************************************************************************/
//CDataUDP
//Description:Send Data RAW UDP Net Interface
/************************************************************************/

class CDataUDP
{
public:
	CDataUDP();
	virtual~CDataUDP(void){};
public:

	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	char m_szRemoteIP[64];
	unsigned short m_nRemotePort;
	char m_szLocalIP[64];
	unsigned short m_nLocalPort;
	unsigned long m_nLocalIP;
	unsigned long m_nRemoteIP;

protected:
	bool SocketCreate(unsigned int dwProtocol);
	unsigned short checksum(unsigned short *pwbuffer, unsigned int size);
	unsigned int SendTo(unsigned int dwLocalIpAddr, unsigned short wLocalPort, unsigned int dwRemoteIpAddr, unsigned short wRemotePort, unsigned char *pbyBuf, unsigned int dwBufLen);

	SOCKET m_hSocket;
	char m_RawBuf[256];


};


#endif