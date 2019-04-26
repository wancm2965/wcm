// DM642DevEx.h: interface for the DM642Device class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DM642DEVEX_H__
#define __DM642DEVEX_H__

#include "XThreadBase.h"
#include "XAutoLock.h"

//#include "../VIDEC_WW/WWEncoder.h"

#define FILE_DEVICE_DM64  0x8000

// Define Interface reference/dereference routines for
// Interfaces exported by IRP_MN_QUERY_INTERFACE

#define DM64_IOCTL(index) \
    CTL_CODE(FILE_DEVICE_DM64, index, METHOD_BUFFERED, FILE_READ_DATA)

#define IOCTL_WARMRESET \
	CTL_CODE(FILE_DEVICE_DM64, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_RUN \
	CTL_CODE(FILE_DEVICE_DM64, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INITEMIF \
	CTL_CODE(FILE_DEVICE_DM64, 0x802, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_LOAD \
	CTL_CODE(FILE_DEVICE_DM64, 0x803, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_HOOK \
	CTL_CODE(FILE_DEVICE_DM64, 0x804, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_UNHOOK \
	CTL_CODE(FILE_DEVICE_DM64, 0x805, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_POST_MSG \
	CTL_CODE(FILE_DEVICE_DM64, 0x806, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_RETRIEVE_MSG \
	CTL_CODE(FILE_DEVICE_DM64, 0x807, METHOD_IN_DIRECT, FILE_READ_ACCESS)


//  {24131E5D-D439-4BC0-9574-8DF65A983FF9}
static const GUID AH2K_DEVEX_CLASS_GUID = 
{ 0x24131E5D, 0xD439, 0x4BC0, 0x95, 0x74, 0x8D, 0xF6, 0x5A, 0x98, 0x3F, 0xF9 };


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
	DT_EX_COUNT,
}DEV_EX_TYPE;

#define VEN_EX_ID			_T("104C")
#define DEV_EX_ID_AH1000	_T("A010")
#define DEV_EX_ID_AH2K		_T("A011")
#define DEV_EX_ID_AH2KA		_T("A012")
#define DEV_EX_ID_AH410		_T("A020")
#define DEV_EX_ID_AH420L	_T("A021")
#define DEV_EX_ID_AH210		_T("A030")
#define DEV_EX_ID_AH1100L	_T("A031")
#define DEV_EX_ID_AH2100L	_T("A032")
#define DEV_EX_ID_AH2200L	_T("A033")
#define DEV_EX_ID_AH2040	_T("A040")
#define DEV_EX_ID_AH2210L	_T("A036")
#define DEV_EX_ID_AH4220E	_T("A043")
#define DEV_EX_ID_AH2205L	_T("A038")
#define DEV_EX_ID_AH3110L	_T("B110")
#define DEV_EX_ID_AH3102SE	_T("D102")
#define DEV_EX_ID_AH3101	_T("B101")
#define DEV_EX_ID_AH3101A	_T("B10B")
#define DEV_EX_ID_AH3101EA	_T("C10B")
#define DEV_EX_ID_AH3111	_T("B111")
#define DEV_EX_ID_AH3111A	_T("B11B")
#define DEV_EX_ID_AH3111E	_T("C111")
#define DEV_EX_ID_AH3210	_T("B210")
#define DEV_EX_ID_AH3210A	_T("B21A")
#define DEV_EX_ID_AH3111EA	_T("C11B")
#define DEV_EX_ID_AH3102SEA	_T("D10C")
#define DEV_EX_ID_QAVP3210	_T("F210")
#define DEV_EX_ID_QAVP3400	_T("F400")
#define DEV_EX_ID_AH3201EA	_T("C201")

#define MAX_IOBUF_COUNT		3
class XAH2K;

typedef struct tagIOBUF
{
	PBYTE			pBuf;
	OVERLAPPED		Overlapped;
} IOBUF, *PIOBUF;

class DM642DevEx 
	: public XThreadBase
{
public:
	DM642DevEx(void);
	virtual ~DM642DevEx(void);
public:
	static int GetDevExCount(const GUID*pGUID);
	static int GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type);
	static int BootLoad(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen,bool b64M);

	int Open(int nDevID,const GUID*pGUID,bool bEveryChannelHasAudio);
	void Close(int idChan);

	int OpenWWDev(XAH2K *pChan);
	void CloseWWDev(XAH2K *pChan);

	int PostMsg(PBYTE pData,DWORD dwLen);
	void SetChan(DWORD idChan, XAH2K *pChan);

protected:
	HANDLE		m_hMutex;
	OVERLAPPED	m_ovPostMsg;

	GUID		m_guidDev;
public:
	HANDLE		m_hDevEx;

#ifdef __WWENCODER_H__
	WWDEV		m_hWWDev;
#endif
	int			m_nDevID;

	bool		m_bEveryChannelHasAudio;

// related read data control
public:

protected:
	bool			m_bRunning;
	IOBUF			m_aIOBuf[MAX_IOBUF_COUNT];
	int				m_nBufLen;
	int				m_nReadingIOBufCount;
	int				m_nCurrentIOBufIndex;
	XAH2K			*m_pChan[4];
	XCritSec		m_csChan;
protected:
	void ThreadProcMain(void);
	bool PrepareRead(PIOBUF pIOBuf);

	int ResetDev(void);
};

#endif