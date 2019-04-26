// XSocket.cpp: implementation of the XSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "XSocket.h"
#include "XSocketUDP.h"


#ifdef _WIN32
static XSocketInitializer g_XSocketInitializer;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XSocket::XSocket(void)
:m_hSocket(INVALID_SOCKET)
{
}

XSocket::~XSocket(void)
{
}

void XSocket::Attach(SOCKET hSocket)
{
	m_hSocket=hSocket;
}

void XSocket::Detach(void)
{
	m_hSocket=INVALID_SOCKET;
}

void XSocket::Close(void)
{
	if (m_hSocket!=INVALID_SOCKET)
	{
#ifdef WIN32
		closesocket (m_hSocket);
#else
		close(m_hSocket);
#endif
		m_hSocket=INVALID_SOCKET;
	}
}


bool XSocket::SetNoneBlocking(bool bNB)
{
#ifdef _WIN32
	unsigned long lValue = bNB ? 1 : 0;
	int nRet = ioctlsocket(m_hSocket, FIONBIO, &lValue);
	if (nRet != 0)
	{
		return false;
	}
	return true;
#else
	if (bNB)
	{
		if (fcntl(m_hSocket, F_SETFL, O_NONBLOCK) == -1)
		{
			return false;
		}
	}
	else
	{
		if (fcntl(m_hSocket, F_SETFL, 0) == -1)
		{
			return false;
		}
	}
	return true;
#endif
}

bool XSocket::SetTos(SOCKET hSocket,unsigned char nTos)
{
	if (hSocket == INVALID_SOCKET)
		return false;

	int nValue=nTos;
	int err=setsockopt(
				hSocket,
#ifdef _WIN32
				IPPROTO_IP,
#else
				SOL_IP,
#endif
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	return (err == 0);
}


unsigned short XSocket::GetLocalPort(void)
{
	if (m_hSocket == INVALID_SOCKET)
		return 0;
	
	sockaddr_in addr;
#ifdef WIN32	
	int nLengthAddr = sizeof (sockaddr);
#else
	socklen_t nLengthAddr = sizeof (sockaddr);
#endif
	if (getsockname (m_hSocket, (sockaddr*)&addr, &nLengthAddr) == SOCKET_ERROR)
		return 0;

	return ntohs (addr.sin_port);
}


unsigned long XSocket::GetLocalIP(void)
{
	if (m_hSocket == INVALID_SOCKET)
		return 0;
	
	sockaddr_in addr;
#ifdef WIN32	
	int nLengthAddr = sizeof (sockaddr);
#else
	socklen_t nLengthAddr = sizeof (sockaddr);
#endif
	if (getsockname (m_hSocket, (sockaddr*)&addr, &nLengthAddr) == SOCKET_ERROR)
		return 0;

	return ntohl (addr.sin_addr.s_addr);
}


char XSocket::m_szLocalHostName[256]="";
const char*XSocket::GetLocalHostName(void)
{
	strcpy(m_szLocalHostName,"");
    if(gethostname(m_szLocalHostName,sizeof(m_szLocalHostName)) == 0) 
	{
    }

    return XSocket::m_szLocalHostName;
}

const char*XSocket::htoa(unsigned long nIP)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(nIP);
	return inet_ntoa (addr.sin_addr);
}

unsigned long XSocket::atoh(const char*cszIP)
{
	if (cszIP==NULL || strlen(cszIP)==0)
		return 0;

	in_addr addr;
	addr.s_addr = inet_addr(cszIP);

	if (addr.s_addr == INADDR_NONE) 
	{
		hostent *hent = gethostbyname(cszIP);
		if (hent==NULL) 
		{
			return 0;
		}
		addr = *(in_addr *) hent->h_addr;
	}

	return ntohl(addr.s_addr);
}


char XSocket::m_szLocalInterface[256]="";
void XSocket::SetLocalInterface(const char*cszLocalInterface)
{
	strcpy(m_szLocalInterface,cszLocalInterface);
}

const char*XSocket::GetLocalInterface(void)
{
	return m_szLocalInterface;
}
