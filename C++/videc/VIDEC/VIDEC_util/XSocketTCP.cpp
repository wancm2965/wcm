// XSocketTCP.cpp: implementation of the XSocketTCP class.
//
//////////////////////////////////////////////////////////////////////

#include "XSocketTCP.h"


/*
ulimit -n 50000 
echo 1024 65000 > /proc/sys/net/ipv4/ip_local_port_range
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XSocketTCP::XSocketTCP(void)
{

}

XSocketTCP::~XSocketTCP(void)
{

}

bool XSocketTCP::Bind(unsigned long ulIP,unsigned short usPort)
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		return false;
	}

	int nEnable = 1;
	setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR,
			   (char *)&nEnable, sizeof(nEnable));

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr=htonl(ulIP);
	sa.sin_port=htons(usPort);

	if (bind(m_hSocket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		return false;
	}

	if (listen(m_hSocket, 200) == -1)
	{
		return false;
	}

	return true;
}

bool XSocketTCP::Connect(unsigned long ulIP,unsigned short usPort,unsigned long ulInterface,bool bNoneBlocking)
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		return false;
	}

	if (ulInterface)
	{
		int option = 1;
		setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR,
				(char *)&option, sizeof(option));

		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr=htonl(ulInterface);
		sa.sin_port=htons(0);

		if (bind(m_hSocket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			//return false;
		}
	}

	if (bNoneBlocking)
	{
		XSocket::SetNoneBlocking(true);
	}

	int nEnable = 1;
	setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE,
			   (char *)&nEnable, sizeof(nEnable));
	setsockopt( m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nEnable, sizeof(nEnable) );


	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(usPort);
	sa.sin_addr.s_addr=htonl(ulIP);
	memset(&(sa.sin_zero), '\0', 8);

	if (-1 == connect(m_hSocket, (sockaddr*)&sa, sizeof(sa)))
	{
		if (!bNoneBlocking)
			return false;

#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (WSAEWOULDBLOCK!=dwError && EINTR!=dwError && EAGAIN!=dwError && dwError!=WSAEINTR && dwError!=WSAETIMEDOUT)
		{
			return false;
		}
#else
		if (errno!=0 && errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINPROGRESS && errno!=ETIMEDOUT && EINTR!=errno)
		{
			return false;
		}
#endif
	}

	int err=0;
	int nValue=160;
	err=setsockopt(
				m_hSocket,
#ifdef _WIN32
				IPPROTO_IP,
#else
				SOL_IP,
#endif
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}

	int sock_buf_size = 320*1024;

	err = setsockopt( m_hSocket, SOL_SOCKET, SO_SNDBUF,
					(char *)&sock_buf_size, sizeof(sock_buf_size) );

	err = setsockopt( m_hSocket, SOL_SOCKET, SO_RCVBUF,
					(char *)&sock_buf_size, sizeof(sock_buf_size) );

	struct linger ling; 
	ling.l_onoff=1; 
	ling.l_linger=0;/* (容许逗留的时间为5秒) */
	err=setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,(const char*)&ling,sizeof(ling));


	return true;
}

int XSocketTCP::Receive(char*pBuffer,int nLen)
{
	int nRet = recv(m_hSocket,pBuffer,nLen,MSG_NOSIGNAL);

	if (nRet<=0)
	{
#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (WSAEWOULDBLOCK!=dwError && EINTR!=dwError && EAGAIN!=dwError && dwError!=WSAEINTR && dwError!=WSAETIMEDOUT)
		{
			return -1;
		}
		else
		{
			return 0;
		}
#else
		if (errno!=0 && errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINPROGRESS && errno!=ETIMEDOUT && EINTR!=errno)
		{
			return -1;
		}
		else
		{
			return 0;
		}
#endif
	}

	return nRet;
}

int XSocketTCP::Send(const char*pData,int nLen)
{
	int nRet = 0;
	nRet=send(m_hSocket,pData,nLen,MSG_NOSIGNAL);

	if (nRet<=0)
	{
#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (dwError!=0 && WSAEWOULDBLOCK!=dwError && EINTR!=dwError && EAGAIN!=dwError && dwError!=WSAEINTR && dwError!=WSAETIMEDOUT)
		{
			return -1;
		}
		else
		{
			return 0;
		}
#else
		if (errno!=0 && errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINPROGRESS && errno!=ETIMEDOUT && EINTR!=errno)
		{
			return -1;
		}
		else
		{
			return 0;
		}
#endif
	}

	return nRet;
}

SOCKET XSocketTCP::Accept(void)
{
	socklen_t nLengthAddr=sizeof(sockaddr);
	sockaddr sa;
	memset(&sa,0,nLengthAddr);

	while (true)
	{
		SOCKET hSocket=accept(m_hSocket,&sa,&nLengthAddr);
		if (hSocket!=INVALID_SOCKET)
		{
			int err=0;
			int nValue=160;
			err=setsockopt(
						hSocket,
#ifdef _WIN32
						IPPROTO_IP,
#else
						SOL_IP,
#endif
						IP_TOS,
						(char*)&nValue,
						sizeof(nValue));
			if (err != 0)
			{
				nValue=0;
			}

			int nEnable = 1;
			setsockopt( hSocket, SOL_SOCKET, SO_KEEPALIVE,(char *)&nEnable, sizeof(nEnable));
			setsockopt( hSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nEnable, sizeof(nEnable) );

			struct linger ling; 
			ling.l_onoff=1; 
			ling.l_linger=0;/* (容许逗留的时间为5秒) */
			err=setsockopt(hSocket,SOL_SOCKET,SO_LINGER,(const char*)&ling,sizeof(ling));

			int sock_buf_size = 320*1024;

			err = setsockopt( hSocket, SOL_SOCKET, SO_SNDBUF,
							(char *)&sock_buf_size, sizeof(sock_buf_size) );

			err = setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF,
							(char *)&sock_buf_size, sizeof(sock_buf_size) );
		}
		else
		{
#ifdef _WIN32
			DWORD dwError=WSAGetLastError();
			if (dwError!=0 && WSAEWOULDBLOCK!=dwError && EINTR!=dwError && EAGAIN!=dwError && dwError!=WSAEINTR && dwError!=WSAETIMEDOUT)
			{
				return INVALID_SOCKET;
			}
#else
			if (errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINPROGRESS && errno!=ETIMEDOUT && EINTR!=errno)
			{
				return INVALID_SOCKET;
			}
#endif
		}
		return hSocket;
	}

	return INVALID_SOCKET;
}


unsigned long XSocketTCP::GetPeerIP(void)
{
	if (m_hSocket == INVALID_SOCKET)
		return 0;
	
#ifdef WIN32	
	int nLengthAddr = sizeof (SOCKADDR);
#else
	socklen_t nLengthAddr = sizeof (sockaddr);
#endif

	sockaddr_in addr;
	if (getpeername (m_hSocket, (sockaddr*)&addr, &nLengthAddr) == SOCKET_ERROR)
		return 0;

	return ntohl(addr.sin_addr.s_addr);
}