// DM642DevEx.h: interface for the DM642Device class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PCIEDEVEX_H__
#define __PCIEDEVEX_H__

#include "XThreadBase.h"
#include "XAutoLock.h"
#include "XListPtr.h"

//#include "../VIDEC_WW/WWEncoder.h"
/* Used 32768-65535 */
#define FILE_DEVICE_DM64  0x8000
#define CODE_BASE_DM64    0x800

#define FILE_DEVICE_FPGA  0x8000
#define CODE_BASE_FPGA    CODE_BASE_DM64 + 0x100

//add ?? 
#define AH2K_MAX_WIDTH		1920 
#define AH2K_MAX_HEIGHT		1080 


// Define Interface reference/dereference routines for
// Interfaces exported by IRP_MN_QUERY_INTERFACE

//For DSP board
#define DM64_IOCTL(index) \
	CTL_CODE(FILE_DEVICE_DM64, index, METHOD_BUFFERED, FILE_READ_DATA)

#define IOCTL_WARMRESET \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_RUN \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INITEMIF \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 2, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_LOAD \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 3, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_HOOK \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 4, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_UNHOOK \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 5, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_POST_MSG \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 6, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_RETRIEVE_MSG \
	CTL_CODE(FILE_DEVICE_DM64, CODE_BASE_DM64 + 7, METHOD_IN_DIRECT, FILE_READ_ACCESS)

//Add for FPGA board
#define IOCTL_REG_READ \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 1,  METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_REG_WRITE \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 2,  METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_POST_MSG_FPGA \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 3,  METHOD_BUFFERED, FILE_WRITE_ACCESS)


//  {24131E5D-D439-4BC0-9574-8DF65A983FF9}
static const GUID AH2K_DEVEX_CLASS_GUID = 
{ 0x24131E5D, 0xD439, 0x4BC0, 0x95, 0x74, 0x8D, 0xF6, 0x5A, 0x98, 0x3F, 0xF9 };

static const GUID AH2KEX_DEVEX_CLASS_GUID = 
{ 0x2bb29a53, 0xcb41, 0x4400, 0xbf, 0xa5, 0x26, 0x04, 0xa5, 0xc4, 0xd5, 0x6e };

typedef enum {
	DT_EX_INVALID=0,
	DT_EX_AH1000,
	DT_EX_AH2K,
	DT_EX_AH2KA,
	DT_EX_AH410,
	DT_EX_AH420L,
	DT_EX_AH210,
	DT_EX_AH1100L,
	DT_EX_AH2100L,
	DT_EX_AH2200L,
	DT_EX_AH2040,
	DT_EX_AH2210L,
	DT_EX_AH4220E,
	DT_EX_AH2205L,
	DT_EX_AH3110L,
	DT_EX_AH3102SE,
	DT_EX_AH3101,
	DT_EX_AH3111,
	DT_EX_AH3111A,
	DT_EX_AH3111E,
	DT_EX_AH3210,
	DT_EX_AH3210A,
	DT_EX_AH3111EA,
	DT_EX_AH3101A,
	DT_EX_AH3101EA,
	DT_EX_AH3102SEA,
	DT_EX_QAVP3210,
	DT_EX_QAVP3400,
	DT_EX_AH3201EA,
	DT_EX_HAVP3201,
	DT_EX_HAVP3002SM,
	DT_EX_AH3002SE_T,
	DT_EX_AH3110ED_T,
	DT_EX_AH3110D_T,
	DT_EX_AH3002SE,
	DT_EX_AH3210ED,
	DT_EX_AH3110ED,
	DT_EX_AH3210D,
	DT_EX_AH3110D,
	DT_EX_AH5112SE,
	DT_EX_AH5011SE,
	DT_EX_AH5111SE,
	DT_EX_AH5102SE,
	DT_EX_AH5002SE,
	DT_EX_AH5211SE,
	DT_EX_AH5210SEB,
	DT_EX_COUNT,
}DEV_EX_TYPE;

#define FPGA_VEN_EX_ID			_T("10EE")
#define VEN_EX_ID				_T("104C")
#define DEV_EX_ID_AH1000		_T("A010")
#define DEV_EX_ID_AH2K			_T("A011")
#define DEV_EX_ID_AH2KA			_T("A012")
#define DEV_EX_ID_AH410			_T("A020")
#define DEV_EX_ID_AH420L		_T("A021")
#define DEV_EX_ID_AH210			_T("A030")
#define DEV_EX_ID_AH1100L		_T("A031")
#define DEV_EX_ID_AH2100L		_T("A032")
#define DEV_EX_ID_AH2200L		_T("A033")
#define DEV_EX_ID_AH2040		_T("A040")
#define DEV_EX_ID_AH2210L		_T("A036")
#define DEV_EX_ID_AH4220E		_T("A043")
#define DEV_EX_ID_AH2205L		_T("A038")
#define DEV_EX_ID_AH3110L		_T("B110")
#define DEV_EX_ID_AH3102SE		_T("D102")
#define DEV_EX_ID_AH3101		_T("B101")
#define DEV_EX_ID_AH3101A		_T("B10B")
#define DEV_EX_ID_AH3101EA		_T("C10B")
#define DEV_EX_ID_AH3111		_T("B111")
#define DEV_EX_ID_AH3111A		_T("B11B")
#define DEV_EX_ID_AH3111E		_T("C111")
#define DEV_EX_ID_AH3210		_T("B210")
#define DEV_EX_ID_AH3210A		_T("B21A")
#define DEV_EX_ID_AH3111EA		_T("C11B")
#define DEV_EX_ID_AH3102SEA		_T("D10C")
#define DEV_EX_ID_QAVP3210		_T("F210")
#define DEV_EX_ID_QAVP3400		_T("F400")
#define DEV_EX_ID_AH3201EA		_T("C201")
#define DEV_EX_ID_HAVP3201		_T("F201")
#define DEV_EX_ID_HAVPM3002SM	_T("F002")
#define DEV_EX_ID_AH3002SE_T	_T("C002")
#define DEV_EX_ID_AH3110ED_T	_T("C110")
#define DEV_EX_ID_AH3110D_T		_T("A11A")
#define DEV_EX_ID_AH3002SE		_T("D002")		
#define DEV_EX_ID_AH3210ED		_T("D210")
#define DEV_EX_ID_AH3110ED		_T("D110")
#define DEV_EX_ID_AH3210D		_T("A210")
#define DEV_EX_ID_AH3110D		_T("A110")
#define DEV_EX_ID_AH5112SE		_T("0007")	
#define DEV_EX_ID_AH5011SE		_T("0008")
#define DEV_EX_ID_AH5111SE		_T("0009")
#define DEV_EX_ID_AH5102SE		_T("000A")
#define DEV_EX_ID_AH5002SE		_T("000B")
#define DEV_EX_ID_AH5211SE		_T("000C")
#define DEV_EX_ID_AH5210SEB		_T("000D")

#define MAX_IOBUF_COUNT		3 

class AH2KDev;

class CIOCPModel;

typedef struct tagIOBUF
{
	PBYTE			pBuf;
	OVERLAPPED		Overlapped;

} IOBUF, *PIOBUF;

typedef struct _tagPortARG
{
	unsigned char bar;         /* Pci BaseAddress number */
	unsigned long offs;        /* offset */
	union 
	{
		unsigned long  ldata;  /* send/recv data buffer */
		unsigned short sdata;  /* send/recv data buffer */
		unsigned char  cdata;  /* send/recv data buffer */
	}u;
} PORT_ACCESS ,*PPORT_ACCESS;

class PcieDevEx 
	: public XThreadBase
{
public:
	PcieDevEx(void);
	virtual ~PcieDevEx(void);
public:
	static int GetDevExCount(const GUID*pGUID);
	static int GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type);
	static int BootLoad(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen,bool b64M);

	int Open(int nDevID,const GUID*pGUID,bool bEveryChannelHasAudio);
	void Close(int idChan);

	int PostMsg(PBYTE pData,DWORD dwLen);
	void SetChan(DWORD idChan, AH2KDev *pChan);

	int RegisterWriteULong(unsigned char bar,unsigned long offset,unsigned long data);

protected:
	HANDLE		m_hMutex;
	OVERLAPPED	m_ovPostMsg;

	GUID		m_guidDev;
public:
	HANDLE		m_hDevEx;

	int			m_nDevID;

	bool		m_bEveryChannelHasAudio;
	DEV_EX_TYPE m_deviceType;

// related read data control
public:

protected:
	bool			m_bRunning;
	IOBUF			m_aIOBuf[MAX_IOBUF_COUNT];
	int				m_nBufLen;
	int				m_nReadingIOBufCount;
	int				m_nCurrentIOBufIndex;
	AH2KDev    *m_pChan[4];
	XCritSec		m_csChan;
protected:
	void ThreadProcMain(void);
	bool PrepareRead(PIOBUF pIOBuf);

	int ResetDev(void);

private:
	CIOCPModel * m_IOCP;                         // 主要对象，完成端口模型

public:
	bool ThreadProcMainIOCP(PBYTE pDataBuf,int dwBytesRead,void* pIoContext);
	bool PrepareReadIOCP(void* pIoContext ,PIOBUF pIOBuf,void * pBufBack );
	bool PrepareReadIOCPFirst(void* pContext);
	bool ExpiredReset(int & nTimeoutCount);
	void CanceIO(void);

};







//注意,如使用完成端口模型启用此宏定义   
#define USEIOCP  

//==========================================================================
//	/ 这个类CIOCPModel是本代码的核心类，用于完成端口(IOCP)
//	/ 其中的PER_IO_DATA结构体是封装了用于每一个重叠操作的参数
//==========================================================================

#define MAX_BUFFER_LEN      AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT  

// 同时投递的读设备请求的数量(这个要根据实际的情况灵活设置)   
// 10时后面采集的图像只有3帧   3时后面采集的图像只有10帧 
#define MAX_POST_RECV    1    

#define  NOLISTRECORD  

//失败时循环再次调用次数 
#define MAX_FAIL_COUNT    10     

//malloc失败时循环再次malloc次数 
#define MAX_MALLOC_COUNT    100      

// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                    //标志投递的操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义 
}OPERATION_TYPE;

//单IO数据结构体定义(用于每一个重叠操作的参数) 
typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED    m_Overlapped;                                  // 每一个重叠网络操作的重叠结构      
	HANDLE            m_fileHandle;                                    // 这个文件操作所使用的句柄
	char *                m_szBuffer;                                        // 用于给重叠操作传参数的缓冲区
	int                     m_nBuffLen;
	OPERATION_TYPE m_OpType;                                    // 标识操作的类型(对应上面的枚举)

	// 初始化
	_PER_IO_CONTEXT()
	{
		m_szBuffer = NULL;
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		if(m_szBuffer) ZeroMemory( m_szBuffer,MAX_BUFFER_LEN );
		m_fileHandle = NULL;
		m_nBuffLen   = MAX_BUFFER_LEN;
		m_OpType     = NULL_POSTED;
	}

	~_PER_IO_CONTEXT()
	{
		if( m_fileHandle!=NULL )
		{
			m_fileHandle = NULL;
		}
	}

	// 重置缓冲区内容
	void ResetBuffer()
	{
		if(m_szBuffer) ZeroMemory( m_szBuffer,MAX_BUFFER_LEN );
	}

} PER_IO_CONTEXT, *PPER_IO_CONTEXT;


//单句柄数据结构体定义(用于每一个完成端口，也就是每一个文件操作句柄的参数) 
typedef struct _PER_FILE_CONTEXT
{  
	HANDLE      m_fileReadHandle;                                  //文件操作句柄
	PER_IO_CONTEXT* m_pNewIoContext;

	_PER_FILE_CONTEXT()
	{
		m_fileReadHandle = NULL;
		m_pNewIoContext =NULL;
	}

	~_PER_FILE_CONTEXT()
	{
		if(m_pNewIoContext)
		{
			delete m_pNewIoContext;
			m_pNewIoContext = NULL;
		}

		if( m_fileReadHandle)
		{
		    m_fileReadHandle = NULL;
		}
	}

} PER_FILE_CONTEXT, *PPER_FILE_CONTEXT;


// CIOCPModel类
class CIOCPModel
	: public XThreadBase
{
public:
	CIOCPModel(PcieDevEx * p,HANDLE pFile);
	~CIOCPModel(void);

protected:
	void         ThreadProcMain(void);

public:
	bool         m_bRunning;
	XListPtr    m_listBuffer;
	//XCritSec   m_csListBuffer;

public:
	// 启动服务
	bool Start();
	//	停止服务前先停线程
	void StopThread();
	//	停止服务
	void Stop();
	//重新启动IOCP,但线程不停
	bool ReStart( HANDLE pHandle);

	void SetMainDevEx( PcieDevEx * pDev ) { if(pDev) { m_pMain=pDev ; } }
	void SetMainFileHandle( HANDLE pHandle) { if(pHandle) { m_fileReadHandle=pHandle; } }

protected:
	// 初始化IOCP
	bool _InitializeIOCP();

	// 最后释放资源
	void _DeInitialize();

	// 初始化 绑定至完成端口中 
	bool _InitializeFileRead();

	// 投递数据请求
	bool _PostRead( PER_IO_CONTEXT* pIoContext );

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP( PER_FILE_CONTEXT *pContext);

	// 获得本机的处理器数量
	int _GetNoOfProcessors();

public:

	void  ClearBuffer(void);

private:

	HANDLE                       m_hShutdownEvent;              // 用来通知线程系统退出的事件，为了能够更好的退出线程
	HANDLE                       m_hIOCompletionPort;          // 完成端口的句柄
	int		                         m_nThreads;                          // 生成的线程数量
	PcieDevEx *                  m_pMain;                              // 主设备的设备指针
	PER_FILE_CONTEXT*    m_fileReadContext;               // 用于读取文件操作的Context信息
	HANDLE                       m_fileReadHandle;
	bool                             m_bCallerClose;
public:
	char *                           m_pIOBuffer;

};


#endif