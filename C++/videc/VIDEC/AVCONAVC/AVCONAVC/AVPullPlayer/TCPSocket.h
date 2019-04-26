// TCPSocket.h
//
#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__


#ifdef WIN32
#include <winsock2.h>
#include <WS2TCPIP.H>
typedef const struct sockaddr* LPCSOCKADDR;
#define MSG_NOSIGNAL	(0)
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in  *LPSOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr  *LPSOCKADDR;
typedef const struct sockaddr* LPCSOCKADDR;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr  *LPIN_ADDR;

typedef  int    SOCKET;
typedef  unsigned short USHORT;
typedef  unsigned long  ULONG;
typedef  const char*    LPCSTR;
typedef  char*			LPSTR;
typedef  const char*    LPCTSTR;
typedef  unsigned int   UINT;
typedef  char* 	LPTSTR;
typedef  unsigned long HANDLE;

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

class CSockAddr
: public sockaddr_in
{
// constructors
public:
	CSockAddr (void)
	{
		sin_family = AF_INET;
		sin_port = 0;
		sin_addr.s_addr = 0;
	}

	CSockAddr (const SOCKADDR& sa)
	{
		memcpy (this, &sa, sizeof(SOCKADDR));
	}

	CSockAddr (const SOCKADDR_IN& rsin)
	{
		memcpy (this, &rsin, sizeof (SOCKADDR_IN));
	}

	CSockAddr (const ULONG ulAddr, const USHORT ushPort = 0)
	{
		sin_family = AF_INET;
		sin_port = htons (ushPort);
		sin_addr.s_addr = htonl (ulAddr);
	}

	CSockAddr (const char* pchIP, const USHORT ushPort = 0)
	{
		sin_family = AF_INET;
		sin_port = htons (ushPort);
		sin_addr.s_addr = inet_addr(pchIP);
	}

public:	
	// Return the address in dotted-decimal format
	LPSTR DottedDecimal (void)
	{
		return inet_ntoa (sin_addr);
	}
	
	// Get port and address (even though they're public)
	USHORT Port (void) const
	{
		return ntohs (sin_port);
	}

	ULONG IPAddr (void) const
	{
		return ntohl (sin_addr.s_addr);
	}
	
	// operators added for efficiency
	const CSockAddr& operator=(const SOCKADDR& sa)
	{
		memcpy (this, &sa, sizeof(SOCKADDR));
		return *this;
	}

	const CSockAddr& operator=(const SOCKADDR_IN& rsin)
	{
		memcpy (this, &rsin, sizeof(SOCKADDR_IN));
		return *this;
	}

	operator SOCKADDR()
	{
		return *((LPSOCKADDR) this);
	}

	operator LPSOCKADDR()
	{
		return (LPSOCKADDR) this;
	}

	operator LPSOCKADDR_IN()
	{
		return (LPSOCKADDR_IN) this;
	}
};

// member functions truly block and must not be used in UI threads
// use this class as an alternative to the MFC CSocket class

class TCPSocket
{
// constructors
public:
	TCPSocket (void)
	{
		m_hSocket = INVALID_SOCKET;
	}

public:	
	void Cleanup (void);
	bool Create (int nType = SOCK_STREAM);
	void Close (void);
	static void Close(SOCKET hSocket);
	bool Bind(LPCSOCKADDR psa);
	bool Bind(char*szIp,unsigned short usPort);
	bool Listen();
	bool Connect(LPCSOCKADDR psa);
	bool Connect(LPCSOCKADDR psa,int nSecs);
	bool Accept (TCPSocket& s, LPSOCKADDR psa);
	int Send (const char* pch, const int nSize);
	int Send (const char* pch, const int nSize, const int nSecs);
	int Receive (char* pch, const int nSize);
	int Receive (char* pch, const int nSize, const int nSecs);
	bool GetPeerAddr (LPSOCKADDR psa);
	bool GetSockAddr (LPSOCKADDR psa);
	static CSockAddr GetHostByName (const char* pchName, const USHORT ushPort = 0);
	static const char* GetHostByAddr (LPCSOCKADDR psa);
	static bool GetHostIPAddressByName(const char* cszHostName,char* lpszHostIPAddress);
	operator SOCKET ()
	{
		return m_hSocket;
	}

public:
	SOCKET m_hSocket;
};



#endif