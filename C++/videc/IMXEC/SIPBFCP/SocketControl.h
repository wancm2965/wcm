/************************************************************************/
//SocketControl.h
//Copyright(c):AVCON
//Author:Gao
//Notice:Socket Interface , Only Supported WIN32 LINUX 
//Date:2014-08-15
/************************************************************************/

#ifndef __H_Socket_Control__
#define __H_Socket_Control__

#include "SocketHeader.h"
#include "XThreadBase.h"

#define MAXTIMERNUM  10 
#define MAXRECVBUFSIZE  2048 

#define SOCK_IPTCP SOCK_STREAM 
#define SOCK_IPUDP SOCK_DGRAM 
#define SINVALID INVALID_SOCKET
#define SERROR SOCKET_ERROR
#define SINIT 0
#define SCREATE_T 1
#define SCREATE_F -1
#define SACCEPT_T 2
#define SACCEPT_F -2
#define SBIND_T 3
#define SBIND_F -3
#define SLISTEN_T 4
#define SLISTEN_F -4
#define SCONNECT_T 5
#define SCONNECT_F -5
#define SSEND_T 6
#define SSEND_F -6
#define SRECV_T 7
#define SRECV_F -7
#define SSENDTO_T 8
#define SSENDTO_F -8
#define SRECVFROM_T 9
#define SRECVFROM_F -9
#define SSETADDR_F -10
#define SSETADDR_T 10

class CSocketControl : public XThreadBase
{
public:
	CSocketControl(CSocketControlCallback & rCallBack);
	virtual ~CSocketControl();

protected:
	virtual void ThreadProcMain(void);
	bool m_bRunning;
	CSocketControlCallback & m_rCallBack;

	typedef struct tagTimer
	{
		int          id;
		unsigned int nTimer;     //ms
	}TimerEvent;

public:
	 //nSockType 1流式套接字TCP server, 2流式套接字TCP client ,3 数据报套接字 bind, 4 数据报套接字 no bind
	int   Open(unsigned short nLocalPort,char*cszLocalIP =NULL,int nSockType=3,unsigned short nRemotePort=0,char* cszRemoteIP =NULL,int nTCPMax=10,int nSleep =5);
	void  Close(void);
	int   SendToData(unsigned char* pData,int nDataLen,unsigned short nPort =0,unsigned long ulRemoteIP =0,int nRepeat=1); //use udp
	int   SendToData(unsigned char* pData,int nDataLen,unsigned short nPort =0,char * strRemoteIP =NULL,int nRepeat=1);    //use udp
	int   SendData(unsigned char*pData,int nDataLen,int nRepeat=1);                                                        //use udp or tcp
	void  SetTimer(int id,unsigned int nTimer);                                                                            //nTimer ms
	void  SetTCPKeepAliveEnable(bool bEnable,int nIdleTimer,int nIntervalTimer=1000);                                      //use tcp nIdleTimer ms , nIntervalTimer ms
	void  SetTCPManualAddrEnable(bool bEnable=true);                                                                       //use tcp client ,true is TCP客户端使用本地指定端口连接

private:
	void  ThreadProcMainUDP();
	void  ThreadProcMainTCPServer();
	void  ThreadProcMainTCPClient();
	void  LogOutput(int nType,char *szFormat, ...);

private:
	SOCKET         m_hSocket;                //SOCKET  Handle
	int            m_nSleepRecvCheck;        //default 5 ms
	TimerEvent     m_TimerEvent[8];          //notice only use one timer
	int            m_nTimerIndex;
	unsigned long  m_nBaseTimeStamp;

private:
	int            m_nSocketStatu;            //SOCKET 状态
	int            m_nErrno;                  //记录socket 错误码
	sockaddr_in    m_struSockLocalAddr;       //local socket address
	int            m_nSockType;               //1流式套接字TCP server, 2流式套接字TCP client ,3 数据报套接字 bind, 4 数据报套接字 no bind
	sockaddr_in    m_struSockRemoteAddr;      //remote socket address
	sockaddr_in    m_struSockRecvAddr;        //recv remote socket address
	int            m_nSockTimeout;            //Socket select timeout ms
	int            m_nConnectNum;
	bool           m_bConnect;
	bool           m_bSetTCPKeepAlive;        //true is set tcp keepalive opt ,default false
	int            m_nKeepIdleTimer;          //开始首次发心跳包KeepAlive进行探测前的TCP空闭时间 ms
	int            m_nKeepIntervalTimer;      //发心跳包KeepAlive的间隔时间 ms
	bool           m_bTCPSpecifiedPort;       //true is TCP 客户端使用本地指定端口连接,default false
	bool           m_bTCPClientBind;          //TCP 客户端bind后为true
	bool           m_bNoneBlock;

private:
	void   BindSocket();
	void   CloseSocket();
	int    ConnectSocket();
	SOCKET GetSocketHandle();
	void   InitSocketLocalAddr();
	void   GetSocketLocalAddr(int & port, char *ip);
	void   GetSocketRecvAddr(int & port, char *ip);
	void   GetSocketRemoteAddr(int & port, char *ip);
	void   ListenSocket(int nMaxAcceptNum);
	void   CreateSocket(int SocketType);                                      //1流式套接字, 2数据报套接字
	void   CreateSocket();
	void   AcceptSocket(CSocketControl& socketP);
	int    SendSocket(void* SsendbufP,int sendsize);
	int    SendToSocket(void* SsendbufP,int sendsize);
	int    RecvSocket(void* SrecvbufP,int recvsize,int GSRType);
	void   SetSockTimeOut(int nTimeOut,timeval &tm);
	int    RecvFromSocket(void* SrecvbufP,int recvsize,int GSRType);
	void   SetLocalAddr(unsigned short SocketPort,void* SocketIPAS);          //设置本地SOCKET地址
	void   SetRemoteAddr(unsigned short SocketPort,void* SocketIPAS);
	int    SendToSocket(void* SsendbufP,int sendsize,unsigned short SocketPort,void* SocketIPAS);
	int    SendToSocket(void* SsendbufP,int sendsize,unsigned short SocketPort,unsigned long ulSocketIPAS);
	int    SendToSocket(void* SsendbufP,int sendsize,struct sockaddr * pSockAddr);
	void   ThreadTimerCallBack(int nTimerCount);
	void   CopySocketParam(SOCKET hSocket,int nSocketStatu,int nGErrno,sockaddr_in * struGSockAddr);
	char * GetSocketAddr(struct sockaddr * pAddr,int &port);
	char * GetSocketError(int code);
	bool   GetLocalFirstIP(char * localIP,int & nLen);
	int    SocketSelect(int nTimeOut,SOCKET hSocket,int & nError,fd_set & recvfds);

private:
	bool   SetRecvSendBufOpt();
	bool   SetNoneBlockingOpt(bool bNone);
	bool   SetTosOpt(unsigned char nTos);
	bool   SetReuseAddrOpt(bool bReuse);
	bool   SetKeepAliveOpt(bool bKeepAlive);
	bool   SetKeepAliveOpt(bool bKeepAlive,int nIdleTimer,int nIntervalTimer=1000);

};

#endif // __H_Socket_Control__
