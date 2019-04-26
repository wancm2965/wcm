// XSocket.h: interface for the XSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XSOCKET_H__
#define __XSOCKET_H__


#include <NETEC/NETEC_Export.h>

#ifdef _WIN32


#include <winsock2.h>
#include <WS2TCPIP.H>

#ifdef NETEC_ARM
#include <winerror.h>
#define EINTR           4
#define EAGAIN          11
#pragma comment(lib,"ws2.lib")
#else
#include <errno.h>
#pragma comment(lib,"ws2_32.lib")
#endif


typedef int32_t socklen_t;

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL	(0)
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif

#ifndef EALREADY
#define EALREADY WSAEALREADY
#endif

#ifndef ENOTSOCK
#define ENOTSOCK WSAENOTSOCK
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ WSAEDESTADDRREQ
#endif 

#ifndef EMSGSIZE
#define EMSGSIZE WSAEMSGSIZE
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE WSAEPROTOTYPE
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT WSAENOPROTOOPT
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#endif

#ifndef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP WSAEOPNOTSUPP
#endif

#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#endif

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

#ifndef EADDRINUSE
#define EADDRINUSE WSAEADDRINUSE
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#endif

#ifndef ENETDOWN
#define ENETDOWN WSAENETDOWN
#endif

#ifndef ENETUNREACH
#define ENETUNREACH WSAENETUNREACH
#endif

#ifndef ENETRESET
#define ENETRESET WSAENETRESET
#endif

#ifndef ECONNABORTED
#define ECONNABORTED WSAECONNABORTED
#endif 

#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif

#ifndef ENOBUFS
#define ENOBUFS WSAENOBUFS
#endif

#ifndef EISCONN
#define EISCONN WSAEISCONN
#endif

#ifndef ENOTCONN
#define ENOTCONN WSAENOTCONN
#endif

#ifndef ESHUTDOWN
#define ESHUTDOWN WSAESHUTDOWN
#endif

#ifndef ETOOMANYREFS
#define ETOOMANYREFS WSAETOOMANYREFS
#endif 

#ifndef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#endif


#ifndef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#endif

#ifndef ELOOP
#define ELOOP WSAELOOP
#endif


#ifndef ENAMETOOLONG
#define ENAMETOOLONG WSAENAMETOOLONG
#endif

#ifndef EHOSTDOWN
#define EHOSTDOWN WSAEHOSTDOWN
#endif

#ifndef EHOSTUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#endif

#ifndef ENOTEMPTY
#define ENOTEMPTY WSAENOTEMPTY
#endif

#ifndef EPROCLIM
#define EPROCLIM WSAEPROCLIM
#endif

#ifndef EUSERS
#define EUSERS WSAEUSERS
#endif 

#ifndef EDQUOT
#define EDQUOT WSAEDQUOT
#endif

#ifndef ESTALE
#define ESTALE WSAESTALE
#endif

#ifndef EREMOTE
#define EREMOTE WSAEREMOTE
#endif

#ifndef SOCKET_EACCES
#define SOCKET_EACCES WSAEACCES
#endif


//#undef EACCES
//#undef ENOTEMPTY // remove errno.h's definition
//#undef ENAMETOOLONG // remove errno.h's definition

class XSocketInitializer  
{
public:
	XSocketInitializer(void)
	{
		WORD wVersion; 
		WSADATA wsaData;
		wVersion = MAKEWORD(2,2);
		WSAStartup(wVersion,&wsaData);
	}
	virtual ~XSocketInitializer(void)
	{
		WSACleanup(); 
	}
};

#else 

#ifndef SOCKET
#define SOCKET int32_t
#endif

#ifdef NETEC_IOS
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL	(0)
#endif
#endif


#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef SOCKET_ERROR
#  define SOCKET_ERROR   (-1)
#endif
#ifndef INVALID_SOCKET
#  define INVALID_SOCKET (-1)
#endif
#define closesocket(s) close(s)

#endif /* _WIN32 */

class NETEC_API XSocket  
{
public:
	XSocket(void);
	virtual ~XSocket(void);
public:
	virtual void Attach(SOCKET hSocket);
	virtual void Detach(void);
	virtual void Close(void);
	virtual bool SetNoneBlocking(bool bNB);

	virtual operator SOCKET(void){return m_hSocket;};
	static bool SetTos(SOCKET hSocket,unsigned char nTos);

    static const char*GetLocalHostName(void);

	static void SetLocalInterface(const char*cszLocalInterface);
	static const char*GetLocalInterface(void);
protected:
	SOCKET		m_hSocket;

	static char m_szLocalHostName[256];
	static char m_szLocalInterface[256];
};


NETEC_API unsigned short netec_htons(unsigned short x);
NETEC_API unsigned short netec_htons_2(const void* px);
NETEC_API uint32_t netec_htonl(uint32_t x);
NETEC_API uint32_t netec_htonl_2(const void* px);
NETEC_API int32_t netec_is_BE();

#define netec_ntohs(x) netec_htons(x)
#define netec_ntohs_2(px) netec_htons_2(px)
#define netec_ntohl(x) netec_htonl(x)
#define netec_ntohl_2(px) netec_htonl_2(px)


#endif
