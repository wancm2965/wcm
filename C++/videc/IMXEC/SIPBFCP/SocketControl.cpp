#include "stdafx.h"
#include "SocketControl.h"
#include <mmsystem.h>

#ifdef _WIN32
#define SocketErrNo  WSAGetLastError() 
#define SOCKBLOCKERR  WSAEWOULDBLOCK 
#else
#define SocketErrNo  errno 
#define SOCKBLOCKERR  EINPROGRESS 
#endif

//#define SOCKLOGOUTPUT 1 
#define SOCKLOGTYPE  2 

bool CSocketMgr::SockInit()
{
#ifdef _WIN32

	int err;
	WSADATA WSockData;
	err=WSAStartup(0x0101,&WSockData);
	if(err != 0)
	{
		return false;
	}
#endif

	return true;
}

bool CSocketMgr::SockEnd()
{
#ifdef _WIN32

	int err;
	err=WSACleanup();
	if(err != 0)
	{
		return false;
	}
#endif

	return true;
}

CSocketControl::CSocketControl(CSocketControlCallback & rCallBack)
:m_rCallBack(rCallBack)
{
	m_nSocketStatu=SINIT;
	m_nErrno=0;
	m_hSocket=NULL;
	m_bRunning=false;
	InitSocketLocalAddr();
	m_nSleepRecvCheck = 5;
	memset(m_TimerEvent,0,sizeof(m_TimerEvent));
	m_nTimerIndex =0;
	m_nBaseTimeStamp = 0;
	m_nSockTimeout = 600;
	m_nConnectNum =0;
	m_bConnect=false;
	m_bSetTCPKeepAlive=false;
	m_nKeepIdleTimer=2000;
	m_nKeepIntervalTimer=1000;
	
	m_bTCPSpecifiedPort=false;
	m_bTCPClientBind=false;
	m_bNoneBlock=true;

}

CSocketControl::~CSocketControl()
{
}

void CSocketControl::LogOutput(int nType,char *szFormat, ...)
{
	if(nType > SOCKLOGTYPE) return;

#if defined(SOCKLOGOUTPUT)

	va_list args;
	va_start(args, szFormat);
	char buffer[2048]={0};

#ifdef _WIN32
	_vsnprintf(buffer,sizeof(buffer),szFormat,args);
	OutputDebugString(buffer);
#else
	vsnprintf(buffer,sizeof(buffer),szFormat,args);
	printf(buffer);
#endif

	va_end(args);

#endif
}

int CSocketControl::Open(unsigned short nLocalPort,char*cszLocalIP,int nSockType,unsigned short nRemotePort,char* cszRemoteIP,int nTCPMax,int nSleep)
{
	int nRet = 0;
	char * ip ="0";
	if(cszLocalIP) ip=cszLocalIP;
	SetLocalAddr(nLocalPort,ip);
	SetRemoteAddr(nRemotePort,cszRemoteIP);
	m_nSockType=nSockType;

	LogOutput(2,"Open remote ip=%s,port=%d,local ip=%s,port=%d,SockType:%d\n",cszRemoteIP,nRemotePort,cszLocalIP,nLocalPort,nSockType);

	CreateSocket();

	if(m_nSocketStatu==SCREATE_T)
	{
		if(m_nSockType == 1)
		{
			BindSocket();
			ListenSocket(nTCPMax);
			if (m_nSocketStatu==SLISTEN_T)
			{
				nRet =1;
			}
		}
		else if(m_nSockType == 3)
		{
			BindSocket();
			if (m_nSocketStatu==SBIND_T)
			{
				nRet =1;
			}
		}
		else
		{
			nRet =1;
		}
	}

	if(nSleep > 0) m_nSleepRecvCheck =nSleep;

	if(nRet ==1 && (m_nSockType == 1 || m_nSockType == 2 || m_nSockType == 3))
	{
		m_bRunning=true;
		if (!StartThread())
		{
			LogOutput(1,"StartThread Error\n");
			nRet=0;
		}
	}

	return nRet;
}

void CSocketControl::Close(void)
{
	if(m_bRunning)
	{
		m_bRunning=false;
		LogOutput(2,"Socket Thread Exit 111\n");
		WaitForStop();
		LogOutput(2,"Socket Thread Exit 222\n");
	}
	CloseSocket();
}

int CSocketControl::SendToData(unsigned char*pData,int nDataLen,unsigned short nPort,unsigned long ulRemoteIP,int nRepeat)
{
	int nRet=0;
	for(int i=0;i<nRepeat;i++)
	{
		if(m_nSockType ==3 || m_nSockType ==4)
		{
			if(nPort >0 && ulRemoteIP !=0)
				nRet=SendToSocket(pData,nDataLen,nPort,ulRemoteIP);
			else
				nRet=SendToSocket(pData,nDataLen);
		}
	}
	return nRet;
}

int CSocketControl::SendToData(unsigned char*pData,int nDataLen,unsigned short nPort,char * strRemoteIP,int nRepeat)
{
	int nRet=0;
	for(int i=0;i<nRepeat;i++)
	{
		if(m_nSockType ==3 || m_nSockType ==4)
		{
			if(nPort >0 && strRemoteIP !=NULL )
				nRet=SendToSocket(pData,nDataLen,nPort,strRemoteIP);
			else
				nRet=SendToSocket(pData,nDataLen);
		}
	}
	return nRet;
}

int CSocketControl::SendData(unsigned char*pData,int nDataLen,int nRepeat)
{
	int nRet=0;
	for(int i=0;i<nRepeat;i++)
	{
		if(m_nSockType ==3 || m_nSockType ==4)
		{
			nRet=SendToSocket(pData,nDataLen);
		}
		else if(m_nSockType ==1 || m_nSockType ==2)
		{
			nRet=SendSocket(pData,nDataLen);
		}
	}
	return nRet;
}

void CSocketControl::SetTimer(int id,unsigned int nTimer)
{
	if(m_nTimerIndex >=MAXTIMERNUM) m_nTimerIndex=0;
	m_TimerEvent[m_nTimerIndex].id =id;
	m_TimerEvent[m_nTimerIndex].nTimer =nTimer;
	m_nTimerIndex ++;
	//notice the only use one timer
	m_TimerEvent[0].id =id;
	m_TimerEvent[0].nTimer =nTimer;
	m_nTimerIndex =1;
	m_nBaseTimeStamp =::timeGetTime();
}

void CSocketControl::ThreadTimerCallBack(int nTimerCount)
{
	unsigned long nTimeStamp=0;
	nTimeStamp =::timeGetTime();
	int i=0;
	for(i=0;i< nTimerCount;i++)
	{
		if(nTimeStamp < m_nBaseTimeStamp)
		{
			nTimeStamp = m_nBaseTimeStamp;
		}
		else if(nTimeStamp - m_nBaseTimeStamp >= m_TimerEvent[i].nTimer)
		{
			m_rCallBack.OnTimerCallback(this,m_TimerEvent[i].id,m_TimerEvent[i].nTimer,nTimeStamp);
		}
	}
}

void CSocketControl::ThreadProcMain(void)
{
	//1流式套接字TCP server, 2流式套接字TCP client ,3 数据报套接字 bind, 4 数据报套接字 no bind
	if(m_nSockType ==3 || m_nSockType ==4) ThreadProcMainUDP();
	else if(m_nSockType ==1) ThreadProcMainTCPServer();
	else if(m_nSockType ==2) ThreadProcMainTCPClient();
}

void CSocketControl::SetSockTimeOut(int nTimeOut,timeval &tm)
{
	if(nTimeOut <=0 )
	{
		nTimeOut = 1000;
	}
	tm.tv_sec  = nTimeOut / 1000;;
	tm.tv_usec = (nTimeOut % 1000) * 1000;
}

void CSocketControl::ThreadProcMainUDP(void)
{
	fd_set recvfdset;
	int  nError =0;
	int selectret=0;
	int recvret=0;
	char recvBuf[MAXRECVBUFSIZE]={0};
	int recvBufLen=0;
	m_nBaseTimeStamp =::timeGetTime();

	while (m_bRunning)
	{	
		selectret=SocketSelect(m_nSockTimeout,m_hSocket,nError,recvfdset);
		if(selectret == SOCKET_ERROR)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else if(selectret == 0)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else
		{
			if(FD_ISSET(m_hSocket,&recvfdset))
			{
				recvBufLen=RecvFromSocket(recvBuf,sizeof(recvBuf),0);                  //udp 中MSG_PEEK 不能用
				if(recvBufLen >0)
				{
					m_rCallBack.OnDataCallback(this,recvBuf,recvBufLen);
				}
			}
		}

		//timer 
		if(m_nTimerIndex >0)
		{
			ThreadTimerCallBack(m_nTimerIndex);
		}
	}
}

int CSocketControl::SocketSelect(int nTimeOut,SOCKET hSocket,int & nError,fd_set & recvfds)
{
	//用于SOCKET 非阻塞控制 
	//fd_set recvfds;
	fd_set sendfds;
	fd_set exceptfds;
	timeval timeout;
	//清空fd_set
	FD_ZERO(&recvfds);
	FD_ZERO(&sendfds);
	FD_ZERO(&exceptfds);
	//设置timeout
	SetSockTimeOut(nTimeOut,timeout);
	int ret=-1;
	FD_SET(hSocket,&recvfds);
	FD_SET(hSocket,&sendfds);
	FD_SET(hSocket,&exceptfds);
	//sendfds = acceptfds;
	//exceptfds = acceptfds;
	ret=select(FD_SETSIZE,&recvfds,&sendfds,&exceptfds,&timeout);
	//ret=select(0,&recvfds,NULL,NULL,&timeout);
	//nError=SocketErrNo;
	return ret;
}

void CSocketControl::ThreadProcMainTCPServer(void)
{
	fd_set recvfdset;
	int  selectret=0;
	int  recvret=0;
	int  recvBufLen=0;
	bool bAccept=false;
	int  nError =0;
	char recvBuf[MAXRECVBUFSIZE]={0};
	m_nBaseTimeStamp =::timeGetTime();
	CSocketControl acceptControl(m_rCallBack);

	while (m_bRunning)
	{
		//notice if not use keep alive
		//if(! m_bConnect) bAccept=false;
		if(! bAccept)
		{
			selectret=SocketSelect(m_nSockTimeout,m_hSocket,nError,recvfdset);
			if(selectret == SOCKET_ERROR || selectret == 0)
			{
				Sleep(m_nSleepRecvCheck);
				continue;
			}
				AcceptSocket(acceptControl);
				if(acceptControl.m_nSocketStatu != SACCEPT_T)
				{
					continue;
				}
				else
				{
					bAccept=true;
					m_bConnect=true;
					m_rCallBack.OnConnectCallback(&acceptControl);
					LogOutput(2,"TCPServer  Accept Socket OnConnect OK\n");
				}
		}

		selectret=SocketSelect(m_nSockTimeout,acceptControl.m_hSocket,nError,recvfdset);
		if(selectret == SOCKET_ERROR)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else if(selectret == 0)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else
		{
			if(FD_ISSET(acceptControl.m_hSocket,&recvfdset))
			{
				recvBufLen=acceptControl.RecvSocket(recvBuf,sizeof(recvBuf),0);
				if (recvBufLen >0)
				{
					m_rCallBack.OnDataCallback(&acceptControl,recvBuf,recvBufLen);
				}
				if(recvBufLen <=0)
				{
					m_nConnectNum --;
					acceptControl.CloseSocket();
					bAccept =false;
					LogOutput(2,"TCPServer Accept Socket OnDisConnect\n");
					Sleep(m_nSleepRecvCheck);
				}
			}
		}

		//timer 
		if(m_nTimerIndex >0)
		{
			ThreadTimerCallBack(m_nTimerIndex);
		}
	}

	LogOutput(2,"Accept Socket Close\n");
	acceptControl.CloseSocket();
}

void CSocketControl::ThreadProcMainTCPClient(void)
{
	fd_set recvfdset;
	int  nError =0;
	int  selectret=0;
	int  recvret=0;
	char recvBuf[MAXRECVBUFSIZE]={0};
	int  recvBufLen=0;
	m_nBaseTimeStamp =::timeGetTime();
	bool bConnect=false;

	while (m_bRunning)
	{
		//notice if not use keep alive
		//if(! m_bConnect) bConnect=false;
		if(! bConnect)
		{
			if(m_bTCPSpecifiedPort && !m_bTCPClientBind)
			{
				BindSocket();
				//notice if not use,sometimes client is not connect server
				Sleep(10);
			}

			if(m_bNoneBlock)
			{
				SetNoneBlockingOpt(m_bNoneBlock); 
			}
			ConnectSocket();
			if(m_nSocketStatu != SCONNECT_T)
			{
				CloseSocket();
				Sleep(m_nSleepRecvCheck);
				CreateSocket();
				continue;
			}
			else
			{
				bConnect=true;
				m_bConnect=true;
				m_rCallBack.OnConnectCallback(this);
				LogOutput(2,"TCPClient Connect OK\n");
			}
		}
		selectret=SocketSelect(m_nSockTimeout,m_hSocket,nError,recvfdset);
		if(selectret == SOCKET_ERROR)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else if(selectret == 0)
		{
			Sleep(m_nSleepRecvCheck);
		}
		else
		{
			if(FD_ISSET(m_hSocket,&recvfdset))
			{
				recvBufLen=RecvSocket(recvBuf,sizeof(recvBuf),0);
				if(recvBufLen >0)
				{
					m_rCallBack.OnDataCallback(this,recvBuf,recvBufLen);
				}
				if(recvBufLen <=0)
				{
					bConnect =false;
					LogOutput(2,"TCPClient DisConnect\n");
					Sleep(m_nSleepRecvCheck);
				}
			}
		}

		//timer 
		if(m_nTimerIndex >0)
		{
			ThreadTimerCallBack(m_nTimerIndex);
		}
	}
}

void CSocketControl::SetLocalAddr(unsigned short SocketPort,void* SocketIPAS)
{
	m_struSockLocalAddr.sin_family=AF_INET;
	m_struSockLocalAddr.sin_port=htons(SocketPort);  
	m_struSockLocalAddr.sin_addr.s_addr=inet_addr((char * )(SocketIPAS));
}

void CSocketControl::SetRemoteAddr(unsigned short SocketPort,void* SocketIPAS)
{
	m_struSockRemoteAddr.sin_family=AF_INET;
	m_struSockRemoteAddr.sin_port=htons(SocketPort);  
	m_struSockRemoteAddr.sin_addr.s_addr=inet_addr((char * )(SocketIPAS));
}

void CSocketControl::CreateSocket(int SocketType)
{
	if (m_nSocketStatu==SINIT)
	{
		m_hSocket=socket(AF_INET,SocketType,0);
		if (m_hSocket==SINVALID)
		{
			m_nSocketStatu=SCREATE_F;
			m_hSocket=NULL;
			m_nErrno=SocketErrNo;
			LogOutput(1,"create socket Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nSocketStatu=SCREATE_T;
			m_nErrno=0;
		}
	}
}

void CSocketControl::CreateSocket()
{
	m_bTCPClientBind=false;
	int SocketType=0;
	if(m_nSockType == 1 || m_nSockType ==2) SocketType=1;
	else if(m_nSockType == 3 || m_nSockType ==4) SocketType=2;
	CreateSocket(SocketType);
}

void CSocketControl::AcceptSocket(CSocketControl& socketP)
{
	int GSockAddSize=sizeof(sockaddr);
	if (m_nSocketStatu==SLISTEN_T && socketP.m_nSocketStatu==SINIT)
	{
		socketP.m_hSocket=accept(m_hSocket,(struct sockaddr *)&(socketP.m_struSockRemoteAddr),&GSockAddSize);
		if (socketP.m_hSocket==SINVALID)
		{
			m_nErrno=SocketErrNo;
			socketP.m_nSocketStatu=SACCEPT_F;
			socketP.m_hSocket=NULL;
			socketP.m_nErrno=m_nErrno;
			LogOutput(1,"accept Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),socketP.m_nSocketStatu);
		}
		else
		{
			m_nErrno=0;
			socketP.m_nSocketStatu=SACCEPT_T;
			socketP.m_nErrno=0;
			socketP.m_nSockType=m_nSockType;   //set
			m_nConnectNum++;
			SetTosOpt(160);
			if(m_bSetTCPKeepAlive) SetKeepAliveOpt(m_bSetTCPKeepAlive,m_nKeepIdleTimer,m_nKeepIntervalTimer);
			int port=0;
			char * ip=GetSocketAddr((struct sockaddr *)&socketP.m_struSockRemoteAddr,port);
			LogOutput(2,"accept OK, accept count=%d,remote ip=%s,port=%d\n",m_nConnectNum,ip,port);
		}
	}
}

void CSocketControl::BindSocket()
{
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCREATE_F && m_nSocketStatu!=SACCEPT_F)
	{
		SetReuseAddrOpt(true);

		if (bind(m_hSocket,(struct sockaddr *)&m_struSockLocalAddr,sizeof(sockaddr))==SERROR)
		{
			m_nSocketStatu=SBIND_F;
			m_nErrno=SocketErrNo;
			LogOutput(1,"bind Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nSocketStatu=SBIND_T;
			m_nErrno=0;
			m_bTCPClientBind=true;
			LogOutput(2,"bind OK\n");
		}
	}
}

void CSocketControl::ListenSocket(int nMaxAcceptNum)
{
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCREATE_F && m_nSocketStatu!=SACCEPT_F)
	{
		if (listen(m_hSocket,nMaxAcceptNum)==SERROR)
		{
			m_nSocketStatu=SLISTEN_F;
			m_nErrno=SocketErrNo;
			LogOutput(1,"listen Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nSocketStatu=SLISTEN_T;
			m_nErrno=0;
			if(m_bNoneBlock) SetNoneBlockingOpt(m_bNoneBlock);
		}
	}
}

int CSocketControl::ConnectSocket()
{
	int GSSTemp =-1;
	
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCREATE_F && m_nSocketStatu!=SACCEPT_F)
	{
		m_bNoneBlock=true;
		GSSTemp=connect(m_hSocket,(struct sockaddr *)&m_struSockRemoteAddr,sizeof(sockaddr));
		int nErr=SocketErrNo;
		if ( !m_bNoneBlock)
		{
			if (GSSTemp==SERROR)
			{
				//notice the linux wait test
				m_nSocketStatu=SCONNECT_F;
				m_nErrno=nErr;
				LogOutput(1,"connect Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
			}
			else
			{
				m_nSocketStatu=SCONNECT_T;
				m_nErrno=0;
				GSSTemp =0;
				SetTosOpt(160);
				int port=0;
				char * ip=GetSocketAddr((struct sockaddr *)&m_struSockRemoteAddr,port);
				LogOutput(2,"connect OK,remote ip=%s,port=%d\n",ip,port);
			}
		}
		if (m_bNoneBlock)
		{
#ifdef _WIN32

			if (GSSTemp==SERROR)
#else
			if (GSSTemp <0)

#endif

			{
				//connect error
				if (nErr != SOCKBLOCKERR)
				{
					m_nSocketStatu=SCONNECT_F;
					m_nErrno=nErr;
					LogOutput(1,"connect Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
				}
				else
				{
					int selectret=0;
					int error=-1;
					int nLen=4;
					fd_set recvfdset;
					int  nError =0;
					selectret=SocketSelect(m_nSockTimeout,m_hSocket,nError,recvfdset);
					if(selectret == SOCKET_ERROR || selectret == 0)
					{
						m_nSocketStatu=SCONNECT_F;
						m_nErrno=nErr;
						LogOutput(1,"connect Error Timeout,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
						return GSSTemp;
					}
					if (getsockopt(m_hSocket, SOL_SOCKET,SO_ERROR, (char *)&error, &nLen) < 0)
					{
						m_nSocketStatu=SCONNECT_F;
						m_nErrno=nErr;
						LogOutput(1,"connect Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
						return GSSTemp;
					}
					if (error > 0)
					{
						m_nSocketStatu=SCONNECT_F;
						m_nErrno=nErr;
						LogOutput(1,"connect Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
						return GSSTemp;
					}

					m_nSocketStatu=SCONNECT_T;
					m_nErrno=0;
					SetTosOpt(160);
					if(m_bSetTCPKeepAlive) SetKeepAliveOpt(m_bSetTCPKeepAlive,m_nKeepIdleTimer,m_nKeepIntervalTimer);
					GSSTemp =0;
					int port=0;
					char * ip=GetSocketAddr((struct sockaddr *)&m_struSockRemoteAddr,port);
					LogOutput(2,"connect OK,remote ip=%s,port=%d\n",ip,port);
				}
			}
		}
	}

	return GSSTemp;
}

int CSocketControl::SendSocket(void* SsendbufP,int sendsize)
{
	int GSSendByteNum =0;

	//if (m_nSocketStatu ==SCONNECT_F)
	//{
	//	m_bConnect=false;
	//	LogOutput(1,"send Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
	//}

	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCONNECT_F && m_nSocketStatu!=SACCEPT_F)
	{
		GSSendByteNum=send(m_hSocket,(char *)SsendbufP,sendsize,0);
		if (GSSendByteNum==SERROR)
		{
			m_nErrno=SocketErrNo;
			m_bConnect=false;
			LogOutput(1,"send Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nErrno=0;
			if(SOCKLOGTYPE >=2)
			{
				int port=0;
				char * ip=GetSocketAddr((struct sockaddr *)&m_struSockRemoteAddr,port);
				LogOutput(2,"send remote ip=%s,port=%d,len:%d\n",ip,port,GSSendByteNum);
			}
		}
	}

	return GSSendByteNum;
}

int CSocketControl::RecvSocket(void* SrecvbufP,int recvsize,int GSRType)
{
	int GSRecvByteNum =0;
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCONNECT_F && m_nSocketStatu!=SACCEPT_F)
	{
		GSRecvByteNum=recv(m_hSocket,(char *)SrecvbufP,recvsize,GSRType);
		if (GSRecvByteNum==SERROR)
		{
			m_nErrno=SocketErrNo;
			LogOutput(1,"recv Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nErrno=0;
			if(SOCKLOGTYPE >=2)
			{
				int port=0;
				char * ip=GetSocketAddr((struct sockaddr *)&m_struSockRemoteAddr,port);
				LogOutput(2,"recv remote ip=%s,port=%d,len:%d\n",ip,port,GSRecvByteNum);
			}
		}
	}
	return GSRecvByteNum;
}

int CSocketControl::SendToSocket(void* SsendbufP,int sendsize,unsigned short SocketPort,void* SocketIPAS)
{
	sockaddr_in SockAddr;
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_port=htons(SocketPort);  
	SockAddr.sin_addr.s_addr=inet_addr((char * )(SocketIPAS));
	return SendToSocket(SsendbufP,sendsize,(struct sockaddr *)&SockAddr);
}

int CSocketControl::SendToSocket(void* SsendbufP,int sendsize,unsigned short SocketPort,unsigned long ulSocketIPAS)
{
	sockaddr_in SockAddr;
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_port=htons(SocketPort);  
	SockAddr.sin_addr.s_addr=ulSocketIPAS;
	return SendToSocket(SsendbufP,sendsize,(struct sockaddr *)&SockAddr);
}

int CSocketControl::SendToSocket(void* SsendbufP,int sendsize,struct sockaddr * pSockAddr)
{
	int GSSendByteNum =0;
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCONNECT_F && m_nSocketStatu!=SACCEPT_F)
	{
		GSSendByteNum=sendto(m_hSocket,(char *)SsendbufP,sendsize,0,pSockAddr,sizeof(struct sockaddr_in));
		if (GSSendByteNum==SERROR)
		{
			m_nErrno=SocketErrNo;
			LogOutput(1,"sendto Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nErrno=0;
			int port=0;
			char * ip=GetSocketAddr(pSockAddr,port);
			LogOutput(2,"sendto remote ip=%s,port=%d,len:%d\n",ip,port,GSSendByteNum);
		}
	}
	return GSSendByteNum;
}

int CSocketControl::SendToSocket(void* SsendbufP,int sendsize)
{
	return SendToSocket(SsendbufP,sendsize,(struct sockaddr *)&m_struSockRemoteAddr);
}

int CSocketControl::RecvFromSocket(void* SrecvbufP,int recvsize,int GSRType)  //GSRType 表示调用方式 0 无特殊行为 MSG_PEEK 表示复制到缓冲区 MSG_OOB 表示处理带外数据
{
	int GSockAddSize=sizeof(sockaddr);
	int GSRecvByteNum =0;
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCONNECT_F && m_nSocketStatu!=SACCEPT_F)
	{
		GSRecvByteNum=recvfrom(m_hSocket,(char *)SrecvbufP,recvsize,GSRType,(struct sockaddr *)&m_struSockRecvAddr,&GSockAddSize);
		if (GSRecvByteNum==SERROR)
		{
			m_nErrno=SocketErrNo;
			LogOutput(1,"recvfrom Error,err=%d,info=%s,status:%d\n",m_nErrno,GetSocketError(m_nErrno),m_nSocketStatu);
		}
		else
		{
			m_nErrno=0;
			int port=0;
			char * ip=GetSocketAddr((struct sockaddr *)&m_struSockRecvAddr,port);
			LogOutput(2,"recvfrom remote ip=%s,port=%d,len:%d\n",ip,port,GSRecvByteNum);
		}
	}
	return GSRecvByteNum;
}

void CSocketControl::CloseSocket()
{
	if (m_nSocketStatu!=SINIT && m_nSocketStatu!=SCREATE_F && m_nSocketStatu!=SACCEPT_F)
	{
		if (m_hSocket!=0)
		{
			if (closesocket(m_hSocket)==SERROR)
			{
				m_nErrno=SocketErrNo;
				LogOutput(1,"closesocket Error,err=%d,info=%s\n",m_nErrno,GetSocketError(m_nErrno));
			}
			else
			{
				m_nSocketStatu=SINIT;
				m_hSocket=NULL;
				m_nErrno=0;
				m_bTCPClientBind=false;
			}
		}
	}
}

SOCKET CSocketControl::GetSocketHandle()
{
	return m_hSocket;
}

void CSocketControl::GetSocketLocalAddr(int &port, char *ip)
{
	port=ntohs(m_struSockLocalAddr.sin_port);
	if(ip) strcpy(ip,inet_ntoa(m_struSockLocalAddr.sin_addr));
}

void CSocketControl::GetSocketRecvAddr(int &port, char *ip)
{
	port=ntohs(m_struSockRecvAddr.sin_port);
	if(ip) strcpy(ip,inet_ntoa(m_struSockRecvAddr.sin_addr));
}

void CSocketControl::GetSocketRemoteAddr(int &port, char *ip)
{
	port=ntohs(m_struSockRemoteAddr.sin_port);
	if(ip) strcpy(ip,inet_ntoa(m_struSockRemoteAddr.sin_addr));
}

char * CSocketControl::GetSocketAddr(struct sockaddr * pAddr,int &port)
{
	char *ip =NULL;
	sockaddr_in * pAddr2=(sockaddr_in *)pAddr;
	port=ntohs(pAddr2->sin_port);
	ip=inet_ntoa(pAddr2->sin_addr);
	return ip;
}

void CSocketControl::InitSocketLocalAddr()
{
	m_struSockLocalAddr.sin_family=AF_INET;
	m_struSockLocalAddr.sin_port=0;  
	m_struSockLocalAddr.sin_addr.s_addr=INADDR_ANY;
}

void CSocketControl::CopySocketParam(SOCKET hSocket,int nSocketStatu,int nGErrno,sockaddr_in * struGSockAddr)
{
	m_hSocket=hSocket;
	m_nSocketStatu=nSocketStatu;
	m_nErrno=nGErrno;
	memcpy(&m_struSockLocalAddr,struGSockAddr,sizeof(sockaddr_in));
}

bool CSocketControl::SetNoneBlockingOpt(bool bNone)
{
#ifdef _WIN32
	unsigned long lValue = bNone ? 1 : 0;
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

bool CSocketControl::SetRecvSendBufOpt()
{
	int err= 0;
	int nRecvBufSize=16*1024*1024;
	int nSendBufSize=65536;
	if ((0 != setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSize, sizeof(int))) ||
		(0 != setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nSendBufSize, sizeof(int))))
	{
		err= -1;
	}
	return (err == 0);
}

//AF43 4 100110 152[0x98] NGN/3G Singaling H
//EF   5 101110 184[0xB8] NGN/3G voice
//EF用于承载语音的流量，因为语音要求低延迟，低抖动，低丢包率，是仅次于协议报文的最重要的报文。
//AF4用来承载语音的信令流量
bool CSocketControl::SetTosOpt(unsigned char nTos)
{
	int nValue=nTos;
	int err=setsockopt(
				m_hSocket,
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

bool CSocketControl::SetReuseAddrOpt(bool bReuse)
{
	int err= -1;
	if (bReuse)
	{
		int option = 1;
		err=setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR,(char *)&option, sizeof(option));
	}
	return (err == 0);
}

void CSocketControl::SetTCPKeepAliveEnable(bool bEnable,int nIdleTimer,int nIntervalTimer)
{
	m_bSetTCPKeepAlive=bEnable;
	m_nKeepIdleTimer=nIdleTimer;
	m_nKeepIntervalTimer=nIntervalTimer;
}

//设置keepalive后 keepalive通知我们异常断开的情况: 这里采用select模式，测试环境为XP系统和Win7系统，返回值如下：
//1． 正常断开 //select 函数正常返回 recv函数返回0
//2． 异常断开
//a)  程序异常退出，如 client 端重启，应用非正常关闭等 //select函数正常返回 recv函数返回SOCKET_ERROR WSAGetLastError()得到的结果为WSAECONNRESET(10054)
//b)  网络断开   //结果同上 select 函数正常返回 recv函数返回SOCKET_ERROR WSAGetLastError()得到的结果为WSAECONNRESET(10054)
bool CSocketControl::SetKeepAliveOpt(bool bKeepAlive,int nIdleTimer,int nIntervalTimer)
{
	int err= 0;
	if(bKeepAlive)
	{
		int nEnable = 1;
		if(nIdleTimer <=0) nIdleTimer =1000;
		if(nIntervalTimer <=0) nIntervalTimer =1000;

#ifdef _WIN32

		//notice //#include <mstcpip.h>
#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)  

		struct tcp_keepalive
		{
			ULONG onoff;
			ULONG keepalivetime;
			ULONG keepaliveinterval;
		};

		err=setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE,(char *)&nEnable, sizeof(nEnable));
		if (err == SOCKET_ERROR)
		{
			return false;
		}
		//KeepAlive实现，单位毫秒
		tcp_keepalive alive_in;
		tcp_keepalive alive_out;
		alive_in.keepalivetime = nIdleTimer;         //开始首次KeepAlive探测前的TCP空闭时间 多长时间没有数据就开始send心跳包
		alive_in.keepaliveinterval = nIntervalTimer; //两次KeepAlive探测间的时间间隔 每隔多长时间send一个心跳包
		alive_in.onoff = nEnable;
		unsigned long nReturn = 0;
		err = WSAIoctl(m_hSocket, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),&alive_out, sizeof(alive_out), &nReturn, NULL, NULL);
		if (err == SOCKET_ERROR)
		{
			return false;
		}

#else

		//KeepAlive实现，单位秒 //notice the linux wait test
		int keepAlive = nEnable;                  //设定KeepAlive
		int keepIdle = nIdleTimer / 1000;         //开始首次KeepAlive探测前的TCP空闲时间
		int keepInterval = nIntervalTimer / 1000; //两次KeepAlive探测间的时间间隔
		int keepCount = 3;                        //判定断开前的KeepAlive探测次数
		setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive));
		setsockopt(sock,SOL_TCP,TCP_KEEPIDLE,(void *)&keepIdle,sizeof(keepIdle));
		setsockopt(sock,SOL_TCP,TCP_KEEPINTVL,(void *)&keepInterval,sizeof(keepInterval));
		setsockopt(sock,SOL_TCP,TCP_KEEPCNT,(void *)&keepCount,sizeof(keepCount));

#endif

	}
	return (err == 0);
}

bool CSocketControl::SetKeepAliveOpt(bool bKeepAlive)
{
	int err= 0;
	if(bKeepAlive)
	{
		int nEnable = 1;
		err=setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE,(char *)&nEnable, sizeof(nEnable));
		err=setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nEnable, sizeof(nEnable) );
	}
	return (err == 0);
}

char * CSocketControl::GetSocketError(int code)
{
	char * strError="Unknown.";

#ifdef _WIN32

	switch(code)
	{
	case 10004: strError = "Interrupted system call 中断的系统呼叫"; break; 
	case 10009: strError = "Bad file number"; break; 
	case 10013: strError = "ermission denied 进入被拒绝"; break; 
	case 10014: strError = "Bad address"; break; 
	case 10022: strError = "Invalid argument (not bind)"; break; 
	case 10024: strError = "Too many open files"; break; 
	case 10035: strError = "Operation would block"; break; 
	case 10036: strError = "Operation now in progress"; break; 
	case 10037: strError = "Operation already in progress"; break; 
	case 10038: strError = "Socket operation on non-socket"; break; 
	case 10039: strError = "Destination address required"; break; 
	case 10040: strError = "Message too long"; break; 
	case 10041: strError = "rotocol wrong type for socket"; break; 
	case 10042: strError = "Bad protocol option"; break; 
	case 10043: strError = "protocol not supported "; break; 
	case 10044: strError = "Socket type not supported"; break; 
	case 10045: strError = "Operation not supported on socket"; break; 
	case 10046: strError = "rotocol family not supported"; break; 
	case 10047: strError = "Address family not supported by protocol family"; break; 
	case 10048: strError = "Address already in use socket"; break; 
	case 10049: strError = "Can't assign requested address"; break; 
	case 10050: strError = "Network is down"; break; 
	case 10051: strError = "Network is unreachable"; break; 
	case 10052: strError = "Net dropped connection or reset"; break; 
	case 10053: strError = "Software caused connection abort"; break; 
	case 10054: strError = "Connection reset by peer 连接被强行关闭或被防火樯proxy中断"; break; 
	case 10055: strError = "No buffer space available Tracker的buffer已满"; break; 
	case 10056: strError = "Socket is already connected"; break; 
	case 10057: strError = "Socket is not connected"; break; 
	case 10058: strError = "Can't send after socket shutdown"; break; 
	case 10059: strError = "Too many references, can't splice"; break; 
	case 10060: strError = "Connection timed out "; break; 
	case 10061: strError = "Connection refused Tracker"; break; 
	case 10062: strError = "Too many levels of symbolic links"; break; 
	case 10063: strError = "File name too long 过长或含无效的字串"; break; 
	case 10064: strError = "Host is down"; break; 
	case 10065: strError = "No Route to Host"; break; 
	case 10066: strError = "Directory not empty"; break; 
	case 10067: strError = "Too many processes"; break; 
	case 10068: strError = "Too many users"; break; 
	case 10069: strError = "Disc Quota Exceeded"; break; 
	case 10070: strError = "Stale NFS file handle"; break; 
	case 10091: strError = "Network SubSystem is unavailable"; break; 
	case 10092: strError = "WINSOCK DLL Version out of range  winsock.dll问题"; break; 
	case 10093: strError = "Successful WSASTARTUP not yet performed"; break; 
	case 10071: strError = "Too many levels of remote in path"; break; 
	case 11001: strError = "Host not found"; break; 
	case 11002: strError = "Non-Authoritative Host not found"; break; 
	case 11003: strError = "Non-Recoverable errors: formERR, REFUSED, NOTIMP"; break; 
	case 11004: strError = "Valid name, no data record of requested type"; break;
	default: strError ="Unknown Error."; break;
	}

#else

	strError =strerror(code);
#endif

	return strError;
}

bool CSocketControl::GetLocalFirstIP(char * localIP,int & nLen)
{
	bool bRet=false;

#ifdef _WIN32

	char host_name[256] ={0};
	if (gethostname(host_name, sizeof(host_name) -1 ) == SOCKET_ERROR) return bRet;
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0) return bRet;
	for(int i = 0; phe->h_addr_list[i] != 0; i++)
	{
		bRet=false;
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
		char * p=inet_ntoa(addr);
		if(p)
		{
			if(localIP && nLen>0)
			{
				nLen=strlen(p);
				strcpy(localIP,p);
				bRet=true;
			}
			//IP 0. , 127.0.0.1 , rule out
			if( strlen(p) >0 && p[0] >='1' && (strcmp(localIP,"127.0.0.1") != 0) )
			{
				break;
			}
		}
	}

#endif

	return bRet;
}

void CSocketControl::SetTCPManualAddrEnable(bool bEnable)
{
	m_bTCPSpecifiedPort =bEnable;
}
