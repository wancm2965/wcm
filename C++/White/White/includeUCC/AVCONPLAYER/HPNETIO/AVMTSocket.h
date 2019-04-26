// AVMTSocket.h: interface for the AVMTSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTSOCKET_H__
#define __AVMTSOCKET_H__


#ifdef WIN32


#include <winsock2.h>
#include <WS2TCPIP.H>
#include <errno.h>

#pragma comment(lib,"ws2_32.lib")

typedef int socklen_t;

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL	(0)
#endif

class XMCSocketInitializer  
{
public:
	XMCSocketInitializer(void)
	{
		WORD wVersion; 
		WSADATA wsaData;
		wVersion = MAKEWORD(2,2);
		WSAStartup(wVersion,&wsaData);
	}
	virtual ~XMCSocketInitializer(void)
	{
		WSACleanup(); 
	}
};

#else 

#ifndef SOCKET
#define SOCKET int
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
#include <linux/rtc.h>
#include <errno.h>

#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)
#define closesocket(s) shutdown(s,SHUT_RDWR);close(s)

#endif /* WIN32 */

class AFX_EXT_CLASS AVMTSocket  
{
public:
	AVMTSocket(void);
	virtual ~AVMTSocket(void);
public:
	void Attach(SOCKET hSocket);
	void Detach(void);
	void Close(void);
	bool SetNoneBlocking(bool bNB);

	unsigned short GetLocalPort(void);
	operator SOCKET(void){return m_hSocket;};
	static bool SetTos(SOCKET hSocket,unsigned char nTos);
	static unsigned long GetIPAddress(const char*cszHost);
	static const char*GetDottedDecimal(unsigned long ulIP);
	static unsigned long GetLocalIP(void);
	static void SetLocalIP(unsigned long ulLocalIP);
    static const char*GetLocalHostName(void);
protected:
	SOCKET		m_hSocket;

	static char m_szLocalHostName[256];
	static unsigned long m_ulLocalIP;
};

#endif
