#include "XSocketUDP.h"

XSocketUDP::XSocketUDP(void)
{

}

XSocketUDP::~XSocketUDP(void)
{

}

int XSocketUDP::Open(const char*cszLocalIP,unsigned short nLocalPort,bool bReuseAddr)
{
	// construct an socket
	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_hSocket < 0)
	{
		return -1;
	}

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port   = htons(nLocalPort);
	addr.sin_addr.s_addr = htonl(XSocket::atoh(cszLocalIP));

	socklen_t namelen = sizeof(sockaddr_in);

	if (bReuseAddr)
	{
		int option = 1;
		setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR,(char *)&option, sizeof(option));
	}

	if (0 != bind(m_hSocket, (sockaddr *)&addr, namelen))
	{
		return -1;
	}

	SetOpt();

	return 0;
}

int XSocketUDP::Connect(const char*cszHost,unsigned short usPort)
{
	int bRet=-1;

	if((m_hSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))==INVALID_SOCKET)
	{
		m_hSocket=INVALID_SOCKET;
		return bRet;
	}
	
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(XSocket::atoh(cszHost));
	addr.sin_port = htons(usPort);
	int nError=connect(m_hSocket,(sockaddr *) &addr, sizeof(addr));
	if (nError==0)
		bRet=0;

	return bRet;

}

void XSocketUDP::Close(void)
{
#ifndef _WIN32
	if (m_hSocket!=INVALID_SOCKET)
	{
		shutdown(m_hSocket,SHUT_RDWR);
	}
#endif
	XSocket::Close();
}

void XSocketUDP::SetOpt()
{
	//int nSendBufSize=1*1024*1024;//65536;
	int nRecvBufSize=16*1024*1024;//8192*1500;
	int nSendBufSize=65536;
	//int nRecvBufSize=8192*1500;
	if ((0 != setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSize, sizeof(int))) ||
		(0 != setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nSendBufSize, sizeof(int))))
	{
	}

	//int nTemp=0;
	//int nTempLen=sizeof(int);
	//getsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nTemp, &nTempLen);
	//getsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nTemp, &nTempLen);
	//nTemp=nTemp;

//	timeval tv;
//	tv.tv_sec = 0;
//#if defined (BSD) || defined (OSX)
//	// Known BSD bug as the day I wrote this code.
//	// A small time out value will cause the socket to block forever.
//	tv.tv_usec = 10000;
//#else
//	tv.tv_usec = 100;
//#endif
//
//#ifdef UNIX
//	// Set non-blocking I/O
//	// UNIX does not support SO_RCVTIMEO
//	int opts = fcntl(m_hSocket, F_GETFL);
//	if (-1 == fcntl(m_hSocket, F_SETFL, opts | O_NONBLOCK))
//	{
//
//	}
//#elif WIN32
//	DWORD ot = 1; //milliseconds
//	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ot, sizeof(DWORD)) < 0)
//	{
//		
//	}
//#else
//	// Set receiving time-out value
//	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval)) < 0)
//	{
//		
//	}
//#endif
}