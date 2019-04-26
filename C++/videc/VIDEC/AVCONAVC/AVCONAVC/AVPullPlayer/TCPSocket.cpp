// TCPSocket.cpp
#include "stdafx.h"
#include "TCPSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool SetNonBlocking(SOCKET hSocket,bool bNB)
{
#ifdef WIN32
	unsigned long lValue = bNB ? 1 : 0;
	int nRet = ioctlsocket(hSocket, FIONBIO, &lValue);
	return (nRet==0);
#else
	if (bNB)
	{
		if (fcntl(hSocket, F_SETFL, O_NONBLOCK) == -1)
		{
			return false;
		}
	}
	else
	{
		if (fcntl(hSocket, F_SETFL, 0) == -1)
		{
			return false;
		}
	}
	return true;
#endif
}

void TCPSocket::Cleanup (void)
{
	if (m_hSocket == INVALID_SOCKET)
		return;

#ifdef WIN32
	shutdown(m_hSocket,SD_BOTH);
	closesocket (m_hSocket);
#else
	shutdown(m_hSocket,SHUT_RDWR);
	close(m_hSocket);
#endif
	m_hSocket = INVALID_SOCKET;
}

bool TCPSocket::Create (int nType)
{
	if (m_hSocket != INVALID_SOCKET) return true;
	
	if((m_hSocket = socket (AF_INET, nType, 0)) == INVALID_SOCKET) return false;

	return true;
}

bool TCPSocket::Bind(LPCSOCKADDR psa)
{
	if (m_hSocket == INVALID_SOCKET)	return false;

	int option = 1;
	setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR,
			   (char *)&option, sizeof(option));
	
	if (bind (m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) return false;

	int nValue=160;
	int err=setsockopt(
				m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}
	
	return true;
}

bool TCPSocket::Bind(char*szIp,unsigned short usPort)
{
	SOCKADDR_IN sa;
	memset(&sa,0,sizeof(SOCKADDR));
	sa.sin_family = PF_INET;
	sa.sin_addr.s_addr = szIp == 0 ? htonl(INADDR_ANY) : inet_addr(szIp);
	sa.sin_port = htons(usPort);

	return Bind((SOCKADDR*)&sa);	
}

bool TCPSocket::Listen (void)
{
	if (m_hSocket == INVALID_SOCKET)	return false;
	
	if(listen(m_hSocket, 10) == SOCKET_ERROR) return false;

	return true;
}

bool TCPSocket::Accept (TCPSocket& sConnect, LPSOCKADDR psa)
{
	if (m_hSocket == INVALID_SOCKET)	return false;
	
	if (sConnect.m_hSocket != INVALID_SOCKET)	return false;

#ifdef WIN32	
	int nLengthAddr = sizeof (SOCKADDR);
#else
	socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif

	sConnect.m_hSocket = accept (m_hSocket, psa, &nLengthAddr);
	if (sConnect == INVALID_SOCKET)	return false;
	
	int nValue=160;
	int err=setsockopt(
				sConnect.m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}

	return true;
}

void TCPSocket::Close (void)
{
	if (INVALID_SOCKET == m_hSocket) return;
	
#ifdef WIN32	
	shutdown(m_hSocket,SD_BOTH);
	closesocket (m_hSocket);
#else
	shutdown(m_hSocket,SHUT_RDWR);
	close(m_hSocket);
#endif
	m_hSocket = INVALID_SOCKET;
}

void TCPSocket::Close(SOCKET hSocket)
{
	if (INVALID_SOCKET == hSocket) return;
	
#ifdef WIN32	
	shutdown(hSocket,SD_BOTH);
	closesocket (hSocket);
#else
	shutdown(hSocket,SHUT_RDWR);
	close(hSocket);
#endif
	hSocket = INVALID_SOCKET;
}

bool TCPSocket::Connect (LPCSOCKADDR psa)
{
	if (m_hSocket == INVALID_SOCKET) return false;
	
	if (connect(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) return false;

	int nValue=160;
	int err=setsockopt(
				m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}
	
	return true;
}

bool TCPSocket::Connect(LPCSOCKADDR psa,int nSecs)
{
#ifndef WIN32
	return Connect(psa);
#else
	//set the socket in non-blocking mode
	unsigned long	argp = 1;	

	ioctlsocket(m_hSocket,FIONBIO,&argp);

	connect(m_hSocket,psa,sizeof(SOCKADDR_IN));
	//FD_SET fd = {1, m_hSocket};	
	fd_set fd = {1, m_hSocket};	
	timeval tv = {nSecs, 0};
	int	nResult = select(0, 0, &fd, 0, &tv);
	
	//set the socket in blocking mode again.
	argp = 0;	

	ioctlsocket(m_hSocket,FIONBIO,&argp);

	int nValue=160;
	int err=setsockopt(
				m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}
	
	return (nResult!=0);
#endif
}


int TCPSocket::Send (const char* pch, const int nSize)
{
	/*
	if (m_hSocket == INVALID_SOCKET)
		return -1;

	int nBytesSent;
	if ((nBytesSent = send (m_hSocket, pch, nSize,
#ifdef WIN32
		0
#else
#ifdef UNIX
		0
#else
		MSG_NOSIGNAL
#endif
#endif
		)) == SOCKET_ERROR)
	{
#ifdef WIN32
		DWORD dwError=WSAGetLastError();
		TRACE("-------------------------------Send dwError=%u\n",dwError);
		if (dwError==0 || dwError==WSAEWOULDBLOCK || dwError==WSAEINTR || dwError==WSAETIMEDOUT)
			return 0;
#else
		if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			return 0;

		//printf("error:%s(%d)\n",strerror(errno),errno);
#endif
		nBytesSent=-1;
	}
	
	return nBytesSent;
	*/
	int nRet = 0;
	nRet=send(m_hSocket,pch,nSize,MSG_NOSIGNAL);

	if (nRet<=0)
	{
#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (dwError!=0 && WSAEWOULDBLOCK!=dwError && dwError==WSAEINTR && dwError==WSAETIMEDOUT)
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

int TCPSocket::Send (const char* pch, const int nSize, const int nSecs)
{
	return Send(pch,nSize);
}

int TCPSocket::Receive (char* pch, const int nSize)
{
	/*
	if (m_hSocket == INVALID_SOCKET)
		return -1;
	
	
	int nBytesReceived;
	if ((nBytesReceived = recv (m_hSocket, pch, nSize, 
#ifdef WIN32 
		0
#else 
#ifdef  UNIX
		0
#else 
		MSG_NOSIGNAL
#endif
#endif
		)) == SOCKET_ERROR) 
	{
#ifdef WIN32
		DWORD dwError=WSAGetLastError();
		//TRACE("--------------------------------Receive dwError=%u\n",dwError);
		//if (dwError==0 || dwError==WSAEWOULDBLOCK || dwError==WSAEINTR || dwError==WSAETIMEDOUT)
		//	return 0;
#else
		if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			return 0;

		//printf("error:%s(%d)\n",strerror(errno),errno);
#endif
		return -1;
	}
#ifdef WIN32
	if (nBytesReceived==0)
	{
		return -1;
	}
#endif
	
	return nBytesReceived;
	*/
	int nRet = recv(m_hSocket,pch,nSize,MSG_NOSIGNAL);

	if (nRet<=0)
	{
#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (dwError!=0 && WSAEWOULDBLOCK!=dwError && dwError==WSAEINTR && dwError==WSAETIMEDOUT)
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

int TCPSocket::Receive (char* pch, const int nSize, const int nSeconds)
{
	int nSecs=nSeconds;
	if (m_hSocket == INVALID_SOCKET)
		return -1;
#ifdef WIN32	
	FD_SET fd = {1, m_hSocket};
	TIMEVAL tv = {nSecs, 1};
	int     err;
	err=select (0, &fd, NULL, NULL, &tv);
	if (err <= 0)
		return err;
#else
	fd_set  set;
	struct  timeval tv;
	int     err;

	tv.tv_sec = nSecs; 
	tv.tv_usec = 1;
	
	FD_ZERO(&set);
	
	FD_SET(m_hSocket, &set);
	
	err = select(m_hSocket+1, &set, NULL, NULL, &tv);
	if (err <= 0)
		return err;
#endif

	/*
	int nBytesReceived;
	if ((nBytesReceived = recv (m_hSocket, pch, nSize, 
#ifdef WIN32 
		0
#else 
#ifdef  UNIX
		0
#else 
		MSG_NOSIGNAL
#endif
#endif
		)) == SOCKET_ERROR)
	{
#ifdef WIN32
		DWORD dwError=WSAGetLastError();
		TRACE("1--------------------------------Receive dwError=%u\n",dwError);
		if (dwError==0 || dwError==WSAEWOULDBLOCK || dwError==WSAEINTR || dwError==WSAETIMEDOUT)
			return 0;
#else
		if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			return 0;

		//printf("error:%s(%d)\n",strerror(errno),errno);
#endif
		return -1;
	}	
	return nBytesReceived;
	*/
	int nRet = recv(m_hSocket,pch,nSize,MSG_NOSIGNAL);

	if (nRet<=0)
	{
#ifdef _WIN32
		DWORD dwError=WSAGetLastError();
		if (dwError!=0 && WSAEWOULDBLOCK!=dwError && dwError==WSAEINTR && dwError==WSAETIMEDOUT)
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

bool TCPSocket::GetPeerAddr (LPSOCKADDR psa)
{
	if (m_hSocket == INVALID_SOCKET)
		return false;
	
#ifdef WIN32	
	int nLengthAddr = sizeof (SOCKADDR);
#else
	socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif
	if (getpeername (m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) return false;

	return true;
}

bool TCPSocket::GetSockAddr (LPSOCKADDR psa)
{
	if (m_hSocket == INVALID_SOCKET)
		return false;
	
#ifdef WIN32	
	int nLengthAddr = sizeof (SOCKADDR);
#else
	socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif
	if (getsockname (m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) false;

	return true;
}

CSockAddr TCPSocket::GetHostByName (const char* pchName, const USHORT ushPort)
{
	SOCKADDR_IN sockTemp;
	memset(&sockTemp,0,sizeof(sockTemp));
	hostent* pHostEnt = gethostbyname (pchName);
	if (pHostEnt == 0) return sockTemp;
	
	ULONG* pulAddr = (ULONG*) pHostEnt -> h_addr_list[0];
	sockTemp.sin_family = AF_INET;
	sockTemp.sin_port = htons (ushPort);
	sockTemp.sin_addr.s_addr = *pulAddr;
	
	return sockTemp;
}

const char* TCPSocket::GetHostByAddr (LPCSOCKADDR psa)
{
	hostent* pHostEnt = gethostbyaddr ((char*)&((LPSOCKADDR_IN)psa) -> sin_addr.s_addr, 4,
		PF_INET);
	if (pHostEnt == 0)
		return 0;
	
	return pHostEnt -> h_name;
}


bool TCPSocket::GetHostIPAddressByName(const char* cszHostName,char* lpszHostIPAddress)
{
	SOCKADDR_IN sockTemp;
	hostent* pHostEnt = gethostbyname (cszHostName);
	if (pHostEnt == 0) return false;
	
	ULONG* pulAddr = (ULONG*) pHostEnt -> h_addr_list[0];
	sockTemp.sin_family = AF_INET;
	sockTemp.sin_port = htons (0);
	sockTemp.sin_addr.s_addr = *pulAddr;
	
	CSockAddr sa=sockTemp;

	strcpy(lpszHostIPAddress,sa.DottedDecimal());

	return true;
}

