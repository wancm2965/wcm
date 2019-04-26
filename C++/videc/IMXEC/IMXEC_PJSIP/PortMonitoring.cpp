
// PortMonitoring.cpp:检测端口是否被占用. 适用于XP及较高级版本.

#include "PortMonitoring.h"
#include <stdio.h>
#include "XSocketUDP.h"
#include "XSocketTCP.h"

#define MIBTABLESIZE 200

//---------------------------------------------------------------------------
// 以下为与TCP相关的结构. 
typedef struct tagMIB_TCPEXROW{
	DWORD dwState;      		// 连接状态.
	DWORD dwLocalAddr;     		// 本地计算机地址.
	DWORD dwLocalPort;       	// 本地计算机端口.
	DWORD dwRemoteAddr;    		// 远程计算机地址.
	DWORD dwRemotePort;     	// 远程计算机端口.
	DWORD dwProcessId;
} MIB_TCPEXROW, *PMIB_TCPEXROW;

typedef struct tagMIB_TCPEXTABLE{
	DWORD dwNumEntries;
	MIB_TCPEXROW table[MIBTABLESIZE];    // 任意大小数组变量.
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;


//---------------------------------------------------------------------------
// 以下为与UDP相关的结构. 
typedef struct tagMIB_UDPEXROW{
	DWORD dwLocalAddr;     	    // 本地计算机地址.
	DWORD dwLocalPort;     	    // 本地计算机端口.
	DWORD dwProcessId;
} MIB_UDPEXROW, *PMIB_UDPEXROW;

typedef struct tagMIB_UDPEXTABLE{
	DWORD dwNumEntries;
	MIB_UDPEXROW table[MIBTABLESIZE];    // 任意大小数组变量. 
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


//---------------------------------------------------------------------------
// 所用的iphlpapi.dll中的函数原型定义.
typedef DWORD (WINAPI *PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK)(
	PMIB_TCPEXTABLE *pTcpTable, // 连接表缓冲区.
	BOOL bOrder,                
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

typedef DWORD (WINAPI *PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK)(
	PMIB_UDPEXTABLE *pUdpTable, // 连接表缓冲区.
	BOOL bOrder,                
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

CPortMonitoring::CPortMonitoring()
{
	m_bInitSocketLib=false;
}

CPortMonitoring::CPortMonitoring(bool bInitLib)
{
	m_bInitSocketLib=bInitLib;
}

CPortMonitoring::~CPortMonitoring()
{

}

//检测端口是否被占用				 
bool CPortMonitoring::CheckTCPPort(int nPort)
{
	bool bRet=true;

	XSocketTCP tXSocketTCP;
	unsigned long ulIP=ntohl(inet_addr("127.0.0.1"));
	bRet=tXSocketTCP.Connect(ulIP,nPort,ulIP);
	tXSocketTCP.Close();
	return bRet;

	//notice Win7 not supported
	int i=0;
	PMIB_TCPEXTABLE TCPExTable;
	HMODULE hIpDLL = NULL;
	WSADATA WSAData;
	if(m_bInitSocketLib)
	{
		if( WSAStartup(MAKEWORD(1, 1), &WSAData ))
		{
			goto FuncExit;
		}
	}

	hIpDLL = LoadLibrary( "iphlpapi.dll");
	if ( hIpDLL == NULL)
	{
		goto FuncExit;
	}
	
	PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK pAllocateAndGetTcpExTableFromStack = NULL;
	pAllocateAndGetTcpExTableFromStack =(PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK) 
		GetProcAddress( hIpDLL,	"AllocateAndGetTcpExTableFromStack");

	if(pAllocateAndGetTcpExTableFromStack(&TCPExTable, TRUE, GetProcessHeap(), 2, 2))
	{
		goto FuncExit;
	}
	//获得TCP列表
	bRet=false;
	for( i = 0; i <TCPExTable->dwNumEntries; i++ )
	{
		if(nPort ==	htons( (WORD) TCPExTable->table[i].dwLocalPort) )
		{
			bRet=true;
			goto FuncExit;
		}
	}

FuncExit:
	if(hIpDLL)
	{
		FreeLibrary(hIpDLL);
		hIpDLL=NULL;
	}
	if(m_bInitSocketLib) WSACleanup();

	return bRet;
}

//检测端口是否被占用				 
bool CPortMonitoring::CheckUDPPort(int nPort)
{
	bool bRet=true;

	XSocketUDP tXSocketUDP;
	if (0 == tXSocketUDP.Open(0,nPort,false))
	{
		bRet=false;
	}
	tXSocketUDP.Close();
	return bRet;

	int i=0;
	PMIB_UDPEXTABLE UDPExTable;
	HMODULE hIpDLL = NULL;
	WSADATA WSAData;

	if(m_bInitSocketLib)
	{
		if( WSAStartup(MAKEWORD(1, 1), &WSAData ))
		{
			goto FuncExit;
		}
	}

	hIpDLL = LoadLibrary( "iphlpapi.dll");
	if ( hIpDLL == NULL)
	{
		goto FuncExit;
	}

	PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK pAllocateAndGetUdpExTableFromStack = NULL;
	pAllocateAndGetUdpExTableFromStack =(PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK) 
		GetProcAddress(hIpDLL, "AllocateAndGetUdpExTableFromStack" );

	if(pAllocateAndGetUdpExTableFromStack(&UDPExTable, TRUE, GetProcessHeap(), 2, 2 ))
	{
		goto FuncExit;
	}

	//获得UDP列表.
	bRet=false;
	for( i = 0; i <UDPExTable->dwNumEntries; i++ )
	{
		if(nPort ==	htons( (WORD) UDPExTable->table[i].dwLocalPort) )
		{
			bRet=true;
			goto FuncExit;
		}
	}

FuncExit:
	if(hIpDLL)
	{
		FreeLibrary(hIpDLL);
		hIpDLL=NULL;
	}
	if(m_bInitSocketLib) WSACleanup();

	return bRet;
}
