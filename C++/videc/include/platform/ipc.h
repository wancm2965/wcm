/******************************************************************************
ģ����	: IPC
�ļ���	: ipc.h
����    : IPC���ͷ�ļ�
�汾	��v0.1
******************************************************************************/



#ifndef _IPC_H
#define _IPC_H

///////////////////////////////////////////////////////////////////
//	Linux ����ϵͳͷ�ļ�
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	#ifdef PWLIB_SUPPORT
		#include <ptlib.h>
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <stdarg.h>
	#include <malloc.h>
	#include <errno.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <sys/times.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <termios.h>
	#include <signal.h>
	
	#ifndef _EQUATOR_
		#include <mqueue.h>
		#include <sys/epoll.h>
		#include <sys/resource.h>
	#endif

#endif //_LINUX_


///////////////////////////////////////////////////////////////////
//	Win32 ����ϵͳͷ�ļ�
///////////////////////////////////////////////////////////////////
#ifdef _MSC_VER

	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	#if _MSC_VER <= 1200
		#define _WIN32_WINNT 0x0400
	#else
		#define _WIN32_WINNT 0x0501
	#endif

	#ifdef WINVER
		#undef WINVER
	#endif
	#if _MSC_VER <= 1200
		#define WINVER 0x0400
	#else
		#define WINVER 0x0501
	#endif

#include <malloc.h>  
#include <time.h>

	#ifndef _WINDOWS_
		#include <afx.h>
		#include <afxwin.h>
		#include <winsock2.h>
	#else
		#include <windows.h>
		#include <winsock2.h>
	#endif

	#include <assert.h>
	#pragma comment(lib,"Ws2_32.lib")

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	����ϵͳͷ�ļ������Ͷ��壩
///////////////////////////////////////////////////////////////////
#include "hptype.h"

///////////////////////////////////////////////////////////////////
//	����ϵͳ����������Ͷ���
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	typedef int 				 BOOL;
	#define TRUE				 1
	#define FALSE				 0

	typedef void *(*LINUXFUNC)(void *);

	#define SEMHANDLE			 pthread_cond_t*

	#define TASKHANDLE			 pthread_t

#endif	// _LINUX_


#ifndef _MSC_VER

	typedef unsigned long long	 u64;
	#define SOCKHANDLE			 int
	#define SERIALHANDLE		 int

	#ifndef SOCKADDR
		#define SOCKADDR		 sockaddr
	#endif
	#define SOCKADDR_IN 		 sockaddr_in

#endif	// _MSC_VER

#ifdef _MSC_VER

	#define SEMHANDLE			 HANDLE
	#define TASKHANDLE			 HANDLE

	#define SOCKHANDLE			 SOCKET
	#define SERIALHANDLE		 HANDLE

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	����ϵͳ��غ궨��
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	#define SOCKET_ERROR		  (-1)
	#define INVALID_SOCKET		  (-1)
	#define INVALID_SERIALHANDLE  (-1)

	#define IPC_NO_WAIT 		  0
	#define IPC_WAIT_FOREVER	  (-1)

	//SockShutdown����
	#define STOP_RECV			  0
	#define STOP_SEND			  1
	#define STOP_BOTH			  2

#endif // _LINUX_



#ifdef _MSC_VER

	#define INVALID_SERIALHANDLE  INVALID_HANDLE_VALUE
	#define TIPCCommTimeouts	  COMMTIMEOUTS
	#define TIPCDCB 			  DCB
	#define TIPCComStat 		  COMSTAT

	#define IPC_NO_WAIT 		  ((int)0)
	#define IPC_WAIT_FOREVER	  ((int)-1)

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	����ϵͳ�޹غ궨��
///////////////////////////////////////////////////////////////////
// for Microsoft c++
#ifdef _MSC_VER

	//������
	#ifndef _EQUATOR_
		#pragma pack(push)
		#pragma pack(1)
	#endif

	#define API extern "C"			__declspec(dllexport)
	#define IPC_API 				__declspec(dllexport)

// for gcc
#else

	#define API extern "C"
	#define IPC_API

#endif	// _MSC_VER

//IPC�汾��Ϣ
#define IPCVERSION				"IPC.A7.Platform.20131213"







//δʹ��
const u8  IPCLOCAL				= 0;
//δʹ��
const u8  TCP_GATEWAY			= 255;
//δʹ��
const u16 SECONDS				= 1000;

#ifdef _LINUX_
    #define MULTI_LISTEN_SOCKET_SUPPORT 1
#endif

//�����������׽�����
#ifdef MULTI_LISTEN_SOCKET_SUPPORT
    const u32 MAX_LISTEN_SOCK_NUM       = 16;
#endif //MULTI_LISTEN_SOCKET_SUPPORT


//��������ⲿ�����
#ifdef IPC_MAX_NODE_NUM
	const u32 MAX_NODE_NUM		= IPC_MAX_NODE_NUM;
#else
	const u32 MAX_NODE_NUM		= 512;
#endif

//ͬһ������Ӧ����
const u16 MAX_APP_NUM			 = 512;
//ͬһӦ�����ʵ����
const u16 MAX_INST_NUM			 = 65000;
//���ʱ����
const u16 MAX_TIMER_NUM 		 = 5000;
//��󴮿���
const u8  MAX_SERIALPORT_NUM	 = 10;

//��Ч���ⲿ����
#define IPC_INVALID_NODE			 0
//���ؽ���
#define IPC_LOCAL_NODE				 0

//��Ч��Ӧ�ú�
#define IPC_INVALID_APP 			 0
//����ȫ��Ӧ�ú�
#define IPC_LOCAL_APP				 0

//��Ч��ʵ����
#define IPC_INVALID_INS 			 0xfffb
//����ʵ����
#define IPC_LOCAL_INS				 0

//Ĭ��TCP�������ʱ����(��)
#define DEFAULT_TCP_HEARTBEAT_TIME		10
//Ĭ��TCP����������(��μ���ʧ����Ϊ�Ѿ�����)
#define DEFAULT_TCP_HEARTBEAT_NUM		3
//Ĭ��IPCϵͳ״̬��ӡʱ����(��)
#define DEFAULT_STATUS_MSGOUT_TIME		3600
//������TCP�������
#define UNDETECT_TCP_HEARTBEAT	 		0

#ifndef MINCHAR
	#define MINCHAR 				0x80
#endif

#ifndef MAXCHAR
	#define MAXCHAR 				0x7f
#endif

#ifndef MINSHORT
	#define MINSHORT				0x8000
#endif

#ifndef MAXSHORT
	#define MAXSHORT				0x7fff
#endif

#ifndef MINLONG
	#define MINLONG 				0x80000000
#endif

#ifndef MAXLONG
	#define MAXLONG 				0x7fffffff
#endif

#ifndef MAXBYTE
	#define MAXBYTE 				0xff
#endif

#ifndef MAXWORD
	#define MAXWORD 				0xffff
#endif

#ifndef MAXDWORD
	#define MAXDWORD				0xffffffff
#endif

#ifndef _MSC_VER

//
// Provider capabilities flags.
//

#define PCF_DTRDSR		  ((u32)0x0001)
#define PCF_RTSCTS		  ((u32)0x0002)
#define PCF_RLSD		  ((u32)0x0004)
#define PCF_PARITY_CHECK  ((u32)0x0008)
#define PCF_XONXOFF 	  ((u32)0x0010)
#define PCF_SETXCHAR	  ((u32)0x0020)
#define PCF_TOTALTIMEOUTS ((u32)0x0040)
#define PCF_INTTIMEOUTS   ((u32)0x0080)
#define PCF_SPECIALCHARS  ((u32)0x0100)
#define PCF_16BITMODE	  ((u32)0x0200)

//
// Comm provider settable parameters.
//

#define SP_PARITY		  ((u32)0x0001)
#define SP_BAUD 		  ((u32)0x0002)
#define SP_DATABITS 	  ((u32)0x0004)
#define SP_STOPBITS 	  ((u32)0x0008)
#define SP_HANDSHAKING	  ((u32)0x0010)
#define SP_PARITY_CHECK   ((u32)0x0020)
#define SP_RLSD 		  ((u32)0x0040)

//
// Settable baud rates in the provider.
//


#if defined(_LINUX_) && !defined(_EQUATOR_)
#define BAUD_300		  (u32)300
#define BAUD_1200		  (u32)1200
#define BAUD_2400		  (u32)2400
#define BAUD_4800		  (u32)4800
#define BAUD_9600		  (u32)9600
#define BAUD_19200		  (u32)19200
#define BAUD_38400		  (u32)38400
#define BAUD_57600		  (u32)57600
#define BAUD_115200 	  (u32)115200
#endif

// Equator��ֻ�����ò����ʣ������Ķ���������
#ifdef _EQUATOR_
// ע�⣺�������Ϊ9600
#define BAUD_110		  (u16)110
#define BAUD_300		  (u16)300
#define BAUD_600		  (u16)600
#define BAUD_1200		  (u16)1200
#define BAUD_2400		  (u16)2400
#define BAUD_4800		  (u16)4800
#define BAUD_9600		  (u16)9600
#endif

//
// Settable Data Bits
//
#if (defined(_LINUX_) && !defined(_EQUATOR_))
#define DATABITS_5		  (u8)5
#define DATABITS_6		  (u8)6
#define DATABITS_7		  (u8)7
#define DATABITS_8		  (u8)8
#else
#define DATABITS_5		  ((u16)0x0001)
#define DATABITS_6		  ((u16)0x0002)
#define DATABITS_7		  ((u16)0x0004)
#define DATABITS_8		  ((u16)0x0008)
#define DATABITS_16 	  ((u16)0x0010)
#define DATABITS_16X	  ((u16)0x0020)
#endif

//
// Settable Stop and Parity bits.
//
#if (defined(_LINUX_) && !defined(_EQUATOR_))
#define STOPBITS_10 	  (u8)1
#define STOPBITS_20 	  (u8)2
#else
#define STOPBITS_10 	  ((u16)0x0001)
#define STOPBITS_15 	  ((u16)0x0002)
#define STOPBITS_20 	  ((u16)0x0004)
#endif

#if  (defined(_LINUX_) && !defined(_EQUATOR_))
#define PARITY_NONE 	  (u8)0
#define PARITY_ODD	      (u8)1  
#define PARITY_EVEN 	  (u8)2
#define PARITY_MARK 	  (u8)3
#define PARITY_SPACE	  (u8)4
#else
#define PARITY_NONE 	  ((u16)0x0100)
#define PARITY_ODD		  ((u16)0x0200)
#define PARITY_EVEN 	  ((u16)0x0400)
#define PARITY_MARK 	  ((u16)0x0800)
#define PARITY_SPACE	  ((u16)0x1000)
#endif
//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE		0x00
#define DTR_CONTROL_ENABLE		0x01
#define DTR_CONTROL_HANDSHAKE	0x02

//
// RTS Control Flow Values
//
#define RTS_CONTROL_DISABLE		0x00
#define RTS_CONTROL_ENABLE		0x01
#define RTS_CONTROL_HANDSHAKE	0x02
#define RTS_CONTROL_TOGGLE		0x03

#define NOPARITY				0
#define ODDPARITY				1
#define EVENPARITY				2
#define MARKPARITY				3
#define SPACEPARITY 			4

#define ONESTOPBIT				0
#define ONE5STOPBITS			1
#define TWOSTOPBITS 			2

//
// Escape Functions
//

#define SETXOFF 				1		// Simulate XOFF received
#define SETXON					2		// Simulate XON received
#define SETRTS					3		// Set RTS high
#define CLRRTS					4		// Set RTS low
#define SETDTR					5		// Set DTR high
#define CLRDTR					6		// Set DTR low
#define RESETDEV				7		// Reset device if possible
#define SETBREAK				8		// Set the device break line.
#define CLRBREAK				9		// Clear the device break line.

//
// PURGE function flags.
//
#define PURGE_TXABORT			0x0001	// Kill the pending/current writes to the comm port.
#define PURGE_RXABORT			0x0002	// Kill the pending/current reads to the comm port.
#define PURGE_TXCLEAR			0x0004	// Kill the transmit queue if there.
#define PURGE_RXCLEAR			0x0008	// Kill the typeahead buffer if there.

typedef struct 
{  
	u32 ReadIntervalTimeout; 
	u32 ReadTotalTimeoutMultiplier; 
	u32 ReadTotalTimeoutConstant; 
	u32 WriteTotalTimeoutMultiplier; 
	u32 WriteTotalTimeoutConstant; 
} TIPCCommTimeouts; 

typedef struct
{ 
	u32 DCBlength;			 // sizeof(DCB) 
	u32 BaudRate;			 // current baud rate 
	u32 fBinary: 1; 		 // binary mode, no EOF check 
	u32 fParity: 1; 		 // enable parity checking 
	u32 fOutxCtsFlow:1; 	 // CTS output flow control 
	u32 fOutxDsrFlow:1; 	 // DSR output flow control 
	u32 fDtrControl:2;		 // DTR flow control type 
	u32 fDsrSensitivity:1;	 // DSR sensitivity 
	u32 fTXContinueOnXoff:1; // XOFF continues Tx 
	u32 fOutX: 1;			 // XON/XOFF out flow control 
	u32 fInX: 1;			 // XON/XOFF in flow control 
	u32 fErrorChar: 1;		 // enable error replacement 
	u32 fNull: 1;			 // enable null stripping 
	u32 fRtsControl:2;		 // RTS flow control 
	u32 fAbortOnError:1;	 // abort on error 
	u32 fDummy2:17; 		 // reserved 
	u32 wReserved;			 // not currently used 
	u16 XonLim; 			 // transmit XON threshold 
	u16 XoffLim;			 // transmit XOFF threshold 
	u8	ByteSize;			 // number of bits/Byte, 4-8 
	u8	Parity; 			 // 0-4=no,odd,even,mark,space 
	u8	StopBits;			 // 0,1,2 = 1, 1.5, 2 
	char XonChar;			 // Tx and Rx XON character 
	char XoffChar;			 // Tx and Rx XOFF character 
	char ErrorChar; 		 // error replacement character 
	char EofChar;			 // end of input character 
	char EvtChar;			 // received event character 
	u16  wReserved1;		 // reserved; do not use 
}TIPCDCB; 

typedef struct
{ 
	u32 fCtsHold : 1; 
	u32 fDsrHold : 1; 
	u32 fRlsdHold : 1; 
	u32 fXoffHold : 1; 
	u32 fXoffSent : 1; 
	u32 fEof : 1; 
	u32 fTxim : 1; 
	u32 fReserved : 25; 
	u32 cbInQue; 
	u32 cbOutQue; 
} TIPCComStat; 

#endif //#ifndef _MSC_VER


// ���ݽ��ţ�APP�źͱ���ʵ���Ź���ȫ��ʵ��ID
inline u32	IPCMAKEID( u16 app, u16 ins = 0  )
{
	return (app << 16)+ins;
}

// ��ID�л��Ӧ�ú�
inline u16 IPCGETAPP( u32 GlobIID )
{
	return (u16)(GlobIID >> 16);
}


// ��ID�л��ʵ����
inline u16 IPCGETINS( u32 GlobIID )
{
	return (u16)(GlobIID);
}



//telnet�ն���ʾ����󳤶�
#define MAX_PROMPT_LEN				21

// IPC������ 
#define IPC_ERROR_BASE				0
//�����ɹ�
#define IPC_OK						(IPC_ERROR_BASE + 0)
//����ʧ��
#define IPC_ERROR					(IPC_ERROR_BASE + 1)
//δʹ��
#define IPCERR_SEND_FAIL			(IPC_ERROR_BASE + 2)
//����ͬ����Ϣ��ʱ
#define IPCERR_SEND_TIMEOUT 		(IPC_ERROR_BASE + 3)
//ͬ����ϢӦ�𳬳�
#define IPCERR_SYNCACK_EXCEED		(IPC_ERROR_BASE + 4)
//ͬһӦ����ʵ�������Ѿ�����
#define IPCERR_ALIAS_REPEAT 		(IPC_ERROR_BASE + 5)
//δʹ��
#define IPCERR_APP_REPEAT			(IPC_ERROR_BASE + 6)
//�ⲿ������Ч
#define IPCERR_NODE_INVALID 		(IPC_ERROR_BASE + 7)
//������Ч
#define IPCERR_ILLEGAL_PARAM		(IPC_ERROR_BASE + 8)
//ͬ����Ϣ�Զ���Ӧ��
#define IPCERR_PEER_NOT_RESPOND 	(IPC_ERROR_BASE + 9)

//�����Ϣ����
#define MAX_MSG_LEN 				(u32)0x100000
//ʵ��������󳤶�
#define MAX_ALIAS_LEN				(u8)255
//���ͬ����ϢӦ�𳤶�
#define MAX_SYNCACK_LEN 			MAX_MSG_LEN
//������ݳ���(δʹ��)
#define MAX_DATA_LEN				(MAX_MSG_LEN+MAX_ALIAS_LEN+MAX_SYNCACK_LEN)

//IPC��Ϣ����
// �첽��Ϣ
#define MSG_TYPE_ASYNC				(u16)5
// ͬ����Ϣ
#define MSG_TYPE_SYNC				(u16)4
// ͬ��Ӧ����Ϣ
#define MSG_TYPE_SYNCACK			(u16)3
// ȫ��ͬ����Ϣ
#define MSG_TYPE_GSYNC				(u16)2
// ȫ��ͬ��Ӧ����Ϣ
#define MSG_TYPE_GSYNCACK			(u16)1
// ��ʱ��Ϣ
#define MSG_TYPE_TIMEOUT			(u16)0

//�л�ʵ��״̬��
#define NEXTSTATE(x)				NextState(x,#x)

//IPC �û��Զ�����Ϣ�¼��Ż�׼(С�ڸ�ֵ����ϢΪIPC������Ϣ���ϲ��ֹ���͸���Ϣ)
#define   IPC_USEREVENT_BASE		(u16)0x0400
//������Ϣ�¼���
#define IPCEVENT(x,y)		const u16 x = y

//δʹ��
IPCEVENT(IPC_CONFIRM,		0xff);
//�����������
IPCEVENT(IPC_POWERON,		0x100);
//δʹ��
IPCEVENT(IPC_SWITCHMASTER,	0x101);
//δʹ��
IPCEVENT(IPC_SWITCHSLAVE,	0x102);
//������޿���ʵ��
IPCEVENT(IPC_OVERFLOW,		0x103);
//δʹ��
IPCEVENT(IPC_EXCEPTION_IND, 0x104);
//δʹ��
IPCEVENT(IPC_INSNOTEXIST,	0x105);
//��Զ�˽���TCP��·�Ѷ�
IPCEVENT(IPC_DISCONNECT,	0x106);
//�㲥��Ϣȷ��Ӧ��
IPCEVENT(IPC_BROADCASTACK,	0x107);
//δʹ��
IPCEVENT(IPC_NODECLOSE, 	0x108);
//�������̽����Ϣ
IPCEVENT(IPC_NETBRAECHO,	0x109);
//�������̽����Ϣȷ��Ӧ��
IPCEVENT(IPC_NETBRAECHOACK, 0x110);
//IPCж����Ϣ
IPCEVENT(IPC_QUIT,			0x120);
//����ͨ�Ų�����Ϣ
IPCEVENT(IPC_NETSTATEST,	0x121);
//����ͨ�Ų�����Ϣȷ��Ӧ��
IPCEVENT(IPC_NETSTATESTACK, 0x122);
//δʹ��
IPCEVENT(IPC_APPCONN_ACK,	0x123);

//δʹ��
IPCEVENT(IPC_THREAD_EXIT,	0x124);

// Trc���ٱ�־
typedef enum TRCOPTION
{
	// ����״̬Ǩ��
	TRCSTATE	 = 1,
	// �����¼�
	TRCEVENT	 = 2,
	// ���ٶ�ʱ��
	TRCTIMER	 = 4,
}TRCOPTION;

//״̬ԾǨ���¼�����ʱ��������
#define TRCALL	   (u16)(TRCSTATE|TRCEVENT|TRCTIMER)



#ifdef _LINUX_
	/**
	 * �������紫������������
	 * �κ�һ���ṹҪ�������ϴ��䣬��д��һ�������������У�
	 * Ȼ��Ѵ�����������IPC����IPC������
	 * ��Ҫ��ֱ��ʹ�ô���һ���ṹ
	 */

	class CIPCStream
	{
	public:
		//��ǰ���ݳ���
		u32 m_dwCurrSize;
		//��ǰ���� 128 bytes is enough , GetPackedSize() is the best.
		u8 m_achData[128];
		
		CIPCStream()
		{
			m_dwCurrSize = 0;
			memset(m_achData, 0, sizeof(m_achData));
		}

		u32  PackElement(const u8* pData, u32 dwLen);
		u32  ExtractElement(u8* pData, u32 dwLen);
	};

	#define PackElement2(data)	PackElement((u8*)&data, sizeof(data))
	#define ExtractElement2(data)	ExtractElement((u8*)&data, sizeof(data))

	// IPC��Ϣͷ���ṹ
	class CIPCMessage 
	{
	public:
		//��Ϣ����
		u16 type;
		//��Ϣ��
		u16 event;
		//��Ϣ�峤��
		u32 length;
		//Դ���
		u32 srcnode;
		//Ŀ�Ľ��
		u32 dstnode;
		//Ŀ��Ӧ��ʵ��
		u32 dstid;
		//ԴĿ��ʵ��
		u32 srcid;

		//��Ϣ��
		u8 *content;
		
	

			//ͬ����ϢӦ��
			u8 *output;
			//ͬ����ϢӦ�𳤶�
			u32 outlen;
			//δʹ��
			u16 expire;

		
		//ʵ������
		char *dstAlias;
		//ʵ����������
		u8 dstAliasLen;

		//�õ�IPC��Ϣͷ������
		static u32 GetPackedSize()
		{
			return sizeof(u16) + sizeof(u16) + sizeof(u32) + sizeof(u32) +
				sizeof(u32) +sizeof(u32) + sizeof(u32) + sizeof(u8 *) +
				sizeof(u8 *) + sizeof(u32) + sizeof(u16) + sizeof(char *) + sizeof(u8);
		};

		//IPC��Ϣͷ�����
		void Pack(CIPCStream& tStream);
		//IPC��Ϣͷ�����
		void Extract(CIPCStream& tStream);
	};

#else
	// IPC��Ϣͷ���ṹ
	typedef struct CIPCMessage
	{
		//��Ϣ����
		u16 type;
		//��Ϣ��
		u16 event;
		//��Ϣ�峤��
		u32 length;
		//Դ���
		u32 srcnode;
		//Ŀ�Ľ��
		u32 dstnode;
		//Ŀ��Ӧ��ʵ��
		u32 dstid;
		//ԴĿ��ʵ��
		u32 srcid;
		//��Ϣ��
		u8 *content;
		
	
			//ͬ����ϢӦ��
			u8 *output;
			//ͬ����ϢӦ�𳤶�
			u32 outlen;
			//δʹ��
			u16 expire;

		
		//ʵ������
		char *dstAlias;
		//ʵ����������
		u8 dstAliasLen;
	}
	//������
	#if  (defined(_LINUX_) && !defined(_EQUATOR_))
		__attribute__ ((packed)) 
	#endif
	CIPCMessage;

#endif

//ʵ����Ҫ��Ϣ
typedef struct
{
    u32 state;
	u16 insid;	
	u16 evtrcv;
	u16 evtsnd;
}TIPCInstInfo;
//����Ӧ���пɱ�������ʵ����Ҫ��Ϣ����
#define MAX_INSTINFO_NUM		   100

//Ӧ�ó�����࣬��Ҫ���ڶ����û�Appģ��zIPCTemplate
#if defined(PWLIB_SUPPORT) && defined(_LINUX_)
	class CIPCApp : public PThread
#else
	struct IPC_API CIPCApp
#endif
{
#if defined(PWLIB_SUPPORT) && defined(_LINUX_)	
	PCLASSINFO(CIPCApp, PThread);
#endif
	
public:
	//Ӧ�ý�����Ϣ����
	u32 msgIncome;
	//Ӧ���Ѵ�����Ϣ����
	u32 msgProcessed;
	//Ӧ�������д��������Ϣ����(δʹ��)
	u32 msgWaiting;
	//Ӧ�������д��������Ϣ������ֵ
	u32 msgWaitingTop;
	//�Ѽ����Ķ�ʱ��Ϣ����
	u32 timerProcessed;
	//Ӧ�����������Ϣ����
	u32 maxMsgWaiting;
	//����������������Ϣ����
	u32 msgdropped;
	//Ӧ�ñ���
	char *pAppName;

	//Ӧ��������д���
	u32 queSendId;
	//Ӧ������������
	u32 queRcvId;
	
	/* Ϊ֧��ͬ����Ϣ�����ӵ���Ϣ���ݶ��У����ͬ����Ϣ���ͺ�Ӧ��ʱ��Ϣ
	   ��Ӧ����Ϣ����֮ǰ��������Ϣ */
	//Ӧ�ñ������е���Ϣ����
	u32 bakQueMsgNum;
	//Ӧ�ñ�����д���
	u32 bakQueSendId;
	//Ӧ�ñ���������
	u32 bakQueRcvId;
	
	//Ӧ������ID
	u32 taskID;
	
	// ��ǰ�յ�����Ϣ
	CIPCMessage *curMsgRcv;
	//��Ļtrc��־
	u16 scrnTraceFlag;
	//�ļ�trc��־
	u16 fileTraceFlag;
	//Ӧ��ID
	u16 appId;
	//Ӧ��ǰһ������ʵ����
	u16 wLastIdleInstID;
	// ��ǰ���͵���Ϣ��
	u16 curEvtSnd;
    //��Ļlog����
	u8 scrnLogFlag;
	//�ļ�log����
	u8 fileLogFlag;	

	//Ӧ��������
	TASKHANDLE hAppTask;
	//Ӧ�������ź������
	SEMHANDLE tSemMutex;    

	//Ӧ���������ȼ�
	u8 byAppPri;

	// ���ڼ�¼ʵ�����״̬���յ����ݵĻ��л���
	u8 byInstInfoHead;
	u8 byInstInfoTail;
	TIPCInstInfo tInstInfo[MAX_INSTINFO_NUM]; 
	u8 bRunning;
	HANDLE hThreadID;

public:
#if defined(_LINUX_) && defined(PWLIB_SUPPORT)
	CIPCApp() : PThread(200<<10)
#else
	CIPCApp()
#endif
	{
		msgIncome = 0;
		msgProcessed = 0;
		timerProcessed = 0; 	
		maxMsgWaiting = 0;
		msgdropped = 0;
		msgWaitingTop = 0;
		pAppName = NULL;
		queSendId = 0;
		queRcvId = 0;
		bakQueMsgNum = 0;
		bakQueSendId = 0;
		bakQueRcvId = 0;
		taskID = 0;
		scrnTraceFlag = 0;
		fileTraceFlag = 0;
		scrnLogFlag = 0;
		fileLogFlag = 0;
		appId = IPC_INVALID_APP;
		byAppPri = 100;
		wLastIdleInstID = 0;
		hAppTask = 0;
#ifndef _LINUX_
		tSemMutex = NULL;
#endif
		byInstInfoHead = 0;
		byInstInfoTail = 0; 	
		
		curMsgRcv = NULL;
		curEvtSnd = 0;
		bRunning=true;
		hThreadID =NULL;
	}
	
	virtual ~CIPCApp()
	{
		scrnTraceFlag = 0;
		fileTraceFlag = 0;
		scrnLogFlag = 0;
		fileLogFlag = 0;
	}
	
	//����ʵ���Ų�ѯָ��ʵ��
	virtual class CIPCInstance* GetInstance(u16 insid) = 0;
	//�趨ʵ������
	virtual BOOL32 SetInstAlias(u16 insid, const char * pAlias, u8 len) = 0;
	//���ݱ�����ѯָ��ʵ��
	virtual class CIPCInstance* FindInstByAlias(const char * pAlias, u8 len) = 0;
	//�õ�Ӧ�����ʵ����
	virtual int GetInstanceNumber(void) = 0;
	//�õ�ʵ��������󳤶�
	virtual u8 GetMaxAliasLen(void) = 0;
	//��ʼ��ʵ��������¼��Ϣ
	virtual BOOL32	InitAliasArray(void) = 0;	
	//���ʵ��������¼��Ϣ
	virtual BOOL32	ClearInstAlias(u16 insid, const char * pAlias, u8 len) = 0;
	
	//����Ӧ��
	// APP������ȼ�Ϊ80�����С��80��IPC��ǿ��Ϊ80���˺�, �û����Ե���IPCSetAppPriority()��CIPCApp::SetPriority()�������ȼ�.
	//�ɹ�����IPC_OK      //�����ɹ�    ʧ�ܷ���IPC_ERROR  //����ʧ��
	int CreateApp( char *name, u16 aid, u8 pri, u16 queuesize = 80, u32 uStackSize = 200<<10 );
	//�趨Ӧ���������ȼ�
	BOOL32 SetPriority(u8 byPrior);
	//��ѯ�������ȼ�
	u8 GetPriority(void)
	{
		return byAppPri;
	}
	//Ӧ���˳�
	void QuitApp(void);

	//Ӧ�ñ�������Ϣ�������Ӽ��ٲ�ѯ
	BOOL32 BakMsgNumInc(void);
	BOOL32 BakMsgNumDec(void);
	u32 GetBakMsgNum(void);

	//Ӧ����������Ϣ������ѯ���Ӽ���
	u32 GetMsgIncomeNum(void);
	void MsgIncomeNumInc(void);
	void MsgIncomeNumDec(void);

	//Ӧ�ô�����Ϣ��������
	void MsgProcessedNumInc(void);

	//��ѯ���������Ϣ����
	u32 GetMsgWaitingNum(void);

	//Ӧ�ñ����趨��ѯ
	void SetName(const char * pName);
	char *NameGet(void);

	//��ʱ������������
	void TimerProcessedIncrease(void);

	//�趨�ļ���Ļlog����
	void LogLevelSet(u8 file_level, u8 screen_level);
	//�趨�ļ���Ļtrc��־
	void TrcFlagSet(u16 file_flag, u16 screen_flag);

	//ʵ����Ҫ��Ϣ��ӡ
	void InstInfoShow(void);
	//ʵ����Ҫ��Ϣ����
	void InstInfoAdd(u16 ins, u32 state, u16 evtrcv, u16 evtsnd);

#if defined(_LINUX_) && defined(PWLIB_SUPPORT)
	void Main(void);  // pwlib pure virtual function
#endif
};


//ʵ����ʱ����Ϣ
typedef  struct TIPCInstTimerInfo
{
	TIPCInstTimerInfo *next ;
	u32 timerId ;
	void *timerBlkAdrr ;
}TIPCInstTimerInfo;


struct TIPCTmBlk // ��ʱ���ƿ�
{
    TIPCTmBlk * suc; // ��һ��ʱ����ƿ顣
    TIPCTmBlk * pre; // ��һ��ʱ����ƿ顣
    u64 tick;
    u16 appId;
    u16 instId;
    u16 timerId;
    u32 param;
    u32 settedTick;
    u16 timeToLeft;
    time_t absTime;      //���Զ�ʱ��ʱʱ��
	BOOL bExtendMode;    //�Ƿ�ʹ����չģʽ(Ĭ�ϲ�ʹ��)
	u8  *pExtContent;    //��չ����
	u16  dwExtLength;    //��չ���ݵĳ���
};


#define  TIMERHANDLE  TIPCTmBlk*


//CIPCInstance�������淶��
//����IPC��Ӧ�ñ���Ӹ�CIPCInstance��������һ�������Instance�࣬
//������һ��InstanceEntry()��Ա�����ľ���ʵ��
class IPC_API CIPCInstance
{
public:
	enum
	{
		//����ʵ��
		IPC_PENDING,
		//�㲥
		IPC_EACH = 0xffff,
		//�㲥��Ҫ��ȷ��Ӧ��
		IPC_EACH_ACK = 0xfffe,
		//δʹ��
		IPC_CONN_ACK = 0xfffd,
		//�ػ�ʵ��
		IPC_DAEMON = 0xfffc,
		//Ŀ��ʵ��ʵ������Ч��������Ч
		IPC_INVALID = 0xfffb
	};
	
public:
	//Ӧ�ú�
	u16 m_appId;
	//ʵ����
	u16 m_instId;
	//ʵ������
	char m_alias[MAX_ALIAS_LEN];
	//ʵ����������
	u8 m_aliasLen;
	//ʵ��������󳤶�
	u8 m_maxAliasLen;
	//��ǰ״̬��
	u32 m_curState;
	//��һ��״̬��
	u32 m_lastState;
	
public:
	//�ػ�ʵ����ں���
	virtual void DaemonInstanceEntry( CIPCMessage *const, CIPCApp* )
	{
		log( 0, "IPC: message received in default daemon instance of app %d.\nCheck your daemon implementation, please!\n", GetAppID() );
	}
	//ʵ����ں���
	virtual void InstanceEntry( CIPCMessage *const pMsg) = 0;
	//ʵ����ӡ����
	virtual void InstanceDump(u32=0)
	{
	}
	//�ػ�ʵ����ӡ����
	virtual void DaemonInstanceDump(u32=0)
	{
	}
	//ʵ���˳�
	virtual void InstanceExit(void)
	{
	}
	
	//�趨ʵ������
	int SetAlias(const char * pchAlias, u8 len);
	//ɾ��ʵ������
	void DeleteAlias(void);
	//��ѯʵ������
	BOOL32 GetAlias(char *aliasBuf, u8 bufLen, u8 *aliasLen);	
	//��ѯʵ����������
	u8 GetAliasLen(void);
	
	//��ѯ��һ��ʵ��״̬��
	u32 		LastState();
	//��ѯ��ǰʵ��״̬��
	u32 		CurState();
	
	//��ѯӦ�ú�
	u16 		GetAppID();
	//��ѯʵ����
	u16 		GetInsID();

	//��ʱ���ź���
	SEMHANDLE	   m_tSemTimerList;
	//��ʱ����Ϣ����
	TIPCInstTimerInfo	m_timerInfoListHead;

	//���붨ʱ����Ϣ
	void AddInstTimerInfo(u32 nTimerNo , void * pTimerAddr);
	//ɾ����ʱ����Ϣ
	void DelInstTimerInfo(u32 nTimerNo);
	//��ѯ��ʱ����Ϣ
	void *FindInstTimerInfo(u32 nTimerNo);
	//ɾ�����ж�ʱ����Ϣ
	void DelAllInstTimerInfo();
	//������Զ�ʱ
	int SetTimer(TIMERHANDLE *pTimerHandle, u32 dwTimer, long uMilliSeconds, const void* content, u16 length );
    //ȡ����ʱ
    int KillTimer(TIMERHANDLE hTimerHandle);
protected:
	//������Զ�ʱ
	int SetTimer( u32 nTimer, long nMilliSeconds, u32 nPara = 0 );
	//���þ��Զ�ʱ
	int SetAbsTimer( u32 nTimer, u16 nYear, u8 nMon, u8 nDay, u8 nHour, u8 nMin, u8 nSec, u32 nPara = 0 );
	//ȡ����ʱ
	int KillTimer(u32 nTimerNo);
	// ȡ�����Զ�ʱ��
	int KillAbsTimer( u32 nTimerNo);
	//ʵ����ӡ,�ɵ���IPCSetLogLevel�޸Ĵ�ӡ����
	void log(u8 level, char * format,...);
	//�л�״̬��
	void NextState(u32 state, char *s = 0 );
	
	//�����첽��Ϣ(Ŀ��ʵ������ʵ���ű�ʶ)
	int post(u32 dstiid, u16 event, const void *content=0, u32 length=0, u32 dstnode=0);
	//�����첽��Ϣ(Ŀ��ʵ������ʵ��������ʶ)
	int post(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
		const void* content=0, u32 length=0, u32 dstnode=0);

	//����ͬ����Ϣ(Ŀ��ʵ������ʵ���ű�ʶ)
	int send(u32 dstiid, u16 event, const void *content=0, u32 length=0, u32 dstnode=0, 
		void* ackbuf=NULL, u32 ackbuflen=0, u32 *realacklen=NULL, 
		u16 timeout=1000);
	//����ͬ����Ϣ(Ŀ��ʵ������ʵ��������ʶ)
	int send(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
		const void* content=0, u32 length=0, u32 dstnode=0,
		void* ackbuf=NULL, u32 ackbuflen=0, u32 *realacklen=NULL, 
		u16 timeout=1000);

	//����ͬ����ϢӦ��
	int SetSyncAck(const void* ack=NULL, u32 ackLen=0);
	//����ͬ����ϢӦ��(����ʹ��SetSyncAck)
	int reply(const void* ack=NULL, u32 ackLen=0); 
};

//���������û�App���ģ��
class CIPCAppNoData
{
};

//ʵ���������ٲ��Ҽ�¼����
typedef struct TInstAliasInfo
{
	//ʵ������������ֵ(Ҳ�Ǵ�ŵ�λ���±�)
	u32  aliasRet;
	//��ͬ����ֵ����һ��ʵ�����λ��
	u32  nextLoc;
	//ʵ����
	u16  instId;
}TInstAliasInfo;

//Ӧ��ģ��(ģ�����Ϊʵ���ļ̳����Լ����ʵ��ʵ����)
template <class I, int maxins=1, class A=CIPCAppNoData, u8 maxAliasLen=0>
class zIPCTemplate:public CIPCApp, public A
{
public:
	//����ʵ������
	BOOL32 SetInstAlias(u16 insid, const char * pchAlias, u8 len);
	//���ʵ��������¼��Ϣ
	BOOL32	ClearInstAlias(u16 insid, const char * pAlias, u8 len);
	
private:
	 //�ɾֲ�ʵ���Ų���ʵ��
	CIPCInstance* GetInstance(u16 insid);
	//�ɱ�������ʵ��
	CIPCInstance* FindInstByAlias(const char * pchAlias, u8 len);
	//�����ͨʵ������
	int GetInstanceNumber(void);
	//�������������
	u8 GetMaxAliasLen(void);
	
private:
	//daemonʵ��
	I daemIns;
	//��ͨʵ����
	I insarray[maxins];
	
	//ʵ���������ٲ���
	//ʵ��������¼��Ϣ��ͷ��
	TInstAliasInfo instAliasArray[maxins];
	//ʵ��������¼��Ϣ���ñ���¼������ͬ��ʵ��
	TInstAliasInfo bakInstAliasArray[maxins];
	//��ǰ���ñ��м�¼��ʵ������
	u32 curBakArrayNum;
	
	//��ʼ��ʵ��������¼��Ϣ
	BOOL32 InitAliasArray();
	//ʵ������ת����WORD32������
	u32 AliasToWord(char * pchAlias, u8 len);
	//ȷ��ʵ��������Ϣ��������ñ����ڱ��ñ���
	BOOL32 IsInBakArray(u32 setRet);
};

/*=============================================================================
�� �� ����IPCLog
��	  �ܣ�ȫ��log��������������Ŀ�����Ӧ�ú�0������IPCSetLogLevel(0,xx)�Ϳ���
���Ƹú����Ĵ򿪻�ر�
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u8 level : [in] log level
char *format : [in] message format
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCLog(u8 level, char *format, ...);

/*=============================================================================
�� �� ����IPCMsgDumpSet
��	  �ܣ�set long msg dump detail can/not be whole print, default is printf
10lines
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����BOOL32 LgMsgDumpEnbl : [in] enable or not
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCMsgDumpSet(BOOL32 LgMsgDumpEnbl);

/*=============================================================================
�� �� ����IPCStatusMsgOutSet
��	  �ܣ�set status message can or not be out put every timer
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����BOOL32 OutMsgEnable : [in] enable or not
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCStatusMsgOutSet(BOOL32 OutMsgEnable);

/*====================================================================
��������IPCPrintf
���ܣ�����Ӧ��������ʾ����Ļ,�洢���ļ�,��������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����
bScreen: �Ƿ��������Ļ,
bFile: �Ƿ�������ļ�,
szFormat: ��ʽ,
����ֵ˵����
====================================================================*/
API void IPCPrintf(BOOL32 bScreen, BOOL32 bFile, char *szFormat, ...);


/*====================================================================
��������IPCTelAuthor
���ܣ�����Telnet����Ȩ�û���������(vxworks���û���������Ϊ�ջ�����ʧ��)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����
����ֵ˵����
====================================================================*/
API void IPCTelAuthor(const char * szUsername = NULL ,const char * szPassword = NULL );

/*=============================================================================
�� �� ����IPCVerPrintf, IPCVer
��	  �ܣ�output compile version info to screen
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
#define  IPCVerPrintf() \
IPCPrintf(TRUE, TRUE, "IPC version: %s  compile time: %s %s\n", IPCVERSION, __DATE__, __TIME__)
API void IPCver();

/*=============================================================================
�� �� ����IPCCat
��	  �ܣ�output file content to screen
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����const char*  fname : [in] file to be opened
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCCat(const char*  fname);

#ifdef _LINUX_
typedef void (*PIPCQuitFunc)(int);
/*====================================================================
  ��������IPCRegQuitFunc
  ���ܣ�ע�������˳���������linux����Ч
  �㷨ʵ�֣�
  ����ȫ�ֱ�����
  �������˵����
  	pQuitFunc	- �����˳�����
  ����ֵ˵����TRUE - ע��ɹ�
  ====================================================================*/
API BOOL32 IPCRegQuitFunc( PIPCQuitFunc pQuitFunc );

/*====================================================================
  ��������IPCRegTaskInfo
  ���ܣ�ע��������Ϣ����linux����Ч
  �㷨ʵ�֣�
  ����ȫ�ֱ�����
  �������˵����
  	dwTaskId	- �����
		szName	- �������
  ����ֵ˵������
  ====================================================================*/
API void IPCRegTaskInfo(u32 dwTaskId, const char* szName);

/*====================================================================
  ��������IPCUnRegTaskInfo
  ���ܣ���ע��������Ϣ
  �㷨ʵ�֣�
  ����ȫ�ֱ�����
  �������˵����
  	dwTaskId	- �����
  ����ֵ˵������
  ====================================================================*/
API BOOL32 IPCUnRegTaskInfo(u32 dwTaskId);

/*====================================================================
  ��������IPCTaskShow
  ���ܣ���ӡ������Ϣ����linux����Ч
  �㷨ʵ�֣�
  ����ȫ�ֱ�����
  �������˵����
  ����ֵ˵������
  ====================================================================*/
API void IPCTaskShow();

/*====================================================================
  ��������IPCRegCommand
  ���ܣ�ע�������Telnet��ִ�еĺ���(�ú�������linux����Ч��windows�²���ע��)
  �㷨ʵ�֣�
  ����ȫ�ֱ�����
  �������˵����
  	szName	- Telnet�����������
	pfFunc	- ��Ӧ�ĺ���ָ��
	szUsage	- ����İ�����Ϣ
  ����ֵ˵������
  ====================================================================*/
API void IPCRegCommand(const char* szName, void* pfFunc, const char* szUsage);
#endif

/*=============================================================================
�� �� ����IPCNodeConnTest
��	  �ܣ�test a node's network connection
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 NodeId : [in] id of the node
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCNodeConnTest(u32 NodeId);

/*=============================================================================
�� �� ����IPCNodeIpGet
��	  �ܣ����ָ������Ip��ַ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����dwNodeId : [in] node id

 �� �� ֵ���ɹ����ؽ��IP, ʧ�ܷ���0.
=============================================================================*/
API u32 IPCNodeIpGet(u32 dwNodeId);

/*=============================================================================
  �� �� ����IPCNodeLocalIpGet
  ��    �ܣ���ȡ�ڵ�ı��ص�ַ
  ע    �⣺�����Ѿ����ӵ�tcp�ڵ㣬�ڱ��ش��ڶ����ַ������£���Ҫ֪���Է�ʵ�����ӵı���ip��ַ��
  �㷨ʵ�֣�
  ȫ�ֱ�����
  ��    ����dwNodeId : [in] node id

  �� �� ֵ���ɹ����ؽ��IP, ʧ�ܷ���0.
  =============================================================================*/
API u32 IPCNodeLocalIpGet(u32 dwNodeId);

/*====================================================================
�������� IPCInstShow
���ܣ���ʾĳ��App�е�Instance��״̬��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCInstShow(u16 aid);

/*=============================================================================
�� �� ����IPCInstShowAll
��	  �ܣ���ʾ����Instance��״̬��Ϣ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCInstShowAll(void);

/*=============================================================================
�� �� ����IPCInstDump
��	  �ܣ���ʾĳһinstance���Զ�����Ϣ�����ø�instance��dump������
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����aid : [in] app id
InstId : [in] instance id, 0:��ʾ���еģ�-1:��ʾ��IDLE��
param : [in] reserved parameter
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCInstDump(u16 aid, u16 InstId, u32 param);

/*====================================================================
��������IPCNodeShow
���ܣ���ʾ����Node��״̬��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCNodeShow(void);

/*=============================================================================
�� �� ����IPCIPCAppShow
��	  �ܣ���ʾApp��״̬��Ϣ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCAppShow();

/*=============================================================================
�� �� ����IPCDispatchShow
��	  �ܣ���ʾDispatch task����Ϣ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCDispatchShow();

/*====================================================================
��������IPCTimerShow
���ܣ���ʾtimer״̬��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCTimerShow(void);

/*====================================================================
��������IPCLogShow
���ܣ���ʾ��־ģ��״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ���:
�������˵����
����ֵ˵��: ��
====================================================================*/
API void IPCLogShow(void);

/*====================================================================
��������IPCMemShow
���ܣ�	��ʾ�ڴ�����ջ����Ϣ����Ŀ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCMemShow(void);

/*=============================================================================
�� �� ����IPCIPCAppEchoTest
��	  �ܣ�Application echo testing
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u16 AppId : [in] Application id
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCAppEchoTest(u16 AppId);

/*=============================================================================
�� �� ����IPCAddEventDesc
��	  �ܣ����ڽ��¼��������Ǽǵ���Ϣ����ϵͳ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����const char *	desc : [in]
u16 event : [in]
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCAddEventDesc (const char *  desc, u16 event);

/*=============================================================================
�� �� ����IPCSendTrcOn
��	  �ܣ���IPC��㷢�͸��ٱ�ʶ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCSendTrcOn();

/*=============================================================================
�� �� ����IPCSendTrcOff
��	  �ܣ��ر�IPC��㷢�͸��ٱ�ʶ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCSendTrcOff();

/*=============================================================================
�� �� ����IPCRcvTrcOn
��	  �ܣ���IPC�����ո��ٱ�ʶ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCRcvTrcOn();

/*=============================================================================
�� �� ����IPCRcvTrcOff
��	  �ܣ��ر�IPC�����ո��ٱ�ʶ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCRcvTrcOff();

/*====================================================================
��������IPCTrcAllOn
���ܣ���IPC����Ӧ�õĸ��ٱ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCTrcAllOn(void);

/*====================================================================
��������IPCTrcAllOn
���ܣ���IPC����Ӧ�õĸ��ٱ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCTrcAllOff(void);

/*=============================================================================
�� �� ����IPCOpenLogFile
��	  �ܣ�����־�ļ�
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����const char *	name : [in] file name
u32 maxsize_KB : [in] �ļ���С����
u32 maxfiles : [in]�ļ���Ŀ����
�� �� ֵ��BOOL32 - true �ɹ�, false ʧ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCOpenLogFile(const char *  name, u32 maxsize_KB, u32 maxfiles);

/*=============================================================================
�� �� ����IPCCloseLogFile
��	  �ܣ��ر���־�ļ���
ע	  �⣺�������IPCQuit������Ҫ���øú�����
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCCloseLogFile(void);

/*=============================================================================
�� �� ����IPCSetLogLevel
��	  �ܣ�����һ��APP��CIPCInstance::log�����ļ���
ע	  �⣺������IPCInit֮����øú���������Ч��IPCInit������Ӧ�õĸñ�־��0
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����aid : [in] app id
file_level : [in] file level
screen_level : [in] screen level
if level=5, ��ô0~5 ��ӡ,  6~255 ����ӡ,����Ϊ3,��ֻ��log(0|1|2,"")
�ĵ����ܹ������
�� �� ֵ��
=============================================================================*/
API void IPCSetLogLevel(u16 aid, u8 file_level, u8 screen_level);

/*====================================================================
��������IPCSetFileLogLevel	
���ܣ�����App���ļ���־�ȼ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����wAid App�ĵȼ�
byFileLevel: �ļ�Log�ĵȼ�
����ֵ˵����
====================================================================*/
API void IPCSetFileLogLevel(u16 wAid, u8 byFileLevel);

/*====================================================================
��������IPCSetScrnLogLevel	
���ܣ�����App����Ļ��־�ȼ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����
uAid App�ĵȼ�
uScrnLevel: ��ĻLog�ĵȼ�
����ֵ˵����
====================================================================*/
API void IPCSetScrnLogLevel(u16 wAid, u8 byScrnLevel);

/*=============================================================================
�� �� ����IPCSetTrcFlag
��	  �ܣ�����ÿ��Ӧ�õĸ��ٷ�Χ��Trace ��Ϊ Trace ״̬�� 1: ����Ϣ, 2: ����Ϣ
4: �趨ʱ 8:�ն�ʱ 16:״̬ԾǨ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����aid : [in] app id
file_flag : [in] trace file flag
screen_flag : [in] screen output flag
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCSetTrcFlag(u16 wAid, u16 wFileFlag, u16 wScrnFlag);

/*====================================================================
��������IPCSetFileTrcFlag
���ܣ�����ĳ��Ӧ�õĸ��ٱ�־
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����uAid appl ����
uFileFlag: �ļ����ٱ���
����ֵ˵��:
====================================================================*/
API void IPCSetFileTrcFlag(u16 wAid, u16 wFileFlag);

/*====================================================================
��������IPCSetScrnTrcFlag
���ܣ�����ĳ��Ӧ�õĸ��ٱ�־
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����uAid application ����
uScreenFlag: ��Ļ���ٱ���
����ֵ˵��:
====================================================================*/
API void IPCSetScrnTrcFlag(u16 wAid, u16 wScreenFlag);

/*=============================================================================
�� �� ����IPCRegistModule
��	  �ܣ���IPC�Ǽ�һ��ģ��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����(in)pchModuleName - ģ�飨.exe�ļ���������NULL��β���ַ�����

�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCRegistModule(const char * szModuleName);

/*=============================================================================
�� �� ����IPCSetLogEventDataLength
��	  �ܣ�����ȫ�ֵ���Ϣ������ʾ�ĳ��ȡ�
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u16 wLength : [in] length of log event data
�� �� ֵ��u16 previous length
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API u16 IPCSetLogEventDataLength(u16);

/*=============================================================================
�� �� ����IPCEnableBroadcastAck
��	  �ܣ����������£��û����͹㲥��Ϣ������Ҫ�Է�����ȷ����Ϣ��ȱʡ����£�
PIS������Ϣ�ķ����߷���IPC_BROADCASTACK��Ϣ�����ǿ���ͨ���ú����޸���
��ѡ��Ƿ���IPC_BROADCASTACK���ɽ��ܽڵ�����ġ�
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u16 aid : [in] ��ʾ�㲥��Ϣ�Ľ�����AID��
BOOL32 bEnable : [in] enable or not
�� �� ֵ��BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCEnableBroadcastAck(u16, BOOL32);

/*=============================================================================
�� �� ����IPCGetNodeAddr
��	  �ܣ�����IPC����ַ��������Զ��IP+Port����
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 dwNodeId : [in] ���ID
					TIPCNodeAddr* ptIPCNodeAddr : [out] ����ַ
�� �� ֵ��FALSE - ��ѯ�ɹ�
					TRUE - ��ѯʧ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
typedef struct
{
	u32 m_dwLocalIP;//������
	u32 m_dwPeerIP;//������
    u16 m_wLocalPort;//������
	u16 m_wPeerPort;//������
}TIPCNodeAddr;
API BOOL32 IPCGetNodeAddr( u32 dwNodeId , TIPCNodeAddr* ptIPCNodeAddr );

#if defined(_MSC_VER) || defined(_LINUX_)

//sock5����汾��
#define SOCK5_PROXY_VERSION			(u8)0x05
//�ͻ���֧�ּ�Ȩ����
#define SOCK5_PROXY_AUTHEN_NO_REQUIERD			(u8)0x00
#define SOCK5_PROXY_AUTHEN_USERNAME_PASSWORD	(u8)0x02
//�������
#define SOCK5_PROXY_SUCCESS						(u8)0x00
#define SOCK5_PROXY_AUTHEN_NO_ACCEPTABLE		(u8)0xff
//��������
#define SOCK5_PROXY_CMD_TCP_CONNECT				(u8)0x01
#define SOCK5_PROXY_CMD_TCP_BIND				(u8)0x02
#define SOCK5_PROXY_CMD_UDP_ASSOCIATE			(u8)0x03
//�����ֶ�
#define SOCK5_PROXY_RESERVED_DATA				(u8)0x00
//��ַ����
#define SOCK5_PROXY_IPV4_ADDR					(u8)0x01
#define SOCK5_PROXY_DOMAIN_NAME					(u8)0x03
#define SOCK5_PROXY_IPV6_ADDR					(u8)0x04


#define MAX_SOCK5PROXY_AUTHEN_NUM	(u8)0xff
#define MAX_SOCK5PROXY_NAME_PASS_LENGTH	(u8)64
typedef struct 
{
	u32 m_dwProxyIP;
	u16 m_wProxyPort;
	u8 m_byAuthenNum;
	u8 m_abyAuthenMethod[MAX_SOCK5PROXY_AUTHEN_NUM];
	s8 m_achUseName[MAX_SOCK5PROXY_NAME_PASS_LENGTH+1];
	s8 m_achPassword[MAX_SOCK5PROXY_NAME_PASS_LENGTH+1];
}TIPCSock5Proxy;

/*=============================================================================
�� �� ����IPCConnectTcpNodeThroughSock5Proxy
��	  �ܣ�TCP��Խsock5�������ӷ���ˣ���IPCConnectTcpNode������ͬ���ϲ㲻��Ҫ����IPCConnectToSock5Proxy���������
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����ptIPCSock5Proxy : [in] �����������Ϣ�ṹ;
		  dwServerIP : [in] ������IP
		  wServerPort : [in] �������˿�
		  wHb: [in] �����������(��)
		  byHbNum: [in] byHbNum��δ�����Ӽ��Ӧ�����Ϊ��·�ѶϿ�
		  dwTimeoutMs : [in] ������ʱʱ��
		  pdwLocalIP: [in,out] ��TCP����ʹ�õı���IP
�� �� ֵ��ʧ�ܷ���IPC_INVALID_NODE��
		  �ɹ�����һ����̬����Ľ���, �˺��û��ɽ���������ͨ��
		  �ϲ��������������IPCDisconnectTcpNode���������޴���ʱ��������
-------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API int IPCConnectTcpNodeThroughSock5Proxy( TIPCSock5Proxy* ptIPCSock5Proxy , u32 dwServerIP, u16 wServerPort,
									u16 wHb = 10, u8 byHbNum = 3 , u32 dwTimeoutMs = 0 , u32 *pdwLocalIP = NULL, u8 bySocksAuthVerion = SOCK5_PROXY_VERSION );

/*=============================================================================
�� �� ����IPCConnectToSock5Proxy
��	  �ܣ���sock5�������������TCP��·��UDP��Խsock5��������Ƚ���һ��TCP��·��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����ptIPCSock5Proxy : [in] �����������Ϣ�ṹ
		  dwTimeoutMs : [in] ������ʱʱ��
�� �� ֵ��ʧ�ܷ���INVALID_SOCKET��
		  �ɹ����������ͨ�ŵ�TCP Socket���ɽ�һ������IPCUdpAssociateThroughSock5Proxy����UDP Associate��
		  �ɸ��ã���Ҫ�ϲ�ά����TCP���ӣ���·����Чʱ��Udp AssociateҲӦ��Ϊ��Ч��
		  �������������IPCDisconnectFromSock5Proxy��
-------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API SOCKHANDLE IPCConnectToSock5Proxy( TIPCSock5Proxy* ptIPCSock5Proxy , u32 dwTimeoutMs = 0 , u8 bySocksAuthVerion = SOCK5_PROXY_VERSION);

/*=============================================================================
�� �� ����IPCUdpAssociateThroughSock5Proxy
��	  �ܣ�UDP��Խsock5����
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����hSocket : [in] IPCConnectToSock5Proxy���ص�socket(�ɸ���);
		  dwLocalIP : [in] �����շ�socket IP���Ա����������������ݴ�Խ��������
		  wLocaPort : [in] �����շ�socket �˿ڣ��Ա����������������ݴ�Խ��������
		  pdwProxyMapIP : [out] ���������ӳ��IP��������
		  pwProxyMapPort : [out] ���������ӳ��˿ڣ�������
		  dwTimeoutMs : [in] ������ʱʱ��
�� �� ֵ��ʧ�ܷ���FALSE��
		  �ɹ�����TRUE
-------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCUdpAssociateThroughSock5Proxy( SOCKHANDLE hSocket , u32 dwLocalIP, u16 wLocalPort ,
									u32* pdwProxyMapIP, u16* pwProxyMapPort , u32 dwTimeoutMs = 0 );

/*=============================================================================
�� �� ����IPCDisConnectFromSock5Proxy
��	  �ܣ��Ͽ���sock5������������UDP Associate��TCP����
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����hSocket : [in] IPCConnectToSock5Proxy���ص�socket;
�� �� ֵ��ʧ�ܷ���FALSE��
		  �ɹ�����TRUE
-------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCDisConnectFromSock5Proxy( SOCKHANDLE hSocket );

#endif

/*=============================================================================
�� �� ����IPCCreateTcpNode
��	  �ܣ�����һ��TCP�ڵ㡣���Ժ�����ڵ���Ա����ӡ���ʹ��6682��Ϊport������
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 dwAddr : [in] reserved
u16 wPort : [in] which port to use
bTcpNodeReuse:[in]������node�˳���˿��Ƿ����������û�������������ռ�� 
�� �� ֵ��int - If no error occurs, CreateTcpNode returns a descriptor
referencing the new socket. Otherwise, a value of
INVALID_SOCKET is returned.
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCCreateTcpNode(u32 dwAddr, u16 wPort, BOOL32 bTcpNodeReuse=FALSE);

/*=============================================================================
�� �� ����IPCConnectTcpNode
��	  �ܣ��ڵ�ַuIpv4Addr�Ͷ˿�uTcpPort�����ӷ�����, �����ö���������.
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����g_IPC
��	  ����uIpv4Addr : [in] �������IP��ַ,
wTcpPort : [in] ������������˿ں�,
wHb: [in] �����������(��),
byHbNum: [in] byHbNum��δ�����Ӽ��Ӧ�����Ϊ��·�ѶϿ�,
dwTimeOutMs: [in] �����dwTimeOutMs���������Ӳ��ɹ�, ���س���. ȱʡֵ0, 
��ʾȡconnect��ʱ��ȱʡʱ��(75s).  �ò���Ŀǰ����VxWorks����Ч.
pdwLocalIP: [out] ��ǰ�������õ�IP��ַ

 �� �� ֵ���ɹ�����һ����̬����Ľ���, �˺��û��ɽ���������ͨ��. 
 ʧ�ܷ���IPC_INVALID_NODE.
 -------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API int IPCConnectTcpNode(u32 dwIpv4Addr, u16 wTcpPort, u16 wHb=10, u8 byHbNum=3, u32 dwTimeoutMs=0, u32* pdwLocalIP=NULL);

/*=============================================================================
�� �� ����IPCSetHBParam
��	  �ܣ����ý��Ķ���������
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����dwNodeID: ����,
wHb: ��·�������(����Ϊ��λ),
byHbNum: �������ping��uHbNum��û��Ӧ��, �����Ͽ�.

 �� �� ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE.
 -------------------------------------------------------------------------------
 �޸ļ�¼��
 �� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCSetHBParam(u32 dwNodeID, u16 wHb, u8 byHbNum);

/*=============================================================================
�� �� ����IPCNodeDiscCBReg
��	  �ܣ�������node�����ж�ʱ��֪ͨ��appid��InstId
ע	  �⣺Ŀǰ֧��֪ͨ���ʵ����ÿ�ε��øú�������ָֻ֪ͨ��ʵ����������ע���
����������֪ͨʵ�����������
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����dwNodeId : [in] node id
wAppId : [in] application id
wInsId : [in] instance id
�� �� ֵ���ɹ�����IPC_OK��ʧ�ܲμ�������
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCNodeDiscCBReg(u32 dwNodeId, u16 wAppId, u16 wInsId);

/*=============================================================================
�� �� ����IPCNodeDiscCBRegQ
��	  �ܣ�������node�����ж�ʱ��֪ͨ��appid��InstId
ע	  �⣺ÿ�ε��ô˺�����������һ��Ҫ֪ͨ��ʵ�������Ǵﵽ������
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����dwNodeId : [in] node id
wAppId : [in] application id
wInsId : [in] instance id
�� �� ֵ���ɹ�����IPC_OK��ʧ�ܲμ�������
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCNodeDiscCBRegQ(u32 dwNodeId, u16 wAppId, u16 wInsId);

/*=============================================================================
�� �� ����IPCDisconnectTcpNode
��	  �ܣ��Ͽ���һ��node�ϵ����ӡ�
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u8 node_id : [in] node id
�� �� ֵ��BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCDisconnectTcpNode(u32 nodeId);


#ifdef MULTI_LISTEN_SOCKET_SUPPORT
/*=============================================================================
�� �� ����IPCCloseTcpNode
��	  �ܣ��ر�һ���׽��ֵļ�������Ӧ��IPCCreateTcpNode��������(Ŀǰ��linux����Ч)��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����int nSocketFd : [in] �׽�����������һ��ΪIPCCreateTcpNode�ķ���ֵ
�� �� ֵ��BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCCloseTcpNode(int nSocketFd);

#endif //MULTI_LISTEN_SOCKET_SUPPORT


#ifdef _LINUX_
/*=============================================================================
�� �� ����IPCDisconnectAllTcpNode
��	  �ܣ�ֹͣ��ǰ�ļ���,���Ͽ����ϴ���ȫ������(Ŀǰ��linux����Ч)��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCDisconnectAllTcpNode(void);
#endif

/*=============================================================================
�� �� ����IPCIsValidTcpNode
��	  �ܣ��ж�һ��TCP�ڵ��Ƿ���Ч��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 node_id : [in] node id
�� �� ֵ��BOOL32 - Valid or not
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCIsValidTcpNode (u32 node_id);

/*====================================================================
��������IPCIsNodeCheckEnable
���ܣ��ж�ָ��������·��⹦���Ƿ����á�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCIsNodeCheckEnable(u32 dwNodeId);

/*====================================================================
��������IPCEnableNodeCheck
���ܣ�����ָ��������·��⹦�ܡ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCEnableNodeCheck(u32 dwNodeId);

/*====================================================================
��������IPCDisableNodeCheck
���ܣ�����ָ��������·��⹦�ܡ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCDisableNodeCheck(u32 dwNodeId);

/*====================================================================
��������IPCSetAppPriority
���ܣ�����App�ĵ������ȼ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����wAppId -- Ŀ��App��,
byPrior -- ���ȼ�.

 ����ֵ˵�����ɹ�, ����TRUE; ʧ��, ����FALSE.
====================================================================*/
API BOOL32 IPCSetAppPriority(u16 wAppId, u8 byPrior);

/*====================================================================
��������IPCIPCGETAPPPriority
���ܣ����ָ��App�ĵ������ȼ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����wAppId -- (in)Ŀ��App��,
pbyPrior -- (out)���ȼ�.

 ����ֵ˵�����ɹ�, ����TRUE; ʧ��, ����FALSE.
====================================================================*/
API BOOL32 IPCGetAppPriority(u16 wAppId, u8* pbyPrior);

/*=============================================================================
�� �� ����IPCIsLocalHost
��	  �ܣ��ж��Ƿ��Ǳ���IP����IPCIsLocalHost( inet_addr( "127.0.0.1" ) );
ע	  �⣺�������Windows��ʹ�ñ������������ȵ���IPCInit������WSAStartup��
������Ч�ʽϵͣ���Ҫ�ظ����á�
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 dwIP : [in] ��Ҫ�жϵ�IP
�� �� ֵ��BOOL32	�Ƿ�Ϊ����IP
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCIsLocalHost( u32 dwIP );

/*=============================================================================
�� �� ����IPCPost
��	  �ܣ������첽��Ϣ
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u32 dstid : [in] destination info
u16 event : [in] IPC_ event
const void* content : [in] message data
u16 length : [in] sizeof content in u8s
u32 dstnode : [in] destionation socket node
u32 srcGlobIID : [in] source info
u32 srcnode : [in] source node
BOOL32 bDroppable : [in] drop the message when message queue full
�� �� ֵ��int - 0 Succeeded, others Failed
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCPost(u32 dstid, u16 event, const void* content=0, u32 length=0,
				u32 dstnode=0, u32 srciid=IPCMAKEID(IPC_INVALID_APP, IPC_INVALID_INS), u32 srcnode=IPC_INVALID_NODE, int timeout=2000); 

/* used only in C++ environment */
int IPCPost(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
			const void* content=0, u32 length=0, u32 dstnode=0, u32 srciid=IPCMAKEID(IPC_INVALID_APP, IPC_INVALID_INS), u32 srcnode=IPC_INVALID_NODE);

			/*=============================================================================
			�� �� ����IPCSend
			��	  �ܣ�ȫ��ͬ����Ϣ����
			ע	  �⣺
			�㷨ʵ�֣�
			ȫ�ֱ�����
			��	  ����u32 dstid : [in] destination info
			u16 event : [in] IPC_ event
			const void* content : [in] message data
			u16 length : [in] sizeof content in u8s
			u32 dstnode : [in] destionation socket node
			u32 srcGlobIID : [in] source info
			u32 srcnode : [in] source node
			BOOL32 bDroppable : [in] drop the message when message queue full
			�� �� ֵ��int - 0 Succeeded, others Failed
			-------------------------------------------------------------------------------
			�޸ļ�¼��
			��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCSend(u32 dstiid, u16 event, const void *content=0, u32 length=0, u32 dstnode=0, 
				u32 srciid=IPCMAKEID(IPC_INVALID_APP, IPC_INVALID_INS), u32 srcnode=IPC_INVALID_NODE,
				void* ackbuf=NULL, u32 ackbuflen=0, u32 *realacklen=NULL, 
				u16 timeout=1000);

/* used only in C++ environment */
int IPCSend(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event,		
			const void* content=0, u32 length=0, u32 dstnode=0, 
			u32 srciid=IPCMAKEID(IPC_INVALID_APP, IPC_INVALID_INS), u32 srcnode=IPC_INVALID_NODE,
			void* ackbuf=NULL, u32 ackbuflen=0, u32 *realacklen=NULL, 
			u16 timeout=1000);

/*====================================================================
��������IsIPCInitd
���ܣ��ж�IPC�Ƿ��ѳ�ʼ������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����IPC�ѳ�ʼ��������TRUE�����򷵻�FALSE.
====================================================================*/
API BOOL32 IsIPCInitd(void);

/*=============================================================================
�� �� ����IPCInit
��	  �ܣ�initialize IPC module
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����TelNetEnable : [in] start telnet.exe
          TelNetPort : [in] IPC telnet server port
          pchModuleName : [in] process module name��maxlength = 63
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
#define MAX_MODULE_NAME_LENGTH 63
API BOOL32 IPCInit(BOOL32 TelNetEnable=FALSE, u16 TelNetPort=0, char* pchModuleName = NULL );

/*====================================================================
��������IPCQuit
���ܣ��˳�IPC�����ñ�������APP������ʵ����InstanceExit�������˳�ʵ����
�Ͽ�����IPC��㣬��������IPC�������APP��Ȼ�󷵻ء�IPC�˳�֮��
����IPC���񽫲���ʹ�á�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����BOOL32 bCleanupSock;  TRUE��ʾ�ر�SOCKET��(Ĭ��) FALSE��ʾ���ر�
 ����ֵ˵����
====================================================================*/
//API void IPCQuit(void);
API void IPCQuit(BOOL32 bCleanupSock=TRUE); //modify by gsh 20140719 ��Ϊ������ƾ����˳�����ģ��

/*=============================================================================
�� �� ����IPCSetPrompt
��	  �ܣ������ն���ʾ��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API BOOL32 IPCSetPrompt(const char * prompt);
 
/*====================================================================
��������IPCStopScrnLog
���ܣ���ͣ��Ļ��־���.
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������

 ����ֵ˵������.
====================================================================*/
API void IPCStopScrnLog(void);

/*====================================================================
��������IPCResumeScrnLog
���ܣ��ָ���Ļ��־���.
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������

 ����ֵ˵������.
====================================================================*/
API void IPCResumeScrnLog(void);

/*=============================================================================
�� �� ����IPCGetLocalID
��	  �ܣ�����ȫ�ֱ��
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����
�� �� ֵ��int - ����ȫ�ֱ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API int IPCGetLocalID(); // ����ȫ�ֱ��

/*=============================================================================
�� �� ����IPCEventDesc
��    �ܣ�������Ϣ������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��    ����u16 wEventID : [in]��Ϣ��ID
�� �� ֵ��const char *  ��Ϣ�������������ϢID��Ч���߸���Ϣû�������ַ���������
ֵ��ΪNULL��
-------------------------------------------------------------------------------
�޸ļ�¼��
�� 	 ��  �汾  �޸���  �޸�����
=============================================================================*/
API const char * IPCEventDesc( u16 wEventID );

/*=============================================================================
�� �� ����IPCEventDescShow
��	  �ܣ���ʾ��Ϣ������
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����u16 wEventID : [in]��Ϣ��ID
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPCEventDescShow( u16 wEventID );

/*=============================================================================
�� �� ����IPChelp
��	  �ܣ�inline help function of IPC module
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����const char * pszTopic : [in] which topic's help infomation to be shown
�� �� ֵ��void
-------------------------------------------------------------------------------
�޸ļ�¼��
��		��	�汾  �޸���  �޸�����
=============================================================================*/
API void IPChelp(void);

/*====================================================================
��������IPCSemBCreate
���ܣ�����һ����Ԫ�ź���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(out)ptSemHandle ���ص��ź������

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
====================================================================*/
API BOOL32 IPCSemBCreate(SEMHANDLE *ptSemHandle);

/*====================================================================
��������IPCSemCCreate
���ܣ����������ź���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(out)ptSemHandle �ź���������ز����� 
(in)initcount ��ʼ������
(in)MaxSemCount ������

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE.
====================================================================*/
API BOOL32 IPCSemCCreate(SEMHANDLE *ptSemHandle,u32 initcount,u32 MaxSemCount);

/*====================================================================
��������IPCSemDelete
���ܣ�ɾ���ź���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(in)tSemHandle ��ɾ���ź����ľ��

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE.
====================================================================*/
API  BOOL32 IPCSemDelete(SEMHANDLE tSemHandle );

/*====================================================================
��������IPCSemTake
���ܣ��ź���p����������ź��������ã�����������ʵ����������(APP)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(in)tSemHandle �ź������

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE.
====================================================================*/
API BOOL32	IPCSemTake(SEMHANDLE tSemHandle );

/*====================================================================
��������IPCSemTakeByTime
���ܣ�����ʱ���ź���p���������ź��������ã���ʱ������dwtime����󣬳�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(in)tSemHandle �ź������, (in)dwtime ��ʱ���(��msΪ��λ)

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE.
====================================================================*/
API BOOL32 IPCSemTakeByTime(SEMHANDLE tSemHandle, u32 dwtime);

/*====================================================================
��������IPCSemGive
���ܣ��ź���v����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(in)tSemHandle �ź������

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE.
====================================================================*/
API BOOL32 IPCSemGive(SEMHANDLE tSemHandle );

/*====================================================================
��������IPCTaskCreate
���ܣ�������ִ��һ������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
#ifdef _LINUX_
API TASKHANDLE IPCTaskCreate(LINUXFUNC pvTaskEntry, char* szName, u8 byPriority, u32 dwStacksize, u32 dwParam, u16 wFlag, u32 *puTaskID=NULL);
#else
API TASKHANDLE IPCTaskCreate(void* pvTaskEntry, char* szName, u8 byPriority, u32 dwStacksize, u32 dwParam, u16 wFlag, u32 *puTaskID=NULL);
#endif


/*====================================================================
  ��������IPCTaskSetAffinity
  ���ܣ�
  ��װ
  �㷨ʵ�֣�����ѡ�
  ����ȫ�ֱ�����
  �������˵����hTaskHandle:windows�µ��߳�/���̵ľ����hTaskId:�󶨵��߳�/����/�����ID��
                dwCpuId:��󶨵�CPU(��0��ʼ����,��εݼ�)
                byTaskType���󶨵��߳�/����/��������ͣ�0--�̣߳�1-���̣�����-���񣩣�linux��Ĭ��0����
                pdwPreviousBind�����֮ǰ�ý���/�߳�/���񱻰󶨵�ĳCPU����᷵�ظ�CPU�ţ���Solaris����Ч��
  ����ֵ˵�����ɹ�����TRUE, ʧ�ܷ���FALSE.
  ====================================================================*/
#ifdef _MSC_VER 
API BOOL32 IPCTaskSetAffinity( TASKHANDLE hTaskHandle, u32 dwCpuId, u8 byTaskType = 0, u32* pdwPreviousBind = NULL);
#else
API BOOL32 IPCTaskSetAffinity( u64 hTaskId, u32 dwCpuId, u8 byTaskType = 0, u32* pdwPreviousBind = NULL);   
#endif

/*====================================================================
��������IPCTaskExit
���ܣ��˳��������������˳�֮ǰ�û�Ҫע���ͷű�����������ڴ桢�ź�������Դ��
��װWindows��ExitThread(0)��vxWorks��taskDelete(0)��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskExit(void);

/*====================================================================
��������IPCTaskTerminate
���ܣ�ǿ����ָֹ������
��װWindows��ExitThread()��vxWorks��taskDelete()��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����handle--��ɱ�����handle

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskTerminate(TASKHANDLE handle);

/*====================================================================
��������IPCTaskSuspend
���ܣ�����ǰ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskSuspend(TASKHANDLE handle=0);

/*====================================================================
��������IPCTaskResume
���ܣ�����ǰ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskResume(TASKHANDLE handle);

/*====================================================================
��������IPCTaskSetPriority
���ܣ��ı���������ȼ���

 �㷨ʵ�֣�����ѡ�
 ����ȫ�ֱ�����
 �������˵����
 
  ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskSetPriority(TASKHANDLE taskHandle, u8 newPriority, int newSchedPolicy = 0);

/*====================================================================
��������IPCTaskGetPriority
���ܣ������������ȼ���

 �㷨ʵ�֣�����ѡ�
 ����ȫ�ֱ�����
 �������˵����
 
  ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskGetPriority(TASKHANDLE taskHandle, u8 * puPri, int *SchedPolicy =NULL);

/*====================================================================
��������IPCSetTaskPriorityByName
���ܣ�ͨ��������������������ȼ�

 �㷨ʵ�֣�����ѡ�
 ����ȫ�ֱ�����
 �������˵����
 
  ����ֵ˵����
====================================================================*/
API void IPCSetTaskPriorityByName(s8 *name , u8 priority, int newSchedPolicy = 0);

/*====================================================================
��������IPCSetTaskPriorityByName
���ܣ�ͨ����������ȡ��������ȼ�

 �㷨ʵ�֣�����ѡ�
 ����ȫ�ֱ�����
 �������˵����
 
  ����ֵ˵����
====================================================================*/
API void IPCGetTaskPriorityByName(s8 *name , u8 *priority, int *newSchedPolicy = NULL);

/*====================================================================
��������IPCTaskSelfHandle
���ܣ���õ�������ľ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API TASKHANDLE IPCTaskSelfHandle(void);

/*====================================================================
��������IPCTaskSelfHandle
���ܣ���õ��������ID
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API u32 IPCTaskSelfID(void);

/*====================================================================
��������IPCTaskHandleVerify
���ܣ��ж�ָ�������Ƿ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCTaskHandleVerify(TASKHANDLE handle);

/*====================================================================
��������IPCTaskSafe
���ܣ������������񲻱��Ƿ�ɾ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCTaskSafe(void);

/*====================================================================
��������IPCTaskUnsafe
���ܣ��ͷŵ��������ɾ��������ʹ�õ���������Ա���ȫɾ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API void IPCTaskUnsafe(void);

/*====================================================================
��������IPCTaskDelay
���ܣ�������ʱ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����uMseconds--��ʱ���(ms)

 ����ֵ˵����
====================================================================*/
#define IPCDelay					  IPCTaskDelay

API void IPCTaskDelay(u32 dwMseconds);

/*====================================================================
��������IPCCreateMailbox
���ܣ�������Ϣ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCCreateMailbox(char* szName,u32 dwMsgNumber,u32 dwMsgLength ,u32 *puQueueIDRead,u32 *puQueueIDWrite);

/*====================================================================
��������IPCCloseMailbox
���ܣ��ر�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����uQueueIDRead, uQueueIDWrite--����Ķ���д���

 ����ֵ˵����
====================================================================*/
API void IPCCloseMailbox(u32 dwQueueIDRead, u32 dwQueueIDWrite);

/*====================================================================
������		��IPCSndMsg
����		�������䷢��Ϣ
�㷨ʵ��	��
����ȫ�ֱ�������
�������˵����tQueueID: ����ID,
lpMsgBuf: ����ָ��,
len: ����,
timeout: ��ʱֵ(ms). Windows�¸ò�����Ч, ��������һֱ����ֱ���пյ�buffer.
VxWorks��, ȱʡΪ2��; 
IPC_NO_WAIT(0): ��������������;
IPC_WAIT_FOREVER(-1): ����������ֱ���пյ�buffer.

 ����ֵ˵��  ���ɹ�����TRUE, ʧ�ܻ�ʱ����FALSE.
====================================================================*/					
API BOOL32 IPCSndMsg(u32 tQueueID, const char *lpMsgBuf, u32 len, int timeout=2000);

/*====================================================================
������		��IPCRcvMsg
����		������������Ϣ
�㷨ʵ��	��
����ȫ�ֱ�������
�������˵����
OSTOKEN tQueueID	����ID
u32  dwTimeout	 ��ʱ(����)
LPVOID *lpMsgBuf	������Ϣ����ָ��
����ֵ˵��	��
====================================================================*/
API BOOL32 IPCRcvMsg(u32 tQueueID,u32  dwTimeout,char * lpMsgBuf, u32 len, u32 * pLenRcved);

/*====================================================================
��������IPCClkRateGet
���ܣ��õ�tick���ȣ�n ticks/sec
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API u32 IPCClkRateGet();

/*====================================================================
��������IPCTickGet
���ܣ�ȡ�õ�ǰ��tick��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API u32 IPCTickGet();


/*====================================================================
��������IPCTickGet64
���ܣ�ȡ�õ�ǰ��tick��,64���ȣ�����������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API u64 IPCTickGet64();



API void IPCCpuShow();

//CPU��Ϣ
typedef struct
{
	// ���аٷֱȣ�0��100
	u8 m_byIdlePercent;
}TIPCCpuInfo;

/*====================================================================
��������IPCGetCpuInfo
���ܣ�ȡ�õ�ǰ��CPU��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptCpuInfo���û�CPU��Ϣ�ṹ
����ֵ˵����ΪTRUEʱ��ʾ��ȡ��Ϣ�ɹ��������ʾʧ��
====================================================================*/
API BOOL32 IPCGetCpuInfo( TIPCCpuInfo* ptCpuInfo );

//�ڴ���Ϣ
typedef struct
{
	//�����ڴ��С(KByte)
	u32 m_dwPhysicsSize;
	//��ʹ���ڴ�(KBytes)
	u32 m_dwAllocSize;
	//�����ڴ�(KBytes)
	u32 m_dwFreeSize;
}TIPCMemInfo;

/*====================================================================
��������IPCGetMemInfo
���ܣ�ȡ�õ�ǰ��Mem��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptMemInfo���û�MEM��Ϣ�ṹ
����ֵ˵����ΪTRUEʱ��ʾ��ȡ��Ϣ�ɹ��������ʾʧ��
====================================================================*/
API BOOL32 IPCGetMemInfo( TIPCMemInfo* ptMemInfo );

//ʱ����Ϣ
typedef struct
{
	//��(2006)
	u16 m_wYear;
	//��(1--12)
	u16 m_wMonth;
	//��(1--31)
	u16 m_wDay;
	//ʱ(0--23)
	u16 m_wHour;
	//��(0--59)
	u16 m_wMinute;
	//��(0--59)
	u16 m_wSecond;
}TIPCTimeInfo;

/*====================================================================
��������IPCGetTimeInfo
���ܣ�ȡ�õ�ǰ��Time��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptTimeInfo��ϵͳtime��Ϣ�ṹ
����ֵ˵����ΪTRUEʱ��ʾ��ȡ��Ϣ�ɹ��������ʾʧ��
====================================================================*/
API BOOL32 IPCGetTimeInfo( TIPCTimeInfo* ptTimeInfo );

/*====================================================================
��������IPCSetTimeInfo
���ܣ����õ�ǰ��Time��Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptTimeInfo��ϵͳtime��Ϣ�ṹ
����ֵ˵����ΪTRUEʱ��ʾ��ȡ��Ϣ�ɹ��������ʾʧ��
====================================================================*/
API BOOL32 IPCSetTimeInfo( TIPCTimeInfo* ptTimeInfo );

//���̷�����Ϣ
typedef struct
{
	//���̷�����С(MByte)
	u32 m_dwPhysicsSize;
	//��ʹ�ô��̷�����С(MBytes)
	u32 m_dwUsedSize;
	//���ô��̷�����С(MBytes)
	u32 m_dwFreeSize;
}TIPCDiskInfo;
//���̷�����󳤶�
#define MAX_PARTION_NAME_LENGTH		255
/*====================================================================
��������IPCGetDiskInfo
���ܣ�ȡ�õ�ǰ�Ĵ��̷�����Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����s8 *pchPartionName�� ������
					win32 : "c:"
					Vxworks : "/ata0/"
					Linux : "/"
			  ptDiskInfo��ϵͳ���̷�����Ϣ�ṹ
����ֵ˵����ΪTRUEʱ��ʾ��ȡ��Ϣ�ɹ��������ʾʧ��
====================================================================*/
API BOOL32 IPCGetDiskInfo( s8 *pchPartionName , TIPCDiskInfo* ptDiskInfo );

//////////////////////////////////////////////////////////////////////
//	�׽��ַ�װ���û�ʹ���׽���ǰ�����ȵ���SockInit()���׽��ֿ���г�ʼ����
//	ʹ����֮������Ҫ����SockCleanup()�ͷŶ��׽��ֿ��ʹ�á�����vxWorks��
//	Windows�Ĺر��׽��ֵĺ�����ͬ�������ڵ���socket��connect�Ⱥ���ʱ������
//	ֵ����Ҳ��һ����IPCҲ�����˱�Ҫ�ķ�װ��ԭ���Ǳ�����Windows�»���һ�¡�
/////////////////////////////////////////////////////////////////////

/*====================================================================
��������SockInit
���ܣ��׽��ֳ�ʼ������װWindows�µ�WSAStartup��vxWorks�·���TRUE
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
====================================================================*/
API BOOL32 SockInit(void);

/*====================================================================
��������SockShutdown
���ܣ��Ը�ƽ̨shutdown�ļ򵥷�װ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����hSock--Ҫ�������׽��֣�
			  dwMode--����ģʽ, ��ȡֵΪSTOP_SEND, STOP_RECV��STOP_BOTH

����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
====================================================================*/
#ifdef _LINUX_
API BOOL SockShutdown(SOCKHANDLE hSock, u32 uMode);
#endif

/*====================================================================
��������SockClose
���ܣ��ر��׽��֣���װwindows�µ�closesocket��vxworks�µ�close.
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����tSock--���ر��׽��ֵľ��������socketʱ���أ���

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
====================================================================*/
API BOOL32 SockClose(SOCKHANDLE tSock);

/*====================================================================
��������SockCleanup
���ܣ��׽������٣���װwindows��WSACleanup��vxWorks�·���TRUE
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
====================================================================*/
API BOOL32 SockCleanup(void);

// ���ڷ�װ����Windows���ڲ�������Ϊ׼����VxWorks���ṩ��Ӧ����
/*====================================================================
��������IPCSerialOpen
���ܣ���ָ���Ĵ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵�����ɹ����ش򿪴��ڵľ����ʧ�ܷ���INVALID_SERIALHANDLE
====================================================================*/
API SERIALHANDLE IPCSerialOpen(u8 port);

/*====================================================================
��������IPCSerialClose
���ܣ��ر�ָ���Ĵ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCSerialClose(SERIALHANDLE hCom);

/*====================================================================
��������IPCSerialRead
���ܣ���ָ���Ĵ��ڶ�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCSerialRead(SERIALHANDLE hCom, char *pchBuf, u32 dwu8sToRead, u32 *puu8sRead);

/*====================================================================
��������IPCSerialWrite
���ܣ���ָ���Ĵ���д����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCSerialWrite(SERIALHANDLE hCom, char *pchBuf, u32 dwu8sToWrite, u32 *puu8sWrite);

/*====================================================================
��������IPCSetCommTimeouts
���ܣ����ô��ڳ�ʱ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCSetCommTimeouts(SERIALHANDLE hCom, TIPCCommTimeouts *ptCommTimeouts);

/*====================================================================
��������IPCGetCommState
���ܣ���ô���״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCGetCommState(SERIALHANDLE hCom, TIPCDCB *ptDCB);

/*====================================================================
��������IPCSetCommState
���ܣ����ô���״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCSetCommState(SERIALHANDLE hCom, TIPCDCB *ptDCB);

/*====================================================================
��������IPCEscapeCommFunction
���ܣ�ʹ�ô�����չ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCEscapeCommFunction(SERIALHANDLE hCom, u32 dwFunc);

/*====================================================================
��������IPCPurgeComm
���ܣ���������δ�����ݣ���ֹ���ڶ�д����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCPurgeComm(SERIALHANDLE hCom, u32 dwFlags);

/*====================================================================
��������IPCClearCommError
���ܣ���Windows��ClearCommError��ͬ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCClearCommError(SERIALHANDLE hCom, u32 *pdwErrors, TIPCComStat *ptStat);

/*====================================================================
��������IPCGetCommTimeouts
���ܣ�ȡ�ô򿪴��ڵĳ�ʱ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵����
====================================================================*/
API BOOL32 IPCGetCommTimeouts(SERIALHANDLE hCom, TIPCCommTimeouts *ptCommTimeouts);

/*====================================================================
��������IPCAgentStart
���ܣ�����һ��IPC���Դ���, ���а���һ�����Է�����APP��һ���ͻ���APP, ����
��Ҫ��ͬ��APP��, ��Ϣ������̶���һ����.
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����(in)port -- �˿ں�, ��������APP��������ڵ����øö˿ڴ���֮

 ����ֵ˵����
====================================================================*/
API int IPCAgentStart(u16 port);

/*====================================================================
��������malloc
���ܣ����ڴ���з����ڴ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����size: ��������ڴ���С��

 ����ֵ˵�����ɹ�����ָ����䵽���ڴ���ָ�룬ʧ�ܷ���NULL.
====================================================================*/
API void *IPCAllocMem(size_t size);

/*====================================================================
��������free
���ܣ��ͷ����ȷ�����ڴ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����token: ָ����ͷŵ��ڴ���ָ�롣

 ����ֵ˵������.
====================================================================*/
API void IPCFreeMem(void *token);

/*====================================================================
��������IPCSetPriRealTime
���ܣ��Ѷ�ʱ������Ϣ���͵�������ߵ���ϵͳnet���񻹸ߵ����ȼ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����

 ����ֵ˵������.
====================================================================*/
API void IPCSetPriRealTime();

/*=============================================================================
�� �� ����IPCAddrListGet
��	  �ܣ���ȡ������ַ������
ע	  �⣺�������Windows��ʹ�ñ������������ȵ���IPCInit������WSAStartup��
������Ч�ʽϵͣ���Ҫ�ظ����á�
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ���� u32	 adwIP[] : [in/out]�û���������ڵ�ַ�����ݵ�������׵�ַ
u16   wNum : [in]�û����������Ĵ�С
�� �� ֵ�� ���ر�����ַ���е�ַ����������ʱ����0
=============================================================================*/
API u16  IPCAddrListGet(u32 adwIPList[], u16 wNum);

/*
//���ܣ��ڴ�أ� ��ʼ�����ڴ���
//������
//nLess128KNums   ,less than the  128K ram  segments counts
//�� �� ֵ�� �ɹ�����TRUE, ʧ�ܷ���FALSE.
*/

API BOOL32 IPCPoolCreate( int nLess128KNums);
API BOOL32 IPCPoolCreate128KB(int n128kNums);
API BOOL32 IPCPoolCreate256KB(int n256kNums);
API BOOL32 IPCPoolCreate512KB(int n512kNums);
API BOOL32 IPCPoolCreate1MB(int n1MNums);
API BOOL32 IPCPoolCreate2MB(int n2MNums);
API BOOL32 IPCPoolCreate4MB(int n4MNums);
API BOOL32 IPCPoolCreate8MB(int n8MNums);
API BOOL32 IPCPoolCreate16MB(int n16MNums);


//ϵͳ���Ʋ���
#define IPC_ETHERNET_ADAPTER_MAX_IP 16			/*ÿ���ӿ�������ַ����*/
#define IPC_ETHERNET_ADAPTER_MAX_NUM 32			/*ÿ���豸���ӿ���*/
#define IPC_ETHERNET_ADAPTER_NAME_LENGTH 64 /*ÿ���ӿ�������󳤶�*/
#define IPC_ETHERNET_ADAPTER_MAC_LENGTH	6		/*�ӿڵ�MAC��ַ����*/

//����״̬,��Ӧ��TIPCEthernetAdapterInfo �ṹ��nState�ֶ�
#define IPC_ETHERNET_ADAPTER_UNKNOWN 0
#define IPC_ETHERNET_ADAPTER_UP 1
#define IPC_ETHERNET_ADAPTER_DOWN 2

/* ����������Ϣ */
typedef struct {
    u32 nId; 										/*������*/
    u32 nState; 									/*����״̬*/
    u8  achName[IPC_ETHERNET_ADAPTER_NAME_LENGTH]; 		/*������*/
    u8  achDescription[IPC_ETHERNET_ADAPTER_NAME_LENGTH];	/*������Ϣ����*/
    u8  achMacAddr[IPC_ETHERNET_ADAPTER_MAC_LENGTH]; 		/*���������ַ*/
    u32 nIpNum; 									/*��ַ����*/
    u32 anIp[IPC_ETHERNET_ADAPTER_MAX_IP]; 				/*��ַ*/
}TIPCEthernetAdapterInfo;

/* ���������Ϣ */
typedef struct {
    u32 nNum; 														/*��������*/
    TIPCEthernetAdapterInfo atEthernetAdapter[IPC_ETHERNET_ADAPTER_MAX_NUM];/*������Ϣ*/
}TIPCEthernetAdapterInfoAll;


/*=============================================================================
�� �� ����IPCGetEthernetAdapterInfoAll
��	  �ܣ���ȡ��ϵͳ������̫��������Ϣ
ע	  �⣺�������Windows��ʹ�ñ������������ȵ���IPCInit������WSAStartup��
������Ч�ʽϵͣ���Ҫ�ظ����á�
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����TEthernetAdapterInfoAll ��������Ϣ�ṹ���ο����ݽṹ�Ķ���
�� �� ֵ�� TRUE/ FALSE �ɹ�/ʧ��
=============================================================================*/
API BOOL IPCGetEthernetAdapterInfoAll(TIPCEthernetAdapterInfoAll * ptEthernetAdapterInfoAll);

/*=============================================================================
�� �� ����IPCGetEthernetAdapterInfo
��	  �ܣ����������Ż�ȡ������Ϣ
ע	  �⣺�������Windows��ʹ�ñ������������ȵ���IPCInit������WSAStartup��
������Ч�ʽϵͣ���Ҫ�ظ����á�
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ����nEthAdapterId �������0-ETHERNET_ADAPTER_MAX_NUM�����֧��16������
            TEthernetAdapterInfo ��������Ϣ�ṹ
�� �� ֵ��TRUE/FALSE  �ɹ�/ʧ��
=============================================================================*/
API BOOL IPCGetEthernetAdapterInfo(u32 nEthAdapterId, TIPCEthernetAdapterInfo * ptEthernetAdapterInfo);






/*=============================================================================
�� �� ����IPCSetMemCheck
��	  �ܣ���ȡ�ڴ��ַ����
ע	  �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��	  ���� u32 * pdwCheckMem :	Ҫ�����ڴ��ַ
u32 dwCheckValue ��  ��������ֵ
BOOL32	bEqual ����������Ϊ��ʱ����ʾҪ�����ڴ��ַ���ݲ���������ֵʱ��Ҫ�澯
��������Ϊ��ʱ����ʾҪ�����ڴ��ַ���ݵ�������ֵʱ��Ҫ�澯
�� �� ֵ�� �ɹ�����TRUE, ʧ�ܷ���FALSE.
=============================================================================*/
API BOOL32	IPCSetMemCheck(u32 * pdwCheckMem, u32 dwCheckValue, BOOL32	bEqual);

/* ������Ա������ģ�����Ա������ʵ�� */
template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zIPCTemplate<I, maxins, A, maxAliasLen>::InitAliasArray()
{
	for(int i = 0; i < maxins; i++)
	{
		instAliasArray[i].instId = 0xFFFF;
		instAliasArray[i].aliasRet = 0; 
		instAliasArray[i].nextLoc = 0xFFFF;
		
		bakInstAliasArray[i].instId = 0xFFFF;
		bakInstAliasArray[i].aliasRet = 0;
		bakInstAliasArray[i].nextLoc = 0xFFFF;
	}
	curBakArrayNum = 0;
	
	return TRUE;
}

template <class I, int maxins, class A, u8 maxAliasLen>
u32 zIPCTemplate<I, maxins, A, maxAliasLen>::AliasToWord(char * pchAlias, u8 len)
{
	u32  ret = 0;
	for(u8 i = 0; i < len; i++ )
	{
		ret =  (ret<<5) + ret + (*(pchAlias + i));
	}
	
	ret = ret & (maxins-1);
	return	ret;
}

template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zIPCTemplate<I, maxins, A, maxAliasLen>::IsInBakArray(u32  setRet)
{
	/*���ȼ��������*/
	if(instAliasArray[setRet].instId == 0xFFFF)    /*�����������λ�û�û�б�ռ��*/
	{
		return FALSE;	  /*���ʵ����ŵ���������*/
	}
	else		 /*�����������λ�û��Ѿ���ռ��*/
	{
		return TRUE;   /*���ʵ����ŵ�bak�����еĿ���λ��*/
	}
}

template <class I, int maxins, class A, u8 maxAliasLen>
int zIPCTemplate<I, maxins, A, maxAliasLen>::GetInstanceNumber(void)
{
	return maxins;
}

template <class I, int maxins, class A, u8 maxAliasLen>
u8 zIPCTemplate<I, maxins, A, maxAliasLen>::GetMaxAliasLen(void)
{
	return maxAliasLen;
}

template <class I, int maxins, class A, u8 maxAliasLen>
CIPCInstance* zIPCTemplate<I, maxins, A, maxAliasLen>::GetInstance(u16 insid)
{
	if(insid == CIPCInstance::IPC_DAEMON)
		return &daemIns;
	
	if((insid > 0) && (insid <= maxins))
		return &insarray[insid-1];
	
	return NULL;
}

template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zIPCTemplate<I, maxins, A, maxAliasLen>::SetInstAlias(u16 insid, const char * pchAlias, u8 len)
{
	u32  dwCycTime = 0;
	
	/*�ȰѼ�¼��Ϣ������ٲ���������*/
	if(insid != CIPCInstance::IPC_DAEMON)
	{
		u32 setRet = AliasToWord((char *)pchAlias, len);	/*ͨ��������������ֵ*/
		if(setRet >= maxins)
			return FALSE;
		
		BOOL32	isBak = IsInBakArray(setRet);	/*ȷ�����λ��*/
		if( isBak == FALSE) 	/*�������������*/
		{
			instAliasArray[setRet].aliasRet = setRet;	 
			instAliasArray[setRet].instId = insid;
			instAliasArray[setRet].nextLoc = 0xFFFF;   /*��ʼ��*/
		}
		else		 /*�����bak������*/
		{
			u32 loc = curBakArrayNum; 
			/*�����ݼ�¼��bak������Ӧλ����*/
			for(loc = 0; loc < maxins; loc++)
			{
				if(bakInstAliasArray[loc].instId == 0xFFFF)
					break;
			}
			if(loc >= maxins)
			{
				return FALSE;
			}
			
			bakInstAliasArray[loc].aliasRet = setRet;
			bakInstAliasArray[loc].instId = insid;
			bakInstAliasArray[loc].nextLoc = 0xFFFF;   /*��ʼ��*/
			
			/*����ͬ����ֵ�����ݿ���������*/
			if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*����������δ����*/
			{
				instAliasArray[setRet].nextLoc = loc;	/*ָ��bak�����д��λ��*/
			}
			else
			{ 
				TInstAliasInfo * tempAlias = &bakInstAliasArray[instAliasArray[setRet].nextLoc];
				if(tempAlias == NULL)
					return FALSE;
				
				dwCycTime = 0;
				while(tempAlias->nextLoc != 0xFFFF)
				{
					if(tempAlias->nextLoc >= maxins)
						return FALSE;
					
					tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
					if(tempAlias == NULL)
						return FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						IPCPrintf(TRUE, FALSE, "SetInstAlias() return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				
				tempAlias->nextLoc = loc; /*�ҵ�bak���������һ����ͬ����ֵ��λ��*/
			}
			
			curBakArrayNum++;	  /*bak�����Ÿ�������*/
		}
	}
	
	CIPCInstance* pIns=GetInstance(insid);
	if( pIns != NULL && len <= maxAliasLen)
	{
		CIPCInstance* pOtherIns = FindInstByAlias(pchAlias, len);
		if( (pOtherIns != NULL) && (pOtherIns->GetInsID() != pIns->GetInsID()) )
		{
			return FALSE;
		}
		memcpy(pIns->m_alias, pchAlias, len);
		pIns->m_aliasLen = len;
		//return pIns->SetAlias(pchAlias, len);
	}
	return TRUE;
}  

template <class I, int maxins, class A, u8 maxAliasLen>
CIPCInstance* zIPCTemplate<I, maxins, A, maxAliasLen>::FindInstByAlias(const char * pchAlias, u8 len)
{
	/* �ȿ��Ƿ���Daemonʵ�� */
	u8	 aliasLen=0;
	u32  dwCycTime = 0;
	
	if(maxAliasLen < len)
	{
		return NULL;
	}
	
	char aliasBuf[MAX_ALIAS_LEN];
	memset(aliasBuf, 0, MAX_ALIAS_LEN);
	
	CIPCInstance *pIns = GetInstance(CIPCInstance::IPC_DAEMON);		
	if(pIns != NULL && 
		pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))
	{
		if( (aliasLen == len) && 
			(memcmp(aliasBuf, pchAlias, len) == 0) )
		{
			return pIns;
		}
	}
	
	/* �ٿ��Ƿ���ͨʵ�� */
	TInstAliasInfo * tempAlias = NULL;
	u32  setRet = AliasToWord((char *)pchAlias, len);
	/*�Ȳ���������*/
	if(setRet >= maxins)
		return NULL;
	
	tempAlias = &instAliasArray[setRet];
	if(tempAlias == NULL)
		return NULL;
	
	pIns = GetInstance(tempAlias->instId);
	if((pIns != NULL) && (pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))) 
	{
		if( (aliasLen == len) && (memcmp(aliasBuf, pchAlias, len) == 0) )
		{
			return pIns;
		}
	}
	/*�ٲ���bak����*/
	do
	{
		if((tempAlias == NULL) || (tempAlias->nextLoc >= maxins))
			return NULL;
		
		tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
		if(tempAlias == NULL)
			return NULL;
		
		CIPCInstance * pIns = GetInstance(tempAlias->instId);
		if((pIns != NULL) && (pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))) 
		{
			if((aliasLen == len) && (memcmp(aliasBuf, pchAlias, len) == 0))
			{
				return pIns;
			}
		}
		
		dwCycTime++;
		if(dwCycTime > maxins)
		{
			IPCPrintf(TRUE, FALSE, "FindInstByAlias() return Cycle as dwCycTime=%d\n", dwCycTime);
			return NULL;
		}
	}while(tempAlias->nextLoc != 0xFFFF); 
	
	return NULL;
}

/*���ʵ��������¼��Ϣ*/
template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zIPCTemplate<I, maxins, A, maxAliasLen>::ClearInstAlias(u16 insid, const char * pAlias, u8 len)
{
	u32  dwCycTime = 0;
	
	if((maxAliasLen < len) || (insid == CIPCInstance::IPC_DAEMON))
	{
		return FALSE;
	}
	
	u32 setRet = AliasToWord((char *)pAlias, len);	  /*ͨ��������������ֵ*/
	if(setRet >= maxins)
		return FALSE;
	
	/*�Ȳ��������飬�ٲ��ұ�������*/
	if(instAliasArray[setRet].instId == insid)
	{
		if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*����������δ����*/
		{
			instAliasArray[setRet].aliasRet = 0;	   /*��ʼ��*/
			instAliasArray[setRet].instId = 0xFFFF;
			instAliasArray[setRet].nextLoc = 0xFFFF;   
		}
		else
		{
			TInstAliasInfo * tempAlias = &instAliasArray[setRet];
			if(tempAlias == NULL)
				return FALSE;
			
			if(tempAlias->nextLoc >= maxins)
				return FALSE;
			
			TInstAliasInfo * relocAlias = &bakInstAliasArray[tempAlias->nextLoc];
			if(relocAlias == NULL)
				return FALSE;
			
			dwCycTime = 0;
			while(relocAlias->nextLoc != 0xFFFF)
			{
				tempAlias->aliasRet = relocAlias->aliasRet;    //����һ���¼��Ϣ��䱾���¼��Ϣ
				tempAlias->instId = relocAlias->instId;
				
				if(relocAlias->nextLoc >= maxins)
					return	FALSE;
				
				tempAlias = relocAlias; 		//λ���Ƶ���һ��
				relocAlias = &bakInstAliasArray[relocAlias->nextLoc];
				if(relocAlias == NULL)
					return	FALSE;
				
				dwCycTime++;
				if(dwCycTime > maxins)
				{
					IPCPrintf(TRUE, FALSE, "ClearInstAlias(1) return Cycle as dwCycTime=%d\n", dwCycTime);
					return FALSE;
				}
			}
			
			tempAlias->aliasRet = relocAlias->aliasRet;    //�ѵ����һ���¼��Ϣ��
			tempAlias->instId = relocAlias->instId;
			tempAlias->nextLoc = 0xFFFF;
			
			relocAlias->aliasRet = 0;	   //������һ���¼��
			relocAlias->instId = 0xFFFF;
			relocAlias->nextLoc = 0xFFFF;
			
			curBakArrayNum--;	  /*bak�����Ÿ�������*/
		}
	}
	else
	{
		if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*����������δ����*/
		{
			return	FALSE;
		}
		else
		{ 
			TInstAliasInfo * tempAlias = &bakInstAliasArray[instAliasArray[setRet].nextLoc];
			if(tempAlias == NULL)
				return FALSE;
			
			TInstAliasInfo * relocAlias = &(instAliasArray[setRet]);
			
			dwCycTime = 0;
			while(tempAlias->instId != insid)
			{
				relocAlias = tempAlias;
				
				if(tempAlias->nextLoc != 0xFFFF)
				{
					if(tempAlias->nextLoc >= maxins)
						return	FALSE;
					
					tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
					if(tempAlias == NULL)
						return FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						IPCPrintf(TRUE, FALSE, "ClearInstAlias(2) return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				else
				{
					return	FALSE;
				}
			}
			
			/*�ҵ�bak�����ж�Ӧʵ����¼λ�ã������λ�õļ�¼ֵ���Ѽ�¼������������*/
			if(tempAlias->nextLoc == 0xFFFF)
			{
				if(relocAlias != NULL)
					relocAlias->nextLoc = 0xFFFF;
				
				tempAlias->aliasRet = 0;	 //��λ��Ϊ������ֵ�����һλ�ã�ֱ�����
				tempAlias->instId = 0xFFFF;
			}
			else
			{
				relocAlias = &bakInstAliasArray[tempAlias->nextLoc];
				if(relocAlias == NULL)
					return FALSE;
				
				dwCycTime = 0;
				while(relocAlias->nextLoc != 0xFFFF)
				{
					tempAlias->aliasRet = relocAlias->aliasRet;    //����һ���¼��Ϣ��䱾���¼��Ϣ
					tempAlias->instId = relocAlias->instId;
					
					if(relocAlias->nextLoc >= maxins)
						return	FALSE;
					
					tempAlias = relocAlias; 		//λ���Ƶ���һ��
					relocAlias = &bakInstAliasArray[relocAlias->nextLoc];
					if(relocAlias == NULL)
						return	FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						IPCPrintf(TRUE, FALSE, "ClearInstAlias(3) return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				
				tempAlias->aliasRet = relocAlias->aliasRet;    //�ѵ����һ���¼��Ϣ��
				tempAlias->instId = relocAlias->instId;
				tempAlias->nextLoc = 0xFFFF;
				
				relocAlias->aliasRet = 0;	   //������һ���¼��
				relocAlias->instId = 0xFFFF;
				relocAlias->nextLoc = 0xFFFF;
			}
			
			curBakArrayNum--;	  /*bak�����Ÿ�������*/
		}
	}	
	
	return	TRUE;
}

#if defined(_MSC_VER) && !defined(_EQUATOR_)
	#pragma pack(pop)
#endif

#endif // _IPC_H
