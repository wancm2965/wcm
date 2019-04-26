/************************************************************************/
//SocketHeader.h
//Copyright(c):AVCON
//Author:Gao
//Notice:Socket Interface , Only Supported WIN32 LINUX 
//Date:2014-08-15
/************************************************************************/

#ifndef __H_Socket_Header__
#define __H_Socket_Header__

//#include <winsock.h>
//#pragma comment(lib,"WSock32.Lib")
//#pragma comment(lib,"ws2_32.lib")

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>
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
#include <string.h>

#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)

#endif

class CSocketMgr
{
public:
	CSocketMgr() {};
	virtual ~CSocketMgr() {};
public:
	bool SockInit();
	bool SockEnd();
};

class CSocketControl;

class CSocketControlCallback
{
public:
	CSocketControlCallback(){};
	virtual~CSocketControlCallback(void){};
public:
	virtual void OnDataCallback(CSocketControl* pSession,const char* pData,int nDataLen)=0;
	virtual void OnTimerCallback(CSocketControl* pSession,int id,unsigned int nTimer,unsigned long ulTimeStamp) {};
	virtual void OnConnectCallback(CSocketControl* pSession) {};

};

#endif // __H_Socket_Header__
