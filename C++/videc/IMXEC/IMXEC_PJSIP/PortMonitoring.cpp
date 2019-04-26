
// PortMonitoring.cpp:���˿��Ƿ�ռ��. ������XP���ϸ߼��汾.

#include "PortMonitoring.h"
#include <stdio.h>
#include "XSocketUDP.h"
#include "XSocketTCP.h"

#define MIBTABLESIZE 200

//---------------------------------------------------------------------------
// ����Ϊ��TCP��صĽṹ. 
typedef struct tagMIB_TCPEXROW{
	DWORD dwState;      		// ����״̬.
	DWORD dwLocalAddr;     		// ���ؼ������ַ.
	DWORD dwLocalPort;       	// ���ؼ�����˿�.
	DWORD dwRemoteAddr;    		// Զ�̼������ַ.
	DWORD dwRemotePort;     	// Զ�̼�����˿�.
	DWORD dwProcessId;
} MIB_TCPEXROW, *PMIB_TCPEXROW;

typedef struct tagMIB_TCPEXTABLE{
	DWORD dwNumEntries;
	MIB_TCPEXROW table[MIBTABLESIZE];    // �����С�������.
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;


//---------------------------------------------------------------------------
// ����Ϊ��UDP��صĽṹ. 
typedef struct tagMIB_UDPEXROW{
	DWORD dwLocalAddr;     	    // ���ؼ������ַ.
	DWORD dwLocalPort;     	    // ���ؼ�����˿�.
	DWORD dwProcessId;
} MIB_UDPEXROW, *PMIB_UDPEXROW;

typedef struct tagMIB_UDPEXTABLE{
	DWORD dwNumEntries;
	MIB_UDPEXROW table[MIBTABLESIZE];    // �����С�������. 
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


//---------------------------------------------------------------------------
// ���õ�iphlpapi.dll�еĺ���ԭ�Ͷ���.
typedef DWORD (WINAPI *PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK)(
	PMIB_TCPEXTABLE *pTcpTable, // ���ӱ�����.
	BOOL bOrder,                
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

typedef DWORD (WINAPI *PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK)(
	PMIB_UDPEXTABLE *pUdpTable, // ���ӱ�����.
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

//���˿��Ƿ�ռ��				 
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
	//���TCP�б�
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

//���˿��Ƿ�ռ��				 
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

	//���UDP�б�.
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
